/* ===========================================================

Created feb 6, 10 by Richard Osborne
Copyright (c) 2010, Richard Osborne
=============================================================
*/

#if !defined(__MBOT_H__)
#define __MBOT_H__

#include <CoreFramework/Core/Job.h>
#include <CoreFramework/Core/GodzAtomics.h>
#include <CoreFramework/Core/atomic_ref.h>
#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/Math/WMatrix3.h>
#include "PathFinder.h"


namespace GODZ
{
	class MActor;
	class NavigationMesh;

	/*
	* Multithreaded bot
	*/
	class GODZ_API MBot : public Job
	{
	public:
		MBot();

		//Called before the thread runs this job for the 1st time
		virtual void Start();

		//Execute job specific code. Expect this code to be called only once unless the job is declared as
		//'permenant'. Never put an infinite loop here. The job thread will keep calling this
		//if this job sets the permanent flag.
		virtual bool Run(float dt);

		//Returns true if this Job Type is 'unique' meaning it should be assigned it's own thread
		virtual bool isPermanent();	//Gets it's own thread

		//Called when this job needs to be stopped and perform cleanup
		virtual void Stop();

		//Set the creature we will be controlling...
		void SetEntity(MActor* pawn);

		//Creature we are trying to kill or navigate to
		void SetTarget(atomic_ptr<MActor>& target);

		void SetNavMesh(NavigationMesh* mesh);

	private:
		bool HasRayHit(const WRay& ray);
		void RotateTowardsGoal(const Vector3& pos, const Vector3& goalPos, const Matrix3& m, float dt);
		void ComputeTravelDir(long sourcePolygon, const WMatrix& m, const Vector3& goalPos, float dt);

		SyncPoint<atomic_weak_ptr<MActor> > m_target;
		MActor* m_actor;
		NavigationMesh* m_navmesh;
		long mGoalPolyIndex;
		NavigationPath m_path;
		Vector3 mTravelDir;				//direction we will be traveling....
		Quaternion m_rot;
	};
}



#endif //__MBOT_H__