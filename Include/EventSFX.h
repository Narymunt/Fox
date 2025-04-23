// EventSFX.h: interface for the CEventSFX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXEVENTSFX_
#define _FOXEVENTSFX_

#include "Sound.h"

//=== zdarzenie dzwiekowe, wchodzi w sk³ad banku dŸwiêkowego 
//=== zawiera kilkanaœcie plików wave, które s¹ odgrywane w ustalonej kolejnosc

class __declspec(dllexport) CEventSFX  
{

public:
	
	CEventSFX();			// konstruktor inicjalizuje
	virtual ~CEventSFX();

	virtual void SetLosowanie(unsigned char ucValue);	// czy ma byc losowany
	virtual unsigned char GetLosowanie(void);	// zwraca flage

	virtual void SetPoKolei(unsigned char ucValue);	// czy odtwarzac wave po kolei ? 
	virtual unsigned char GetPoKolei(void);	// pobierz flage

	virtual void SetWaveCount(unsigned int uiValue);	// ile wave w tym zdarzeniu ? 
	virtual unsigned int GetWaveCount(void); // zwraca liczbe wave w zdarzeniu
	
	// pobiera / ustawia nazwe zdarzenia

	virtual void SetEventName(unsigned int uiEventIndex, char cName[]);
	virtual char *GetEventName(unsigned int uiEventIndex);

	// ustawia/pobiera nazwe pliku wave dla danego zdarzenia

	virtual void SetWaveFilename(unsigned int uiWaveIndex, char cName[]);
	virtual char *GetWaveFilename(unsigned int uiWaveIndex);

	// ustawia/pobiera opis wave w zdarzeniu dzwiekowym

	virtual void SetFilenameDesc(unsigned int uiWaveIndex, char cName[]);
	virtual char *GetFilenameDesc(unsigned int uiWaveIndex);

private:

	char				*m_pNazwaZdarzenia;	// zapisana na sta³e [256]

	unsigned char		m_ucLosowanie;		// czy wave ma byc wylosowany
	unsigned char		m_ucPoKolei;		// leci jeden za drugim

	unsigned int		m_uiLiczbaWave;		// ile dzwiekow jest w tym banku

	char				*m_pNazwaPliku;		// 256 bajtow * liczba wave
	char				*m_pOpis;			// 1024 opis dŸwiêku * liczba wave 

	CSound				**m_pWave;			// tutaj te wave

};

#endif 
