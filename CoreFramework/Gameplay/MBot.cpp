

#include "MBot.h"
#include <CoreFramework/Core/MActor.h>
#include <CoreFramework/Collision/PhysicsObjectProxy.h>
#include <CoreFramework/Collision/RayCollider.h>
#include <CoreFramework/Math/WMatrixUtil.h>

namespace GODZ
{

MBot::MBot()
: m_navmesh(NULL)
, mGoalPolyIndex(0)
, m_rot(Quaternion::getIdentity())
, mTravelDir(Vector3::Zero())
{
}

void MBot::SetNavMesh(NavigationMesh* mesh)
{
	m_navmesh = mesh;
}

void MBot::SetTarget(atomic_ptr<MActor>& target)
{
	m_target.set( target );
}

void MBot::SetEntity(MActor* pawn)
{
	m_actor = pawn;
}


bool MBot::isPermanent()
{
	return false;
}

void MBot::Start()
{
	//Log("MBot::Start() \n");
}

void MBot::Stop()
{
}

//Note: MBots expect to be the only object that tells the actor proxy where to rotate, etc.
bool MBot::Run(float dt)
{
	//Log("MBot::Run( %f) \n", dt);
	atomic_weak_ptr<MActor> handle = m_target;
	atomic_ptr<MActor> target = handle.lock();

	if (target != NULL)
	{
		PhysicsObjectProxy proxy = m_actor->GetPhysicsObject();
		PhysicsObjectProxy targetproxy = target->GetPhysicsObject();

		if( !proxy.IsValid() || !targetproxy.IsValid() )
			return true;

		Vector3 pos;
		proxy.GetPosition(pos);

		Vector3 tpos;
		targetproxy.GetPosition(tpos);

		Matrix3 m;
		m_actor->GetOrientation(m);

		//Log("pos %f %f %f\n", pos.x, pos.y, pos.z);
		//Log("tpos %f %f %f\n", tpos.x, tpos.y, tpos.z);

		//rotate to face target.
		RotateTowardsGoal(pos, tpos, m, dt);


		//find which polygon we standing on
		WRay ray;
		ray.dir = m.GetUp();
		ray.dir.Invert();
		ray.origin = pos;
		size_t si = 0;
		bool isStandingOnPolygon = m_navmesh->IsPolygonHit(ray, si);

		//make sure its a valid downcast
		godzassert(si < LONG_MAX);
		long sourcePoly = static_cast<long>(si);


		//Log("target %d isStandingOnPolygon %d\n", target->IsStandingOnPolygon(), isStandingOnPolygon );
		//now pathfind to the target (if we do not already have a path or the target has moved)....
		//1. get my dest
		if (target->IsStandingOnPolygon() && isStandingOnPolygon )
		{
			long goalPolyIndex = target->GetNavMeshPolyIndex();

			//Log("goalPolyIndex %d \n", goalPolyIndex);

			//long sourcePoly = m_actor->GetNavMeshPolyIndex();
			if (sourcePoly != goalPolyIndex)
			{
				size_t numpolys = m_path.GetNumPolygons();
				if (goalPolyIndex != mGoalPolyIndex 
					|| numpolys == 0 // is path built?
					)
				{
					//need to find path to the target...
					mGoalPolyIndex = goalPolyIndex;

					//build the path...
					m_path.buildPath(sourcePoly, mGoalPolyIndex, m_navmesh);
					
					//face the next polygon?
				}
				else if (numpolys > 0)
				{
					//if we have a path, let's plot course towards the enemy
					ComputeTravelDir(sourcePoly, m, tpos, dt);

					//Log("TravelDir %f %f %f\n", mTravelDir.x, mTravelDir.y, mTravelDir.z);

					Vector3 dir = (mTravelDir * m_actor->GetMoveSpeed());
					proxy.SetDesiredVelocity(dir);
				} //else
			} //polygon
			else
			{
				//Log("MBot::Reached goal( %f) \n", dt);
				//matching polygon
				Vector3 dir = (tpos - pos);
				float distSQ = dir.GetLengthSQ();
				const float mindistSQ = 150 * 150;

				
				//get no closer than 100 meters to the player for now....
				if (distSQ < mindistSQ)
				{
					proxy.SetDesiredVelocity(ZERO_VECTOR);
				}
				else
				{
					//travel at a constant move speed towards the player. TODO: eventually we might want to
					//take into account the actual plane normal we moving on to determine how steep it is
					//and tweak our speed accordingly....
					dir.Normalize();
					dir *= m_actor->GetMoveSpeed();
					proxy.SetDesiredVelocity(dir);
				}
			}
		} //standing on a poly
	} //has target

	Sleep(0.1f);
	return true;
}

bool MBot::HasRayHit(const WRay& ray)
{
	size_t numpolys = m_path.GetNumPolygons();
	for(size_t i=0;i<numpolys;i++)
	{
		const NavMeshPolygon& p = m_navmesh->GetPolygon(m_path.GetPolygon(i) );
		CollisionFace face;
		if (RayTriOverlap(ray, p.vertex[0], p.vertex[1], p.vertex[2], face))
		{
			return true;
		}
	} //loop

	return false;
}


void MBot::RotateTowardsGoal(const Vector3& pos, const Vector3& goalPos, const Matrix3& m, float dt)
{
	//Log("pos %f %f %f\n", pos.x,pos.y,pos.z);
	//Log("goalPos %f %f %f\n", goalPos.x,goalPos.y,goalPos.z);
	//Log("m.GetUp() %f %f %f\n", m.GetUp().x, m.GetUp().y, m.GetUp().z);

	//Note: we cannot be directly above the target (produces NaNs)
	WMatrix3 rot = RotateToFace(pos, goalPos, m.GetUp());

	Quaternion dest(rot);
	//Log("dest %f %f %f %f\n", dest.q.x,dest.q.y,dest.q.z, dest.q.w);

	m_rot = Quaternion::slerp(m_rot, dest, dt);

	m_rot.normalize();
	//Log("m_rot %f %f %f %f\n", m_rot.q.x,m_rot.q.y,m_rot.q.z, m_rot.q.w);
	m_actor->SetRotation(m_rot);

	//Note: bot can sometimes do some 'jitter' however I think that caused by movement routines not sure
}

void MBot::ComputeTravelDir(long sourcePolygon, const WMatrix& m, const Vector3& goalPos, float dt)
{
	mTravelDir.MakeZero(); //stay put by default

	PhysicsObjectProxy proxy = m_actor->GetPhysicsObject();

	//1. First, need to compute the line between source & dest
	Vector3 pos;
	proxy.GetPosition(pos);
	Vector3 dir = goalPos - pos;
	float goal_dist = dir.GetLength();
	dir.Normalize();


	Vector3 dist_to = (dir * m_actor->GetMoveSpeed());
	float d = dist_to.GetLength();

	//make sure we're not traveling past the goal
	if (d > goal_dist)
	{
		dist_to = dir;
	}

	Vector3 nextMove = pos + dist_to;
	
	//See if it's a straight shot to the dest?

	long next_poly = m_path.FindNextPolygon(sourcePolygon);

	//if we didnt get another poly, that means we've arrived
	if (next_poly > -1)
	{
		WRay ray;
		ray.dir = m.GetUp();
		ray.dir.Invert(); //point down
		ray.origin = nextMove;

		if (HasRayHit(ray) )
		{
			mTravelDir = dir;
		}
		else
		{
			//invalid move, so just move towards the next poly for now...

			//1. See if there's another poly behind that one and make a straight line. Mind you, this line
			//is more likely to go off path though :(
			long n3 = m_path.FindNextPolygon(next_poly);


			if (n3 > -1)
			{
				//run that way.....
				const NavMeshPolygon& s = m_navmesh->GetPolygon(sourcePolygon);
				const NavMeshPolygon& d = m_navmesh->GetPolygon(n3);

				mTravelDir = d.mCenter - s.mCenter;
				mTravelDir.Normalize();
			}
		}
	}
	else
	{
		mTravelDir = dir;
	}

}


}