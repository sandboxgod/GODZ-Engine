
#include "AnimationInstance.h"
#include "AnimationController.h"
#include <CoreFramework/Core/SkeletalMesh.h>
#include <CoreFramework/Core/SkelMeshInstance.h>

using namespace GODZ;
using namespace std;

bool AnimTrack::GetKeys(float time, AnimKey &start, AnimKey &end)
{
	size_t size = keys.size();
	for (size_t i=0;i<size;i++)
	{
		float t = keys[i].time;
		if (t<=time)
		{
			start = keys[i];
		}
		else if (t > time)
		{
			end = keys[i];
			return true;
		}
	}

	return 0;
}

// Serializes this object
void AnimTrack::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() < 1006 && !ar.IsSaving())
	{
		rstring text;
		ar << text;
		boneName = HString(text);
	}
	else
	{
		ar << boneName;
	}

	size_t keyCount=keys.size();
	ar << keyCount;

	for (udword i=0;i<keyCount;i++)
	{
		if (ar.IsSaving())
		{
			keys[i].Serialize(ar);
		}
		else
		{
			AnimKey key;
			key.Serialize(ar);
			keys.push_back(key);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////

AnimSequence::~AnimSequence()
{
}

AnimTrack* AnimSequence::GetTrack(udword index)
{
	godzassert(index<tracks.size());
	return &tracks[index];
}

AnimTrack* AnimSequence::GetTrackFor(Bone* inBone)
{
	size_t size = tracks.size();
	
	for(udword i=0;i<size;i++)
	{
		const HString& trackName = tracks[i].GetTrackName();
		if (trackName.getHashCode() == inBone->GetName().getHashCode())
		{
			return &tracks[i];
		}
	}

	return NULL;
}

void AnimSequence::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	ar << Name;
	ar << maxTime;
	ar << framesPerSecond;

	size_t trackCount = tracks.size();
	ar << trackCount;
	ar << mAnimType;

	for(UInt32 i=0;i<trackCount;i++)
	{
		if (ar.IsSaving())
		{
			tracks[i].Serialize(ar);
		}
		else
		{
			AnimTrack track;
			track.Serialize(ar);
			tracks.push_back(track);
		}
	}
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

AnimationInstance::AnimationInstance()
{
}

void AnimationInstance::AddAnimTrack(const AnimTrack &track, udword sequenceIndex)
{
	godzassert(sequenceIndex<sequences.GetNumItems());
	sequences[sequenceIndex].AddTrack(track);
}

void AnimationInstance::AddAnimSet(const char* sequenceName)
{
	AnimSequence s(sequenceName);
	sequences.Push(s);
}

AnimationTable& AnimationInstance::GetAnimationTable()
{
	return m_pTable;
}

AnimSequence* AnimationInstance::GetSequence(udword index)
{
	size_t numSequences = sequences.GetNumItems();
	if (index<numSequences)
	{
		return &sequences[index];
	}

	return 0;
}

size_t AnimationInstance::GetNumOfAnimSets()
{
	return this->sequences.GetNumItems();
}

bool AnimationInstance::RemoveSequence(udword index)
{
	return sequences.DropNodeAt(index);
}

void AnimationInstance::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	size_t seqCount = sequences.GetNumItems();
	ar << seqCount;

	for (udword i=0; i<seqCount; i++)
	{
		if(ar.IsSaving())
		{
			sequences[i].Serialize(ar);
		}
		else
		{
			AnimSequence s;
			s.Serialize(ar);
			sequences.Push(s);
		}
	}

	if(!ar.IsSaving())
	{
		sequences.BuildArray(); //optimize array access
	}

	m_pTable.Serialize(ar);
}

void AnimationInstance::Update(AnimController* animControl, Bone *bone, udword boneIndex)
{
	AnimSequence* currentSequence = NULL;
	int index = animControl->GetSequence();

	godzassert(index < static_cast<int>(sequences.GetNumItems()));
	currentSequence = &sequences[index];

	if (currentSequence == NULL)
	{
		if (sequences.GetNumItems()>0)
		{
			//get default pose
			currentSequence=&sequences[0];
		}
		else
		{
			return;
		}
	}

	//compute the current time
	float time = animControl->GetTime();

	//todo: not every animation should auto-loop like this!
	if (time < 0 || time > currentSequence->GetMaxTime())
	{
		animControl->Reset(currentSequence);
	}

	//check to see if we need to blend into this animation
	AnimTrack* prevTrack = NULL;
	int prevIndex = animControl->GetPreviousSequence();
	if (prevIndex > -1 && animControl->GetEase() > 0.0f)
	{
		AnimSequence* prevSeq = &sequences[prevIndex];
		prevTrack = prevSeq->GetTrack(boneIndex);
	}

	//find the start and end keys
	AnimKey key, prevKey;

	AnimTrack* track = currentSequence->GetTrack(boneIndex);

	//if there is no track - this is probably Bip01 so just return identity...
	if (track == NULL)
	{
		bone->m_frame.MakeIdentity();
		return;
	}

		
	key = ComputeAnimKey(time, track);

	if (prevTrack != NULL)
	{
		prevKey = ComputeAnimKey(animControl->GetPreviousTime(), prevTrack);

		float lerp = animControl->GetEase() / animControl->GetEaseSetting();
		key = BlendKeys(lerp, key, prevKey);
		//_asm nop;
	}

	WMatrix16f frame(1);
	if (key.bLinearKey)
	{
		frame.Rotate(key.euler);
	}
	else
	{
		QuaternionToMatrix(key.rot,frame);
	}
	
	frame.PreTranslate(key.pos);
	bone->m_frame = frame;
	
}

AnimKey AnimationInstance::ComputeAnimKey(float time, AnimTrack* track)
{
	AnimKey start, end, key;

	track->GetKeys(time, start, end);

	//determine if we need to interpolate between the keys....
	if (start.time == time)
	{
		key = start;
	}
	else if (end.time == start.time)
	{
		key = start;
	}
	else if (end.time < start.time)
	{
		//this means we are missing a key-- like the bone last key was at 24.
		//however, the sequence ends at 30. so just use last known end time
		key = start;
	}
	else
	{
		float flerpvalue = float(time-start.time)/float(end.time-start.time);
		key = BlendKeys(flerpvalue, start, end);
	}

	return key;
}

AnimKey AnimationInstance::BlendKeys(float flerpvalue, const AnimKey& start, const AnimKey& end)
{
	AnimKey key;
	key.scale.MakeZero();

	if (start.rot == end.rot)
	{
		//if the rotation is the same, we can't slerp so....
		//float flerpvalue = float(time-start.time)/float(end.time-start.time);

		key = start;

		//interpolate position
		key.pos.x = start.pos.x + flerpvalue * (end.pos.x - start.pos.x);
		key.pos.y = start.pos.y + flerpvalue * (end.pos.y - start.pos.y);
		key.pos.z = start.pos.z + flerpvalue * (end.pos.z - start.pos.z);
	}
	else
	{
		//godzassert(end.time > start.time);  -- new, just removed
		//float flerpvalue = float(time-start.time)/float(end.time-start.time);
		
		//interpolate between the start and end keys
		if (key.bLinearKey)
		{
			//debugging - we don't ever use linear interpolation
			key.euler.x = start.euler.x + flerpvalue * (end.euler.x - start.euler.x);
			key.euler.y = start.euler.y + flerpvalue * (end.euler.y - start.euler.y);
			key.euler.z = start.euler.z + flerpvalue * (end.euler.z - start.euler.z);
		}
		else
		{
			key.euler.MakeZero();

			//SlerpQuaternions(start.rot,end.rot,flerpvalue,key.rot);
			key.rot = Quat4f::slerp(start.rot,end.rot,flerpvalue);
		}

		//interpolate position
		key.pos.x = start.pos.x + flerpvalue * (end.pos.x - start.pos.x);
		key.pos.y = start.pos.y + flerpvalue * (end.pos.y - start.pos.y);
		key.pos.z = start.pos.z + flerpvalue * (end.pos.z - start.pos.z);

		//key.scale.x = start.scale.x + flerpvalue * (end.scale.x - start.scale.x);
		//key.scale.y = start.scale.y + flerpvalue * (end.scale.y - start.scale.y);
		//key.scale.z = start.scale.z + flerpvalue * (end.scale.z - start.scale.z);

	}

	return key;
}