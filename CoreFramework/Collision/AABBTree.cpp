
#include "AABBTree.h"
#include "CollisionList.h"
#include "ICollisionCallback.h"
#include "RayCollider.h"
#include <CoreFramework/Core/CoreFramework.h>
#include <CoreFramework/Core/MemoryMngr.h>
#include <CoreFramework/Core/MeshInstance.h>
#include <CoreFramework/Core/ModelResource.h>
#include <CoreFramework/Core/Mesh.h>
#include <CoreFramework/Core/WEntity.h>
#include <CoreFramework/Math/vectormath.h>
#include <CoreFramework/Math/WPlane.h>
#include <CoreFramework/Math/BulletMathUtil.h>
#include <CoreFramework/ozlib/intr_spheretri.h>
#include <CoreFramework/Collision/PhysicsObject.h>
#include <CoreFramework/Collision/PhysicsWorld.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>
#include <BulletCollision/CollisionShapes/btConvexHullShape.h>

using namespace GODZ;
using namespace std;



AABBTreeInfo::AABBTreeInfo(udword depth, udword min_num_verts, bool bPrune)
: right_children(0)
, left_children(0)
, curr_max_depth(0)
, m_bPrune(0)
{
	max_tree_depth=depth;
	min_vertices=min_num_verts;
	m_bPrune=bPrune;
}

bool AABBTreeInfo::IsBalanced()
{
	return right_children==left_children;
}


////////////////////////////////////////////////////////////////////////////////////

AABBNode::AABBNode()
: m_nVerts(0)
, m_VB(NULL)
, m_plane(NULL)
{
	for(int i=0;i<2;i++)
	{
		children[i] = NULL;
	}
}

AABBNode::AABBNode(const AABBNode* pNode)
: m_nVerts(0)
, m_VB(NULL)
, m_plane(NULL)
{
	m_box = pNode->m_box;
	m_nVerts=pNode->m_nVerts;

	if (m_nVerts > 0)
	{
		m_VB=new Vector3[pNode->m_nVerts];
	}

	for(size_t i=0;i<m_nVerts;i++)
	{
		m_VB[i] = pNode->m_VB[i];
	}

	//generate the planes; there is 1 plane per triangle (3 vertices)
	if (m_nVerts > 0)
	{
		m_plane = new WPlane[m_nVerts/3];
	}

	size_t planeIdx= 0;
	for (udword i=0;i<m_nVerts;i+=3)
	{
		m_plane[planeIdx].BuildPlane(m_VB[i],m_VB[i+1],m_VB[i+2]); 
		planeIdx++;
	}

	//clone children (creates new instances for each child)
	for(int i=0;i<2;i++)
	{
		if (pNode->children[i])
		{
			//clone the child node
			children[i] = new AABBNode(pNode->children[i]);
		}
		else
		{
			children[i] = NULL;
		}
	}
}

AABBNode::AABBNode(vector<Vector3> vertexList, AABBTreeInfo &treeInfo,  udword depth)
: m_VB(NULL)
, m_nVerts(0)
, m_plane(NULL)
{
	children[0]=0;
	children[1]=0;
	BuildTree(vertexList, treeInfo, depth);
}

AABBNode::~AABBNode()
{
	//if not a clone, free the vertex buffer
	if (m_VB != NULL)
	{
		delete[] m_VB;
		m_VB=0;
	}

	//if not a clone, free the plane buffer
	if (m_plane != NULL)
	{
		delete[] m_plane;
		m_plane=0;
	}

	for(int i=0;i<2;i++)
	{
		children[i] = NULL;
	}
}

GenericReference<CollisionPrimitive> AABBNode::GetClone()
{
	return new AABBNode(this);
}

