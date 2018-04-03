
#pragma once

#include "Godz.h"
#include <vector>

namespace GODZ
{
	template<class T>
	class GODZ_API GodzVector : public std::vector<T>
	{
	public:
		bool contains(T node)
		{
			std::vector<T>::iterator iter;
			for (iter = begin(); iter != end(); iter++)
			{
				if (*iter == node)
				{
					return true;
				}
			}

			return false;
		}

		bool remove(T node)
		{
			std::vector<T>::iterator iter;
			for (iter = begin(); iter != end(); iter++)
			{
				if (*iter == node)
				{
					erase(iter);
					return true;
				}
			}

			return false;
		}
	};
}

