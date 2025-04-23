// Font.cpp: implementation of the CFont class.
//
//////////////////////////////////////////////////////////////////////

#include "Font.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== stworz obiekt

CFont::CFont()
{
	m_pFont = 0;
	
	m_FontColor = D3DCOLOR_ARGB(128,255,0,0);
	
	m_bInitialized = FALSE;
	
	m_Align = DT_LEFT;

}

//=== usun obiekt === 

CFont::~CFont()
{
	if(m_pFont!=NULL)	
	{
		m_pFont->Release();
		m_pFont = NULL;
	}
}

//=== inicjalizuj ===

HRESULT CFont::Initialize(IDirect3DDevice8 *g_pDevice,HFONT hFont, D3DCOLOR FontColor)
{
	HRESULT hr;

	if(m_pFont)
	{
		m_pFont->Release();
		m_pFont = NULL;
	}

	hr = D3DXCreateFont(g_pDevice,hFont, &m_pFont);

	m_FontColor = FontColor;

	m_bInitialized = TRUE;
	return S_OK;

}

//=== rysuj text na ekranie

HRESULT CFont::OutputText(char *pString, int x, int y)
{
	if(!m_bInitialized)	return E_FAIL;

	HRESULT hr;

	RECT FontRect = { x, y, 0, 0 };	// ewentualnie poprawic jezeli bedzie wolno

	m_pFont->Begin();

	m_pFont->DrawTextA(pString,-1,&FontRect,DT_CALCRECT,0);
	hr = m_pFont->DrawTextA(pString,-1,&FontRect, m_Align, m_FontColor);

	m_pFont->End();
	return hr;
}

//=== ustaw color czcionki

void CFont::SetColor(D3DCOLOR FontColor)
{
	m_FontColor = FontColor;
}

//=== pobierz color czcionki

D3DCOLOR CFont::GetColor(void)
{
	return m_FontColor;
}

