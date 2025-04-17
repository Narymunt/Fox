// Font.h: interface for the CFont class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FONT_H__4B98F207_E1CB_4051_BCF4_C4D2708D1E4E__INCLUDED_)
#define AFX_FONT_H__4B98F207_E1CB_4051_BCF4_C4D2708D1E4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <math.h>
#include <d3d8.h>
#include <d3dx8.h>

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

#endif // !defined(AFX_FONT_H__4B98F207_E1CB_4051_BCF4_C4D2708D1E4E__INCLUDED_)
