/* ===========================================================

Created feb 20, 10 by Richard Osborne
Copyright (c) 2010, Richard Osborne
=============================================================
*/

#if !defined(__PathFinder_H__)
#define __PathFinder_H__

#include <CoreFramework/Core/CoreFramework.h>
#include <CoreFramework/Core/NavigationMesh.h>


namespace GODZ
{
	class NavigationMesh;

	struct GODZ_API PathFinderNode
	{
		float g; //goal;
		float h; //heuristic

		//best guess for cost of this path for going through this node
		float GetFitness() { return g + h; }

		HashCodeID mLayerID;
		long mPolyIndex;
		PathFinderNode* parent;

		PathFinderNode()
			: parent(NULL)
			, mLayerID(0)
		{
		}
	};

	class GODZ_API NavigationPath
	{
	public:
		size_t GetNumPolygons();
		long GetPolygon(size_t i) { return m_path[i]; }
		long FindNextPolygon(long index);

		//performs A* search to find optimal path to dest
		void buildPath(long sourcePolygon, long destPolygon, NavigationMesh* mesh);

	private:
		float GetCostEstimate(const NavMeshPolygon& source, const NavMeshPolygon& dest);
		PathFinderNode* HasNode(std::vector<PathFinderNode*>& list, const NavMeshPolygonLink& link);
		float GetCostTravelToRoot(const NavMeshPolygon& pn, PathFinderNode* node);
		void DestroyNodes(std::vector<PathFinderNode*>& list);

		//list of navmesh polygons we should traverse to arrive at the goal
		std::vector<long> m_path;
	};
}



#endif //__PathFinder_H__