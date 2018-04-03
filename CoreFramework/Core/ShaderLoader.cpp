
#include "ShaderLoader.h"
#include "Godz.h"
#include "ShaderManager.h"
#include "StringTokenizer.h"
#include "Windows.h"


using namespace GODZ;

void GODZ::LoadShaders()
{
	WIN32_FIND_DATA fdata;
	ZeroMemory(&fdata, sizeof(WIN32_FIND_DATA));
	LPWIN32_FIND_DATA FindFileData = &fdata;
	HANDLE hFind = FindFirstFile("Shaders\\*.fxo", FindFileData);

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		Log ("GODZ::LoadShaders() error: can't load shaders. Make sure your ($TargetDir) or Working Directory is set properly");
	} 
	else 
	{
		ShaderManager* sm = ShaderManager::GetInstance();

		do
		{
			Log ("Loaded %s\n", FindFileData->cFileName);
			rstring filename = "Shaders\\";
			filename += FindFileData->cFileName;

			ShaderDefinition shader;
			shader.filename = filename;

			StringTokenizer st(FindFileData->cFileName, ".");
			shader.shaderName = st[0];
			
			sm->AddShaderDefinition(shader);

		} while(FindNextFile(hFind, FindFileData) == TRUE);

		FindClose(hFind);
	}

}