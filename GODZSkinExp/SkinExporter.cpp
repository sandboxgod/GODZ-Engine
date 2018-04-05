
#include "GODZSkinExp.h"
#include "SkinExporter.h"
#include "decomp.h"
#include "CS/PhyExp.h"
#include "CS/BipExp.h"
#include "CS/Tracks.h"
#include "CS/KeyTrack.h"

#include "IGame/IGame.h"
#include "IGame/IGameObject.h"
#include "IGame/IGameProperty.h"
#include "IGame/IGameControl.h"
#include "IGame/IGameModifier.h"
#include "IGame/IConversionManager.h"
#include "Control.h"
#include "simpobj.h"

using namespace GODZ;

//checks the vector for a value
template <class T> bool ContainsValue(std::vector<T> &list, T value)
{
	for (size_t i=0; i< list.size(); i++)
	{
		if (value == list[i])
		{
			return true;
		}
	}

	return 0;
}


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

SkinExporter::SkinExporter(int framesPerSample, Interface *pInterface)
:pIgame(NULL), 
stream(NULL), 
TheManager(NULL)
{
	ZeroMemory(&header, sizeof(GodzExporterHeader));
	header.uMagic=SCENE_EXPORT_MAGIC;
	header.uVersion=SCENE_EXPORT_VERSION;
	header.framePerSample=framesPerSample;
	Interface * ip = GetCOREInterface();
	TheManager = ip->Log();
	this->pInterface=pInterface;
}

//Returns the index of the bone. if not stored, it is added to the bones list.
int SkinExporter::AssignBone(INode* node, bool bAssignToList)
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

		index++;
	}

	if (bAssignToList)
	{
		vBones.push_back(node);
		return static_cast<int>(vBones.size()-1);
	}

	return -1;
}

void SkinExporter::DumpMatrix(const Matrix3 &m)
{
	AffineParts ap;
	float scaleAxAngle;
	Point3 scaleAxis;
	//Matrix3 m = tm;
	
	decomp_affine(m, &ap);

	//write position vector
	fwrite(&ap.t.x, sizeof(float), 1, stream);
	fwrite(&ap.t.y, sizeof(float), 1, stream);
	fwrite(&ap.t.z, sizeof(float), 1, stream);

	//write rotation - Quaternions are dumped directly
	fwrite(&ap.q.x, sizeof(float), 1, stream);
	fwrite(&ap.q.y, sizeof(float), 1, stream);
	fwrite(&ap.q.z, sizeof(float), 1, stream);
	fwrite(&ap.q.w, sizeof(float), 1, stream);

	//write scale vector
	AngAxisFromQ(ap.u, &scaleAxAngle, scaleAxis);
	fwrite(&ap.k.x, sizeof(float), 1, stream);
	fwrite(&ap.k.y, sizeof(float), 1, stream);
	fwrite(&ap.k.z, sizeof(float), 1, stream);

	//yaw pitch roll - export euler angles
	Matrix3 rot(1);
	memcpy(&rot, &m, sizeof(Matrix3));
	float y,p,r;
	rot.GetYawPitchRoll(&y,&p,&r);
	fwrite(&y, sizeof(float), 1, stream);
	fwrite(&p, sizeof(float), 1, stream);
	fwrite(&r, sizeof(float), 1, stream);

	//write scale axis
	//fwrite(&scaleAxis.x, sizeof(float), 1, stream);
	//fwrite(&scaleAxis.y, sizeof(float), 1, stream);
	//fwrite(&scaleAxis.z, sizeof(float), 1, stream);
}

