
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>
#include "Godz.h"

// Logging Utility...
namespace GODZ
{
	class GODZ_API xLog 
	{
	private:
		FILE *stream;
	public:
		xLog();

		void write(const char *text)
		{
			if (stream != NULL)
			{
				fwrite( text, sizeof( char ), strlen(text), stream );
				flush();
			}
		}

		bool isOpen()
		{
			return stream != NULL;
		}

		void flush()
		{
			fflush(stream);
		}

		~xLog()
		{
			fclose(stream);
			stream=NULL;
		}

		//friend ostream& operator<<(ostream& out, const VString& text);
	};
}

#endif _LOGGER_H_