int AABBNode::FindBestAxis(std::vector<Vector3> &v)
{
	//THERES SOME BUG in this code- makes the AABB Tree bug out- cant find
	//triangles for collision tests
	size_t vertexNum=v.size();

	//divide this box into two boxes - pick a better axis
	int iAxis=0;
	int iAxisResult[3]; //stores how close end result is, the lower the better

	Vector3 center = m_box.GetCenter();

	for (iAxis=0;iAxis<3;iAxis++)
	{
		int left=0,right=0;

		for(udword i=0;i<vertexNum;i+=3)
		{		
			float faceCenter[3];
			faceCenter[0] = (v[i].x + v[i+1].x + v[i+2].x) / 3.0f;
			faceCenter[1] = (v[i].y + v[i+1].y + v[i+2].y) / 3.0f;
			faceCenter[2] = (v[i].z + v[i+1].z + v[i+2].z) / 3.0f;
		
			if (faceCenter[iAxis] <= center[iAxis])
			{
				left++;
			}
			else
			{
				right++;
			}
		} // vertices

		iAxisResult[iAxis] = abs(left-right);
	} //axis

	int index=0;
	int result=iAxisResult[0];
	for (int i=1;i<3;i++)
	{
		if (iAxisResult[i] < result)
		{
			result = iAxisResult[i];
			index=i;
		}
	}

	//Log("result: %d\n", iAxisResult[index]);

	return index;
}

void AABBNode::BuildTree(vector<Vector3> v, AABBTreeInfo &treeInfo, udword depth) //, HeapUnit *pUnit
{
	size_t vertexNum=v.size();

	if (vertexNum == 0)
	{
		WBox box(Vector3(0,0,0), Vector3(0,0,0));
		SetBounds(box);
		return;
	}

	// Build the node bounding box based from the triangle list	
	WBox Box(&v[0], vertexNum);
	SetBounds(Box);

	if (depth+1 > treeInfo.curr_max_depth)
		treeInfo.curr_max_depth=depth+1;

	bool bMakeChildren=false;

	if (vertexNum > treeInfo.min_vertices && (depth<treeInfo.max_tree_depth || treeInfo.max_tree_depth == 0))
	{
		bMakeChildren=true;
	}

	if (bMakeChildren)
	{
        // Find the longest axii of the node's box
		int iAxis=Box.GetLongestAxis();
		Vector3 center = Box.GetCenter();

		vector<Vector3> leftSide;
		vector<Vector3> rightSide;
		
		//btw, things that could go wrong- if the mesh doesn't send over the triangles
		//correctly, then you might see huge boxes that are misaligned (bad leaves).
		//things to check is making sure the vertex buffer is correctly aligned along
		//the adjancey buffers, etc
		for(size_t i=0;i<vertexNum;i+=3)
		{
			//Log("vert %f %f %f\n", v[count].x, v[count].y, v[count].z);

				//float faceCenter = ComputeFaceCenter(v, iAxis); //debug friendly :)
				//Log("facecenter %f fsplit %f\n", faceCenter, fSplit);

				float faceCenter[3];
				faceCenter[0] = (v[i].x + v[i+1].x + v[i+2].x) / 3.0f;
				faceCenter[1] = (v[i].y + v[i+1].y + v[i+2].y) / 3.0f;
				faceCenter[2] = (v[i].z + v[i+1].z + v[i+2].z) / 3.0f;
				//Vector3 faceCenter(x,y,z);

				if (faceCenter[iAxis] <= center[iAxis]) //fSplit
				{
					size_t idx = leftSide.size();

					//Store the verts to the left.
					leftSide.push_back(v[i]);
					leftSide.push_back(v[i+1]);
					leftSide.push_back(v[i+2]);
				}
				else
				{
					//Store the verts to the right.
					rightSide.push_back(v[i]);
					rightSide.push_back(v[i+1]);
					rightSide.push_back(v[i+2]);
				}
		}


		//size_t total = leftSide.size() + rightSide.size();
		if (leftSide.size() > treeInfo.min_vertices && rightSide.size() > treeInfo.min_vertices)
		{
			godzassert(leftSide.size() % 3 == 0);
			godzassert(rightSide.size() % 3 == 0);

			//Build child nodes

			treeInfo.left_children++;
			//Log("Created Left AABBNode for depth %d verts %d\n",depth, leftSide.size());
			//children[0] = new(pUnit) AABBNode(leftSide, treeInfo, pUnit, depth+1);
			children[0] = new AABBNode(leftSide, treeInfo, depth+1);
			godzassert(children[0] != NULL);
			godzassert(m_box.ContainsBox(children[0]->m_box));

			treeInfo.right_children++;
			//Log("Created Right AABBNode for depth %d verts %d\n",depth, rightSide.size());
			//children[1] = new(pUnit) AABBNode(rightSide, treeInfo, pUnit, depth+1);
			children[1] = new AABBNode(rightSide, treeInfo, depth+1);
			godzassert(children[1] != NULL);
			godzassert(m_box.ContainsBox(children[1]->m_box));
			
		}
		else
		{
			//I hate this situation, happens very rarely now. doesn;t happen at all if
			//we're pruning.
			bMakeChildren=false;
		}
	}
	
	if (!bMakeChildren)
	{

		//Log("*************************************************\n");
		//Log("Storing data to AABBNode for depth %d verts %d box %f %f %f max: %f %f %f\n",depth, vertexNum,Box.min.x,Box.min.y,Box.min.z,Box.max.x,Box.max.y,Box.max.z);
		
		//Store the data directly
		m_VB=new Vector3[vertexNum];
//		m_VB = (Vector3*)pUnit->Allocate((sizeof(Vector3) * vertexNum));
		//memcpy(&m_VB[0], &v[0], sizeof(Vector3) * vertexNum);
		m_nVerts=vertexNum;

		for(size_t i=0;i<vertexNum;i++)
		{
			m_VB[i] = v[i];
			godzassert(m_box.ContainsPoint(m_VB[i]));
		}

		//generate the planes
		m_plane = new WPlane[vertexNum/3];
		godzassert(m_plane != NULL);

		size_t planeIdx= 0;
		for (udword i=0;i<vertexNum;i+=3)
		{
			m_plane[planeIdx].BuildPlane(m_VB[i],m_VB[i+1],m_VB[i+2]); 
			//float size = m_plane[planeIdx].N.GetLength();
			//godzassert(size > 0.99f && size < 1.1f);

			planeIdx++;
		}
	}
}

