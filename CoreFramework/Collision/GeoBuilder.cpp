
#include "GeoBuilder.h"
#include <CoreFramework/Core/RenderDeviceEvent.h>
#include <CoreFramework/Core/SceneData.h>
#include <CoreFramework/Math/WPlane.h>
#include <CoreFramework/Math/WBox.h>

namespace GODZ
{

void GeoBuilder::BuildBoxMesh(const Vector3& min, const Vector3& max, TriangleList& mesh)
{
	/*
	min.x min.y max.z    max.x max.y max.z     min.x max.y max.z 
	max.x max.y max.z min.x min.y max.z max.x min.y max.z 
	min.x min.y min.z max.x max.y min.z max.x min.y min.z 

	max.x max.y min.z min.x min.y min.z min.x max.y min.z 
	min.x min.y max.z max.x min.y min.z max.x min.y max.z 
	max.x min.y min.z min.x min.y max.z min.x min.y min.z 

	max.x min.y max.z max.x max.y min.z max.x max.y max.z 
	max.x max.y min.z max.x min.y max.z max.x min.y min.z 
	max.x max.y max.z min.x max.y min.z min.x max.y max.z 

	min.x max.y min.z max.x max.y max.z max.x max.y min.z 
	min.x max.y max.z min.x min.y min.z min.x min.y max.z 
	min.x min.y min.z min.x max.y max.z min.x max.y min.z 
	*/

	//Build polygons in object space...
	TriPolygon p;

	p.v[0] = Vector3(min.x, min.y, max.z);
	p.v[1] = Vector3(max.x, max.y, max.z);
	p.v[2] = Vector3(min.x, max.y, max.z);

	WPlane plane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #2
	//max.x max.y max.z min.x min.y max.z max.x min.y max.z 
	p.v[0] = Vector3(max.x, max.y, max.z);
	p.v[1] = Vector3(min.x, min.y, max.z);
	p.v[2] = Vector3(max.x, min.y, max.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #3
	//min.x min.y min.z max.x max.y min.z max.x min.y min.z 
	p.v[0] = Vector3(min.x, min.y, min.z);
	p.v[1] = Vector3(max.x, max.y, min.z);
	p.v[2] = Vector3(max.x, min.y, min.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #4
	//max.x max.y min.z min.x min.y min.z min.x max.y min.z 
	p.v[0] = Vector3(max.x, max.y, min.z);
	p.v[1] = Vector3(min.x, min.y, min.z);
	p.v[2] = Vector3(min.x, max.y, min.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #5
	//min.x min.y max.z max.x min.y min.z max.x min.y max.z 
	p.v[0] = Vector3(min.x, min.y, max.z);
	p.v[1] = Vector3(max.x, min.y, min.z);
	p.v[2] = Vector3(max.x, min.y, max.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #6
	//max.x min.y min.z    min.x min.y max.z    min.x min.y min.z 
	p.v[0] = Vector3(max.x, min.y, min.z);
	p.v[1] = Vector3(min.x, min.y, max.z);
	p.v[2] = Vector3(min.x, min.y, min.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #7
	//max.x min.y max.z    max.x max.y min.z    max.x max.y max.z 
	p.v[0] = Vector3(max.x, min.y, max.z);
	p.v[1] = Vector3(max.x, max.y, min.z);
	p.v[2] = Vector3(max.x, max.y, max.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #8
	//max.x max.y min.z   max.x min.y max.z   max.x min.y min.z 
	p.v[0] = Vector3(max.x, max.y, min.z);
	p.v[1] = Vector3(max.x, min.y, max.z);
	p.v[2] = Vector3(max.x, min.y, min.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);


	//polygon #9
	//max.x max.y max.z min.x max.y min.z min.x max.y max.z 
	p.v[0] = Vector3(max.x, max.y, max.z);
	p.v[1] = Vector3(min.x, max.y, min.z);
	p.v[2] = Vector3(min.x, max.y, max.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #10
	//min.x max.y min.z   max.x max.y max.z   max.x max.y min.z 
	p.v[0] = Vector3(min.x, max.y, min.z);
	p.v[1] = Vector3(max.x, max.y, max.z);
	p.v[2] = Vector3(max.x, max.y, min.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #11
	//min.x max.y max.z   min.x min.y min.z   min.x min.y max.z 
	p.v[0] = Vector3(min.x, max.y, max.z);
	p.v[1] = Vector3(min.x, min.y, min.z);
	p.v[2] = Vector3(min.x, min.y, max.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);

	//polygon #12
	//min.x min.y min.z  min.x max.y max.z min.x max.y min.z 
	p.v[0] = Vector3(min.x, min.y, min.z);
	p.v[1] = Vector3(min.x, max.y, max.z);
	p.v[2] = Vector3(min.x, max.y, min.z);

	plane.BuildPlane(p.v[0], p.v[1], p.v[2]);
	p.n = plane.N;

	mesh.AddPoly(p);
}

}


