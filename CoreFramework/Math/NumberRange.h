
#pragma once

#include <CoreFramework/Core/CoreFramework.h>

namespace GODZ
{
	//finds a number in the range. For ex, you can setup a Range of numbers [10...5]. This object
	//will compute percentages and/or values within the range
	struct GODZ_API NumberRange
	{
		float m_diff, min, max;

		NumberRange(float max, float min)
			: m_diff(0)
		{
			m_diff = -min;

			//compute a number range that sort of normalizes our min/max
			this->max = max + m_diff;
			this->min = min + m_diff;
		}

		//computes the percent for the argument within the range
		float ComputeValue(float num)
		{
			return (num + m_diff) / (max + min) ;
		}

		
		//returns a value within the range at the given percent. For ex, if the range is [10...5]
		//and the percent 0.46 is passed in, the result will be 6.9
		float ComputePercent(float pcnt)
		{
			return (pcnt * (max + min)) + m_diff;
		}
	};
}