void SkinExporter::DumpBezierKeys(IGameControl * sc, IGameNode *node)
{
	INode *maxnode = node->GetMaxNode();
	INode *parent = maxnode->GetParentNode();

	//GODZLog::Log("maxnode %d parent %d\n", (maxnode!=0), (parent!=0));
	IGameNode *parentGameNode = pIgame->GetIGameNode(parent);
	//GODZLog::Log("parentGameNode %d\n", (parentGameNode!=0));
	IGameObject* obj = parentGameNode->GetIGameObject();

	//GODZLog::Log("obj %d\n", (obj!=0));
	obj->InitializeData();
	IGameObject* gameObj = node->GetIGameObject();
	gameObj->InitializeData();
	
	
	GODZLog::Log("output BezierKeys\n");
	
	int count = 0;

	IGameKeyTab keys;
	bool bReadKeys = (sc->GetBezierKeys(keys,IGAME_POS));

	if(bReadKeys)
		count = keys.Count();

	//output # of poskeys
	fwrite(&count, sizeof(int), 1, stream);

	if (bReadKeys)
	{
		for(int i=0;i<keys.Count();i++)
		{
			int keyTime = keys[i].t/header.iSceneTicks;
			fwrite(&keyTime, sizeof(int), 1, stream);

			Point3 pt = keys[i].bezierKey.pval;
			WritePoint3(pt);
		}
	}

	IGameKeyTab rotkeys;
	bReadKeys = (sc->GetTCBKeys(rotkeys,IGAME_ROT));

	if (bReadKeys)
	{
		count = rotkeys.Count();
	}
	else
	{
		count = 0;
	}

	//output # of rotkeys
	fwrite(&count, sizeof(int), 1, stream);

	if (bReadKeys)
	{
		for(int i=0;i<rotkeys.Count();i++)
		{
			int keyTime = rotkeys[i].t/header.iSceneTicks;
			fwrite(&keyTime, sizeof(int), 1, stream);

			AngAxis a = rotkeys[i].tcbKey.aval;
			Quat q(a);
			//Quat q = rotkeys[i].tcbKey.qval;

			//write rotation - Quaternions are dumped directly
			fwrite(&q.x, sizeof(float), 1, stream);
			fwrite(&q.y, sizeof(float), 1, stream);
			fwrite(&q.z, sizeof(float), 1, stream);
			fwrite(&q.w, sizeof(float), 1, stream);
		}
	}
	

	/*
	sc->GetBezierKeys(keys,IGAME_POS);

	//write pos keys
	int keyCount = keys.Count();
	fwrite(&keyCount, sizeof(int), 1, stream);

//	int iStart			= sc->GetKeyTime( 0 );
	//for(int i = 0;i<keyCount;i++)

	//Control* maxControl = sc->GetMaxControl(IGAME_POS);
	Interval range = maxControl->GetTimeRange(TIMERANGE_ALL);
	Interval wvalid = FOREVER;

	GODZLog::Log("range %d/%d duration:%d\n", range.Start(), range.End(), range.Duration());
	//GODZLog::Log("range %d/%d duration:%d\n", wvalid.Start(), wvalid.End(), wvalid.Duration());

	for(int i = 0;i<keyCount;i++)
	{
		int time = keys[i].t/header.iSceneTicks;
		fwrite(&time, sizeof(int), 1, stream); //time
		//fwrite(&i, sizeof(int), 1, stream); //time	

		//GODZLog::Log("bezier key %d\n", time);
		GMatrix localTM = node->GetLocalTM( keys[i].t ); //keys[i].t
		DumpMatrix(localTM.ExtractMatrix3());
		//Matrix3 nodeTM = maxnode->GetNodeTM( keys[i].t );
		//Matrix3 relativeTM = nodeTM * Inverse ( parent->GetNodeTM( keys[i].t ));
		//DumpMatrix(relativeTM);
	}
	*/

	/*
	int fps = 5;
	if(sc->GetFullSampledKeys(keys,fps,IGameControlType(IGAME_TM), true) )	//header.framePerSample
	{ 
		int count = keys.Count();
		fwrite(&count,sizeof(int),1,stream);

		GODZLog::Log("full sample --> %d\n",count);
		for(int i=0;i<count;i++)
		{
			int time = keys[i].t/header.iSceneTicks;
			fwrite(&time, sizeof(int), 1, stream); //time

			//DumpMatrix(keys[i].sampleKey.gval.ExtractMatrix3());

			//GMatrix localTM = node->GetLocalTM( keys[i].t ); //keys[i].t
			//DumpMatrix(localTM.ExtractMatrix3());


			//dump in max coords
			Matrix3 parentTM = parent->GetNodeTM( keys[i].t );
			Matrix3 nodeTM = maxnode->GetNodeTM( keys[i].t );

			if (maxnode->GetParentNode())
			{
				maxnode->GetParentNode()->EvalWorldState(0);
			}

			if (nodeTM == parentTM)
			{
				GODZLog::Log("Anim bone - identity found!");
				Matrix3 identity(1);
				DumpMatrix(identity);
			}
			else
				DumpMatrix(keys[i].sampleKey.gval.ExtractMatrix3());

			//Matrix3 relativeTM = nodeTM * Inverse ( parent->GetNodeTM( keys[i].t ));
			//DumpMatrix(relativeTM);
		}
	}
	*/

	/*
	//write quat keys - use TCB Controller
	sc->GetTCBKeys(keys,IGAME_ROT);

	keyCount = keys.Count();
	fwrite(&keyCount, sizeof(int), 1, stream);

	for(int i = 0;i<keyCount;i++)
	{
		int time = keys[i].t/header.iSceneTicks;
		fwrite(&time, sizeof(int), 1, stream); //time
		Quat q = keys[i].tcbKey.aval;
		//AngAxis a(q);

		//write rotation - Quaternions are dumped directly
		fwrite(&q.x, sizeof(float), 1, stream);
		fwrite(&q.y, sizeof(float), 1, stream);
		fwrite(&q.z, sizeof(float), 1, stream);
		fwrite(&q.w, sizeof(float), 1, stream);
		

		//dump matrix
		GODZLog::Log("bezier key %d\n", time);
		GMatrix localTM = node->GetLocalTM( time ); //keys[i].t
		DumpMatrix(localTM.ExtractMatrix3());
	}*/
}

