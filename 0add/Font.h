// Font.h: interface for the CFont class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXFONT_
#define _FOXFONT_

#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>

//=== klasa do rysowania fontow
//=== TODO:
//		- zmiana czcionki
//		- czytanie ttf z dysku

class __declspec(dllexport) CFont  
{
public:

	CFont();
	virtual ~CFont();

	HRESULT			Initialize(IDirect3DDevice8 *g_pDevice,HFONT hFont, D3DCOLOR FontColor);		// init
	HRESULT			OutputText(char *pString, int x, int y);	

	virtual void		SetColor(D3DCOLOR FontColor);	// ustaw color czcionki
	
	virtual D3DCOLOR	GetColor(void);	// podaj color czcionki

private:

	D3DCOLOR				m_FontColor;
	
	int						m_Align;			// alignowanie
	
	LPD3DXFONT				m_pFont;	// jaka czcionka
	
	RECT					m_FontRect;
	BOOL					m_bInitialized;

};

#endif 
