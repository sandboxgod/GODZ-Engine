/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/


#if !defined(__COLMATH_H__)
#define __COLMATH_H__

#include <CoreFramework/Math/Vector3.h>

namespace GODZ
{
	GODZ_API bool triBoxOverlap(const float boxcenter[3],const float boxhalfsize[3], const Vector3 triverts[3]);


}

#endif //__COLMATH_H__