
#include "NameTable.h"
#include "IReader.h"
#include "IWriter.h"

using namespace GODZ;
using namespace std;

NameTable::NameTable()
{
}

NameTable::NameTable(const char *first, ...)
{
	/*
   va_list marker;
   const char *i=first;

   va_start( marker, first );     // Initialize variable arguments.
   while( i != NULL )
   {
	   const char*last=i;
	   
	   i = va_arg( marker, char*);
	   if (i!=NULL)
		strings.push_back(last);
   }  
   va_end( marker );              // Reset variable arguments. 
   */

	godzassert(0); //feature ain't working right
   
}

NameTable::~NameTable()
{
	/*
	//debug
	vector<FName>::iterator strIter;
	for(strIter=strings.begin();strIter!=strings.end();strIter++)
	{
		FName& name = *strIter;
		_asm nop;
	}*/
}

const char* NameTable::AddString(const char* text)
{
	if (text==NULL)
		return NULL;

	//search to see if string already used....
	vector<FName>::iterator strIter;
	for(strIter=strings.begin();strIter!=strings.end();strIter++)
	{
		FName& name = *strIter;
		if (strcmp(name, text)==0)
			return name;
	}

	FName name(text);
	strings.push_back(name);
	size_t size=strings.size();
	return strings[size-1];
}

int NameTable::GetIndex(const char* name)
{
	size_t size = strings.size();
	for (udword i=0;i<size;i++)
	{
		const char* text = strings[i];
		if (strcmp(text,name)==0)
			return i;
	}

	return -1;
}

size_t NameTable::GetNumStrings() 
{ 
	return strings.size(); 
}

void NameTable::LoadContent(IReader *reader, int numOfStrings)
{
	for(int i=0; i<numOfStrings;i++)
	{
		rstring text = reader->ReadString();
		AddString(text);
	}
}

void NameTable::SaveContent(IWriter *writer)
{
	vector<FName>::iterator strIter;
	for(strIter=strings.begin();strIter!=strings.end();strIter++)
	{
		writer->WriteString( (*strIter) );
	}
}
