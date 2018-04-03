/* ===========================================================
	WEntity

	Represents an entity that exists at a location in world
	space.

	Romans 8:28 'More Than Conquerors'
	"And we know that in all things God works for the good of
	those who love him, who have been called according to his
	pupose."
	
	Romans 8:31
	"What, then, shall we say in response to this? If God is
	for us, who can be against us?"

	Copyright (c) 2010
	========================================================== 
*/

#if !defined(_WENTITY_H_)
#define _WENTITY_H_

#include "GenericObject.h"
#include "GDZArchive.h"
#include "RenderQueue.h"
#include "atomic_ref.h"
#include "Mesh.h"
#include "EntityComponent.h"
#include "SlotIndex.h"
#include <CoreFramework/Collision/PhysicsObjectProxy.h>
#include <CoreFramework/Math/WMatrix.h>
#include <CoreFramework/Math/WMatrix3.h>
#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/Math/AxisAngle.h>
#include <CoreFramework/Math/EulerAngle.h>
#include "EntityTemplate.h"

namespace GODZ
{
	//forward declar
	class AABBTree;
	class AnimController;
	class AnimSequence;
	class Layer;
	class LevelNode;
	class WPhysicsVolume;
	class PhysicsObject;
	class MActor;
	class Skeleton;

	//Default scale of an entity
	static const Vector3 DEFAULT_SCALE=Vector3(1.0f,1.0f,1.0f);

	enum PhysicsUpdateType
	{
		PhysicsUpdate_Position,		// Means we only use position updates
		PhysicsUpdate_PosRotation,	// Physics will control both Position and Rotation
	};

	// An object that is bound to world space.
	class GODZ_API WEntity: public GenericObject
	{
		DeclareGeneric(WEntity, GenericObject)

	public:
		WEntity();
		~WEntity();

		void					SetPhysicsUpdateType(PhysicsUpdateType type) { m_physicsType = type; }
		PhysicsUpdateType		GetPhysicsUpdateType() const { return m_physicsType; }

		void					AttachActor(WEntity* entity);
		WEntity*				GetAttachment(size_t index);
		size_t					GetNumAttachments() { return m_attachedActors.size(); }

		bool					GetCanCollideWithWorld();
		void					SetCanCollideWithWorld(bool collide);

		//Creates a collision model for this entity.
		MotionType				CreateCollisionModel();

		//notify from level this entity has been reaped
		virtual void			RemoveFromWorld();

		//Returns the collision tree this entity is using (if the mesh uses static collision)
		AABBTree*				GetCollTree();

		//Returns the animation controller used by this actor
		AnimController*			GetAnimController();

		//Adjust the draw scale of an object
		const Vector3&			GetDrawScale() const;
		void					SetDrawScale(const Vector3& scale);

		//Returns the Collison model
		CollisionPrimitive*		GetCollisionModel();
		void					ResetCollision();

		//Level this entity is apart of
		LevelNode*				GetLevel();
		void					SetLevel(LevelNode* level);

		//returns the 3x3 pure rotation matrix
		const WMatrix3&			GetOrientationMatrix();
		void					SetOrientationMatrix(const WMatrix3& m);

		const WMatrix3&			GetRotation();
		void					SetRotation(const WMatrix3& m);

		//returns concatenated S*R*T matrix
		const Matrix4&			GetRenderMatrix() { return m_renderMat; }

		Skeleton*				GetSkeleton() { return m_pSkeleton; }

		//returns the mesh instance
		MeshInstance*			GetMeshInstance() { return m_pMeshInstance; }

		//Returns this entity's location within world space
		const Vector3&			GetLocation() const;
		
		//Moves this actor to the desired position
		void					SetLocation(const Vector3& NewLocation);

		//Returns true if this entity can be rendered
		bool					IsVisible();
		void					SetVisible(bool isVisible);

		//Event Notification that this entity is out-of-bounds.
		virtual void			NotifyOutsideOfScene();

		//Notification animation sequence has ended
		virtual void			OnAnimEnd(AnimSequence* pSequence);

		//Called every during each TimeDilation (see LevelNode)
		virtual void			OnTick(float deltaTime);

		virtual void			OnHit(float damage, WEntity* who);

		//Notification (editor mode) this entity has possibly been updated.
		virtual void			OnPropertiesUpdated();

		//Notification this entity has been spawned on the level
		virtual void			OnAddToWorld();

		//Notification this entity has been removed from the level
		virtual void			OnRemoved();

		//Entity doesn't destroy itself if it receives an out-of-world notification
		void					SetIgnoreOutOfWorld(bool bIgnoreOutOfWorld=true);

