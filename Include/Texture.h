// Texture.h: interface for the CTexture class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXTEXTURE_
#define _FOXTEXTURE_

#include <windows.h>
#include <d3d8.h>


// textura tylko do obslugi trojkatow, dla sprites jest CSprite
// konstruktor na podstawie pliku, lub wskaznika
// TODO:
//	- konstruktor ze wskaznika

class __declspec(dllexport) CTexture  
{
public:
	CTexture(IDirect3DDevice8 *pDevice, char *fp_szFileName);
	virtual ~CTexture();

	void Set(IDirect3DDevice8 *pDevice);

private:

	IDirect3DTexture8 *pTexture;

};

#endif 