void SetHitLocation(const WBox& box, Vector3 vb[3], CollisionResult &result)
{
	//Log("bottom-box %f %f %f\n",box.GetBottom().x,box.GetBottom().y,box.GetBottom().z);

	//find closest point on triangle to the bottom of the collision box
	result.m_hitLocation = ClosestPtPointTriangle(box.GetBottom(), vb[0], vb[1], vb[2]);
	//Log("tri(a) %f %f %f\n",vb[0].x,vb[0].y,vb[0].z);
	//Log("tri(b) %f %f %f\n",vb[1].x,vb[1].y,vb[1].z);
	//Log("tri(c) %f %f %f\n",vb[2].x,vb[2].y,vb[2].z);
}

inline bool getLowestRoot(f32 a, f32 b, f32 c, f32 maxR, f32* root)
{
	// check if solution exists
	f32 determinant = b*b - 4.0f*a*c;

	// if determinant is negative, no solution
	if (determinant < 0.0f) return false;

	// calculate two roots: (if det==0 then x1==x2
	// but lets disregard that slight optimization)

	f32 sqrtD = (f32)sqrt(determinant);
	f32 r1 = (-b - sqrtD) / (2*a);
	f32 r2 = (-b + sqrtD) / (2*a);

	// sort so x1 <= x2
	if (r1 > r2) { f32 tmp=r2; r2=r1; r1=tmp; }

	// get lowest root
	if (r1 > 0 && r1 < maxR)
	{
		*root = r1;
		return true;
	}

	// its possible that we want x2, this can happen if x1 < 0
	if (r2 > 0 && r2 < maxR)
	{
		*root = r2;
		return true;
	}

	return false;
}

void SetPlaneDistance(const WSphere& s, const Vector3& velocity, CollisionResult& result)
{
	Vector3 normalizedVelocity = velocity;
	normalizedVelocity.Normalize();
	normalizedVelocity.x = -normalizedVelocity.x;
	normalizedVelocity.y = -normalizedVelocity.y;
	normalizedVelocity.z = -normalizedVelocity.z;
	result.m_distance = 0.0f;
	intersectRaySphere(result.m_hitLocation, normalizedVelocity, s.center, s.radius, result.m_distance);
}

bool AABBNode::SweepHit(Vector3 triangle[3], const Vector3& velocity, CollisionResult& result)
{
	godzassert(0); //not applicable to AABBNodes...

	return false;
}

