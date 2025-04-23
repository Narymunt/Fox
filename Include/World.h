// World.h: interface for the CWorld class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXWORLD_
#define _FOXWORLD_

#include <windows.h>
#include <d3d8.h>

// glowna klasa obslugujaca episod swiat bedacy czescia epizodu
// TODO:
//	- dynamiczna tablica zmiennych
//  - stala tablica wlasciwosci
//	- tablica procedur per tick
//  - tablica procedur per frame
//	- tablica warunkow per tick
//	- tablica warunkow per frame

class CScene;

class __declspec(dllexport) CWorld  
{

public:
	CWorld();
	virtual ~CWorld();

private:

	CScene	**m_pScene;		// czyli ekrany z modulami

};

#endif // !defined(AFX_WORLD_H__B897F7A9_0230_472D_B7FA_D0BD4C34AEF3__INCLUDED_)