bool SkinExporter::DumpSampleKeys(IGameControl * sc, IGameNode *node)
{
	
	IGameControl * c = sc;
	bool bSampleKeys=true;
	

	//dump node transformation			
	INode *maxnode = node->GetMaxNode();
	INode *parent = maxnode->GetParentNode();

	//GODZLog::Log("maxnode %d parent %d\n", (maxnode!=0), (parent!=0));
	IGameNode *parentGameNode = 0;
	parentGameNode = pIgame->GetIGameNode(parent);
	
	if (parentGameNode)
	{		
		//GODZLog::Log("parentGameNode %d\n", (parentGameNode!=0));
		IGameObject* obj = parentGameNode->GetIGameObject();

		//GODZLog::Log("obj %d\n", (obj!=0));
		obj->InitializeData();
	}
	
	//GODZLog::Log("before IGameKeyTab\n");
	//IGameKeyTab keys;

	//sample the keys, get max time
	/*int maxTime=0;
	if (c->GetQuickSampledKeys(keys,IGameControlType(IGAME_POS)))
	{
		for (int i=0; i<keys.Count();i++)
		{
			maxTime = keys[i].t;
		}
	}

	if (keys.Count() == 0)
	{
		bSampleKeys=0;
		fwrite(&bSampleKeys, sizeof(bool), 1, stream);
		GODZLog::Log("Warning: No sample keys\n");
		return 0;
	}*/

	//output sample keys flag
	fwrite(&bSampleKeys, sizeof(bool), 1, stream);

	
	Control* maxControl = sc->GetMaxControl(IGAME_ROT);
	Interval range = maxControl->GetTimeRange(TIMERANGE_ALL);
	TimeValue duration = range.End() - range.Start();
	//duration often equals zero....
    DWORD cTicksPerFrame = GetTicksPerFrame();
    DWORD cFrameRate = GetFrameRate(); 
	

	IGameKeyTab Key;
	bool bRelative=true;
	//header.iSceneTicks = GetTicksPerFrame() - max API
	int fps = header.iSceneTicks / cFrameRate;
	if (cFrameRate > header.iSceneTicks)
	{
		fps = 1; //we can't pass zero into GetFullSampledKeys()
	}

	//int fps = duration / (header.iSceneTicks); //* keys.Count());
	GODZLog::Log("FPS: %d\n", fps);
	if(c->GetFullSampledKeys(Key,fps,IGameControlType(IGAME_TM), bRelative) )	//header.framePerSample
	{ 
		int count = Key.Count();
		fwrite(&count,sizeof(int),1,stream);

		GODZLog::Log("sampled keys: %d\n", count);

		for(int i=0;i<count;i++)
		{
			int fc = Key[i].t/header.iSceneTicks;
			fwrite(&fc,sizeof(int),1,stream);

			//Dump local matrix
			DumpMatrix(Key[i].sampleKey.gval.ExtractMatrix3());
			
			
			//GMatrix localTM = node->GetLocalTM( Key[i].t );
			//DumpMatrix(localTM.ExtractMatrix3());

			//not used by importer
			//GMatrix objectTM = node->GetNodeTM( Key[i].t );
			//DumpMatrix(objectTM.ExtractMatrix3());
			Matrix3 currTM = maxnode->GetNodeTM( Key[i].t );
			DumpMatrix(currTM);

			//not used by importer
			//GMatrix worldTM = node->GetObjectTM( Key[i].t );
			//DumpMatrix(worldTM.ExtractMatrix3());						
			//}

			IGameControl::MaxControlType T = sc->GetControlType(IGAME_POS);

			//write bool out if there is sample keys...	
			if (parentGameNode)
			{
				//Get the matrix relative to the parent bone (test)
				Matrix3 nodeTM = maxnode->GetNodeTM( Key[i].t );
				Matrix3 parentTM = parent->GetNodeTM( Key[i].t );

				Matrix3 relativeTM = nodeTM * Inverse ( parentTM);
				DumpMatrix(relativeTM);
			}
			else
			{

				//Get the matrix relative to the parent bone (test)
				Matrix3 nodeTM = maxnode->GetNodeTM( Key[i].t );
				//Matrix3 parentTM = parent->GetNodeTM( Key[i].t );

				//Matrix3 relativeTM = nodeTM * Inverse ( parentTM);
				DumpMatrix(nodeTM);
			}
		}
	}

	//GODZLog::Log("after GetFullSampledKeys\n");


	/*
	IGameKeyTab PosKeys;
	IGameKeyTab RotKeys;
	IGameKeyTab ScaleKeys;

	//write rotation data
	if (!c->GetQuickSampledKeys(RotKeys,IGameControlType(IGAME_ROT)))
	{
		//throw error
		return;
	}

	//write position data
	if (!c->GetQuickSampledKeys(PosKeys,IGameControlType(IGAME_POS)))
	{
		//throw error
		return;
	}

	//write scale data
	if (!c->GetQuickSampledKeys(ScaleKeys,IGameControlType(IGAME_SCALE)))
	{
		//throw error
		return;
	}

	if (PosKeys.Count() != RotKeys.Count() || PosKeys.Count() != ScaleKeys.Count())
	{
		//throw error
		return;
	}

	int count = PosKeys.Count();
	fwrite(&count,sizeof(int),1,stream);

	for(int i=0;i<PosKeys.Count();i++)
	{
		int fc = PosKeys[i].t/header.iSceneTicks;
		fwrite(&fc,sizeof(int),1,stream);			
			
		//dump pos/rot/scale data
		fwrite(&PosKeys[i].sampleKey.pval.x,sizeof(float),1,stream);	
		fwrite(&PosKeys[i].sampleKey.pval.y,sizeof(float),1,stream);	
		fwrite(&PosKeys[i].sampleKey.pval.z,sizeof(float),1,stream);	

		fwrite(&PosKeys[i].sampleKey.qval.x,sizeof(float),1,stream);	
		fwrite(&PosKeys[i].sampleKey.qval.y,sizeof(float),1,stream);	
		fwrite(&PosKeys[i].sampleKey.qval.z,sizeof(float),1,stream);	
		fwrite(&PosKeys[i].sampleKey.qval.w,sizeof(float),1,stream);	

		fwrite(&PosKeys[i].sampleKey.sval.s.x,sizeof(float),1,stream);	
		fwrite(&PosKeys[i].sampleKey.sval.s.y,sizeof(float),1,stream);	
		fwrite(&PosKeys[i].sampleKey.sval.s.z,sizeof(float),1,stream);	
	}*/

	return true;
}

