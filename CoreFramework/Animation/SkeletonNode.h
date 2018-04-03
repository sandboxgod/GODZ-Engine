/* ===========================================================
	SkeletonNode.h

	Copyright (c) 2011
	========================================================== 
*/

#pragma once

#include <CoreFramework/Collision/CollisionPrimitive.h>
#include <CoreFramework/Core/HString.h>
#include <CoreFramework/Collision/PhysicsObjectProxy.h>
#include <CoreFramework/Core/RenderDeviceObject.h>
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Collision/CollisionUtil.h>

namespace GODZ
{
	//Represents either a bone or Helper object for a primitive
	struct GODZ_API SkeletonNode
	{
		HString m_name;
		GenericReference<CollisionPrimitive> m_primitive;
		PhysicsObjectProxy m_proxy;
		MotionType m_motionType;

		//length in object space; points from the center of the primitive to the origin
		Vector3 m_offset;
		Matrix3 m_initRot;

		//Transient
		Future<GenericReference<RenderDeviceObject> > m_renderObject;

		SkeletonNode()
			: m_primitive(NULL)
		{
		}

		~SkeletonNode()
		{
		}

		friend GDZArchive& operator<<(GDZArchive& ar, SkeletonNode& node)
		{
			ArchiveVersion version(1,1);
			ar << version;

			ar << node.m_name;

			int t = (int)node.m_motionType;
			ar << t;
			node.m_motionType = (MotionType)t;

			ar << node.m_offset;
			ar << node.m_initRot;

			bool hasPrimitive = (node.m_primitive != NULL);
			ar << hasPrimitive;

			if (hasPrimitive)
			{
				int type;

				if (ar.IsSaving())
				{
					type = node.m_primitive->GetCollisionType();
				}

				ar << type;

				if (!ar.IsSaving())
				{
					godzassert(node.m_primitive == NULL);

					//get instance of this type of collision
					node.m_primitive = CollisionUtil::GetInstance((CollisionPrimitive::CollisionType)type);
				}

				node.m_primitive->Serialize(ar);
				node.m_primitive->SetName( node.m_name );
			}

			return ar;
		}
	};
}