bool AABBNode::SweepHit(WMatrix16f& invMat, CollisionPrimitive* otherPrimitive, const Vector3& primVelocity, const Vector3& nodeVelocity, CollisionResult& result, ICollisionCallback* call)
{
	//TODO:
	//PhysicsObject* physicsObject = call->getPhysicsObject1();
	Matrix3 initMat(1);


	//if ( physicsObject->Collide( &invMat, primVelocity, m_boxShape, Vector3::Zero(), nodeVelocity, initMat, initMat, result ) )

	CollisionList pList;
	pList.SetBailOnContact(true);
	AABBCollisionPrimitive::SweepHit(otherPrimitive, primVelocity, nodeVelocity, result, &pList);

	if (pList.GetNumResults() > 0) //if this node was hit
	{
		int countHit = 0;
		if (IsLeaf())
		{
			//check the triangles within this node
			for(size_t i=0;i<m_nVerts;i+=3)
			{
				btVector3 triPts1( m_VB[i].x, m_VB[i].y, m_VB[i].z );
				btVector3 triPts2( m_VB[i+1].x, m_VB[i+1].y, m_VB[i+1].z );
				btVector3 triPts3( m_VB[i+2].x, m_VB[i+2].y, m_VB[i+2].z );

				//if (otherPrimitive->SweepHit(triPts, primVelocity, result))
				
				btTriangleShape triangleConvexShape( triPts1, triPts2, triPts3 );
				triangleConvexShape.setMargin(0.01f);

				//TODO: Need proper angular velocity of the primitive as well....
				Vector3 p;
				Matrix3 rot;
				otherPrimitive->GetTransform(p, rot);

				//if ( physicsObject->Collide( &invMat, primVelocity, &triangleConvexShape, Vector3::Zero(), nodeVelocity, initMat, initMat, result ) )
				if( call->getPhysicsWorld()->Collide( otherPrimitive->GetShape(), p, primVelocity, rot, rot, &triangleConvexShape, Vector3::Zero(), nodeVelocity, initMat, initMat, result ) )
				{
					//Override the hit normal returned because for collide-and-slide it's
					//better to use the actual surface normal. We might be trying to sweep
					//the primitive forward however a triangle that faces upward gets hit
					int planeIndex = i / 3;
					result.m_plane = m_plane[planeIndex];
					result.m_normal = result.m_plane.N;		

					/*
					if( result.m_normal.z < -0.5 || result.m_normal.z > 0.5)
					{

						Matrix3 m;
						Vector3 p;
						physicsObject->GetWorldTransform(p, m);
						Log("main-pos %f %f %f\n", p.x, p.y, p.z );

						Log("aabb tree d: %f vel: %f \n", result.m_distance, primVelocity.GetLength() );

						Log("hit triangle1 %f %f %f\n", triPts1.x(), triPts1.y(), triPts1.z() );
						Log("hit triangle2 %f %f %f\n", triPts2.x(), triPts2.y(), triPts2.z() );
						Log("hit triangle3 %f %f %f\n", triPts3.x(), triPts3.y(), triPts3.z() );

						Log("result normal %f %f %f\n", result.m_normal.x, result.m_normal.y, result.m_normal.z );

						//result.m_plane = m_plane[planeIndex];
						Log("plane normal %f %f %f\n", m_plane[planeIndex].N.x, m_plane[planeIndex].N.y, m_plane[planeIndex].N.z );
					}
					*/	

					call->OnCollisionOccured(result); //notify callback interface about each surface
					if( call->getEarlyBail() )
					{
						return true; //send back true only for early bails
					}
					countHit++;
				}
			}
		}

		//Go through children
		AABBNode* left = GetLeftChild();
		AABBNode* right = GetRightChild();

		if (left && left->SweepHit(invMat, otherPrimitive, primVelocity, nodeVelocity, result, call))
		{
			if( call->getEarlyBail() )
			{
				return true; //send back true only for early bails
			}
			countHit++;
		}

		if (right && right->SweepHit(invMat, otherPrimitive, primVelocity, nodeVelocity, result, call))
		{
			if( call->getEarlyBail() )
			{
				return true; //send back true only for early bails
			}
			countHit++;
		}

		return countHit > 0;
	}

	return false;
	
	 

/*
	CollisionList pList;
	pList.SetBailOnContact(true);
	AABBCollisionPrimitive::SweepHit(otherPrimitive, primVelocity, nodeVelocity, result, &pList);

	if (pList.GetNumResults() > 0) //if this node was hit
	{
		int countHit = 0;
		if (IsLeaf())
		{
			//check the triangles within this node
			for(size_t i=0;i<m_nVerts;i+=3)
			{
				Vector3 triPts[3];
				triPts[0].x = m_VB[i].x;
				triPts[0].y = m_VB[i].y;
				triPts[0].z = m_VB[i].z;

				triPts[1].x = m_VB[i+1].x;
				triPts[1].y = m_VB[i+1].y;
				triPts[1].z = m_VB[i+1].z;

				triPts[2].x = m_VB[i+2].x;
				triPts[2].y = m_VB[i+2].y;
				triPts[2].z = m_VB[i+2].z;

				if (otherPrimitive->SweepHit(triPts, primVelocity, result))
				{
					call->OnCollisionOccured(result); //notify callback interface about each surface
					if( call->getEarlyBail() )
					{
						return true; //send back true only for early bails
					}
					countHit++;
				}
			}
		}

		//Go through children
		AABBNode* left = GetLeftChild();
		AABBNode* right = GetRightChild();

		if (left && left->SweepHit(invMat, otherPrimitive, primVelocity, nodeVelocity, result, call))
		{
			if( call->getEarlyBail() )
			{
				return true; //send back true only for early bails
			}
			countHit++;
		}

		if (right && right->SweepHit(invMat, otherPrimitive, primVelocity, nodeVelocity, result, call))
		{
			if( call->getEarlyBail() )
			{
				return true; //send back true only for early bails
			}
			countHit++;
		}

		return countHit > 0;
	}

	return false;
	*/

}


