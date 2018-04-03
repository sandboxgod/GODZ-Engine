/* ===========================================================
	SkyDomeInfo

	Note: just remember to disable zwriteeanble
	Copyright (c) 2010
	========================================================== 
*/

#pragma once

#include "Godz.h"
#include "ObjectStateChangeEvent.h"


namespace GODZ
{
	/*
	* Wrapper for sky domes
	*/
	struct GODZ_API SkyDomeInfo
	{
		VisualID m_id;
	};


	class GODZ_API CreateUpdateSkyDomeEvent : public ObjectStateChangeEvent
	{
	public:
		CreateUpdateSkyDomeEvent(SkyDomeInfo& m)
			: mSky(m)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_CreateSkyDome)
		{
		}

		virtual rstring getDebug() { return GetString("SkyDome %d", mSky.m_id); }

		SkyDomeInfo mSky;
	};

	class GODZ_API RemoveSkyDomeEvent : public ObjectStateChangeEvent
	{
	public:
		RemoveSkyDomeEvent(SkyDomeInfo& m)
			: mSky(m)
			, ObjectStateChangeEvent(ObjectStateChangeEventID_RemoveSkyDome)
		{
		}

		virtual rstring getDebug() { return GetString("Remove SkyDome %d", mSky.m_id); }

		SkyDomeInfo mSky;
	};
}

