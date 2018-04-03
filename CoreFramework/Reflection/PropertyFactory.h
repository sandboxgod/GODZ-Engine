/* ===========================================================
	PropertyValue

	Copyright (c) 2010
	========================================================== 
*/

#pragma once

#include "PropertyValue.h"
#include "PropertyStruct.h"

namespace GODZ
{
	class GODZ_API PropertyFactory
	{
	public:
		static GenericReference<PropertyValue> create(EPropertyType type)
		{
			switch (type)
			{
			default:
				{
					return NULL; //unhandled type
				}
				break;
			case PT_OBJECT:
				{
					return new PropertyObject();
				}
				break;
			case PT_INT:
				{
					return new PropertyInt();
				}
				break;
			case PT_FLOAT:
				{
					return new PropertyFloat();
				}
				break;
			case PT_UINT32:
				{
					return new PropertyUInt32();
				}
				break;
			case PT_RESOURCE:
				{
					return new PropertyResource();
				}
				break;
			case PT_STRUCT:
				{
					return new PropertyStruct();
				}
				break;
			}
		}
	};
}
