
#include "GODZSkinExp.h"
#include "SkinExporter.h"


using namespace GODZ;



////////////////////////////////////////////////////////////////////////////
//Variable Declarations


/* studio max
UserCoord Whacky = {
	1,	//Right Handed
	1,	//X axis goes right
	4,	//Y Axis goes in
	3,	//Z Axis goes down.
	0,	//U Tex axis is left
	1,  //V Tex axis is Down
};*/

/* direct x first try
UserCoord Whacky = {
	0,	//left Handed
	1,	//X axis goes right
	3,	//Y Axis goes down
	4,	//Z Axis goes in.
	0,	//U Tex axis is left
	1,  //V Tex axis is Down
};
*/

UserCoord Whacky = {
	0,	//left Handed
	1,	//X axis goes right
	2,	//Y Axis goes up
	5,	//Z Axis goes out.
	0,	//U Tex axis is left
	1,  //V Tex axis is Down
};

SkinExporter::SkinExporter(int framesPerSample)
:pIgame(NULL), stream(NULL)
{
	header.framePerSample=framesPerSample;
	ZeroMemory(&header, sizeof(GodzExporterHeader));
}

//Returns the index of the bone. if not stored, it is added to the bones list.
size_t SkinExporter::AssignBone(INode* node)
{
	int index=0;
	std::vector<INode*>::iterator nodeIter;
	for(nodeIter=vBones.begin();nodeIter!=vBones.end();nodeIter++)
	{
		INode* n = *nodeIter;
		if (n==node)
		{
			return index;
		}
	}

	vBones.push_back(node);
	return vBones.size()-1;
}

fpos_t SkinExporter::GetSeekPos()
{
	fpos_t off;
	fgetpos(stream,&off);
	return off;
}

void SkinExporter::WriteTextures(IGameMaterial *material)
{
	int texCount = material->GetNumberOfTextureMaps();
	fwrite(&texCount,sizeof(int),1,stream);
	for(int i=0;i<texCount;i++)
	{
		IGameTextureMap * tex = material->GetIGameTextureMap(i);
		TCHAR * name = tex->GetBitmapFileName();
		WriteText(name);
	}
}

void SkinExporter::WriteMaterial(IGameMaterial *material)
{
	TCHAR* text = material->GetMaterialName();
	WriteText(text);

	WriteTextures(material);

	int subs = material->GetSubMaterialCount();
	fwrite(&subs,sizeof(int),1,stream);

	//write subs
	for(int j=0;j<subs;j++)
	{
		IGameMaterial *subMat = material->GetSubMaterial(j);
		WriteMaterial(subMat);
	}	
}

void SkinExporter::WritePoint4(Point4 &p)
{
	fwrite(&p.x, sizeof(float), 1, stream);
	fwrite(&p.y, sizeof(float), 1, stream);
	fwrite(&p.z, sizeof(float), 1, stream);
	fwrite(&p.w, sizeof(float), 1, stream);
}

void SkinExporter::WritePoint3(Point3 &p)
{
	fwrite(&p.x, sizeof(float), 1, stream);
	fwrite(&p.y, sizeof(float), 1, stream);
	fwrite(&p.z, sizeof(float), 1, stream);
}

void SkinExporter::WritePoint2(Point2 &p)
{
	fwrite(&p.x, sizeof(float), 1, stream);
	fwrite(&p.y, sizeof(float), 1, stream);
}

void SkinExporter::WriteBBox(IGameObject *obj)
{
	Box3 box;
	obj->GetBoundingBox(box);
	Point3 min = box.Min();
	Point3 max = box.Max();
	WritePoint3(min);
	WritePoint3(max);
}

void SkinExporter::WriteFaceEx(FaceEx *f, IGameMesh *gm, int faceIndex)
{
	//write face normal
	for(int i=0;i<3;i++)
	{
		fwrite(&f->norm[i], sizeof(DWORD), 1, stream);
	}

	//write material used by this face
	fwrite(&f->matID, sizeof(int), 1, stream);

	//write vertex indicies
	for(int i=0;i<3;i++)
	{
		fwrite(&f->vert[i], sizeof(DWORD), 1, stream);
	}

	//write out vertex normals	
	for(int i=0;i<3;i++)
	{
		Point3 vnorm;
		gm->GetNormal(faceIndex, i, vnorm);		
		fwrite(&vnorm.x, sizeof(float), 1, stream);
		fwrite(&vnorm.y, sizeof(float), 1, stream);
		fwrite(&vnorm.z, sizeof(float), 1, stream);
	}

	//write texture vertex indicies
	for(int i=0;i<3;i++)
	{
		fwrite(&f->texCoord[i], sizeof(DWORD), 1, stream);
	}	
}

