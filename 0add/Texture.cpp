// Texture.cpp: implementation of the CTexture class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "Texture.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// tworzy texture z pliku

CTexture::CTexture(IDirect3DDevice8 *pDevice, char *fp_szFileName)
{
	D3DXCreateTextureFromFile(pDevice, fp_szFileName, &pTexture);

}

// zwolnij
 
CTexture::~CTexture()
{
	if (pTexture != NULL)
	{
		pTexture->Release();
		pTexture=NULL;
	}
}

// ustaw texture

void CTexture::Set(IDirect3DDevice8 *pDevice)
{
	pDevice->SetTexture(0, pTexture);
}