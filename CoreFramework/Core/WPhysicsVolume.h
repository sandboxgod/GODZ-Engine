/* ===========================================================
	Physics Volume existing within world space

	Created Feb 21, '04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_WPHYSICSVOLUME_H_)
#define _WPHYSICSVOLUME_H_

#include "WEntity.h"

namespace GODZ
{
	//Forward Declarations
	class WEntity;

	class GODZ_API WPhysicsVolume : public WEntity
	{
		DeclareGeneric(WPhysicsVolume, WEntity)

	public:
		WPhysicsVolume();

		//grows this volume to fit within the world.
		void ExpandToWorldSize();

		const Vector3& GetGravity();
		void SetGravity(const Vector3& grav);

	private:
		Vector3 gravity;	//gravity force * mass 
	};
}

#endif