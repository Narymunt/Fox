// BankSFX.h: interface for the CBankSFX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXBANKSFX_
#define _FOXBANKSFX_

//=== bank dzwiekowy

//=== TODO:
//===	- znaczniki 

// w banku sfx znajduja sie CSound, ktore sa odtwarzane losowo, albo z parametrami

#include "EventSFX.h"
#include "FileSystem.h"

class __declspec(dllexport) CBankSFX  
{

public:

	CBankSFX(char cBankArchiveName[], char cBankFilename[]);		// konstruktor inicjalizuje wskazniki
	virtual ~CBankSFX();	// zwalnia pamiec jezeli uzywana 

	// poniewaz banki sa takie same, a wave roznia sie ze wzgledu na wersje jezykowa, 
	// pokazujemy mu katalog/archiwum z ktorego ma wczytywac wave's dla danego zdarzenia

//	virtual void InitializeEventByName(char cEventName[], char cWaveFileSystem[]);
//	virtual void DeInitializeEventByName(char cEventName[]);

	// tutaj tak samo tylko, ze wedlug indexu, przydatne przy czyszczeniu

//	virtual void InitializeEventByIndex(unsigned int uiEventIndex, char cWaveFileSystem[]);
//	virtual void DeInitializeEventByIndex(unsigned int uiEventIndex);

	// inicjalizowanie wszystkich i deinicjalizowanie wszystkich zajetych

//	virtual void InitializeALL(char cFileSystem[], char cBankName[]);	// wczytuje dane
	virtual void DeInitializeALL(void);	// usuwa elementy

	// odtwarza event wedlug indexu lub nazwy

//	virtual void PlayEventByName(char cEventName[]);	// odtwarzaj event audio
//	virtual void PlayEventByIndex(unsigned int uiEventIndex);	// odtwarzaj event audio

	// ustaw lub pobierz wersje pliku

	virtual void SetVersion(unsigned char ucIndex);	
	virtual unsigned char GetVersion(void);	

private:

	unsigned char		m_ucVersion;		// wersja 0... shinji, 128... fremen

	unsigned int		m_uiEventCount;		// ile zdarzen dzwiekowych ? 

	CEventSFX			**m_pEventsSFX;		// tutaj nasze zainicjalizowane (lub nie)
											// zdarzenia dzwiekowe

	CFileSystem			*m_pBankSFXFileSystem;	// z tego miejsca wczytujemy 
	CFileSystem			*m_pEventSFXFileSystem;	// z tego miejsca wave's

};


#endif 

//= end =
