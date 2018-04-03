
#include "GSAFileImporter.h"
#include "SkeletalMesh.h"
#include "GenericPackage.h"
#include "BoneSystem.h"
#include "Material.h"
#include "TextureResource.h"
#include "ResourceManager.h"
#include "SkelMeshInstance.h"
#include "StringTokenizer.h"
#include "Timer.h"
#include <list>
#include "Win32Interface.h"
#include "crc32.h"
#include "ShaderManager.h"
#include "TriangleList.h"
#include <CoreFramework/Animation/AnimationInstance.h>
#include <CoreFramework/Animation/SkeletonNode.h>
#include <CoreFramework/Collision/SphereCollisionPrimitive.h>
#include <CoreFramework/Collision/OBBCollisionPrimitive.h>
#include <CoreFramework/Collision/AABBTree.h>

using namespace GODZ;
using namespace std;


GSAMatrix::operator WMatrix16f()
{
	WMatrix16f tm(1);
	tm.PreTranslate(pos);
	QuaternionToMatrix(rot, tm);
	return tm;
}

void GSAMatrix::SetMatrix(WMatrix& tm)
{
	tm.PreTranslate(pos);
	QuaternionToMatrix(rot, tm);
}



////////////////////////////////////////////////////////////////////////////////////


GSAFileImporter::GSAFileImporter()
: stream(NULL)
, boneWeights(NULL)
, m_pBlendVerts(NULL)
{
	const char* defaultshader = GlobalConfig::m_pConfig->ReadNode("Shaders","DefaultShader");
	if (defaultshader != NULL)
	{
		HString htext(defaultshader);
		mDefaultShader = htext;
	}

	const char* skinshader = GlobalConfig::m_pConfig->ReadNode("Shaders","DefaultSkinShader");
	if (skinshader != NULL)
	{
		HString htext(skinshader);
		mSkinShader = htext;
	}
}

GSAFileImporter::~GSAFileImporter()
{
	if (boneWeights != NULL)
	{
		delete[] boneWeights;
		boneWeights = NULL;
	}

	if (m_pBlendVerts != NULL)
	{
		delete[] m_pBlendVerts;
		m_pBlendVerts=NULL;
	}
}

void GSAFileImporter::ConvertMaxToDX(GSAMatrix& matrix)
{
	float z = matrix.pos.z;
	matrix.pos.z = matrix.pos.y;
	matrix.pos.y = z;

	Quat4f q = matrix.rot;
	matrix.rot = Quat4f(-q.q.x,-q.q.z,-q.q.y,q.q.w);
	EulerAngle ang = matrix.euler;
	matrix.euler = EulerAngle(ang.y,ang.r,ang.p);
}

atomic_ptr<Mesh> GSAFileImporter::ImportMesh(const char* file, GenericPackage* p, GodzVector<rstring>& list)
{
	atomic_ptr<Mesh> mesh;
	atomic_ptr<MeshInstance> meshInstance;

	fileOpen(&stream, file, FileOpenMode_ReadBinary);
	if( stream == NULL )
	{
		DisplayMessage("Scene Import Error", "Cannot open file %s", file);
		return atomic_ptr<Mesh>();
	}
	
	filename=file;
	fread(&header,sizeof(GodzExporterHeader),1,stream);

	if(header.uMagic!=SCENE_EXPORT_MAGIC)
	{
		DisplayMessage("Scene Import Error", "%s is not a godz scene export file", file);
		return atomic_ptr<Mesh>();
	}

	Deprecated_GodzExporterHeader oldHeader;
	if (header.uVersion == 1)
	{
		//then it's the old, obsolete format.....
		fseek(stream, 0, SEEK_SET);
		
		fread(&oldHeader,sizeof(Deprecated_GodzExporterHeader),1,stream);

		header.numObjects = 1;
		header.iSceneTicks = oldHeader.iSceneTicks;
		header.framePerSample = oldHeader.framePerSample;
		header.materialOffset = oldHeader.materialOffset;
		header.iNumMaterials = oldHeader.iNumMaterials;
	}

	UInt32 numObjects = header.numObjects;
	for(UInt32 i=0; i < numObjects; i++)
	{
		MeshInfo meshInfo;

		if (header.uVersion > 1)
		{
			fread(&meshInfo,sizeof(MeshInfo),1,stream);
		}
		else
		{
			//then it's the old, obsolete format..... Here, all we would have to do is copy everything else
			//into the mesh....
			UpdateMeshInfo(oldHeader, meshInfo);
		}

		if (meshInfo.iNumNormals > 0)
		{
			if (i == 0) //Create the Mesh
			{
				if (meshInfo.iNumBones > 0)
				{
					SkeletalMesh* skMesh = new SkeletalMesh();
					mesh = skMesh;

					skMesh->CreateOrigBones(meshInfo.iNumBones);
					meshInstance = new SkelMeshInstance();
					skMesh->SetDefaultInstance(meshInstance);
				}
				else
				{
					mesh = new Mesh();
					meshInstance = new MeshInstance();
					mesh->SetDefaultInstance(meshInstance);
				}

				GUID meshID = mesh->CreateGUID();

				//The mesh will be ref'ed by the Package
				p->AddObject(mesh, meshID);

				GUID meshInstanceID = meshInstance->CreateGUID();
				p->AddObject(meshInstance, meshInstanceID);
				

				//Version #3+ adds support for collision shapes to be attached to meshes
				if (header.uVersion > 2)
					ReadShapes(NULL, meshInstance, list);

				//read materials -- hacky, but we need the mesh definition before reading the mats
				fseek(stream, header.materialOffset, SEEK_SET);
				int matCount;
				fread(&matCount, sizeof(int), 1, stream);
				for(int i=0; i<matCount; i++)
				{
					ReadMaterial(mesh, p, list);
				}
			}

			//we should clear the cached vertices, etc before reading the next mesh
			ReadMesh(p, mesh, meshInfo, list);
		} //if (meshInfo.iNumNormals > 0)

		if (meshInfo.iNumBones > 0)
		{
			if (boneWeights!=NULL)
			{
				delete[] boneWeights;
				boneWeights=NULL;
			}

			boneWeights = new BoneWeights[meshInfo.iNumBones];

			SkeletalMesh* skMesh = SafeCast<SkeletalMesh>(mesh);
			ReadSkin(skMesh, meshInfo, list);
		}
	}

	//chop up the mesh if it meets the criteria
	SkeletalMesh* skMesh = SafeCast<SkeletalMesh>(mesh);
	if (skMesh == NULL)
	{
		/*
		//unit test stuff for plane
		Vector3 va(-7000,0,0);
		Vector3 vb(1,0,0);

		WPlane planez(va, vb);
		float test = (planez.N.Dot(va)) - planez.D;
		godzassert(test < EPSILON);
		godzassert(planez.ClassifyPoint(Vector3(-8000,0,0)) == Halfspace_BEHIND);
		godzassert(planez.ClassifyPoint(Vector3(8000,0,0)) == Halfspace_FRONT);
		*/

		//now split this mesh if it's too big or whatever...
		const char* defaultwidth = GlobalConfig::m_pConfig->ReadNode("Editor","SplitWidth");
		const char* minnumtri = GlobalConfig::m_pConfig->ReadNode("Editor","SplitMinNumberOfTriangles");

		if (defaultwidth != NULL && minnumtri != NULL)
		{
			vector<WPlane> planes;

			size_t minnumtriangles = atoi(minnumtri);

			float splitwidth = atof(defaultwidth);
			const WBox& box = mesh->GetBounds();
			Vector3 bmax,bmin;
			bmin = box.GetMin();
			bmax = box.GetMax();

			float w = bmax.x - bmin.x;
			float loc = bmin.x + splitwidth;
			for(float x = splitwidth; x < w - splitwidth; x += splitwidth)
			{
				WPlane plane(Vector3(loc,0,0), Vector3(1,0,0));
				planes.push_back(plane);
				meshInstance->Slice(planes, minnumtriangles );
				planes.clear();

				loc += splitwidth;
			}
			
			if (planes.size() > 0)
			{
				
			}
			planes.clear();

			//support for depth / generate planes to split along depth axis...
			float depth = bmax.z - bmin.z;
			float splitdepth = splitwidth;
			const char* defaultdepth = GlobalConfig::m_pConfig->ReadNode("Editor","SplitDepth");
			if (defaultdepth != NULL)
			{
				splitdepth = atof(defaultdepth);
			}

			loc = bmin.z + splitdepth;
			for(float z = splitdepth; z < depth - splitdepth; z += splitdepth)
			{
				WPlane plane(Vector3(0,0,loc), Vector3(0,0,1));
				planes.push_back(plane);
				meshInstance->Slice(planes, minnumtriangles );
				planes.clear();

				loc += splitdepth;
			}

			if (planes.size() > 0)
			{
			}
		}

		DebugMeshInstance(meshInstance);
	}

	mesh->CreateRenderDeviceObjects();

	fclose(stream);
	return mesh;
}