		atomic_ptr<Mesh>		GetMesh();
		void					SetMesh(atomic_ptr<Mesh>& mesh);
		virtual void			OnMeshChanged();

		//Returns true if visibility data has changed for this actor
		bool					HasVisualsChanged() { return m_visualsHasChanged; }
		void					ResetVisualChangedState() { m_visualsHasChanged = false; }

		void					SetStatic(bool isStatic);

		void					SetVelocity(const Vector3& vel);
		const Vector3&			GetVelocity();

		const AxisAngle&		GetAngularVelocity() { return m_angularVel; }
		void					SetAngularVelocity(const AxisAngle& axisAng);

		//Serialize this entity
		void					Serialize(GDZArchive& ar);

		//Updates this entity's location within the collision tree.
		bool					UpdateSpatialLocation();

		//Updates transformation matrix
		void					UpdateTransform(float dt);

		void					OnUpdateTransform();

		VisualID				GetRuntimeID() { return m_id; }
		EntityID				GetEntityID() { return mEntityID; }

		void					SetPhysicsObject(PhysicsObjectProxy& obj);
		PhysicsObjectProxy*		GetPhysicsObject();

		//Returns movement type; by default, an entity is assumed to be static
		virtual MotionType		GetMotionType() { return MotionType_Static; }

		void					SetLayer(Layer* layer, const atomic_ptr<WEntity>& ptr, const EntityID& id);
		Layer*					GetLayer() { return m_layer; }

		void					SetIsShadowCaster(bool flag);
		bool					GetIsShadowCaster() const { return m_isShadowCaster; }

		void					HideEditorMaterial();

		// Returns a Weak reference to this object that other callers should check to
		// validate of this entity is still valid, etc.
		// Note, this weak ref is not valid until the entity has been added to the world
		atomic_weak_ptr<WEntity>& GetWeakReference() { return m_weakReference; }

		//Components!
		void					AddEntityComponent(atomic_ptr<EntityComponent> component);
		bool					RemoveEntityComponent(EntityComponent* component);
		EntityComponent*		GetEntityComponent(HashCodeID hash);

		//Templates!
		void					SetTemplate(atomic_ptr<EntityTemplate> entitytemplate);
		EntityTemplate*			GetTemplate();

	protected:
		virtual void			UpdateGeometryInfo(GeometryInfo& info) {}
		virtual void			UpdatePhysics(CollisionStatus physicsState);
		void					UpdateRenderTransform();
	


		struct GODZ_API AttachmentInfo
		{
			Vector3 m_rel;			//relative location/offset
			WMatrix3 m_rot;			//relative rotation
			atomic_weak_ptr<WEntity> entity;
		};

		VisualID				m_id;					//TODO: Move into VisualComponent
		EntityID				mEntityID;				//Handle for this object

		bool					m_bVisible;				//display flag
		bool					m_bIgnoreOutOfBounds;	//don't destroy if out-of-world	
		bool					m_bEditable;			//actor contains editable properties (editor mode)
		bool					m_bUpdateScaleRotTM;	//batch together updates to the scale / rot/ translation
		bool					m_bCollideWithWorld;
		bool					m_visualsHasChanged;	//tracks if this actor requires a visibility update
		bool					m_isShadowCaster;

		Vector3					m_vScale;				//Draw scale (only used for rendering - not included in matrix)
		Vector3					m_pos;					//Location in the world

		Vector3					m_vel;					//current character velocity
		AxisAngle				m_angularVel;			//angular velocity

		WMatrix3				m_orientation;			//Object orientation (pure rotation matrix)
		Matrix4					m_renderMat;

		GenericReference<CollisionPrimitive>		m_model;				//collision model
		atomic_ptr<Mesh>	mesh;						//mesh definition
		MeshInstance* m_pMeshInstance;					//mesh instance
		Skeleton*				m_pSkeleton;			//node hierarchy
		AnimController*			m_animController;		//current animation instance
		LevelNode *				m_pLevel;				//World this entity belongs to
		Layer*					m_layer;

		std::vector<AttachmentInfo>	m_attachedActors;	//attachments

		typedef std::map<HashCodeID, atomic_ptr<EntityComponent> > ComponentMap;
		ComponentMap m_components;						//Entity plugin / components

		PhysicsObjectProxy		m_physicsObject;		//Physics simulation object

		long					m_lastFrameNumber;		//physics frame number

		atomic_ptr<EntityTemplate> m_template;	//entity template

		int m_prevAnimFrame;							//used for animation triggers
		PhysicsUpdateType		m_physicsType;

		// Stores a weak reference to our instance that we can pass to observers
		atomic_weak_ptr<WEntity> m_weakReference;
	};
}

#endif
