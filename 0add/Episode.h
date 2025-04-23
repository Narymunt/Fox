// Episode.h: interface for the CEpisode class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXEPISODE_
#define _FOXEPISODE_

#include <windows.h>

// glowna klasa obslugujaca episod bedaca czescia cgame
// TODO:
//	- dynamiczna tablica zmiennych
//  - stala tablica wlasciwosci
//	- tablica procedur per tick
//  - tablica procedur per frame
//	- tablica warunkow per tick
//	- tablica warunkow per frame

class CWorld;

class __declspec(dllexport) CEpisode  
{
public:
	CEpisode();
	virtual ~CEpisode();

	// zmiana aktywnosci

	virtual void	SetState(bool bState);
	virtual bool	GetState(void); 


private:

	bool	m_bActive;			// czy ma byc brany pod uwage przy petli

	CWorld	**m_pWorlds;		// swiaty wewnatrz episodu

};

#endif 