bool GSAFileImporter::AppendGSA(const char* file, SkeletalMesh* mesh, GodzVector<rstring>& list)
{
	fileOpen(&stream, file, FileOpenMode_ReadBinary);
	if( stream == NULL )
	{
		DisplayMessage("Scene Import Error", "Cannot open file %s", file);
		return NULL;
	}
	
	filename=file;
	fread(&header,sizeof(GodzExporterHeader),1,stream);

	if(header.uMagic!=SCENE_EXPORT_MAGIC)
	{
		DisplayMessage("Scene Import Error", "%s is not a godz scene export file", file);
		return NULL;
	}

	Deprecated_GodzExporterHeader oldHeader;
	if (header.uVersion == 1)
	{
		//then it's the old, obsolete format.....
		fseek(stream, 0, SEEK_SET);
		
		fread(&oldHeader,sizeof(Deprecated_GodzExporterHeader),1,stream);

		header.numObjects = 1;
		header.iSceneTicks = oldHeader.iSceneTicks;
		header.framePerSample = oldHeader.framePerSample;
	}

	MeshInfo meshInfo;

	if (header.uVersion > 1)
	{
		//new format
		fread(&meshInfo,sizeof(MeshInfo),1,stream);
	}
	else
	{
		UpdateMeshInfo(oldHeader, meshInfo);
	}

	bool ret = true;

	if (meshInfo.iNumBones > 0)
	{
		ReadAnimation(mesh, meshInfo, list);
	}
	else
	{
		ret = false;
	}

	fclose(stream);
	return ret;
}

void GSAFileImporter::UpdateMeshInfo(const Deprecated_GodzExporterHeader& oldHeader, MeshInfo& meshInfo)
{
	meshInfo.iNumVertices = oldHeader.iNumVertices;
	meshInfo.vertsOffset = oldHeader.vertsOffset;
	meshInfo.iNumFaces = oldHeader.iNumFaces;
	meshInfo.facesOffset = oldHeader.facesOffset;
	meshInfo.iNumTVerts = oldHeader.iNumTVerts;
	meshInfo.tvertsOffset = oldHeader.tvertsOffset;
	meshInfo.iNumNormals = oldHeader.iNumNormals;
	meshInfo.normOffset = oldHeader.normOffset;
	meshInfo.iNumBones = oldHeader.iNumBones;
	meshInfo.boneOffset = oldHeader.boneOffset;
	meshInfo.nameOffset = oldHeader.nameOffset;
	meshInfo.skinVertOffset = oldHeader.skinVertOffset;
	meshInfo.iNumSkinVerts = oldHeader.iNumSkinVerts;
	meshInfo.initialSkinOffset = oldHeader.initialSkinOffset;
	meshInfo.skeletonOffset = oldHeader.skeletonOffset;
}

