
#pragma once

#include "WEntity.h"

namespace GODZ
{
	class GODZ_API WSkyDome : public WEntity
	{
		DeclareGeneric(WSkyDome, WEntity)

	public:
		enum DomeType
		{
			DomeType_Full,
			DomeType_Half
		};

		WSkyDome();

		void SetDomeType(DomeType type);

		//Notification this entity has been spawned on the level
		virtual void OnAddToWorld();
		
		//Notification this entity has been removed from the level
		virtual void			OnRemoved();

	protected:
		virtual void			UpdateGeometryInfo(GeometryInfo& info);

		DomeType mType;
	};
};
