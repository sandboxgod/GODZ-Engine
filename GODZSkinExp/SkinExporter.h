
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

#include <CoreFramework/Core/BoneSystem.h> //GODZ Engine - provides a bone system representation
#include <vector>

namespace GODZ
{
	class SkinExporter
	{
	public:
		SkinExporter(int framesPerSample, Interface* pInterface);
		bool SaveGameScene(const TCHAR* szFilename);

	protected:
		std::vector<INode*> vBones;
		IGameScene* pIgame;
		FILE* stream;
		GodzExporterHeader header;
		LogSys* TheManager;
		Interface* pInterface;

		int AssignBone(INode* node, bool bAssignToList=true);

		void DumpMatrix(const Matrix3& matrix);
		void DumpBezierKeys(IGameControl * sc, IGameNode* node);
		bool DumpSampleKeys(IGameControl * sc, IGameNode* node);
		void SaveCollisionVolumes(IGameNode* node);
		fpos_t GetSeekPos();
		
		//writes bounding box of this object
		void WriteBBox(IGameObject* obj);

		//writes animation data for this node
		void WriteBoneKeys(IGameNode* node);
		
		void WriteFaceEx(FaceEx* f, IGameMesh* gm, int faceIndex);
		
		//writes a string
		void WriteText(TCHAR* text);
		void WriteTextures(IGameMaterial* material);

		//writes a material
		void WriteMaterial(IGameMaterial* material);
		void WriteMatrix(GMatrix& matrix);

		void WritePoint2(Point2& p);
		void WritePoint3(Point3& p);
		void WritePoint4(Point4& p);

		void WriteSkin(IGameSkin* skin, MeshInfo& meshInfo);
		void WriteSkinnedVertex(IGameSkin* skin, int index);


		void SaveGameNode(IGameNode* pGameNode);

		MSTR GetMotionKey() { return "MotionType"; }
	};
}