void GSAFileImporter::ReadMaterial(Mesh* mesh, GenericPackage* pkg, GodzVector<rstring>& list)
{
	rstring matName = ReadText();
	

	bool bIsMulti; //multi-subobject material
	fread(&bIsMulti,sizeof(bool),1,stream);

	if (!bIsMulti)
	{
		
		//TODO: If the material has a Normal map set, then create a BumpMaterial instead
		atomic_ptr<Material> material( new Material() );
		GUID materialUniqueID = material->CreateGUID();
		material->SetName(matName);
		pkg->AddObject(material, materialUniqueID);
		

		if (!list.contains(matName))
			list.push_back(matName);

		Vector3 v = ReadVector3f();

		FloatMaterialParameter* fp = new FloatMaterialParameter();
		fp->addValue(v.x);
		fp->addValue(v.y);
		fp->addValue(v.z);
		material->SetParameter(ParameterTypes::AmbientColor, fp);

		v = ReadVector3f();

		fp = new FloatMaterialParameter();
		fp->addValue(v.x);
		fp->addValue(v.y);
		fp->addValue(v.z);
		material->SetParameter(ParameterTypes::DiffuseColor, fp);

		v = ReadVector3f();

		fp = new FloatMaterialParameter();
		fp->addValue(v.x);
		fp->addValue(v.y);
		fp->addValue(v.z);
		material->SetParameter(ParameterTypes::SpecularColor, fp);

		v = ReadVector3f();

		fp = new FloatMaterialParameter();
		fp->addValue(v.x);
		fp->addValue(v.y);
		fp->addValue(v.z);
		material->SetParameter(ParameterTypes::EmissiveColor, fp);

		float value;
		fread(&value,sizeof(float),1,stream); //read opacity

		fp = new FloatMaterialParameter();
		fp->addValue(value);
		material->SetParameter(ParameterTypes::OpacityColor, fp);

		fread(&value,sizeof(float),1,stream); //read specular level

		fp = new FloatMaterialParameter();
		fp->addValue(value);
		material->SetParameter(ParameterTypes::SpecularPowerColor, fp);

		//read textures
		int texCount;
		fread(&texCount,sizeof(int),1,stream);

		for(int i=0;i<texCount;i++)
		{
			rstring text = ReadText();
			const char* value = text;

			if (!list.contains(matName))
				list.push_back(text);

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

			//read material slot
			int matSlot;
			fread(&matSlot, sizeof(int), 1, stream);

			MatSlot textureSlot = (MatSlot)matSlot;

			//Can only grab texture from diffuse slots
			if (value != NULL && strcmp(text,"") != 0)
			{
				ResourceManager* RM = ResourceManager::GetInstance();

				//Steps for texture resources
				//1. check to see if texture already exists....
				//2. If not, then we need to check to see if the file exists....
				//3. If it doesn't exist, then that means artist gave us wrong texture name. Display a dialog and give them chance to find texture
				HString textureName(text);
				ResourceObject* baseTex = pkg->findResource(textureName);

				GenericReference<TextureResource> resource = NULL;
				if (baseTex != NULL)
				{
					resource = static_cast<TextureResource*>(baseTex);
				}

				if (resource == NULL)
				{
					//text contained proper file
					resource = RM->GetNewTextureResource(text);
					resource->LoadTextureIntoMem(text);
					resource->CreateRenderDeviceObject();
					resource->SetPackage(pkg);
				}

				//TODO: if resource is Null, we need to let the artist select the texture!
				if (resource != NULL)
				{
					//figure out which slot to assign to......
					switch(textureSlot)
					{
					default:
						{
							godzassert(0); //unhandled type
							Log("Error: unhandled textureslot type %d name %s\n", textureSlot, text.c_str());
						}
						break;
					case ID_DI: //diffuse
						{
							TextureMaterialParameter* tex = new TextureMaterialParameter();
							tex->SetTexture(resource);
							material->SetParameter(TextureTypes::Diffuse, tex);
						}
						break;
					case ID_BU: //bump
						{
							TextureMaterialParameter* tex = new TextureMaterialParameter();
							tex->SetTexture(resource);
							material->SetParameter(TextureTypes::Normal, tex);
						}
						break;
					case ID_OP: //opacity
						{
							TextureMaterialParameter* tex = new TextureMaterialParameter();
							tex->SetTexture(resource);
							material->SetParameter(TextureTypes::Opacity, tex);
						}
						break;
					case ID_AM: //ambient
						{
							TextureMaterialParameter* tex = new TextureMaterialParameter();
							tex->SetTexture(resource);
							material->SetParameter(TextureTypes::Ambient, tex);
						}
						break;
					}
				}
			}
		}

		//check to see if the mesh already had this material added...
		HString newMat = material->GetName();
		MeshInstance* defMeshInstance = mesh->GetDefaultInstance();
		if (defMeshInstance->GetMaterialByName(newMat) == NULL)
		{
			ShaderManager* sm = ShaderManager::GetInstance();

			//set the default shader
			SkeletalMesh* skMesh = SafeCast<SkeletalMesh>(mesh);
			if (skMesh != NULL && !mSkinShader.isEmpty())
			{
				ShaderResource* sr = sm->FindShader(mSkinShader);
				material->SetShader(sr);
			}
			else if (!mDefaultShader.isEmpty())
			{
				ShaderResource* sr = sm->FindShader(mDefaultShader);
				material->SetShader(sr);
			}
			
			defMeshInstance->AddMaterial(material);
		}
		else
		{
			//hash name collision encountered!
			Log("Encountered a material name collision for %s\n", matName);
		}
	} //bIsMulti

	
	int subMatCount;
	fread(&subMatCount, sizeof(int), 1, stream);
	for (int i=0;i<subMatCount;i++)
	{
		ReadMaterial(mesh, pkg, list);
	}
}