AABBNode* AABBNode::GetLeftChild()
{
	return children[0];
}

AABBNode* AABBNode::GetRightChild()
{
	return children[1];
}

bool AABBNode::Intersects(CollisionPrimitive* other)
{
	bool bBoxCollision = AABBCollisionPrimitive::Intersects(other);
	if (!bBoxCollision)
		return false;

	if (IsLeaf()) //if this is a leaf
	{
		
		if (m_nVerts==3) //we built a box around one triangle so it is accurate
			return true;

		//get the collision box
		WBox box;
		other->GetBox(box);

		//check the triangles within this node to make sure we are really colliding
		for(size_t i=0;i<m_nVerts;i+=3)
		{			
			bool bHit = TriBoxOverlap(box.GetCenter(), box.GetExtent(), &m_VB[i]);
			if (bHit)
			{
				return true;
			}
		}

		return false;
		

		//return true;
	}

	for(int i=0;i<2;i++)
	{
		if (children[i]->Intersects(other))
			return true;
	}
	return false;
}

bool AABBNode::IsHitBy(const WRay& ray, TraceResult& result)
{
	WBox box;
	GetBox(box);

	if (!RayAABB(ray, box))
		return false;

	if (IsLeaf())
	{
		if (m_nVerts>3)
		{
			for(udword i=0;i<m_nVerts;i+=3)
			{
				CollisionFace face;
				bool bHit = RayTriOverlap(ray,m_VB[i],m_VB[i+1],m_VB[i+2],face);
				if (bHit)
				{
					return true;
				}
			}
		}
	}

	AABBNode* leftChild = GetLeftChild();
	AABBNode* rightChild = GetRightChild();

	if (leftChild!=NULL && leftChild->IsHitBy(ray,result))
		return true;

	if (rightChild != NULL && rightChild->IsHitBy(ray,result))
		return true;

	return false;
}

bool AABBNode::IsLeaf()
{
	return GetLeftChild() == 0 && GetRightChild() == 0;
}

void AABBNode::Serialize(GDZArchive& ar) //, HeapUnit *pUnit
{
	AABBCollisionPrimitive::Serialize(ar);

	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	//Log("box min: %f %f %f max: %f %f %f size: %d\n", m_box.min.x, m_box.min.y, m_box.min.z, m_box.max.x, m_box.max.y, m_box.max.z, size);
	
	//serialize the left side
	bool bLeft=false;
	if (ar.IsSaving())
	{
		AABBNode* left = children[0];
		bLeft = left != NULL;
	}

	ar << bLeft;

	if (bLeft)
	{
		if (!ar.IsSaving())
		{
			//children[0] = new(pUnit) AABBNode;
			children[0] = new AABBNode;
		}

		//Log("left: %d\n", bLeft);
		children[0]->Serialize(ar); //pUnit
	}

	//serialize the right side
	bool bRight=0;
	if (ar.IsSaving())
	{
		AABBNode* right = children[1];
		bRight = right != NULL;
	}

	ar << bRight;

	if (bRight)
	{
		if (!ar.IsSaving())
		{
			//children[1] = new(pUnit) AABBNode;
			children[1] = new AABBNode;
		}

		//Log("right: %d\n", bRight);

		children[1]->Serialize(ar); //pUnit
	}

	
	ar << m_nVerts;

	if (!ar.IsSaving())
	{
		m_VB = new Vector3[m_nVerts];
		//m_VB = (Vector3*)pUnit->Allocate(sizeof(Vector3) * m_nVerts);

		m_plane = new WPlane[m_nVerts/3];
	}

	for(size_t i=0;i<m_nVerts;i++)
	{
		ar << m_VB[i];
	}

	for(size_t i=0;i<m_nVerts/3;i++)
	{
		ar << m_plane[i];
	}
}