void SkinExporter::WriteText(TCHAR *text)
{
	size_t len = strlen(text) + 1;
	fwrite(&len, sizeof(size_t), 1, stream);
	fwrite(text, sizeof(TCHAR), len, stream);
}

void SkinExporter::WriteSkinnedVertex(IGameSkin *skin, int index)
{
	//write number of bones affecting this vertex
	int numBones=skin->GetNumberOfBones(index);
	fwrite(&numBones,sizeof(int),1,stream);

	for(int j=0;j<numBones;j++)
	{
		//write out weight
		float weight = skin->GetWeight(index,j);
		fwrite(&weight,sizeof(float),1,stream);

		//write out bone ID		
		INode* node = skin->GetBone(index,j);
		size_t id = AssignBone(node);
		fwrite(&id,sizeof(size_t),1,stream);
	}
}

void SkinExporter::WriteMatrix(GMatrix &matrix)
{
	for(int i=0;i<4;i++)
	{
		Point4 p = matrix.GetRow(i);
		WritePoint4(p);
	}
}

void SkinExporter::WriteInitialTM(IGameNode *node)
{
	GMatrix m = node->GetWorldTM();
	WriteMatrix(m);
}

void SkinExporter::DumpSampleKeys(IGameControl * sc, DWORD Type)
{
	Tab<Matrix3>sKey;  
	Tab<GMatrix>gKey;
	IGameKeyTab Key;
	IGameControl * c = sc;

	if(!c)
		return;

	if(c->GetFullSampledKeys(Key,header.framePerSample,IGameControlType(Type)) )
	{

		int count = Key.Count();
		fwrite(&count,sizeof(int),1,stream);
		//AddXMLAttribute(sampleData,_T("SampleRate"),Buf.data());

		for(int i=0;i<Key.Count();i++)
		{
			//CComPtr<IXMLDOMNode> data;
			//CreateXMLNode(pXMLDoc,sampleData,_T("Sample"),&data);
			int fc = Key[i].t;
			fwrite(&fc,sizeof(int),1,stream);
			//Buf.printf("%d",fc);
			//AddXMLAttribute(data,_T("frame"),Buf.data());

			if(Type == IGAME_TM)
			{
				//Even though its a 4x4 we dump it as a 4x3 ;-)
				DumpMatrix(Key[i].sampleKey.gval.ExtractMatrix3());
			}
		}
	}
}


void SkinExporter::WriteBoneKeys(IGameNode *node)
{
	IGameControl *pGameControl = node->GetIGameControl();
	IGameControl::MaxControlType T = pGameControl->GetControlType(IGAME_POS);

	if (T != IGameControl::MaxControlType::IGAME_BIPED)
	{
		Interface * ip = GetCOREInterface();
		LogSys *TheManager = ip->Log();
		TheManager->LogEntry(SYSLOG_ERROR, DISPLAY_DIALOG, "Error occured wrting bone anim data", _T("%s - %s\n"), "Unedxpected Error");
	}
	else
	{
		DumpSampleKeys(pGameControl,IGAME_TM);
	}

}

void SkinExporter::WriteSkin(IGameSkin *skin)
{
	if(skin->GetSkinType()== IGameSkin::IGAME_PHYSIQUE)
	{
		header.iNumSkinVerts = skin->GetNumOfSkinnedVerts();
		header.skinVertOffset=GetSeekPos();
		for(int i=0; i<header.iNumSkinVerts;i++)
		{
			int type = skin->GetVertexType(i);

			//write vertex type
			fwrite(&type,sizeof(int),1,stream); 

			if(type==IGameSkin::IGAME_RIGID)
			{
				WriteSkinnedVertex(skin,i);
			}
			else //blended
			{
				WriteSkinnedVertex(skin,i);
			}
		}
	}

	header.iNumBones=vBones.size();

	//now that all the skinned vertices have been written, write the bones list.
	header.boneOffset=GetSeekPos();
	std::vector<INode*>::iterator boneIter;
	for (boneIter=vBones.begin();boneIter!=vBones.end();boneIter++)
	{
		INode *node = *boneIter;
		WriteText(node->GetName());

		IGameScene *subIgame = GetIGameInterface();
		subIgame->InitialiseIGame(node,false);
		IGameNode *pGameNode = subIgame->GetIGameNode(node);
		IGameObject* obj = pGameNode->GetIGameObject();
		obj->InitializeData();
		//WriteInitialTM(pGameNode,stream);
		WriteBBox(obj);

		//get animation data for this bone
		WriteBoneKeys(pGameNode);

		//release resources
		subIgame->ReleaseIGame();
	}
}