void GSAFileImporter::ReadMesh(GenericPackage* p, Mesh* mesh, MeshInfo& meshInfo, GodzVector<rstring>& list)
{
	std::vector<VertexNormal> optVertexNormals; //optimized vertex normals
	std::vector<VertexNormal> vertexNormals;
	std::vector<Face> faces;
	std::vector<FaceNormal> faceNormals;
	std::vector<TFace> tfaces;
	std::vector<Vertex> verts;
	std::vector<TVert> tverts;

	//read out optimized vertex normals - (24 for a Box for example, 8 verts, 12 faces)
	fseek(stream, meshInfo.normOffset, SEEK_SET);
	for(int i = 0;i < meshInfo.iNumNormals;i++)
	{
		VertexNormal v;
		v.index=i;
		fread(&v.nx, sizeof(float), 1, stream);
		fread(&v.ny, sizeof(float), 1, stream);
		fread(&v.nz, sizeof(float), 1, stream);
		optVertexNormals.push_back(v);
	}

	//12 faces for a box
	fseek(stream, meshInfo.facesOffset, SEEK_SET);
	for(int i = 0;i<meshInfo.iNumFaces;i++)
	{
		Face face;
		FaceNormal faceNormal;
		TFace tface;
	
		//read face normal
		DWORD norm[3];
		for(int i=0;i<3;i++)
		{
			fread(&norm[i], sizeof(DWORD), 1, stream);
		}

		faceNormal.nx = norm[0];
		faceNormal.ny = norm[1];
		faceNormal.nz = norm[2];

		int matID;
		fread(&matID, sizeof(int), 1, stream);
		face.material_id = matID;

		DWORD vert[3];

		//read vertex indicies
		for(int i=0;i<3;i++)
		{
			fread(&vert[i], sizeof(DWORD), 1, stream);
			face.v[i]=vert[i];
		}

		//read out vertex normals (total of 36 for a box)
		for(int i=0;i<3;i++)
		{
			VertexNormal vn;
			vn.index=face.v[i];
			fread(&vn.nx, sizeof(float), 1, stream);
			fread(&vn.ny, sizeof(float), 1, stream);
			fread(&vn.nz, sizeof(float), 1, stream);
			vertexNormals.push_back(vn);

			//Log("VertexNormal %d, %f %f %f\n",vn.index,vn.nx,vn.ny,vn.nz);
		}

		//read texture vertex indicies
		DWORD texCoord[3];
		for(int i=0;i<3;i++)
		{
			fread(&texCoord[i], sizeof(DWORD), 1, stream);
			tface.tv[i]=texCoord[i];
		}

		faces.push_back(face);
		faceNormals.push_back(faceNormal);
		tfaces.push_back(tface);
	}


	//read out vertices (8)
	fseek(stream, meshInfo.vertsOffset, SEEK_SET);
	for (int i=0;i<meshInfo.iNumVertices;i++)
	{
		Vertex v;
		fread(&v.pos.x, sizeof(float), 1, stream);
		fread(&v.pos.y, sizeof(float), 1, stream);
		fread(&v.pos.z, sizeof(float), 1, stream);

		verts.push_back(v);
		//Log("Import--> Vertex %d, %f %f %f\n",i,v.x,v.y,v.z);
	}

	//read out texture vertices
	fseek(stream, meshInfo.tvertsOffset, SEEK_SET);
	for (int i=0;i<meshInfo.iNumTVerts;i++)
	{
		TVert uv;
		fread(&uv.u, sizeof(float), 1, stream);
		fread(&uv.v, sizeof(float), 1, stream);
		tverts.push_back(uv);

		//Log("Texture Vertex %d, %f %f\n",i,uv.u,uv.v);
	}


	//read out mesh name
	fseek(stream, meshInfo.nameOffset, SEEK_SET);

	rstring text = ReadText();
	mesh->SetName(text);

	if (!list.contains(text))
		list.push_back(text);

	//===========================================================
	//MARK BAD REGION BEGIN



	//MARK BAD REGION END

	size_t totalNumVerts = faces.size() * 3;
	GSAVertexDup* dups = new GSAVertexDup[meshInfo.iNumVertices];


	MeshInstance* meshInstance = mesh->GetDefaultInstance();

	SkeletalMesh* skMesh=NULL;
	if (mesh->IsA(SkeletalMesh::GetClass()))
	{
		skMesh = SafeCast<SkeletalMesh>(mesh);
	}

	//faces == faceNormals == texture faces
	godzassert(tfaces.size()==faceNormals.size()); //parser expects textures to equal # of normals

	//rosborne may 13, '07 - now what I want to do is make a new ModelResource (vertex/index buffer) per material.
	//this way we can batch together draw calls and reuse settings for all models sharing the same material!



	size_t numMaterials = meshInstance->GetNumMaterials();
	for (size_t mat_index = 0; mat_index < numMaterials; mat_index++)
	{
		//Step 1: separate faces into their respective groups
		std::vector<MaterialFace> material_face;

		int index=0;
		vector<Face>::iterator fIter;
		for(fIter=faces.begin();fIter!=faces.end();fIter++)
		{
			Face face = (*fIter);
			udword material_id=face.material_id;

			//we shouldn't have to check material ids but fer some reason 3d studio max sometimes
			//exports a bunch of bogus ids for a single textured mesh!
			if ( material_id>(numMaterials-1))
			{
				material_id=0;
			}

			if (material_id == mat_index)
			{
				MaterialFace mface;
				mface.face = face;
				mface.tface = tfaces[index];
				material_face.push_back(mface);
			}

			index++;
		} //end_loop find all faces for this material


		//Step 2: Produce a ModelResource for this material
		//create a vertex for every occurance within the vertex normal list...
		vector<Vertex> newVertexList;		//stores new vertices
		vector<unsigned long> newAttributeList;		//stores material assignments
		vector<unsigned short> indexBuf;
		
		//int index=0;
		int vertexIndex=0;
		vector<MaterialFace>::iterator mIter;
		for(mIter=material_face.begin();mIter!=material_face.end();++mIter)
		{
			//Face face = (*fIter);
			//TFace tf = tfaces[index];

			MaterialFace mface = (*mIter);
			Face face = mface.face;
			TFace tf = mface.tface;

			//Log("face %d %d %d tface: %d %d %d\n", face.v[0], face.v[1], face.v[2], tf.tv[0], tf.tv[1], tf.tv[2]);
			for (int i=0;i<3;i++)
			{
				VertexNormal vn = vertexNormals[face.v[i]];

				//get the x,y,z for this new vertex
				Vertex v = verts[face.v[i]];

				//get the normals for this new vertex
				v.normal[0]=vn.nx;
				v.normal[1]=vn.ny;
				v.normal[2]=vn.nz;



				//get the u,v for this new vertex using the textured face order
				int tvertex_index=tf.tv[i];
				v.u = tverts[tvertex_index].u;
				v.v = tverts[tvertex_index].v;

				//let's optimize this mesh as we build it.. Search for an existing vertex
				//that matches this one and see if we can reuse it....
				udword uDuplicateIndex=0;
				Vertex* pMatchVertex=NULL;
				for (udword j=0;j<newVertexList.size();j++)
				{
					Vertex& nv = newVertexList[j];
					if (nv==v)
					{
						pMatchVertex=&newVertexList[j];
						uDuplicateIndex=j;
						break;
					}
				}


				if (pMatchVertex==NULL)
				{
					//this is a new vertex...
					if (skMesh != NULL)
					{
						//create a vertex duplicate that maps us back to the original vertex
						VertexDupIndex ind;
						ind.m_index = vertexIndex;
						ind.m_matIndex = mat_index;
						dups[face.v[i]].indices.push_back(ind);

					}
					newVertexList.push_back(v);
					indexBuf.push_back(vertexIndex);
					vertexIndex++;
				}
				else
				{
					//reuse existing vertex - we don't need to create a duplicate since we
					//can just throw this index into the index buffer
					indexBuf.push_back(uDuplicateIndex);
				}
			}

			unsigned long matu32 = static_cast<unsigned long>(mat_index);
			newAttributeList.push_back(matu32); //attribute buffer - material ID #
			index++;
		}

		size_t sizeOfVertexBuffer = newVertexList.size();

		if(sizeOfVertexBuffer == 0)
			continue;

		Log("Total Vertices: %d\n", vertexIndex);
		godzassert(sizeOfVertexBuffer>0); //, "Mesh definition does not contain any vertices.");

		//Attach a VB/IB to the mesh
		ResourceManager* RM = ResourceManager::GetInstance();
		MeshResource resource;

		//need unique name for CRC32
		rstring resourceName = GetString("%s::%d",text.c_str(),mat_index);

		if (!list.contains(resourceName))
			list.push_back(resourceName);

		unsigned long hash = ECRC32::Get_CRC(resourceName);

		if (mesh->IsA(SkeletalMesh::GetClass()))
		{
			//when it's a skeletal mesh we stick to BlendVertex (for matrix palette skinning)
			resource = RM->CreateDynamicModel(newVertexList, indexBuf, newAttributeList,hash);

			//computes tangents for bump mapping
			resource->ComputeBounds();
			resource->SetPackage(p);

			meshInstance->AddModelResource(meshInstance->GetMaterial(mat_index), resource);
		}
		else
		{
			size_t indexCount = indexBuf.size();
			size_t vertices = newVertexList.size();

			resource = RM->CreateModelResource(newVertexList, indexBuf, newAttributeList, hash);
			
			//computes tangents for bump mapping
			resource->ComputeBounds();
			resource->SetPackage(p);
			meshInstance->AddModelResource(meshInstance->GetMaterial(mat_index), resource);
		}

		mesh->ComputeBounds();
	} //loop material




	if (skMesh != NULL)
	{
		//NOTE: This stuff might be broken now since I changed modelResources to be bound to materials.
		//not sure how that change effects this stuff here which only used for CPU Skinning

		//TODO: Bring back for CPU Skinning
		skMesh->AllocVertexDuplication(meshInfo.iNumVertices);
		for(int i=0;i<meshInfo.iNumVertices;i++)
		{
			size_t numIndices = dups[i].indices.size();

			godzassert (numIndices > 0);

			UInt32 index = static_cast<UInt32>(i);
			VertexDuplication* vd = skMesh->GetVertexDuplicate(index);
			vd->CreateIndices(numIndices);
			for(size_t j=0;j<numIndices;j++)
			{
				vd->m_indices[j] = (dups[i].indices[j]);
			}
		}
	}

	delete[] dups;

}

