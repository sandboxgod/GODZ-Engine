/* ===========================================================
	Bot

	Copyright (c) 2010, Richard Osborne
	========================================================== 
*/

#if !defined(__TRIANGLELIST_H__)
#define __TRIANGLELIST_H__

#include <CoreFramework/Math/Vector3.h>
#include <vector>

namespace GODZ
{
	//triangle
	struct TriPolygon
	{
		Vector3 v[3];
		Vector3 n;
	};

	class GODZ_API TriangleList
	{
	public:
		void AddPoly(const TriPolygon& poly)
		{
			m_polys.push_back(poly);
		}

		size_t GetNumTriangles() const
		{
			return m_polys.size();
		}

		const TriPolygon& GetPoly(size_t index) const
		{
			return m_polys[index];
		}

	private:
		std::vector<TriPolygon> m_polys;
	};
}

#endif //