// Game.h: interface for the CGame class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXGAME_
#define _FOXGAME_

#include <windows.h>
#include <d3d8.h>

// glowna klasa obslugujaca gre
// TODO:
//	- dynamiczna tablica zmiennych
//  - stala tablica wlasciwosci
//	- tablica procedur per tick
//  - tablica procedur per frame
//	- tablica warunkow per tick
//	- tablica warunkow per frame

class CEpisode;

class __declspec(dllexport) CGame  
{
public:
	CGame();
	virtual ~CGame();

	/*
	virtual void Process(unsigned int message);
	virtual void Upgrade
	*/

	//=== do danych importowanych z edytora

	virtual void			SetFileVersion(unsigned char cVersion);	// ustawia
	virtual unsigned char	GetFileVersion(void); // zwraca

	virtual void			SetPropertiesCount(unsigned int iCount);
	virtual unsigned int	GetPropertiesCount(void);

	virtual void			SetVariablesCount(unsigned int iCount);
	virtual unsigned int	GetVariablesCount(void);

	virtual void			SetCycleProcedureCount(unsigned int iCount);
	virtual unsigned int	GetCycleProcedureCount(void);

	virtual void			SetTikProcedureCount(unsigned int iCount);
	virtual unsigned int	GetTikProcedureCount(void);

	virtual void			SetIfCount(unsigned int uiCount);
	virtual unsigned int	GetIfCount(void);

private:
	CEpisode	**m_pEpisodes;		// tutaj poszczegolne episody

	unsigned long	m_ulTimer001;		// pierwszy timer

	unsigned long	m_ulLastFrameCount;	// ostatni numer rysowanej klatki
	unsigned long	m_ulLastFrameRate;	// ostatni fps 

	//=== importowane z pliku ===

	unsigned char m_ucFileVersion;	// wersja pliku

	char	m_cGameName;	// nazwa gry

	
	// pierwsze to liczba wlasciwosci
	// drugie to liczba wlasciwosci + zmienne
	// czyli zeby uzyskac zmienne to trzeba odjac jedno od drugiego

	unsigned int m_uiPropertiesCount;	// lista zmiennych wlasciwosci importowanych
	unsigned int m_uiVariablesCount;

	unsigned int m_uiCycleProcedureCount;	// lista procedur eksportowanych (cykl)
	unsigned int m_uiTikProcedureCount;	// lista procedur eksportowanych (zegar)

	unsigned int m_uiIfCount;			// lista / liczba warunkow eksportowanych
	unsigned int m_uiSpritesCount;	// lista sprites eksportowanych (ile ich jest)

};

#endif 