void GSAFileImporter::DebugMeshInstance(MeshInstance* meshInstance)
{
	size_t nummats = meshInstance->GetNumMaterials();
	for(size_t i=0;i<nummats;i++)
	{
		Material* m = meshInstance->GetMaterial(i);

		ModelContainer* mc = meshInstance->GetModelResources(m);
		Log("Material #%d\n", i);

		//run x-axis split again.....
		if (mc != NULL)
		{
			std::vector<ModelResource*> newResources;
			std::vector<ModelResource*> removedResources;

			size_t nummodels = mc->size();
			for(size_t j=0; j<nummodels;j++)
			{
				ModelResource* r = mc->get(j);
//				WBox b = r->GetBox();
//				Log("box (%f %f %f) (%f %f %f) index: %d\n", b.min.x, b.min.y, b.min.z, b.max.x, b.max.y, b.max.z, r->GetNumIndicies());
			}
		}
	}
}

void GSAFileImporter::ImportBezierKeys(AnimationInstance *animInstance, udword seqNum, const char* boneName, bool bOnlyImportFirstKey)
{
	//get position keys
	int keyCount;
	fread(&keyCount,sizeof(int),1,stream);

	AnimTrack track;
	track.SetBone(boneName);
	list<AnimKey> keys;			//list of keys


	int time=0;//, startTime=0;
	for(int i=0;i<keyCount;i++)
	{
		AnimKey key;
		fread(&time,sizeof(int),1,stream);

		//time -= startTime; //reduce the animation from 0 -> ....
		key.time = time;
		key.pos = ReadVector3f();
		key.scale = Vector3(1,1,1);
		//Quat4f q = ReadQuat4f();
		//Log("time %d quat %f %f %f %f\n",key.time,q.x,q.y,q.z,q.w);

		/*
		//Read Local
		GSAMatrix localTM;
		ReadMatrix(localTM);

		key.pos = localTM.pos;
		key.rot = localTM.rot;
		key.euler = localTM.euler;
		key.scale = localTM.scale;
		//key.bLinearKey = true;
		*/

		//key.rot.x = -key.rot.x;
		//key.rot.y = -key.rot.y;
		//key.rot.z = -key.rot.z;
		//key.rot.w = -key.rot.w;

		//key.rot.y = -key.rot.y;
		//Log("\npos: %f %f %f\n", key.pos.x,key.pos.y,key.pos.z);
		//Log("rot: %f %f %f %f time %d\n",key.rot.x,key.rot.y,key.rot.z,key.rot.w,time);
		//Log("euler y:%f p:%f r:%f\n",key.euler.y,key.euler.p,key.euler.r);

		/*
		//ReadMatrix(localTM);
		this->ConvertMaxToDX(localTM);
		//key.pos = localTM.pos;
		key.rot = localTM.rot;
		key.scale = localTM.scale;
		Log("pos: %f %f %f\n", localTM.pos.x,localTM.pos.y,localTM.pos.z);
		Log("rot: %f %f %f %f time %d\n",localTM.rot.x,localTM.rot.y,localTM.rot.z,localTM.rot.w,time);
		*/

		//track.AddKey(key);
		keys.push_back(key);
	}

	//read quat keys (rotation)
	fread(&keyCount,sizeof(int),1,stream);
	for(int i=0;i<keyCount;i++)
	{
		AnimKey key;
		fread(&time,sizeof(int),1,stream);
		key.time = time;
		key.rot = ReadQuat4f();
		//key.rot.w = -key.rot.w; //invert rotations incoming from 3dsmax
		key.scale = Vector3(1,1,1);

		//now find the closest key to the time
		list<AnimKey>::iterator keyIter;
		list<AnimKey>::iterator best=keys.end();
		bool bFound=0;
		for(keyIter=keys.begin();keyIter!=keys.end();keyIter++)
		{
			AnimKey &akey = *keyIter;
			if (time == akey.time)
			{
				//fill in the rotation for this key
				keyIter->rot = key.rot;
				bFound=0; //turn off this flag so a new key wont get added (we already found a key)
				break;
			}
			else if (time < akey.time)
			{
				best = keyIter;
				bFound=true;
			}
			else if (!bFound)
			{
				bFound=true;
				best = keyIter;
			}
		}

		if (bFound)
		{
			keys.insert(best, key);
		}
	}

	//now iterate through the list
	Log("bezeir/tcb keys %d\n", keys.size());
	list<AnimKey>::iterator keyIter;
	for(keyIter=keys.begin();keyIter!=keys.end();keyIter++)
	{
		AnimKey &key = *keyIter;
		track.AddKey(key);

		Log("time: %d\n bezeir pos: %f %f %f\n", key.time, key.pos.x,key.pos.y,key.pos.z);
		Log("tcb rot: %f %f %f %f\n",key.rot.q.x,key.rot.q.y,key.rot.q.z,key.rot.q.w);
	}

	animInstance->AddAnimTrack(track,seqNum);
	AnimSequence *seq = animInstance->GetSequence(seqNum);
	if (seq)
	{
		seq->SetMaxTime(time);
	}
}

