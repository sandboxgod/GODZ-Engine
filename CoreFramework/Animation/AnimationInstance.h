/* ==============================================================
	Animation Instance

	Skeletal Animation system controller
 
	Created Mar 12, '04 by Richard Osborne
	Copyright (c) 2010
	=============================================================
*/

#include <CoreFramework/Math/Quaternion.h>
#include <CoreFramework/Core/GenericNode.h>
#include "AnimationTable.h"

#if !defined(__ANIMATION_INSTANCE__H__)
#define __ANIMATION_INSTANCE__H__

namespace GODZ
{
	//forward decl
	class SkeletalMesh;
	struct Bone;
	class AnimSequence;
	class AnimController;

	//key frame data
	struct GODZ_API AnimKey
	{
		Vector3 pos;
		Quat4f rot;			//for spherical interpolation
		Vector3 euler;		//for linear interpolation
		Vector3 scale;		
		int time;
		bool bLinearKey;	//this is a bezier key

		AnimKey()
			: time(0)
		{
			bLinearKey=0;
		}

		//serializes this animation key
		void Serialize(GDZArchive& ar)
		{
			ar << pos;
			ar << rot;
			ar << euler;
			ar << scale;
			ar << time;
			ar << bLinearKey;
		}
	};

	//A track is a set of timed keys. For every bone in the mesh, there is a matching
	//animation track. Of course, if this animation data is used for a diff skeleton
	//than the one used to define this track, than it's possible there is not a
	//matching bone that should use this track!
	class GODZ_API AnimTrack
	{
	public:
		AnimTrack()
		{
		}

		//adds an animation key to this animation set
		void AddKey(const AnimKey& key)
		{
			keys.push_back(key);
		}	

		//returns the number of keys
		size_t GetNumKeys()
		{
			return keys.size();
		}

		//returns the name of the bone associated with this track
		const HString& GetTrackName()
		{
			return boneName;
		}

		AnimKey& GetKey(udword index)
		{
			godzassert(index < keys.size());
			return keys[index];
		}

		//Finds the timed keys for the given time segment
		bool GetKeys(float time, AnimKey& start, AnimKey& end);

		//sets the index of the bone for this track
		void SetBone(const char* name)
		{
			this->boneName=name;
		}

		void Serialize(GDZArchive& ar);

	protected:
		std::vector<AnimKey> keys;	//list of key frames
		HString boneName;			//name of the bone associated with this track
	};

	class GODZ_API AnimSequence
	{
	public:
		//for serialization
		AnimSequence()
			: maxTime(0)
			, framesPerSecond(30)
			, mAnimType(0)
		{
		}

		AnimSequence(const char* Name)
			: maxTime(0)
			, framesPerSecond(30)
			, mAnimType(0)
		{
			this->Name=Name;
		}

		~AnimSequence();

		void AddTrack(const AnimTrack& track)
		{
			tracks.push_back(track);
		}

		//returns the default frame rate for this sequence
		int GetFPS()
		{
			return framesPerSecond;
		}

		//returns the name of this sequence
		const char* GetName()
		{
			return Name;
		}

		void SetName(const char* sequenceName)
		{
			Name = sequenceName;
		}

		//returns the animation track associated with the bone
		AnimTrack* GetTrackFor(Bone *bone);

		size_t GetSize()
		{
			return tracks.size();
		}

		//returns the length of the animation sequence
		float GetMaxTime()
		{
			return maxTime;
		}
		void SetMaxTime(float time)
		{
			maxTime = time;
		}

		AnimTrack* GetTrack(udword index);

		std::vector<AnimTrack>& GetTracks()
		{
			return tracks;
		}

		//serializes this object
		void Serialize(GDZArchive& ar);

		//Returns the animation category name this sequence belongs to
		HashCodeID GetAnimType()
		{
			return mAnimType;
		}

		//Sets category type this animation belongs to
		void SetAnimType(HashCodeID animType)
		{
			mAnimType = animType;
		}

		void SetPlaybackRate(int secs)
		{
			framesPerSecond=secs;
		}

	protected:
		float maxTime;					//length of the animation sequence
		int framesPerSecond;			//default frames per second
		HashCodeID mAnimType;			//animation hash code (animation type)
		rstring Name;					//name of this animation sequence
		std::vector<AnimTrack> tracks;	//animation tracks (matches # of bones in skeleton)
	};

	//Collection of animation sequences
	class GODZ_API AnimationInstance
	{
	public:
		AnimationInstance();

		//Adds an animation set
		void AddAnimSet(const char* sequenceName);

		//Adds an animation track to the sequence
		void AddAnimTrack(const AnimTrack& track, udword sequenceIndex);

		AnimationTable& GetAnimationTable();

		//Returns the animation sequence
		AnimSequence* GetSequence(udword sequenceIndex);

		//returns # of animation sequences
		size_t GetNumOfAnimSets();

		//removes animation sequence
		bool RemoveSequence(udword index);

		//serializes this skeletal mesh instance
		void Serialize(GDZArchive& ar);

		//updates the animation for the bone using the animator controller 
		void Update(AnimController* animControl, Bone* bone, udword boneIndex);

	protected:
		AnimKey ComputeAnimKey(float time, AnimTrack* track);
		AnimKey BlendKeys(float time, const AnimKey& key1, const AnimKey& key2);

		NodeList<AnimSequence> sequences;	//animation sequence
		AnimationTable m_pTable;
	};
}

#endif