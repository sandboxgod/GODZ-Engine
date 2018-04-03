/* =================================================================

	===============================================================
*/

#pragma once

#include <CoreFramework/Core/Godz.h>

namespace GODZ
{
	class Struct;

	struct GODZ_API StructData
	{
		Struct* m_struct;
		UInt8* m_data;
		size_t m_size;

		StructData()
			: m_data(NULL)
			, m_struct(NULL)
			, m_size(0)
		{
		}

		~StructData()
		{
			delete[] m_data;
		}
	};
}

