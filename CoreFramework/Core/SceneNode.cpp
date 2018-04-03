
#include "SceneNode.h"
#include "WCamera.h"
#include "OctreeSceneNode.h"
#include "ResourceManager.h"
#include "RenderOptions.h"
#include "GodzAtomics.h"
#include <CoreFramework/Collision/PhysicsWorld.h>
#include <CoreFramework/Collision/PhysicsObject.h>
#include <CoreFramework/Collision/CollisionList.h>
#include <CoreFramework/Collision/ICollisionCallback.h>
#include <CoreFramework/Collision/RayCollider.h>

using namespace GODZ;
using namespace std;

SceneNode::SceneNode()
: parent(NULL)
, NodeFlags(0)
, m_bCanExpand(true)
{
}

SceneNode::~SceneNode()
{
	vector<SceneNode*>::iterator childIterator;
	for (childIterator=children.begin();childIterator!=children.end();childIterator++)
	{
		delete *childIterator;
	}
	children.empty();
}

bool SceneNode::AddEntity(PhysicsObject* entity)
{
	bool bResult = false;
	for (size_t i = 0; i < children.size(); i++)
	{
		bResult = children[i]->AddEntity(entity);
		if (bResult)
			return true;
	}

	return false;
}


void SceneNode::AddNode(SceneNode* node)
{
	//if the child is bigger than the parent, expand the parent bound
	godzassert(m_box.GetSize() > node->GetBounds().GetSize());

	node->parent=this;

	//This can be a thread read/write conflict if a child node is added while another thread is trying
	//to iterate through the children.
	children.push_back(node);
}

GODZ_API SceneNode* GODZ::CreateScene(ESceneNodeFlags sceneFlags, const WBox& Box)
{
	return new OctreeSceneNode(Box);
}

SceneNode* SceneNode::AddScene(ESceneNodeFlags sceneFlags, const WBox& Box)
{
	SceneNode* octree = CreateScene(sceneFlags, Box);
	AddNode(octree);
	return octree;
}

void SceneNode::Clear()
{
	vector<SceneNode *>::iterator childIterator;
	for (childIterator=children.begin();childIterator!=children.end();childIterator++)
	{
		SceneNode* s = *childIterator;
		delete s;
	}

	children.empty();

	m_objects.clear();
	children.clear();
}

bool SceneNode::ContainsEntity(PhysicsObject* entity)
{
	size_t size=m_objects.size();
	for(size_t i=0;i<size;i++)
	{
		if (entity == m_objects[i])
		{
			return true;
		}
	}

	return false;
}

void SceneNode::ContainsEntityWithinGraph(PhysicsObject* entity, udword &count)
{	
	if (ContainsEntity(entity))
	{
		count++;
	}

	size_t size = children.size();
	for(udword i = 0; i < size; i++)
	{
		SceneNode *tempNode = children[i];
		tempNode->ContainsEntityWithinGraph(entity, count);
	}
}

void SceneNode::GetActors(std::vector<PhysicsObject*>& RelevantActors)
{
	size_t count = m_objects.size(); //debugging
	for (size_t i=0;i<count;i++)
	{
		RelevantActors.push_back(m_objects[i]);
	}	

	vector<SceneNode*>::iterator nodes;
	for(nodes=children.begin();nodes!=children.end();nodes++)
	{
		SceneNode *scene = *nodes;
		scene->GetActors(RelevantActors);
	}
}


/*
void SceneNode::FindRadiusActors(SceneProximityQuery &query, PhysicsObject *pIgnore)
{
	query.nodes++;


	WBox box = GetBox();
	if (!box.IntersectsBox(query.m_searchBox.GetBox()))
		return;


	if (NodeFlags != SNF_Root) //ignore the level - this is a spatial search
	{
		size_t size=m_objects.size();
		for (size_t i = 0; i < size; i++)
		{
			PhysicsObject* e = m_objects[i];
			if (e != NULL && e != pIgnore)
			{
				//uses location instead of collision models so we can query lights,etc
				Vector3 location = e->GetLocation();
				Vector3 queryPos = query.m_searchBox.GetBox().GetCenter();
				Vector3 dir = queryPos - location;
				float dist = dir.GetLength();
				if (!query.m_fRadius || dist < query.m_fRadius) //only return light sources that fit
				{
					bool bFull = query.AddResult(e, dist);
					if (bFull)
						return;
				}
			}
		} //loop m_objects
	} //NodeFlags != SNF_Root

	vector<SceneNode*>::iterator childIterator;
	for(childIterator=children.begin();childIterator!=children.end();childIterator++)
	{
		SceneNode *child = *childIterator;
		child->FindRadiusActors(pClass, query, pIgnore);
	}
}
*/

