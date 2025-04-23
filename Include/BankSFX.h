// BankSFX.h: interface for the CBankSFX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXBANKSFX_
#define _FOXBANKSFX_

//=== bank dzwiekowy

//=== TODO:
//===	- znaczniki 

// w banku sfx znajduja sie CSound, ktore sa odtwarzane losowo, albo z parametrami

#include "Sound.h"

class __declspec(dllexport) CBankSFX  
{

public:
	CBankSFX();		// konstruktor inicjalizuje wskazniki
	virtual ~CBankSFX();	// zwalnia pamiec jezeli uzywana 

	virtual void InitializeALL(char cFileSystem[], char cBankName[]);	// wczytuje dane
	virtual void DeInitialize(void);	// usuwa elementy

	virtual void PlayEventByName(char cEventName[]);	// odtwarzaj event audio
	virtual void PlayEventByIndex(unsigned long ulIndex);	// odtwarzaj event audio

	virtual void SetVersion(unsigned char ucIndex);	// ustaw wersje
	virtual unsigned char GetVersion(void);	// pobierz wersje

	virtual void SetLosowanie(unsigned int uiEventIndex, unsigned char ucValue);	// czy ma byc losowany
	virtual unsigned char GetLosowanie(unsigned int uiEventIndex);	// zwraca flage

	virtual void SetPoKolei(unsigned int uiEventIndex, unsigned char ucValue);	// czy odtwarzac wave po kolei ? 
	virtual unsigned char GetPoKolei(unsigned int uiEventIndex);	// pobierz flage

	virtual void SetWaveCount(unsigned int uiEventIndex, unsigned int uiValue);	// ile wave w tym zdarzeniu ? 
	virtual unsigned int GetWaveCount(unsigned int uiEventIndex); // zwraca liczbe wave w zdarzeniu

	// ustawia/pobiera nazwe pliku wave dla danego zdarzenia

	virtual void SetWaveFilename(unsigned int uiEventIndex, unsigned int uiWaveIndex, char cName[]);
	virtual char *GetWaveFilename(unsigned int uiEventIndex, unsigned int uiWaveIndex);

	// ustawia/pobiera opis wave w zdarzeniu dzwiekowym

	virtual void SetFilenameDesc(unsigned int uiEventIndex, unsigned int uiWaveIndex, char cName[]);
	virtual char *GetFilenameDesc(unsigned int uiEventIndex, unsigned int uiWaveIndex);

private:

	unsigned char		m_ucVersion;		// wersja 0... shinji, 128... fremen

	// petla #1

	unsigned char		*m_pLosowanie;		// czy wave ma byc wylosowany
	unsigned char		*m_pPoKolei;		// leci jeden za drugim

	unsigned int		*m_pLiczbaWave;		// ile dzwiekow jest w tym banku

	char				*m_pNazwaPliku;		// 256 
	char				*m_pOpis;			// 1024 opis dŸwiêku - potrzebne ? 

	CSound				**m_pWave;			// tutaj te wave

	// koniec - petla #1

	unsigned int		m_uiEventCount;		// ile zdarzen dzwiekowych ? 

};

#endif 

//= end =
