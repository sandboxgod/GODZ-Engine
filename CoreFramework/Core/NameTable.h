/* ===========================================================
	Name Table

	Container used to store strings. Only unique strings are
	stored!

	Special thanks:

	[1] http://www.codeproject.com/cpp/argfunctions.asp?print=true

	Created Mar 12,'04 by Richard Osborne
	Copyright (c) 2010
	========================================================== 
*/

#include "Godz.h"

#if !defined(_NAME_TABLE_H_)
#define _NAME_TABLE_H_

#include "Name.h"

namespace GODZ
{
	//forward declar
	class IReader;
	class IWriter;

	class GODZ_API NameTable
	{
	public:
		//creates an empty string table
		NameTable();

		//creates a string table passed on the string arguments
		NameTable(const char* first, ...);

		~NameTable();

		//Adds the string if an equiv. isn't already stored...
		const char* AddString(const char* text);

		void Clear() { strings.clear(); }

		//Returns # of strings stored within the table
		size_t GetNumStrings();
		
		//Returns -1 if the string isn't stored within the table....
		int GetIndex(const char* name);

		//Reads in a group of names from the reader interface
		void LoadContent(IReader *reader, int numOfStrings);

		//Writes the contents to the writer
		void SaveContent(IWriter *writer);

		const char* operator[](size_t index) 
		{ 
			godzassert(index < strings.size());
			return strings[index] ; 
		}

	private:
		std::vector<FName> strings;
	};
}

#endif
