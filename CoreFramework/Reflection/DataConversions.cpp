
#include "DataConversions.h"
#include <CoreFramework/Core/StringTokenizer.h>

using namespace GODZ;

rstring GODZ::GetFloatAsString(float f)
{
	const int bufferSize = 1024;
	char buf[bufferSize];
	sprintf_s(buf, bufferSize, "%f", f);
	return buf;
}

rstring GODZ::GetVectorAsString(const Vector3& v)
{
	const float* f = v;
	return GetFloatArrayAsString(f, 3);
}

rstring GODZ::GetFloatArrayAsString(const float* v, int numFloats)
{
	rstring text;
	text += "(";

	for(int i=0;i<numFloats;i++)
	{
		text += GetFloatAsString(v[i]);

		//if this isn't the last element insert a comma
		if (i < numFloats - 1) 
			text += ",";
	}
	text += ")";
	return text;
}

rstring GODZ::GetRotatorAsString(EulerAngle& v)
{			
	rstring text;
	text += "(";
	text += GetFloatAsString(v.y);
	text += ",";
	text += GetFloatAsString(v.p);
	text += ",";
	text += GetFloatAsString(v.r);
	text += ")";
	return text;
}

void GODZ::GetStringAsBox(const char* text, WBox& b)
{
	rstring buf = text;
	char seps[]   = " (,:";

	StringTokenizer tk(text, seps);

	Vector3 min, max;
	const char* token = tk.next();
	int c=0;
	while( token != NULL )
	{
		if (c!=0 && c < 4)
		{
			min[c-1]=atof(token);
		}
		else if (c>4)
		{
			max[c-5]=atof(token);
		}

		c++;
		token = tk.next();
	}

	b.SetMinMax(min, max);
}

void GODZ::GetStringAsVector(const char* text, Vector3& v)
{
	char seps[]   = "(,)";

	StringTokenizer tk(text, seps);
	size_t c=0;
	const char* token = tk.next();
	while( token != NULL )
	{
		if (c >= 3)
		{
			return; //avoid memory overflow/etc
		}

		v[c]=atof(token);
		c++;
		token = tk.next();
	}
}

void GODZ::GetStringAsFloatArray(const char* text, float* v, size_t maxElements)
{
	char seps[]   = "(,)";

	StringTokenizer tk(text, seps);
	size_t c=0;
	const char* token = tk.next();
	while( token != NULL )
	{
		if (c >= maxElements)
		{
			return; //avoid memory overflow/etc
		}

		v[c]=atof(token);
		c++;
		token = tk.next();
	}
}

void GODZ::GetStringAsRotator(const char* text, EulerAngle& v)
{
	char seps[]   = "(,";

	StringTokenizer tk(text, seps);
	int c=0;
	const char* token = tk.next();
	while( token != NULL )
	{				
		v[c]=atof(token);
		c++;
		token = tk.next();
	}
}

void GODZ::GetStringAsMatrix(const char* text, WMatrix16f& mat)
{
	godzassert(0); //currently, I have it where its a Quaternion / Position :(

	StringTokenizer tk(text, " ");
	godzassert(tk.size() == 2);
	rstring rot = tk.next();
	rstring pos = tk.next();
	Vector3 rotV;
	GetStringAsVector(rot, rotV);
	Vector3 posV;
	GetStringAsVector(pos, posV);
	mat.Rotate(rotV);
	mat.SetTranslation(posV);
}