void GSAFileImporter::ImportBipedSampleKeys(AnimationInstance *animInstance, udword seqNum, const char* boneName, bool bOnlyImportFirstKey)
{
	bool bHasSampleKeys=0;
	fread(&bHasSampleKeys,sizeof(bool),1,stream);

	if (!bHasSampleKeys)
	{
		return;
	}

	//get the animation data for this bone
	int keyCount;
	fread(&keyCount,sizeof(int),1,stream);

	Log("Full Sampled %d\n",keyCount);

	if (keyCount>0)
	{
		AnimTrack track;
		track.SetBone(boneName);
		int time;
			
		for(int j=0;j<keyCount;j++)
		{
			AnimKey key;
			fread(&time,sizeof(int),1,stream);
			key.time=time;

			//Read Local
			GSAMatrix localTM;
			ReadMatrix(localTM);

			//Read worldTM
			GSAMatrix worldTM;
			ReadMatrix(worldTM);

			//Read MaxNodeTM
			GSAMatrix nodeTM;
			ReadMatrix(nodeTM);

			/*	
			key.pos = localTM.pos;
			key.rot = localTM.rot;
			key.scale = localTM.scale;	
			*/
		
			//Quat4f q = worldTM.rot;
			ConvertMaxToDX(nodeTM);
			key.pos = nodeTM.pos;
			key.rot = nodeTM.rot;
			key.scale = nodeTM.scale;	
			
			//we use the world location of the bones centered at 0,0,0
			ConvertMaxToDX(worldTM);
			key.pos = worldTM.pos;
			key.rot = worldTM.rot;
			key.scale = worldTM.scale;


			if (!bOnlyImportFirstKey			 //import all animation keys
				|| (bOnlyImportFirstKey && j==0) //if this is a reference pose we only need one key
				)
			{
				Log("key pos: %f %f %f\n", key.pos.x,key.pos.y,key.pos.z);
				Log("key rot: %f %f %f %f time %d\n",key.rot.q.x,key.rot.q.y,key.rot.q.z,key.rot.q.w,time);

				track.AddKey(key);
			}
		} //loop

		
		animInstance->AddAnimTrack(track,seqNum);
		AnimSequence* seq = animInstance->GetSequence(seqNum);
		if (seq != NULL)
		{
			seq->SetMaxTime(time);
		}
	}
}

void GSAFileImporter::ReadAnimation(SkeletalMesh* mesh, MeshInfo& meshInfo, GodzVector<rstring>& list)
{
	AnimationInstance* animInstance = mesh->GetAnimationInstance();
	if (animInstance == NULL)
	{
		mesh->CreateAnimationInstance();
		animInstance=mesh->GetAnimationInstance();
	}

	godzassert(animInstance != NULL);

	//read bone definitions
	fseek(stream, meshInfo.boneOffset, SEEK_SET);

	udword sequenceIndex = static_cast<udword>(animInstance->GetNumOfAnimSets());

	bool bOnlyImportFirstKey = false;
	if (sequenceIndex == 0)
	{
		//for skeletal animation poses, only import the first key. No need to import a full animation
		//set for the initial pose.
		bOnlyImportFirstKey = true;
		Log("NOTE: Only importing the first key from this animation set. Assuming this is the initial pose.\n");
	}

	StringTokenizer tokenizer(filename, "\\/.");
	udword size = static_cast<udword>(tokenizer.size());
	rstring sequenceName;

	if (size > 1)
	{
		sequenceName = tokenizer[size-2];
	}

	if (!list.contains(sequenceName))
		list.push_back(sequenceName);

	//create an animation set to store the keys for all the bones in the mesh.
	animInstance->AddAnimSet(sequenceName);

	for (size_t i=0;i<meshInfo.iNumBones;i++)
	{
		//read bone position & orientation (World TM)
		rstring boneImport = ReadText();
		const char* text=  boneImport;
		Log("Anim Bone %s\n", text);

		int nAnimType=0;
		fread(&nAnimType, sizeof(int), 1, stream);
		EExportAnimKey kKeyType = (EExportAnimKey)nAnimType;

		switch(kKeyType)
		{
		default:
			break;
		case EA_FullSampleKeys:
			{
				ImportBipedSampleKeys(animInstance, sequenceIndex, text, bOnlyImportFirstKey);
			}
			break;
		case EA_BezierKeys:
			{
				ImportBezierKeys(animInstance, sequenceIndex, text, bOnlyImportFirstKey);
			}
			break;
		}
	}
}