/*
void SceneNode::GetRenderElements(std::vector<ActorID>& elements, WCamera *camera)
{
	//bottleneck: this has slow down per frame
	const WBox& Box = GetBox();
	
	//Use the Camera's Frustrum to test if this octant is visible.
	if (Box.IsValid() && (NodeFlags != SNF_Root) )
	{
		if (!camera->GetViewFrustum().IsBoxWithinFrustum(Box))
		{
			return;
		}

		//use render thread copy of the actor list
		size_t size = m_objects.size();
		for (size_t i=0; i < size; i++)
		{
			if (m_objects[i]!=NULL)
			{
				elements.push_back(m_objects[i]->GetRuntimeID());
			}
		}
	}

	//Have the children octants publish there entities....
	vector<SceneNode*>::iterator childIterator;
	for(childIterator=children.begin();childIterator!=children.end();childIterator++)
	{
		SceneNode *child = *childIterator;
		child->GetRenderElements(elements, camera);
	}
}
*/

size_t SceneNode::GetChildCount()
{
	return children.size();
}

ModelResource* SceneNode::GetModelResource()
{
	return modelResource;
}

SceneNode* SceneNode::GetParent()
{
	return parent;
}


ulong SceneNode::GetFlags()
{
	return NodeFlags;
}


SceneNode* SceneNode::FindNode(ESceneNodeFlags SceneNodeFlags)
{
	for (size_t i = 0; i < children.size(); i++)
	{
		if (children[i]->GetFlags() == SceneNodeFlags)
			return children[i];
	}

	return NULL;
}

SceneNode* SceneNode::GetRoot()
{
	SceneNode *root = this;
	while(root!=NULL)
	{
		if (root->GetParent() == NULL)
			break;

		root=root->GetParent();
	}

	return root;
}

void SceneNode::RenderChildren(SceneInfo& sceneInfo)
{
}

void SceneNode::SetParent(SceneNode *parent)
{
	this->parent=parent;
}

//note: called from serialize thread
void SceneNode::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	m_box.Serialize(ar);
	ar << m_bCanExpand;

	size_t size;

	if (ar.IsSaving())
	{
		size = children.size();
	}

	ar << size;

	if (!ar.IsSaving())
	{
		//create the children
		if (size > 0)
		{
			children.reserve(size);
			for (udword i=0;i<size;i++)
			{
				//add the child node to this scene.
				AddScene((ESceneNodeFlags)NodeFlags, EMPTY_BOX);
			}
		}
	}

	for (size_t i = 0; i < size; i++)
	{
		children[i]->Serialize(ar);
	}

	//TODO: serialize modelresources
}


//Query to see if the entity still belongs to this region
bool SceneNode::OnEntityMoved(PhysicsObject* entity)
{
	if (!IsEntityElgible(entity))
	{
		ReleaseEntity(entity);

		//ask root who does contain this guy....
		if (parent != NULL)
		{
			return parent->OnEntityMoved(entity);
		}
		else
		{
			//entity out of world bounds
			return false;
		}
	}

	if (entity->GetRegion() != this)
	{
		AddEntity(entity);
	}

	return true;
}


bool SceneNode::ReleaseEntity(PhysicsObject* entity)
{
	size_t size = m_objects.size();
	for(size_t i=0;i<size;i++)
	{
		if ( m_objects[i] == entity )
		{
			entity->SetRegion(NULL);
			m_objects[i] = NULL; //null out ref to the entity
			return true;
		}
	}

	return false;
}

void SceneNode::StoreElement(PhysicsObject* entity)
{
	entity->SetRegion(this);
	size_t size = m_objects.size();
	for(size_t i=0;i<size;i++)
	{
		if (m_objects[i] == NULL)
		{
			m_objects[i] = entity;
			return;
		}
	}

	
	m_objects.push_back(entity);
}

