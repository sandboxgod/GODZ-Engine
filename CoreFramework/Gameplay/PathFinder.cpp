

#include "PathFinder.h"
#include <vector>


namespace GODZ
{

size_t NavigationPath::GetNumPolygons()
{
	return m_path.size();
}

//http://www.codeproject.com/KB/recipes/seunghyopback.aspx
//book AI game programming: chapter 3.1 Basic A* pathfinding
//http://www.policyalmanac.org/games/aStarTutorial.htm
//http://en.wikipedia.org/wiki/A*
/*
 function A*(start,goal)
     closedset := the empty set                 // The set of nodes already evaluated.     
     openset := set containing the initial node // The set of tentative nodes to be evaluated.
     g_score[start] := 0                        // Distance from start along optimal path.
     h_score[start] := heuristic_estimate_of_distance(start, goal)
     f_score[start] := h_score[start]           // Estimated total distance from start to goal through y.
     while openset is not empty
         x := the node in openset having the lowest f_score[] value
         if x = goal
             return reconstruct_path(came_from[goal])
         remove x from openset
         add x to closedset
         foreach y in neighbor_nodes(x)
             if y in closedset
                 continue
             tentative_g_score := g_score[x] + dist_between(x,y)
 
             if y not in openset
                 add y to openset
 
                 tentative_is_better := true
             elseif tentative_g_score < g_score[y]
                 tentative_is_better := true
             else
                 tentative_is_better := false
             if tentative_is_better = true
                 came_from[y] := x
                 g_score[y] := tentative_g_score
                 h_score[y] := heuristic_estimate_of_distance(y, goal)
                 f_score[y] := g_score[y] + h_score[y]
     return failure
 
 function reconstruct_path(current_node)
     if came_from[current_node] is set
         p = reconstruct_path(came_from[current_node])
         return (p + current_node)
     else
         return current_node
*/
void NavigationPath::buildPath(long source, long dest, NavigationMesh* mesh)
{
	m_path.clear();

	const NavMeshPolygon& pa = mesh->GetPolygon(source);
	const NavMeshPolygon& pb = mesh->GetPolygon(dest);

	//do A* search; build the path from source polygon to the dest polygon....
	PathFinderNode* startnode = new PathFinderNode;
	startnode->g = 0;
	startnode->h = GetCostEstimate(pa, pb);
	startnode->mPolyIndex = source;
	startnode->mLayerID = 0;

	std::vector<PathFinderNode*> openList;
	openList.push_back(startnode);

	// The set of nodes already evaluated. 
	std::vector<PathFinderNode*> closedList;

	while(openList.size() > 0)
	{
		//find the best Node B
		float best_f = 0;

		PathFinderNode* best = NULL;

		size_t num = openList.size();
		for(size_t i=0;i<num;i++)
		{
			if (best == NULL || openList[i]->GetFitness() < best_f)
			{
				best = openList[i];
				best_f = best->GetFitness();
			}
		}

		if (best->mPolyIndex == dest)
		{
			//now build the path from the best node [root] to the parent....
			std::vector<long> path;
			PathFinderNode* n = best;
			while(n != NULL)
			{
				path.push_back(n->mPolyIndex);
				n=n->parent;
			}

			//now reverse the list.... This way we go from source to goal....
			size_t num = path.size();

			if (num > 0)
			{				
				size_t max = num-1;
				for(size_t i=0;i<num;i++)
				{
					m_path.push_back(path[max-i]);
				}
			}

			DestroyNodes(openList);
			DestroyNodes(closedList);
			return;
		}

		//remove B from openSet and add to closed
		std::vector<PathFinderNode*>::iterator iter;
		for(iter = openList.begin(); iter != openList.end(); iter++)
		{
			if (*iter == best)
			{
				openList.erase(iter);
				break;
			}
		}

		closedList.push_back(best);

		//Now examine the sibling polygons
		const NavMeshPolygon& poly = mesh->GetPolygon(best->mPolyIndex);
		size_t numSiblings = poly.mSiblings.size();
		for(size_t i=0;i<numSiblings;i++)
		{
			if (poly.mSiblings[i].isStrongLink())
			{
				const NavMeshPolygon& tentative_succesor = mesh->GetPolygon(poly.mSiblings[i].index);
				
				if (HasNode(closedList, poly.mSiblings[i]) != NULL)
				{
					continue;
				}

				float g = best->g + (tentative_succesor.mCenter - poly.mCenter).GetLength();
				
				PathFinderNode* found = HasNode(openList, poly.mSiblings[i]);
				if ( found == NULL )
				{
					PathFinderNode* pn = new PathFinderNode;
					pn->parent = best;
					pn->h = GetCostEstimate(tentative_succesor, pb);
					pn->g = g;
					pn->mPolyIndex = (long)poly.mSiblings[i].index;
					pn->mLayerID = 0;

					openList.push_back(pn);
				}
				else if (g < found->g)
				{
					//update path
					found->g = g;
					found->parent = best;
					found->h = GetCostEstimate(tentative_succesor, pb);
					found->mPolyIndex = (long)poly.mSiblings[i].index;
				}
			}
		}
	}

	DestroyNodes(openList);
	DestroyNodes(closedList);
}

float NavigationPath::GetCostTravelToRoot(const NavMeshPolygon& pn, PathFinderNode* node)
{
	float g = node->g;
	PathFinderNode* n = node;
	while(n != NULL)
	{
		g += n->g;
		n=n->parent;
	}

	return g;
}


float NavigationPath::GetCostEstimate(const NavMeshPolygon& source, const NavMeshPolygon& dest)
{
	return (source.mCenter - dest.mCenter).GetLength();
}

PathFinderNode* NavigationPath::HasNode(std::vector<PathFinderNode*>& list, const NavMeshPolygonLink& link)
{
	std::vector<PathFinderNode*>::iterator iter;
	for(iter = list.begin(); iter != list.end(); iter++)
	{
		PathFinderNode* n = *iter;
		if (n->mLayerID == link.mLayerID && n->mPolyIndex == link.index)
		{
			return n;
		}
	}

	return NULL;
}

void NavigationPath::DestroyNodes(std::vector<PathFinderNode*>& list)
{
	std::vector<PathFinderNode*>::iterator iter;
	for(iter = list.begin(); iter != list.end(); iter++)
	{
		PathFinderNode* n = *iter;
		delete n;
	}

	list.clear();
}

long NavigationPath::FindNextPolygon(long index)
{
	size_t num = m_path.size();
	for(size_t i=0;i<num;i++)
	{
		if (m_path[i] == index)
		{
			if (i + 1 < num)
			{
				return m_path[i+1];
			}
			else
				return -1;
		}
	}

	return -1;
}

}
