
#pragma once

#include <CoreFramework/Core/Godz.h>
#include <CoreFramework/Math/Vector3.h>

namespace GODZ
{
	//Forward Declarations
	struct WBox;
	class TriangleList;

	class GODZ_API GeoBuilder
	{
	public:
		static void BuildBoxMesh(const Vector3& min, const Vector3& max, TriangleList& mesh);
	};
}
