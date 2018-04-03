/* ===========================================================
	SpawnPoint

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__SPAWNPOINT_H__)
#define __SPAWNPOINT_H__

#include <CoreFramework/Core/WEntity.h>

namespace GODZ
{
	class GenericClass;

	class GODZ_API SpawnPoint : public WEntity
	{
		DeclareGeneric(SpawnPoint, WEntity)

	public:
		SpawnPoint();

		void SetSpawnClass(UInt32 className);
		UInt32 GetSpawnClass();

	protected:
		GenericClass* mSpawnClass;
	};
}

#endif //__SPAWNPOINT_H__