fpos_t SkinExporter::GetSeekPos()
{
	fpos_t off;
	fgetpos(stream,&off);
	return off;
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


void SkinExporter::WriteBoneKeys(IGameNode *node)
{
	IGameControl *pGameControl = node->GetIGameControl();
	IGameControl::MaxControlType T = pGameControl->GetControlType(IGAME_POS);

	//write bool out if there is sample keys...	
	/*if (T != IGameControl::MaxControlType::IGAME_BIPED)
	{
		//TheManager->LogEntry(SYSLOG_ERROR, DISPLAY_DIALOG, "Error occured wrting bone anim data", _T("%s - %s\n"), "Unedxpected Error");
		bool bSampleKeys=0;
		fwrite(&bSampleKeys, sizeof(bool), 1, stream);
		GODZLog::Log("Warning: No sample keys\n");
	}
	else	
	{
		bool bSampleKeys=true;
		fwrite(&bSampleKeys, sizeof(bool), 1, stream);
		DumpSampleKeys(pGameControl,node);
	}
	*/

	INode *maxnode = node->GetMaxNode();
	INode *parent = maxnode->GetParentNode();
	IGameNode *parentGameNode = 0;
	
	if (parent)
	{
		parentGameNode = pIgame->GetIGameNode(parent);
	}
	else
	{
		_asm nop;
	}

	//new code - don't write out sample keys for root bone - Bip01
	GODZLog::Log("node id %d MaxControlType %d\n", node->GetNodeID(), (int)T);

	/*
	if (!parentGameNode)
	{
		int animType = (int)EA_None;
		fwrite(&animType, sizeof(int), 1, stream);
		GODZLog::Log("Warning: No sample keys\n");
	}
	else	
	{*/

		/*if (T != IGameControl::MaxControlType::IGAME_BIPED)
		{
			//dummy box most likely - they are not fully sampled.
			GODZLog::Log("Writing Bezeir keys\n");
			int animType = (int)EA_BezierKeys;
			fwrite(&animType, sizeof(int), 1, stream);
			DumpBezierKeys(pGameControl,node);
		}
		else
		*/



		/*
		if (T == IGameControl::MaxControlType::IGAME_MAXSTD)
		{
			int animType = (int)EA_BezierKeys;
			fwrite(&animType, sizeof(int), 1, stream);

			//bezier keys for IGAME_MAXSTD? These keys aren't fully sampled...
			DumpBezierKeys(pGameControl,node);
		}
		else
		*/
		{
			int animType = (int)EA_FullSampleKeys;
			fwrite(&animType, sizeof(int), 1, stream);

			//biped bones come fully sampled
			DumpSampleKeys(pGameControl,node);
		}
	//}

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



void SkinExporter::WriteMatrix(GMatrix &matrix)
{
	for(int i=0;i<4;i++)
	{
		Point4 p = matrix.GetRow(i);
		WritePoint4(p);
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

//Save any shapes attached to a node
void SkinExporter::SaveCollisionVolumes(IGameNode* pGameNode)
{
	INode* parentNode = pGameNode->GetMaxNode();

	fpos_t number_collision_models_offset;
	fgetpos(stream,&number_collision_models_offset);

	int collisionModelCount = 0;
	fwrite(&collisionModelCount,sizeof(int),1,stream);

	Class_ID sphereClass = Class_ID(SPHERE_CLASS_ID,0);
	Class_ID boxClass = Class_ID(BOXOBJ_CLASS_ID,0);

	int children = pGameNode->GetChildCount();
	for(int i=0;i<children;i++)
	{
		//output child node info - SPHERE_CLASS_ID
		IGameNode* pNode = pGameNode->GetNodeChild(i);

		INode* maxNode = pNode->GetMaxNode();
		Object* pObj = maxNode->GetObjectRef();
		ObjectState os = pObj->Eval(0);

		/*
		//Does this shape qualify?
		if (!os.obj->IsSubClassOf(sphereClass) && !os.obj->IsSubClassOf(boxClass)
			)
		{
			continue;
		}
		*/
		
		GODZLog::Log("Collision shape: %s\n",pNode->GetName());
		int shapeType = ShapeType::ShapeType_Mesh;

		if (os.obj->IsSubClassOf(boxClass))
		{
			shapeType = ShapeType::ShapeType_Box;
		}
		else if (os.obj->IsSubClassOf(sphereClass))
		{
			shapeType = ShapeType::ShapeType_Sphere;
		}

		fwrite(&shapeType,sizeof(int),1,stream);

		WriteText(pNode->GetName());

		//pivot tm - offset transformation matrix
		Matrix3 tm(1);
		Point3 pos = maxNode->GetObjOffsetPos();
		tm.PreTranslate(pos);

		Quat quat = maxNode->GetObjOffsetRot();
		PreRotateMatrix(tm, quat);
		ScaleValue scaleValue = maxNode->GetObjOffsetScale();
		ApplyScaling(tm, scaleValue);
		DumpMatrix(tm);

		int motionType = 0; // MotionType_Static
		MSTR motionTypeName = GetMotionKey();
		maxNode->GetUserPropInt(motionTypeName, motionType);
		fwrite(&motionType,sizeof(int),1,stream);
		GODZLog::Log("Motion: %d\n", motionType );

		collisionModelCount++;

		if (os.obj->IsSubClassOf(sphereClass))
		{
			GODZLog::Log("Sphere shape: %s\n",pNode->GetName());
			GenSphere* sphere = static_cast<GenSphere*>(pObj);

			//output the radius
			//GODZLog::Log("param: %s\n", sphere->GetParameterName(0) ); //radius
			//GODZLog::Log("param: %s\n", sphere->GetParameterName(1) ); //segments
			//GODZLog::Log("param: %s\n", sphere->GetParameterName(2) ); //smooth
			float radius;
			Interval evar = FOREVER;
			sphere->pblock->GetValue(0, 0, radius, evar);
			GODZLog::Log("rad: %f\n", radius );

			fwrite(&radius,sizeof(float),1,stream);

			//Get the matrix relative to the parent bone
			Matrix3 nodeTM = maxNode->GetNodeTM( 0 );
			Matrix3 parentTM = parentNode->GetNodeTM( 0 );

			Matrix3 relativeTM = nodeTM * Inverse(parentTM);
			DumpMatrix(relativeTM);

			//Matrix3 objectTM = maxNode->GetObjectTM(0); //world matrix
			//DumpMatrix(pGameNode->GetWorldTM(0).ExtractMatrix3());
		}
		else if (os.obj->IsSubClassOf(boxClass))
		{
			float width,height,length;
			Interval evar = FOREVER;
			GenBoxObject* box = static_cast<GenBoxObject*>(pObj);
			box->pblock->GetValue(0,0,width,evar);
			box->pblock->GetValue(1,0,height,evar);
			box->pblock->GetValue(2,0,length,evar);

			fwrite(&width,sizeof(float),1,stream);
			fwrite(&height,sizeof(float),1,stream);
			fwrite(&length,sizeof(float),1,stream);

			//Get the matrix relative to the parent bone
			Matrix3 nodeTM = maxNode->GetNodeTM( 0 );
			Matrix3 parentTM = parentNode->GetNodeTM( 0 );

			Matrix3 relativeTM = nodeTM * Inverse(parentTM);
			DumpMatrix(relativeTM);
		}
		else //Mesh
		{
			IGameObject* obj = pGameNode->GetIGameObject();

			if(obj->GetIGameType()!=IGameObject::IGAME_MESH)
			{
				IGameMesh* gm = (IGameMesh*)obj;

				/*
				//Write Normals
				int numNormals = gm->GetNumberOfNormals();

				fwrite(&numNormals,sizeof(int),1,stream);
				for(int i = 0;i < numNormals;i++)
				{
					Point3 n = gm->GetNormal(i);
					WritePoint3(n);
				}
				*/

				//Write Vertices
				int numVerts = gm->GetNumberOfVerts();
				fwrite(&numVerts,sizeof(int),1,stream);

				for (int i=0; i< numVerts; i++)
				{
					Point3 v = gm->GetVertex(i);
					WritePoint3(v);
				}

				//Write Faces
				int numFaces = gm->GetNumberOfFaces();
				fwrite(&numFaces,sizeof(int),1,stream);

				for(int i = 0;i < numFaces;i++)
				{
					FaceEx * f = gm->GetFace(i);
					
					/*
					//write face normal
					for(int i=0;i<3;i++)
					{
						fwrite(&f->norm[i], sizeof(DWORD), 1, stream);
					}
					*/

					//write vertex indicies
					for(int i=0;i<3;i++)
					{
						fwrite(&f->vert[i], sizeof(DWORD), 1, stream);
					}
				}
			}
		}
	} // loop children


	//backup offset
	fpos_t offset;
	fgetpos(stream,&offset);

	//write out true model collision count
	fseek(stream, number_collision_models_offset, SEEK_SET);
	fwrite(&collisionModelCount,sizeof(int),1,stream);

	//reset
	fseek(stream, offset, SEEK_SET);
}

void SkinExporter::WriteSkin(IGameSkin* skin, MeshInfo& meshInfo)
{
	meshInfo.initialSkinOffset = GetSeekPos();
	GMatrix initialNodeTM;
	skin->GetInitSkinTM(initialNodeTM);
	DumpMatrix(initialNodeTM.ExtractMatrix3());

	if(skin->GetSkinType()== IGameSkin::IGAME_PHYSIQUE)
	{
		meshInfo.iNumSkinVerts = skin->GetNumOfSkinnedVerts();
		meshInfo.skinVertOffset=GetSeekPos();
		for(int i=0; i<meshInfo.iNumSkinVerts;i++)
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

	GODZLog::Log("Skin Vertices\n");

	pInterface->ProgressUpdate(50); //, true, "Building Hierarchy"
	float update = 50;
	meshInfo.iNumBones=vBones.size();

	//now that all the skinned vertices have been written, write bones
	std::vector<INode*>::iterator boneIter;

	GODZLog::Log("sizeof TCHAR: %d\n", sizeof(TCHAR));

	//bones not in the list
	std::vector<INode*> extraBones;
	GODZLog::Log("num bones %d\n",meshInfo.iNumBones);

	//new code - build hierarchy for the skeleton -
	for (boneIter=vBones.begin();boneIter!=vBones.end();boneIter++)
	{
		INode* node = *boneIter;
		GODZLog::Log("hierarchy node %s\n",node->GetName());
		IGameNode *pGameNode = pIgame->GetIGameNode(node);
		IGameObject* obj = pGameNode->GetIGameObject();
		obj->InitializeData();

		INode *parent = node->GetParentNode();
		while (parent)
		{
			int parentId = AssignBone(parent,false);
			if (parentId == -1 && !ContainsValue<INode*>(extraBones, parent) )
			{
				extraBones.push_back(parent);
				parent = parent->GetParentNode();
			}
			else
			{
				break;
			}
		}
	}

	size_t size = extraBones.size();
	GODZLog::Log("extra  %d\n", size);
	for(size_t i=0;i<size;i++)
	{
		GODZLog::Log("Extra Bone: %s\n", extraBones[i]->GetName());
		//the scene root (Bip01's parent) has no parent - let's not add the scene root
		if (extraBones[i]->GetParentNode())
		{
			vBones.push_back(extraBones[i]);
		}
	}

	meshInfo.iNumBones = vBones.size();
	pInterface->ProgressUpdate(60); //, true, "Exporting Bone Data"

	int count = 0;
	//output the skeleton!
	meshInfo.skeletonOffset=GetSeekPos();
	for (boneIter=vBones.begin();boneIter!=vBones.end();boneIter++)
	{
		INode *node = *boneIter;
		WriteText(node->GetName());
		TCHAR* text = node->GetName();
		GODZLog::Log("bone name %s\n", node->GetName());

		//IGameScene *subIgame = GetIGameInterface();
		//pIgame->InitialiseIGame(node,false);
		IGameNode *pGameNode = pIgame->GetIGameNode(node);
		IGameObject* obj = pGameNode->GetIGameObject();
		obj->InitializeData();

		//write parent ID
		INode *parent = node->GetParentNode();
		int parentId=-1;
		if (parent)
		{
			parentId=AssignBone(parent,false);
		}
		fwrite(&parentId,sizeof(int),1,stream);

		//write bounding box
		WriteBBox(obj);

		/*
		//write bone type? MAXSTD? Biped?
		IGameControl *pGameControl = pGameNode->GetIGameControl();
		IGameControl::MaxControlType T = pGameControl->GetControlType(IGAME_POS);
		int type = (int)T;
		fwrite(&type,sizeof(int),1,stream);
		*/

		//write postion & orientation for skeleton using IGame. I have noticed
		//that IGame 1.22 for max5 does not output correct init TM for non-biped
		//bones. so you might want to ignore this and use upcoming matrix
		GMatrix tm;
		skin->GetInitBoneTM(pGameNode, tm);
		DumpMatrix(tm.ExtractMatrix3());

		//bypass IGame and output the raw matrix. however, you will have to convert
		//this matrix to Left hand coord system yerself...
		Matrix3 parentTM = parent->GetNodeTM( 0 );
		INode *thisNode = pGameNode->GetMaxNode();
		Matrix3 nodeTM = thisNode->GetNodeTM( 0 );

		if (thisNode->GetParentNode())
		{
			thisNode->GetParentNode()->EvalWorldState(0);
		}


		DumpMatrix(nodeTM);

		//Output the number of attached Collision Models (these can be used for
		//bone collision detection)
		SaveCollisionVolumes(pGameNode);

		count++;
	}

	pInterface->ProgressUpdate(70);//, true, "Exporting Anim Data"
	GODZLog::Log("\n=========================================\n");
	//output the animated bones.....
	meshInfo.boneOffset=GetSeekPos();	
	count=0;
	for (boneIter=vBones.begin();boneIter!=vBones.end();boneIter++)
	{
		float progressValue = (float)count / (float)meshInfo.iNumBones;
		progressValue *= 30; //30 is the remaining time we have on the progress bar
		pInterface->ProgressUpdate(70 + progressValue);
		INode *node = *boneIter;
		WriteText(node->GetName());
		GODZLog::Log("bone %s %f\n", node->GetName(), progressValue);

		//store animation data for this bone
		IGameNode *pGameNode = pIgame->GetIGameNode(node);
		IGameObject* obj = pGameNode->GetIGameObject();
		obj->InitializeData();
		WriteBoneKeys(pGameNode);
		count++;
	}
}

void SkinExporter::WriteSkinnedVertex(IGameSkin *skin, int index)
{
	//write number of bones affecting this vertex
	int numBones=skin->GetNumberOfBones(index);
	fwrite(&numBones,sizeof(int),1,stream);

	for(int j=0;j<numBones;j++)
	{
		//write out weight (we will renormalize the bones)
		float weight = skin->GetWeight(index,j);
		fwrite(&weight,sizeof(float),1,stream);

		//write out bone ID
		INode* node = skin->GetBone(index,j);
		if (!node)
		{
			GODZLog::Log("WARNING: No node!\n");
		}
		int id = AssignBone(node);
		fwrite(&id,sizeof(int),1,stream);
	}
}

void SkinExporter::WriteText(TCHAR *text)
{
	//size_t debugsize = sizeof(TCHAR);
	size_t len = strlen(text);
	fwrite(&len, sizeof(size_t), 1, stream);
	fwrite(text, sizeof(TCHAR), len, stream);
}

void SkinExporter::WriteMaterial(IGameMaterial *material)
{
	TCHAR* text = material->GetMaterialName();
	WriteText(text);

	//write isMulti
	bool bMulti = material->IsMultiType();
	fwrite(&bMulti,sizeof(bool),1,stream);

	if (!bMulti)
	{
		//write out properties
		IGameProperty* ambient = material->GetAmbientData();
		IGameProperty* diffuse = material->GetDiffuseData();
		IGameProperty* spec    = material->GetSpecularData();
		IGameProperty* emissive= material->GetEmissiveData();
		IGameProperty* opacity= material->GetOpacityData();
		IGameProperty* specLvl= material->GetSpecularLevelData();

		Point3 p;
		bool bOk = ambient->GetPropertyValue(p);
		GODZLog::Log("ambient %f %f %f\n",p.x,p.y,p.z);
		WritePoint3(p);

		bOk = diffuse->GetPropertyValue(p);
		GODZLog::Log("diffuse %f %f %f\n",p.x,p.y,p.z);
		WritePoint3(p);
		
		bOk = spec->GetPropertyValue(p);
		GODZLog::Log("specular %f %f %f\n",p.x,p.y,p.z);
		WritePoint3(p);

		bOk = emissive->GetPropertyValue(p);
		GODZLog::Log("emissive %f %f %f\n",p.x,p.y,p.z);
		WritePoint3(p);

		float f;
		bOk = opacity->GetPropertyValue(f);
		GODZLog::Log("opacity %f\n",f);
		fwrite(&f,sizeof(float),1,stream);

		bOk = specLvl->GetPropertyValue(f);
		GODZLog::Log("specLvl %f\n",f);
		fwrite(&f,sizeof(float),1,stream);

		WriteTextures(material);
	}

	

	int subs = material->GetSubMaterialCount();
	fwrite(&subs,sizeof(int),1,stream);

	//write subs
	for(int j=0;j<subs;j++)
	{
		IGameMaterial *subMat = material->GetSubMaterial(j);
		WriteMaterial(subMat);
	}
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

		//write out which slot this texture occupies:
		/*
			#define ID_AM 0   // ambient
			#define ID_DI 1   // diffuse
			#define ID_SP 2   // specular
			#define ID_SH 3   // shininesNs
			#define ID_SS 4   // shininess strength
			#define ID_SI 5   // self-illumination
			#define ID_OP 6   // opacity
			#define ID_FI 7   // filter color
			#define ID_BU 8   // bump 
			#define ID_RL 9   // reflection
			#define ID_RR 10  // refraction 
			#define ID_DP 11  // displacement
		*/
		int matSlot = tex->GetStdMapSlot();
		fwrite(&matSlot,sizeof(int),1,stream);
	}


}

void SkinExporter::SaveGameNode(IGameNode* pGameNode)
{
	INode* maxNode = pGameNode->GetMaxNode();

	if (maxNode->UserPropExists( GetMotionKey() ))
	{
		//This isn't a mesh; it's a collision node!
		return;
	}

	IGameObject* obj= pGameNode->GetIGameObject();
	if(obj->GetIGameType()!=IGameObject::IGAME_MESH)
	{
		return;
	}

	IGameMesh * gm = (IGameMesh*)obj;
	gm->SetCreateOptimizedNormalList();
	if (!gm->InitializeData())
		return;

	header.numObjects++;

	MeshInfo meshInfo;
	ZeroMemory(&meshInfo, sizeof(MeshInfo));

	fpos_t meshInfoOffset = GetSeekPos();		//position where mesh info def will start

	fwrite(&meshInfo,sizeof(MeshInfo),1,stream);

	//Save out shapes that are parented by the root node
	SaveCollisionVolumes(pGameNode);

	Mesh* pMesh = gm->GetMaxMesh();

	meshInfo.iNumVertices=gm->GetNumberOfVerts();  //8 verts for a simple texture mapped box
	meshInfo.iNumFaces=gm->GetNumberOfFaces();     //8 ....
	meshInfo.iNumTVerts=gm->GetNumberOfTexVerts(); //12 ....
	meshInfo.iNumNormals=gm->GetNumberOfNormals(); //24 ....

	GODZLog::Log("iNumNormals %d\n", meshInfo.iNumNormals);

	//write out vertex normals
	meshInfo.normOffset=GetSeekPos();
	for(int i = 0;i<meshInfo.iNumNormals;i++)
	{
		Point3 n = gm->GetNormal(i);
		WritePoint3(n);
	}

	//write out faces
	meshInfo.facesOffset=GetSeekPos();
	int numFaces = meshInfo.iNumFaces;
	for(int i = 0;i<numFaces;i++)
	{
		FaceEx * f = gm->GetFace(i);
		//DebugPrint("Normals %d %d %d\n",f->norm[0],f->norm[1],f->norm[2]);
		WriteFaceEx(f,gm,i);
	}

	//write out vertices
	meshInfo.vertsOffset=GetSeekPos();
	for (int i=0;i<meshInfo.iNumVertices;i++)
	{
		Point3 v = gm->GetVertex(i);
		WritePoint3(v);
	}

	//write out texture vertices
	meshInfo.tvertsOffset=GetSeekPos();
	for (int i=0;i<meshInfo.iNumTVerts;i++)
	{
		Point2 tv = gm->GetTexVertex(i);
		WritePoint2(tv);
	}


	meshInfo.nameOffset=GetSeekPos();
	TCHAR* text = pGameNode->GetName();
	WriteText(text);

	pInterface->ProgressUpdate(30); //, true, "Exporting Skin Data"

	//export skeletal mesh data
	int numMod = obj->GetNumModifiers();
	for(int i=0;i<numMod;i++)
	{
		IGameModifier* m = obj->GetIGameModifier(i);
		if (m->IsSkin())
		{
			IGameSkin* skin = (IGameSkin*)m;
			WriteSkin(skin, meshInfo);
		}
	}

	//backup file ptr
	fpos_t ptr = GetSeekPos();

	fseek(stream, meshInfoOffset, SEEK_SET);
	fwrite(&meshInfo,sizeof(MeshInfo),1,stream);

	fseek(stream, ptr, SEEK_SET);
}

bool SkinExporter::SaveGameScene(const TCHAR *szFilename)
{
	Interface* coreExport = GetCOREInterface();

	pIgame = GetIGameInterface();
	pIgame->InitialiseIGame(false);
	pIgame->SetStaticFrame(0);

	int numnodes=pIgame->GetTopLevelNodeCount();
	if (numnodes == 0)
	{
		return false;
	}

	if( (stream  = fopen( szFilename, "wb+" )) == NULL )
	{
		return false;
	}

	header.iSceneTicks = pIgame->GetSceneTicks();

	IGameConversionManager * cm = GetConversionManager();
	cm->SetUserCoordSystem(Whacky);
	cm->SetCoordSystem(IGameConversionManager::IGAME_D3D);

	fwrite(&header,sizeof(GodzExporterHeader),1,stream);

	for(int i=0;i<numnodes;i++)
	{
		IGameNode* node = pIgame->GetTopLevelNode(i);
		SaveGameNode(node);

		int numchildren = node->GetChildCount();

		for(int j=0; j<numchildren; j++)
		{
			IGameNode* childNode = node->GetNodeChild(j);
			SaveGameNode(childNode);
		}
	}

	//export materials
	header.materialOffset=GetSeekPos();
	header.iNumMaterials = pIgame->GetRootMaterialCount();
	fwrite(&header.iNumMaterials, sizeof(int), 1, stream);
	
	for(int i=0; i<header.iNumMaterials; i++)
	{
		IGameMaterial* mat = pIgame->GetRootMaterial(i);
		WriteMaterial(mat);
	}


	//update the file header...
	fseek(stream, 0, SEEK_SET);
	fwrite(&header, sizeof(GodzExporterHeader), 1, stream);

	fclose(stream);
	pIgame->ReleaseIGame();
	return true;
}

bool GODZ::ExportSelNode(const TCHAR *szFilename, int framesPerSec, Interface *pInterface)
{
	SkinExporter export(framesPerSec,pInterface);
	return export.SaveGameScene(szFilename);
}