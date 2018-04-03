/* ===========================================================
	Texture resource

	Created Jan 26, '04 by Richard Osborne
	Copyright (c) 2003, Richard Osborne
	========================================================== 
*/

#pragma once

#include "Godz.h"
#include "GodzAtomics.h"
#include "ResourceObject.h"
#include "TextureInfo.h"

namespace GODZ
{
	class GODZ_API TextureResource : public ResourceObject
	{
	public:
		TextureResource(HashCodeID hash);
		~TextureResource();

		Future<GODZHANDLE>& GetBitmap();			//returns a future request for HBITMAP

		void LoadTextureIntoMem(rstring filename);
		void Serialize(GDZArchive& ar);

		//submits request to render thread to acquire render object
		void CreateRenderDeviceObject();
		Future<TextureInfo>& GetTextureInfo() { return m_textureInfo; }

	protected:
		UInt8* m_pImageData;							//image data
		long m_numBytes;

		Future<GODZHANDLE> m_hbitmap;			//reference to windows bitmap
		Future<TextureInfo> m_textureInfo;
	};
}