bool AABBNode::Trace(const WRay& ray, TraceResult& result)
{
	if (AABBCollisionPrimitive::Trace(ray, result))
	{
		float bestDist = 0;
		udword faces = 0;

		//now find the face that was stabbed by the ray trace
		for (udword i=0;i<m_nVerts;i+=3)
		{
			//WPlane plane(m_VB[i], m_VB[i+1], m_VB[i+2]); //TODO: We need planes for these AABBNodes
			Vector3 hit;
			//bool bStabbed = plane.RayIntersection(ray.origin, ray.dir, hit);

			//see which triangle the ray intersects with
			float t,u,v;
			bool bStabbed = intersect_triangle(ray.origin, ray.dir, m_VB[i], m_VB[i+1], m_VB[i+2], &t, &u, &v);
			if (bStabbed)
			{
				//get hit location on the triangle
				//WPlane plane(m_VB[i], m_VB[i+1], m_VB[i+2]); 

				WPlane* plane = &m_plane[i/3];
				bool bStabbed2 = plane->RayIntersection(ray.origin, ray.dir, t, hit); 
				if(bStabbed == bStabbed2) { //make sure results always matches

					float dist = (ray.origin - hit).GetLength();
					if (faces == 0 || dist < bestDist)
					{
						bestDist = dist;
						result.hitLocation = hit;
						//result.mFace.mDistance = dist;
						result.m_normal = plane->N;
					}
					faces++;
				}
			}
		}

		return faces>0;
		
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////

AABBRoot::AABBRoot()
: AABBNode()
, m_tm(1)
, mInverse(1)
{
}

AABBRoot::AABBRoot(const AABBRoot* root)
: AABBNode(root)
{
	m_tm = root->m_tm;
	mInverse = root->mInverse;

	// Create physics shapes
	SetBounds( root->GetBounds() );
}

AABBRoot::AABBRoot(std::vector<Vector3> triangles, AABBTreeInfo& info,  udword depth)
: AABBNode(triangles, info, depth)
, m_tm(1)
, mInverse(1)
{
}

bool AABBRoot::Trace(const WRay& ray, TraceResult& result)
{
	//convert ray into local space
	WRay modelRay = ray;
	mInverse.Mul(modelRay.origin);
	return AABBNode::IsHitBy(modelRay, result);
}

bool AABBRoot::SweepHit(CollisionPrimitive* other, const Vector3& vel, const Vector3& otherVel, CollisionResult& result, ICollisionCallback* call)
{
	//TODO: Handle AABBTree vs AABBTree!!!
	godzassert(other->GetCollisionType() != GetCollisionType());

	//transform the model into object space
	other->TransformBy(mInverse);
	bool ret = AABBNode::SweepHit(mInverse, other, vel, otherVel, result, call);

	//NOTE: If our m_tm ever got out of sync with the inverse matrix the line below
	//would screw up the other primtive really bad.....

	//transform back to world
	other->TransformBy(m_tm);

	return ret;
}

void AABBRoot::TransformBy(const WMatrix16f& tm)
{
	m_tm = m_tm * tm;
	m_tm.Inverse(mInverse);
}

void AABBRoot::SetTransform(const WMatrix& m)
{
	m_tm = m;
	m_tm.Inverse(mInverse);
}

void AABBRoot::SetTransform(const Vector3& pos, const WMatrix3& matrix)
{
	CopyMatrix(matrix, m_tm);
	m_tm.SetTranslation(pos);
	m_tm.Inverse(mInverse);
}

void AABBRoot::GetBox(WBox& box) const
{
	//get our bounds (object space)
	box = m_box;

	//Transform to World
	box.Transform(m_tm);
}

void AABBRoot::GetSphere(WSphere& s) const
{
	//get our bounds (object space)
	s = m_box;

	//Transform to World
	m_tm.Mul(s.center);
}

void AABBRoot::GetTransform(Vector3& pos, WMatrix3& m) const
{
	pos = m_tm.GetTranslation();
	m = m_tm;
}

void AABBRoot::GetTransform(WMatrix& m) const
{
	m = m_tm;
}

GenericReference<CollisionPrimitive> AABBRoot::GetClone()
{
	return new AABBRoot(this);
}

//note: called from serialize thread
void AABBRoot::Serialize(GDZArchive& ar)
{
	AABBNode::Serialize(ar);

	WMatrix m;

	if (ar.IsSaving())
	{
		m = m_tm;
	}

	ar << m;

	if (!ar.IsSaving())
	{
		TransformBy(m);
	}
}

//////////////////////////////////////////////////////////////////////////////////


AABBTreeBuilder::AABBTreeBuilder(WEntity* pent, AABBTreeInfo& treeInfo)
{
	//Intended for static meshes
	Mesh* mesh = pent->GetMesh();
	MeshInstance* mi = mesh->GetDefaultInstance();

	Matrix4 m(1);
	m.SetScale(pent->GetDrawScale());
	m.SetTranslation(pent->GetLocation());

	vector<Vector3> list;

	MeshInstance::MeshMap& map = mi->GetMeshMap();
	MeshInstance::MeshMap::iterator iter;

	for(iter = map.begin(); iter != map.end(); iter++)
	{
		ModelContainer* mc = iter->second;
		size_t numModels = mc->size();
		for (size_t j = 0; j < numModels; j++)
		{
			ModelResource* resource = mc->get(j);
			size_t size = resource->GetNumIndicies();

			for (size_t i=0; i < size;i+=3)
			{
				int idx0 = resource->GetIndex(i);
				int idx1 = resource->GetIndex(i+1);
				int idx2 = resource->GetIndex(i+2);

				BaseVertex* bv0 = resource->GetVertex(idx0);
				BaseVertex* bv1 = resource->GetVertex(idx1);
				BaseVertex* bv2 = resource->GetVertex(idx2);

				WPlane plane(bv0->pos, bv1->pos, bv2->pos);

				if (plane.N.GetLength() > 0)
				{
					//Transform position vector by entity location
					Vector3 v1 = bv0->pos;
					Vector3 v2 = bv1->pos;
					Vector3 v3 = bv2->pos;

					m.Mul(v1);
					m.Mul(v2);
					m.Mul(v3);

					list.push_back(v1);
					list.push_back(v2);
					list.push_back(v3);
				}
				else
				{
					Log("Face %d has an invalid plane\n", i);
				}
			}	//loop i
		}
	} //loop models

	root = new AABBRoot(list, treeInfo);
	this->treeInfo = treeInfo;
}


AABBTreeBuilder::AABBTreeBuilder(ModelResource* resource, AABBTreeInfo& treeInfo)
{
	godzassert(resource->GetNumVertices() > 0);

	vector<Vector3> list;

	size_t size = resource->GetNumIndicies();

	for (size_t i=0; i < size;i+=3)
	{
		UInt16 idx0 = resource->GetIndex(i);
		UInt16 idx1 = resource->GetIndex(i+1);
		UInt16 idx2 = resource->GetIndex(i+2);

		BaseVertex* bv0 = resource->GetVertex(idx0);
		BaseVertex* bv1 = resource->GetVertex(idx1);
		BaseVertex* bv2 = resource->GetVertex(idx2);

		WPlane plane(bv0->pos, bv1->pos, bv2->pos);

		if (plane.N.GetLength() > 0)
		{
			list.push_back(bv0->pos);
			list.push_back(bv1->pos);
			list.push_back(bv2->pos);
		}
		else
		{
			Log("Face %d has an invalid plane\n", i);
		}
	}

	//Build the AABB Tree
	root = new AABBRoot(list, treeInfo);
	this->treeInfo = treeInfo;
}

AABBTreeBuilder::AABBTreeBuilder(std::vector<Vertex*>& vb, AABBTreeInfo& heuristic)
{
	vector<Vector3> list;
	size_t size = vb.size();

	for (udword i=0; i < size;i++)
	{
		list.push_back(vb[i]->pos);
	}

	//Build the AABB Tree
	root = new AABBRoot(list, heuristic);
	this->treeInfo = heuristic;
}

AABBNode* AABBTreeBuilder::GetRootNode()
{
	return root;
}



