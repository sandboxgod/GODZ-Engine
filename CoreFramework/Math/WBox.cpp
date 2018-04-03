
#include "WBox.h"
#include "OBBox.h"
#include "WSphere.h"
#include "WGeometry.h"
#include "WPlane.h"
#include <CoreFramework/Collision/colmath.h>
#include <CoreFramework/Collision/RayCollider.h>
#include <float.h>


namespace GODZ
{

WBox::WBox()
{
}

WBox::WBox(Vertex* V, size_t count)
{
	SetMinMaxBoundVerts(V, count, min, max);
	updateCenterExtent();
}

WBox::WBox(BaseVertex* V, size_t count)
{
	SetMinMaxBoundVerts(V, count, min, max);
	updateCenterExtent();
}

WBox::WBox(const Vector3& min, const Vector3& max)
{
	this->min = min;
	this->max = max;
	updateCenterExtent();
}

WBox::WBox(Vector3* V, size_t count)
{
	SetMinMaxBound(V, count, min, max);
	updateCenterExtent();
}

void WBox::SetBounds(Vector3* V, int count)
{
	SetMinMaxBound(V, count, min, max);
	updateCenterExtent();
}

void WBox::SetBounds(Vertex* V, int count)
{
	SetMinMaxBoundVerts(V, count, min, max);
	updateCenterExtent();
}

void WBox::SetMinMax(const Vector3& vmin, const Vector3& vmax)
{
	min = vmin;
	max = vmax;

	updateCenterExtent();
}

void WBox::SetDimensions(const Vector3& _center, const Vector3& _extent)
{
	center = _center;
	extent = _extent;

	min = center - extent;
	max = center + extent;
}

bool WBox::ContainsPoint(const Vector3& point) const
{
	if (point >= min && point <= max)
		return true;

	return false;
}

bool WBox::ContainsBox(const WBox& a) const
{
	if (a.min.x < min.x)
		return 0;
	if (a.min.y < min.y)
		return 0;
	if (a.min.z < min.z)
		return 0;

	if (a.min.x > max.x)
		return 0;
	if (a.min.y > max.y)
		return 0;
	if (a.min.z > max.z)
		return 0;

	if (a.max.x < min.x)
		return 0;
	if (a.max.y < min.y)
		return 0;
	if (a.max.z < min.z)
		return 0;

	if (a.max.x > max.x)
		return 0;
	if (a.max.y > max.y)
		return 0;
	if (a.max.z > max.z)
		return 0;

	return true;
}

bool WBox::ContainsTriangle(const Vector3 vec[3]) const
{
	for (int i=0; i < 3; i++)
	{
		if (!ContainsPoint(vec[i]))
		{
			return 0;
		}
	}

	return 1;
}

bool WBox::ContainsTriangle(const BaseVertex vec[3]) const
{
	for (int i=0; i < 3; i++)
	{
		if (!ContainsPoint(vec[i]))
		{
			return false;
		}
	}

	return true;
}

void WBox::GetAABBPoints(Vector3 point[8]) const
{
	point[0]=Vector3(min.x,min.y,min.z);
	point[1]=Vector3(max.x,min.y,min.z);
	point[2]=Vector3(min.x,min.y,max.z);
	point[3]=Vector3(max.x,min.y,max.z);
	point[4]=Vector3(min.x,max.y,min.z);
	point[5]=Vector3(max.x,max.y,min.z);
	point[6]=Vector3(min.x,max.y,max.z);
	point[7]=Vector3(max.x,max.y,max.z);
}

Vector3 WBox::GetBottom() const
{
	return Vector3( (max.x + min.x) * 0.5f, min.y, max.z); //(center.x, min.y, min.z)
}

void WBox::SetCenter(const Vector3& pos)
{
	center = pos;

	min = pos - extent;
	max = pos + extent;
}

int WBox::GetLongestAxis() const
{
	Vector3 boxdim(max-min);
	
	int la=0; // longest axis
	float lav=0.0f; // longest axis length
	
	// for each dimension  
	for (int ii=0; ii<3; ii++) {
		// check if its longer
		if (boxdim[ii]>lav) {
			// store it if it is
			la=ii;
			lav=boxdim[ii];
		}
	}
	
	return la;
}

float WBox::GetMin(int index) const
{
	return min[index];
}

float WBox::GetMax(int index) const
{
	return max[index];
}

float WBox::GetSize() const
{
	Vector3 v = max-min;
	return v.GetLength();
}

//ripped from realtime collision book, pg 180
bool WBox::Intersects(const WRay& ray, float& tmin, Vector3& q) const
{
	tmin = 0.0f;
	float tmax = FLT_MAX;

	for (int i=0; i<3; i++)
	{
		if (fabsf(ray.dir[i]) < EPSILON)
		{
			//ray is parallel to slab. not hit if origin not within slab
			if (ray.origin[i] < min[i] || ray.origin[i] > max[i])
				return false;
		}
		else
		{
			//compute intersection t value of ray with near & far plane of slab
			float ood = 1.0f / ray.dir[i];
			float t1 = (min[i] - ray.origin[i]) * ood;
			float t2 = (max[i] - ray.origin[i]) * ood;

			//Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2)
			{
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}

			//compute intersection of slab intersection intervals
			if (t1 > tmin)
			{
				tmin = t1;
			}

			if (t2 > tmax)
			{
				tmax = t2;
			}

			//Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax)
				return false;
		}
	}

	//Ray intersects all 3 slabs.
	q = ray.origin + ray.dir * tmin;
	return true;
}

bool WBox::Intersects(const Vector3 list[3]) const
{
	return TriBoxOverlap(center, extent, list);
}

bool WBox::Intersects(const WPlane& p) const
{
	//http://www.devmaster.net/forums/archive/index.php/t-10324.html
	Vector3 absNormal(fabs(p.N.x), fabs(p.N.y), fabs(p.N.z));

	float c = Dot(center, p.N);
	float e = Dot(extent, absNormal);
	return p.D < (c - e) || p.D > (c + e);
}

bool WBox::Intersects(const WSphere& s) const
{
	return Intersects(s.center, s.radius);
}

//sphere --- http://toxiclibs.googlecode.com/svn-history/r450/trunk/toxiclibs/src.core/toxi/geom/AABB.java
bool WBox::Intersects(const Vector3& c, float r) const
{
    float s, d = 0;
    // find the square of the distance
    // from the sphere to the box
    if (c.x < min.x) {
        s = c.x - min.x;
        d = s * s;
    } else if (c.x > max.x) {
        s = c.x - max.x;
        d += s * s;
    }

    if (c.y < min.y) {
        s = c.y - min.y;
        d += s * s;
    } else if (c.y > max.y) {
        s = c.y - max.y;
        d += s * s;
    }

    if (c.z < min.z) {
        s = c.z - min.z;
        d += s * s;
    } else if (c.z > max.z) {
        s = c.z - max.z;
        d += s * s;
    }

    return d <= r * r;
}

bool WBox::IntersectsBox(const WBox& other) const
{
	if( min.x > other.max.x || other.min.x > max.x )
		return false;
	if( min.y > other.max.y || other.min.y > max.y )
		return false;
	if( min.z > other.max.z || other.min.z > max.z )
		return false;
	
	return true;
}

//Sweep two AABB's to see if and when they first
//and last were overlapping
/*
bool WBox::AABBSweep
(const Vector3&	Ea,	//extents of AABB A
const Vector3&	A0,	//its previous position
const Vector3&	A1,	//its current position
const Vector3&	Eb,	//extents of AABB B
const Vector3&	B0,	//its previous position
const Vector3&	B1,	//its current position
float&	 u0,	//normalized time of first collision
float&	 u1	//normalized time of second collision
)
*/
bool WBox::AABBSweep(const WBox& A, const Vector3& va, const WBox& B, const Vector3& vb, float& u0, float& u1)
{

Vector3 v = vb - va; //relative velocity (in normalized time)

Vector3 u_0(0,0,0);
//first times of overlap along each axis

Vector3 u_1(1,1,1);
//last times of overlap along each axis

//check if they were overlapping 
// on the previous frame
if( A.IntersectsBox(B) )
{
u0 = u1 = 0;
return true;

}

//find the possible first and last times
//of overlap along each axis
for( long i=0 ; i<3 ; i++ )
{

 

 
if( A.max(i)<B.min(i) && v[i]<0 )
{

 

u_0[i] = (A.max(i) - B.min(i)) / v[i];
}

else if( B.max(i)<A.min(i) && v[i]>0 )
{

 

u_0[i] = (A.min(i) - B.max(i)) / v[i];
}

if( B.max(i)>A.min(i) && v[i]<0 )
{

u_1[i] = (A.min(i) - B.max(i)) / v[i];
}

else if( A.max(i)>B.min(i) && v[i]>0 )
{

 

u_1[i] = (A.max(i) - B.min(i)) / v[i];
}

}

//possible first time of overlap
u0 = MAX( u_0.x, MAX(u_0.y, u_0.z) );

//possible last time of overlap
u1 = MIN( u_1.x, MIN(u_1.y, u_1.z) );

//they could have only collided if
//the first time of overlap occurred
//before the last time of overlap
return u0 <= u1;

}

bool WBox::IntersectsMovingBox(const WBox& a, const Vector3& va, const WBox& b, const Vector3& vb, float& tfirst, float& tlast)
{
	/*
	if (true)
	{
		return AABBSweep(a, va, b, vb, tfirst, tlast);
	}
	*/

	//from pg 230, realtime collision

	//exit early if they already overlap
	if (a.IntersectsBox(b))
	{
		tfirst = tlast = 0.0f;
		return true;
	}

	//use relative velocity treating 'a' as stationary
	Vector3 v = vb - va;

	tfirst=0;
	tlast=1.0f;

	//look at each axis- examine for contact
	for(int i=0;i<3;i++)
	{
		//rno - changed so we will always evaluate, this will force separating axis test to
		//evaluate zero velocity
		if (v[i] == 0)
		{
			v[i] = 0.001f;
		}
		

		if (v[i] < 0.0f)
		{
			if (b.max[i] < a.min[i]) 
				return false; //nonintersecting and moving apart
			if (a.max[i] < b.min[i]) 
			{
				float num = (a.max[i] - b.min[i]) / v[i];
				tfirst = MAX(num, tfirst);
			}
			if (b.max[i] > a.min[i])
			{
				float num = (a.min[i] - b.max[i]) / v[i];
				tlast = MIN(num, tlast);
			}
		}
		if (v[i] >= 0.0f) 
		{
			if (b.min[i] > a.max[i]) 
				return false;
			if (b.max[i] < a.min[i]) 
			{
				float num = (a.min[i] - b.max[i]) / v[i];
				tfirst = MAX(num, tfirst);
			}
			if (a.max[i] > b.min[i]) 
			{
				float num = (a.max[i] - b.min[i]) / v[i];
				tlast = MIN(num, tlast);
			}
		}

		//no overlap possible
		if (tfirst > tlast)
			return false;
	}

	return true;
}

bool WBox::IsValid() const
{
	return (max > min);
}

void WBox::Serialize(GDZArchive& ar)
{
	if (ar.GetFileVersion() > 1002)
	{
		ArchiveVersion version(1,1);
		ar << version;
	}

	ar << min;
	ar << max;
	updateCenterExtent();
}

//Note: There used to be a SetCenter(const Vector3& pos) but that was removed
//cause you must Scale then SetCenter(). If you do the steps vice versa, you
//get a different box. So to keep things simple, there is no SetCenter()

//applies rotation / scale / transform
void WBox::Transform(const WMatrix& m)
{
	static const int MAX_POINTS = 8;
	Vector3 points[MAX_POINTS];
	GetAABBPoints(points);

	for(int i = 0; i < MAX_POINTS; i++)
	{
		m.Mul(points[i]);
	}

	SetMinMaxBound(points, MAX_POINTS, min, max);
	updateCenterExtent();
}

void WBox::Transform(const Vector3& p)
{
	min += p;
	max += p;
	updateCenterExtent();
}

bool WBox::Overlaps(const WBox& Box) const
{
	const Vector3 T = Box.GetCenter() - center;//vector from A to B
	return (fabs(T.x) <= (extent.x + Box.GetExtent().x)) && (fabs(T.y) <= (extent.y + Box.GetExtent().y)) && (fabs(T.z) <= (extent.z + Box.GetExtent().z));
}

bool WBox::operator==(const WBox& Box) const
{
	if (Box.min == min && Box.max == max)
		return true;

	return false;
}

bool WBox::operator!=(const WBox& Box) const
{
	if (Box.min != min || Box.max != max)
		return true;

	return false;
}

WBox::operator WSphere() const
{
	WSphere s;

	//Converts AABB to sphere
	//http://www.gamedev.net/community/forums/topic.asp?topic_id=303047
	s.center = center;
	s.radius = 0.5f * ( max - min ).GetLength();

	return s;
}

//Returns the plane normal thats closest to the argument (expects a normalized vector).
//Since an AABB cant be rotated, this op is fairly straight forward...
Vector3 WBox::GetNearPlane(const Vector3& test)
{
	Vector3 v[6];
	v[0] = Vector3(0,1,0);
	v[1] = Vector3(0,-1,0);
	v[2] = Vector3(1,0,0);
	v[3] = Vector3(-1,0,0);
	v[4] = Vector3(0,0,-1);
	v[5] = Vector3(0,0,1);

	int index = 0;
	float dist = FLT_MAX;
	for(int i=0; i < 6; i++)
	{
		float t = (v[i] - test).GetLengthSQ();
		if (t < dist)
		{
			dist = t;
			index = i;
		}
	}

	return v[index];
}

WBox::operator OBBox() const
{
	OBBox b;
	b.center = center;
	b.extent = extent;
	b.MakeIdentity();
	return b;
}

void WBox::updateCenterExtent( void )
{
	//set center/extent based on min/max
	center = (max + min) * 0.5f;
	extent = (max - min) * 0.5;
}

/*
* reference: http://www.gamedev.net/topic/551816-finding-the-aabb-surface-normal-from-an-intersection-point-on-aabb/
* Do you mean a function that, given an AABB and a point on the AABB (and no 
* other information), returns the normal corresponding to that point?
* If so, I think such a function should be fairly easy to construct. You might 
* start by transforming the query point to the AABB's local space by subtracting the
* AABB center from the point. You can then identify which 'slab' the point corresponds
* to by examining the elements of the transformed point in turn; the element whose magnitude
* is closest to the box extent in that direction will correspond to the slab. Then, the
* sign of the element will give you the side of the slab, and you have your normal.
* (I haven't actually tried this method, but it seems correct.)
*/
Vector3 WBox::GetNormalFromPoint(const Vector3& cpoint) const
{
	Vector3 cardinal_axis[3];
	cardinal_axis[0] = RIGHT_VECTOR;
	cardinal_axis[1] = UP_VECTOR;
	cardinal_axis[2] = FORWARD_VECTOR;

	Vector3 normal(0,0,0);
	float min_distance = FLT_MAX;

	//make local copy
	Vector3 point = cpoint;
	point -= center;
	
	for (int i = 0; i < 3; ++i)
	{
		float distance = fabsf(extent[i] - fabsf(point[i]));
		if (distance < min_distance) 
		{
			min_distance = distance;
			normal = sign(point[i]) * cardinal_axis[i];
		}
	}

	normal.Normalize();
	return normal;
}

}