//shoots ray through all the scene nodes
void SceneNode::Trace(const WRay& ray, TraceResults& hitResults, PhysicsObject* ignore, bool sortResults)
{
	TraceResult result;

	if (m_box.IsValid() && !(NodeFlags & SNF_Root))
	{
		if (RayAABB(ray, m_box) ) //if ray pierces this scene node
		{
			//test internal entities & static meshes
			size_t size = m_objects.size();
			for(size_t i=0;i<size;i++)
			{
				PhysicsObject* actor = m_objects[i];

				if (actor!=NULL && actor != ignore)
				{
					CollisionPrimitive* model = actor->GetCollisionModel();
					if (model != NULL)
					{
						if (model->Trace(ray, result))
						{
							result.m_pHit = actor;
							result.m_distance = (ray.origin - result.hitLocation).GetLength();
							hitResults.InsertResult(result);
						}
					}
				}
			}
		}
	}

	vector<SceneNode*>::iterator childIter;
	for(childIter=children.begin();childIter!=children.end();childIter++)
	{
		(*childIter)->Trace(ray, hitResults, ignore, sortResults);
	}
}


bool SceneNode::SweepHit(CollisionPrimitive* primaryPrimitive, const Vector3& vel, CollisionResult& result, ICollisionCallback* pCallback, PhysicsObject* ignoredObject)
{
	if (m_box.IsValid() && !(NodeFlags & SNF_Root))
	{
		//CollisionList pList;

		WBox box;
		primaryPrimitive->GetBox(box);

		float u0, u1;
		
		if ( WBox::IntersectsMovingBox(box, vel, m_box, ZERO_VECTOR, u0, u1) )
		{
			//test internal entities & static meshes
			size_t size = m_objects.size();
			for(size_t i=0;i<size;i++)
			{
				result.Reset();

				//Don't collide a child with its parent & vice versa
				//(siblings should never collide so we dont check for that)
				PhysicsObject* actor = m_objects[i];
				if (actor==NULL || actor->IsParent( ignoredObject ) || ignoredObject != NULL && ignoredObject->IsParent(actor) )
				{
					continue;
				}

				CollisionPrimitive* model = actor->GetCollisionModel();
				if (model != NULL && actor != ignoredObject)
				{
					result.model = actor;

					Vector3 v;
					actor->GetVelocity(v);

					/*
					if (primaryPrimitive->GetName() == HString("Player"))
					{
						Vector3 dv;
						actor->GetVelocity(dv);
						Log("dv = %f %f %f [%d] %s\n", dv.x, dv.y, dv.z, primaryPrimitive->GetName().getHashCode(), primaryPrimitive->GetName().c_str() );
					}
					*/


					//pCallback->setPhysicsObjects( physicsObject, actor );

					//If we have valid shapes then collide the physics shapes directly
					if( primaryPrimitive->GetShape() != NULL && model->GetShape() != NULL 
						&& model->GetCollisionType() != CollisionPrimitive::CollisionType_AABBTree //hack: for now ignore AABBs
						&& primaryPrimitive->GetCollisionType() != CollisionPrimitive::CollisionType_AABBTree
						)
					{
						Vector3 p;
						Matrix3 rot;
						primaryPrimitive->GetTransform(p, rot);

						//get collision normals & save the result......
						if( actor->Collide(NULL, v, primaryPrimitive->GetShape(), p, vel, rot, rot, result) )
						{
							pCallback->OnCollisionOccured( result );

							if( pCallback->getEarlyBail() )
							{
								return true;
							}
						}
					}
					else if (primaryPrimitive->SweepHit(model, v, vel, result, pCallback))
					{
						if( pCallback->getEarlyBail( ) )
							return true;
					}
				}
			}
		}
	}

	vector<SceneNode*>::iterator childIter;
	for(childIter=children.begin();childIter!=children.end();childIter++)
	{
		if ( (*childIter)->SweepHit(primaryPrimitive, vel, result, pCallback, ignoredObject) )
		{
			return true; //abort
		}
	}

	return false;
}