bool SkinExporter::SaveGameScene(const TCHAR *szFilename)
{
	INode * selNode = GetCOREInterface()->GetSelNode(0);

	if(!selNode)
		return 0;

	
	/*
		if(cS == 0)
		buf = TSTR("3dsmax");
	if(cS == 1)
		buf = TSTR("directx");
	if(cS == 2)
		buf = TSTR("opengl");
		*/

	FILE *stream;
	if( (stream  = fopen( szFilename, "wb+" )) == NULL )
	{
		return 0;
	}

	GodzExporterHeader header;

	pIgame = GetIGameInterface();
	IGameConversionManager * cm = GetConversionManager();
	cm->SetUserCoordSystem(Whacky);
	//cm->SetCoordSystem((IGameConversionManager::CoordSystem)1);
	cm->SetCoordSystem(IGameConversionManager::IGAME_D3D);

	pIgame->InitialiseIGame(selNode,false);
	//pIgame->InitialiseIGame(false);
	pIgame->SetStaticFrame(0);
	//int nodes=pIgame->GetTopLevelNodeCount();

	IGameNode * pGameNode = pIgame->GetIGameNode(selNode);
	IGameObject* obj= pGameNode->GetIGameObject();

	if(obj->GetIGameType()==IGameObject::IGAME_MESH)
	{
		IGameMesh * gm = (IGameMesh*)obj;
		gm->SetUseWeightedNormals();
		if (!gm->InitializeData())
			return 0;		

		header.iNumVertices=gm->GetNumberOfVerts();  //8 verts for a simple texture mapped box
		header.iNumFaces=gm->GetNumberOfFaces();     //8 ....
		header.iNumTVerts=gm->GetNumberOfTexVerts(); //12 ....
		header.iNumNormals=gm->GetNumberOfNormals(); //24 ....

		fwrite(&header,sizeof(GodzExporterHeader),1,stream);		

		//write out vertex normals
		header.normOffset=GetSeekPos();
		for(int i = 0;i<header.iNumNormals;i++)
		{
			Point3 n = gm->GetNormal(i);
			WritePoint3(n);
		}

		//write out faces
		header.facesOffset=GetSeekPos();
		int numFaces = header.iNumFaces;
		for(int i = 0;i<numFaces;i++)
		{
			FaceEx * f = gm->GetFace(i);
			//DebugPrint("Normals %d %d %d\n",f->norm[0],f->norm[1],f->norm[2]);
			WriteFaceEx(f,gm,i);
		}

		//write out vertices
		header.vertsOffset=GetSeekPos();
		for (int i=0;i<header.iNumVertices;i++)
		{
			Point3 v = gm->GetVertex(i);
			WritePoint3(v);
		}

		//write out texture vertices
		header.tvertsOffset=GetSeekPos();
		for (int i=0;i<header.iNumTVerts;i++)
		{
			Point2 tv = gm->GetTexVertex(i);
			WritePoint2(tv);
		}
	}

	header.nameOffset=GetSeekPos();
	TCHAR *text = selNode->GetName();
	WriteText(text);

	//export materials
	header.materialOffset=GetSeekPos();
	header.iNumMaterials = pIgame->GetRootMaterialCount();
	fwrite(&header.iNumMaterials, sizeof(int), 1, stream);
	

	for(int i=0; i<header.iNumMaterials; i++)
	{
		IGameMaterial * mat = pIgame->GetRootMaterial(i);
		WriteMaterial(mat);
		WriteTextures(mat);
	}

	//export skeletal mesh data
	int numMod = obj->GetNumModifiers();
	for(int i=0;i<numMod;i++)
	{
		IGameModifier * m = obj->GetIGameModifier(i);
		if (m->IsSkin())
		{
			IGameSkin *skin = (IGameSkin*)m;
			WriteSkin(skin, header);
		}
	}

	//update the file header...
	fseek(stream, 0, SEEK_SET);
	fwrite(&header, sizeof(GodzExporterHeader), 1, stream);

	fclose(stream);
	pIgame->ReleaseIGame();
	return true;
}

bool GODZ::ExportSelNode(const TCHAR *szFilename)
{
	SkinExporter export(4);
	return export.SaveGameScene(szFilename);
}