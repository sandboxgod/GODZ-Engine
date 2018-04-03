/* ===========================================================
	Sort

	"Plans fail for lack of counsel, but with many advisers
	they succeed." - Proverbs 15:22

	========================================================== 
*/

#if !defined(__SORT__H__)
#define __SORT__H__

#include "CoreFramework.h"
#include <vector>

namespace GODZ
{
	//from http://www.algorithmist.com/index.php/Bubble_sort.c
	template <class T>
	void bubbleSort(std::vector<T>& vec)
	{
	  int i, j;
	  T temp;
	  int array_size = (int)vec.size();
	 
	  //we gotta use integers cause a size_t wont like being decremented below 0
	  for (i = (array_size - 1); i >= 0; i--)
	  {
		for (j = 1; j <= i; j++)
		{
		  if (vec[j-1].getValue() > vec[j].getValue())
		  {
			temp = vec[j-1];
			vec[j-1] = vec[j];
			vec[j] = temp;
		  }
		}
	  }
	}	
}

#endif //__SORT__H__
