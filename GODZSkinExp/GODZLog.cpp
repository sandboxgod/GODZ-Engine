
#include "GODZSkinExp.h"
// Windows Header Files:
#include <windows.h>
#include <stdio.h>

using namespace GODZ;

FILE* GODZLog::stream = NULL;

void GODZLog::Open(const char *filename)
{
	if( (stream  = fopen( filename, "w+" )) == NULL )
	{
	}
}

void GODZLog::Close()
{
	fclose(stream);
	stream=NULL;
}

void GODZLog::Log(const char *format, ...)
{
	va_list	ArgList;
	char	buf[1024];

	va_start(ArgList,format);
	vsprintf(buf,format,ArgList);
	va_end(ArgList);  

	if (stream != NULL)
	{
		fwrite( buf, sizeof( char ), strlen(buf), stream );
		fflush(stream);
	}
}