void GSAFileImporter::ReadSkin(SkeletalMesh* mesh, MeshInfo& meshInfo, GodzVector<rstring>& list)
{
	//read initial skin matrix
	fseek(stream, meshInfo.initialSkinOffset, SEEK_SET);
	GSAMatrix initialSkinTM;
	ReadMatrix(initialSkinTM);
	WMatrix16f skinTM;
	initialSkinTM.SetMatrix(skinTM);

	//read bone definitions
	fseek(stream, meshInfo.skeletonOffset, SEEK_SET);	

	//get the default mesh instance
	MeshInstance* mi = mesh->GetDefaultInstance();
	godzassert(mi != NULL);
	SkelMeshInstance* skelObj = SafeCast<SkelMeshInstance>(mi);

	size_t numbones = static_cast<size_t>(meshInfo.iNumBones);
	skelObj->CreateBones(numbones);

	
	//read the skeleton
	for (DWORD i=0;i<meshInfo.iNumBones;i++)
	{
		rstring boneName = ReadText(); 

		//debug
		const char* name = boneName;
		Bone* bone = skelObj->GetBone((size_t)i);
		bone->SetName(name);

		if (!list.contains(name))
			list.push_back(name);
		
		int parentId=0;
		fread(&parentId, sizeof(int), 1, stream);
		bone->parentIndex=parentId;
		//parents.push_back(parentId);

		//Log("bone name %s index:%d parent: %d\n",name,i,parentId);

		//get bounding box data -- remove me! fix me!
		Vector3 unused_min, unused_max;
		fread(&unused_min.x, sizeof(float), 1, stream);
		fread(&unused_min.y, sizeof(float), 1, stream);
		fread(&unused_min.z, sizeof(float), 1, stream);
		fread(&unused_max.x, sizeof(float), 1, stream);
		fread(&unused_max.y, sizeof(float), 1, stream);
		fread(&unused_max.z, sizeof(float), 1, stream);

		//read max control type
		//fread(&bone->m_nControlType, sizeof(int), 1, stream);  //new - temp?	

		//get initial pose
		GSAMatrix initTM;
		ReadMatrix(initTM);
		initTM.SetMatrix(bone->m_init);
		bone->m_init.Inverse(bone->m_initInv);

		//Quat4f q = initTM.rot;
		//initTM.SetMatrix(bone->m_init);
		Log("\ninit bone %s pos: %f %f %f\n", name,initTM.pos.x,initTM.pos.y,initTM.pos.z);
		Log("rot %f %f %f %f\n", initTM.rot.q.x,initTM.rot.q.y,initTM.rot.q.z,initTM.rot.q.w);
		Log("euler %f %f %f\n",initTM.euler.y,initTM.euler.p,initTM.euler.r);

		Quat4f q = initTM.rot;

		//get max version of initTM
		ReadMatrix(initTM);

		ConvertMaxToDX(initTM);
		//initTM.SetMatrix(bone->m_init);
		Log("init bone (non-IGame) %s pos: %f %f %f\n", name,initTM.pos.x,initTM.pos.y,initTM.pos.z);
		Log("rot %f %f %f %f\n", initTM.rot.q.x,initTM.rot.q.y,initTM.rot.q.z,initTM.rot.q.w);
		Log("euler %f %f %f\n",initTM.euler.y,initTM.euler.p,initTM.euler.r);

		//NEW - use matrix from first key (physique initial TM is bugged)
		initTM.SetMatrix(bone->m_init);
		bone->m_init.Inverse(bone->m_initInv);

		//read collision models
		if (header.uVersion < 3)
		{
			//handle Old header (old gsa files)
			int numModels=0;
			fread(&numModels,sizeof(int),1,stream);

			//read old file format...
			for(int i=0;i<numModels;i++)
			{
				//read dummy data....
				float r;
				fread(&r,sizeof(float),1,stream);

				GSAMatrix localMat;
				ReadMatrix(localMat);
			}
		}
		else
		{
			//TODO: Set parent
			ReadShapes(bone, NULL, list);
		}
	}

	//build a tree for this skeleton
	skelObj->UpdateHierarchy();

	
	//read animation data
	ReadAnimation(mesh, meshInfo, list);

	//max number of bones
	unsigned char nMaxNumBones=0;


	//read skin verts
	fseek(stream, meshInfo.skinVertOffset, SEEK_SET);
	for(int i=0;i < meshInfo.iNumSkinVerts;i++)
	{
		//GSASkinVert skinVert;

		//not used currently
		int vertexType;
		fread(&vertexType,sizeof(int),1,stream); 

		//get number of bones
		int numBones;
		fread(&numBones,sizeof(int),1,stream);

		//if you get this godzassert, go to Vertextypes.h and make MAX_MATRIX_INDEX larger
		//and update corresponding shaders.
		//godzassert(numBones<MAX_MATRIX_INDEX);

		if (numBones > nMaxNumBones)
		{
			nMaxNumBones = (unsigned char)numBones;
		}

		VertexDuplication* vd = mesh->GetVertexDuplicate(i);
		for(int j=0;j<numBones;j++)
		{
			//gen weighting information for each vertex
			float weight;
			fread(&weight,sizeof(float),1,stream);

			int boneIndex;
			fread(&boneIndex,sizeof(int),1,stream);

			boneWeights[boneIndex].m_blendweights.push_back(BlendWeight(weight,i));
			
			//Log("vertex weight %f bone index: %d numBones: %d\n", weight, boneIndex, numBones);

			//if (resource->GetVertexType() == VT_BlendVertex)
			{
				if (j<MAX_MATRIX_INDEX)
				{
					//Log("vert %d index %d weight %f index %d numBones %d\n", i, j, weight, boneIndex, numBones);

					//setup the blending info for skinning
					size_t verts = vd->GetNumIndices();
					Log("verts %d bone index %d\n", verts, j);

					for(udword k=0;k<verts;k++)
					{
						VertexDupIndex index = vd->m_indices[k];

						//GPU skinning code
						Material* m = skelObj->GetMaterial(index.m_matIndex);
						ModelContainer* c = skelObj->GetModelResources(m);
						godzassert(c != NULL);

						ModelResource* resource = c->get(0); //skinned meshes only have 1 resource when import
						BlendVertex *bv = (BlendVertex*)resource->GetVertex(index.m_index);

						bv->weights[j]        = weight;
						bv->matrixIndicies[j] = boneIndex;
					}
				}
				else
				{
					//we are forced to ignore this vertex :(
					Log("WARN: Skin Vertex #%d uses more than the max number of influences %d>%d\n",i,numBones,MAX_MATRIX_INDEX);
				}
			}
		}	//loop j
	}


	//now that we have all the weighted data- copy it to the bones
	for(size_t i=0;i<meshInfo.iNumBones;i++)
	{
		OrigBone* bone = mesh->GetOrigBone(i);
		size_t numWeights = boneWeights[i].m_blendweights.size();
		bone->CreateBlendVerts(numWeights);
		for(size_t j=0;j<numWeights;j++)
		{
			//bone->AddWeightedVertex(boneWeights[i].m_blendweights[j].weight,boneWeights[i].m_blendweights[j].vertexIndex);
			bone->m_blendVerts[j].weight=boneWeights[i].m_blendweights[j].weight;
			bone->m_blendVerts[j].vertexIndex=boneWeights[i].m_blendweights[j].vertexIndex;
		}
	}

	Log("Max # of bones is %d\n", nMaxNumBones);
	mesh->SetMaxNumBones(nMaxNumBones);

	mesh->AttachSkin();
}

