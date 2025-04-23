// Button.h: interface for the CButton class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXBUTTON_
#define _FOXBUTTON_

#include <windows.h>
#include <d3d8.h>
#include "FileSystem.h"
#include "Sprite.h"

// roznego rodzaju przyciski
// TODO:
//		- sprawdzane wspolrzedne powinny byc dynamiczne


class __declspec(dllexport) CButton  
{
public:
	
	// tworzy na podstawie systemu pliku
	
	CButton(char cFileSystem[], char clicked_name[], 
		char onbutton_name[], char pimage_name[],
		IDirect3DDevice8	*pDevice);
	
	virtual ~CButton();

	virtual int Draw(float mousex, float mousey, bool left, bool center, bool right); // rysuj przycisk

	virtual void SetPosition(long lX, long lY);
	
	// hot coords
	
	virtual void SetHotCoords(long lXStart, long lYStart, long lXEnd, long lYEnd);

private:

	// gor¹ce wspó³rzêdne ;) 

	long		m_lXStart;
	long		m_lYStart;
	long		m_lXEnd;
	long		m_lYEnd;

	CSprite		*m_pClicked;		// nacisniety
	CSprite		*m_pOnButton;		// mysz nad nim
	CSprite		*m_pImage;		// zwyczajny
};

#endif 

