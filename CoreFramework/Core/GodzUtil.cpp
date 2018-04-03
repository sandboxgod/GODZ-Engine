
#include "GodzUtil.h"
#include "GenericObject.h"

using namespace GODZ;

GODZ_API rstring GODZ::GetFullPath(EResourceType kType, const char* resourceName)
{
		rstring filename = GetBaseDirectory();
		filename += "\\";
		if (kType == RT_Character)
		{
			filename += GlobalConfig::m_pConfig->ReadNode("Folders","Characters");
		}
		else if (kType == RT_Model)
		{
			filename += GlobalConfig::m_pConfig->ReadNode("Folders","Meshes");
		}
		else
		{
			filename += GlobalConfig::m_pConfig->ReadNode("Folders","Textures");
		}

		filename += "\\";
		filename += resourceName;

		//add file extension
		switch(kType)
		{
		default:
			{
				filename += ".gmz";
			}
			break;
		case RT_Character:
			{
				filename += ".gmz";
			}
			break;
		case RT_Texture:
			{
				filename += ".gmz";
			}
			break;
		case RT_Template:
			{
				filename += ".gmz";
			}
			break;
		case RT_LevelFile:
			{
				filename += ".gmz";
			}
			break;
		}

		return filename;
}