void GSAFileImporter::ReadShapes(Bone* parent, MeshInstance* meshInstance, GodzVector<rstring>& list)
{
	int numModels=0;
	fread(&numModels,sizeof(int),1,stream);

	for(int i=0;i<numModels;i++)
	{
		int shapeType;
		fread(&shapeType,sizeof(int),1,stream);

		rstring text = ReadText();
		list.push_back(text);

		//pivot pos / rot from 3dsmax (atm we dont do anything with the object offset tm)
		//The plan is to handle cases where the pivot isn't centered in the middle...
		GSAMatrix pivotMat;
		ReadMatrix(pivotMat);
		ConvertMaxToDX(pivotMat);

		int motionType;
		fread(&motionType,sizeof(int),1,stream);

		SkeletonNode node;
		node.m_name = text;
		node.m_motionType = (MotionType)motionType;
		
		switch(shapeType)
		{
		case ShapeType::ShapeType_Sphere:
			{
				float radius;
				fread(&radius,sizeof(float),1,stream);

				//Read matrix
				GSAMatrix localMat;
				ReadMatrix(localMat);
				ConvertMaxToDX(localMat);

				//create a sphere in local space
				WSphere s;
				s.radius = radius;
				s.center = localMat.pos;

				GenericReference<SphereCollisionPrimitive> sc = new SphereCollisionPrimitive();
				sc->SetBounds(s);
				node.m_primitive = sc;
			}
			break;
		case ShapeType::ShapeType_Box:
			{
				float width,height,length;
				fread(&length,sizeof(float),1,stream);
				fread(&width,sizeof(float),1,stream);
				fread(&height,sizeof(float),1,stream);
      
				//Read matrix
				GSAMatrix localMat;
				ReadMatrix(localMat);
				ConvertMaxToDX(localMat);

				//QuaternionToMatrix(localMat.rot, node.m_relOrientation);
				//node.m_relPos = localMat.pos;

				WMatrix rel = localMat;

				//create a box in local space
				OBBox obb;

				//Note: Issue, 3dsmax doesn't center the pivot on boxes by default. The pivot
				//might actually be on the bottom. Here, we assume the pivot is centered
				obb.center = localMat.pos;

				rel.GetAxes(obb.axis[0], obb.axis[1], obb.axis[2]);

				//3dsmax coordinates
				//length = Y-Axis (Depth)
				//width = X-Axis
				//Height = Z-axis

				//recall 3dsmax is z-up, this engine is y-up
				//Note, extent is actually half of the box dimension so divide by 2
				obb.extent = Vector3(width/2.0f, height/2.0f, length/2.0f);
				GenericReference<OBBCollisionPrimitive> oc = new OBBCollisionPrimitive();
				oc->SetBounds(obb);
				node.m_primitive = oc;
				const Vector3 origin(0,0,0);

				node.m_offset = obb.center;

				//Also should store off the rotation offset
				Matrix3 m(rel);
				node.m_initRot = m;
			}
			break;

		case ShapeType::ShapeType_Mesh:
			{
				int numverts;
				fread(&numverts,sizeof(int),1,stream);

				std::vector<Vector3> vertexList;

				for(int i = 0; i < numverts;i++)
				{
					Vector3 v;
					fread(&v.x, sizeof(float), 1, stream);
					fread(&v.y, sizeof(float), 1, stream);
					fread(&v.z, sizeof(float), 1, stream);

					vertexList.push_back(v);
				}


				int numfaces;
				fread(&numfaces,sizeof(int),1,stream);

				std::vector<Vector3> verts;

				for(int i = 0; i < numfaces;i++)
				{
					DWORD v[3];

					//read vertex indicies
					fread(&v[0], sizeof(DWORD), 1, stream);
					fread(&v[1], sizeof(DWORD), 1, stream);
					fread(&v[2], sizeof(DWORD), 1, stream);

					verts.push_back(vertexList[ v[0] ]);
					verts.push_back(vertexList[ v[1] ]);
					verts.push_back(vertexList[ v[2] ]);
				}

				//Now build the AABB Tree
				AABBTreeInfo info;
				GenericReference<AABBRoot> root = new AABBRoot(verts, info);
				node.m_primitive = root;
			}
			break;
		}

		if (parent != NULL)
		{
			parent->m_nodes.push_back(node);
		}
		else
		{
			meshInstance->AddNode(node);
		}
	} //loop children
}

Vector3 GSAFileImporter::ReadVector3f()
{
	Vector3 v;
	fread(&v.x, sizeof(float), 1, stream);
	fread(&v.y, sizeof(float), 1, stream);
	fread(&v.z, sizeof(float), 1, stream);
	return v;
}

rstring GSAFileImporter::ReadText()
{
	rstring r;
	size_t len;
	fread(&len, sizeof(size_t), 1, stream);

	if (len > 2056)
	{
		DisplayMessage("Import File Error","Text String to Long");
	}

	if (len > 0)
	{
		char *text = new char[len + 1];
		fread(text, sizeof(char), len, stream);
		text[len] = '\0';
		r=text;
		delete[] text;
	}
	return r;
}

Quat4f GSAFileImporter::ReadQuat4f()
{
	Quat4f v;
	fread(&v.q.x, sizeof(float), 1, stream);
	fread(&v.q.y, sizeof(float), 1, stream);
	fread(&v.q.z, sizeof(float), 1, stream);
	fread(&v.q.w, sizeof(float), 1, stream);
	return v;
}

void GSAFileImporter::ReadMatrix(GSAMatrix &m)
{
	//fread(&m, sizeof(float), 13, stream); //used to be 10
	fread(&m.pos.x, sizeof(float), 1, stream);
	fread(&m.pos.y, sizeof(float), 1, stream);
	fread(&m.pos.z, sizeof(float), 1, stream);

	fread(&m.rot.q.x, sizeof(float), 1, stream);
	fread(&m.rot.q.y, sizeof(float), 1, stream);
	fread(&m.rot.q.z, sizeof(float), 1, stream);
	fread(&m.rot.q.w, sizeof(float), 1, stream);

	fread(&m.scale.x, sizeof(float), 1, stream);
	fread(&m.scale.y, sizeof(float), 1, stream);
	fread(&m.scale.z, sizeof(float), 1, stream);

	fread(&m.euler.y, sizeof(float), 1, stream);
	fread(&m.euler.p, sizeof(float), 1, stream);
	fread(&m.euler.r, sizeof(float), 1, stream);
	
	//Invert rotation component on Quaternion
	m.rot.q.w = -m.rot.q.w;
}

void GSAFileImporter::ReadMatrix16f(WMatrix& m)
{
	fread(&m._11, sizeof(float), 16, stream);
}
