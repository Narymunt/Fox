// BankSFX.cpp: implementation of the CBankSFX class.
//
//////////////////////////////////////////////////////////////////////

#include "BankSFX.h"
#include "FileSystem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== stworzenie obiektu i wyzerowanie wskaznikow ===
//=== bank sfx moze byc w innym pliku a wave w innym ===

CBankSFX::CBankSFX(char cBankArchiveName[], char cBankFilename[])
{
	m_pEventsSFX = NULL;
	
	// wyczysc nazwy

	m_pBankSFXFileSystem = NULL;
	m_pEventSFXFileSystem = NULL;

	// teraz otworz

	m_pBankSFXFileSystem = new CFileSystem(cBankArchiveName);	// bank sfx
//	m_pEve



//m_pFile = new CFileSystem("Resource\\klocki.fox");
	


}

//=== usuniecie obiektu i wyzerowanie wskaznikow ===

CBankSFX::~CBankSFX()
{

	// tutaj czyscimy zdarzenia

	for (unsigned int uiLicznik=0; uiLicznik < m_uiEventCount; uiLicznik++)
	{
		if (m_pEventsSFX[uiLicznik] != NULL )
		{
			delete m_pEventsSFX[uiLicznik];
			m_pEventsSFX[uiLicznik] = NULL;
		}
	}

	// a tutaj wskaznik na wskazniki

	if (m_pEventsSFX != NULL)
	{
		delete m_pEventsSFX;
		m_pEventsSFX = NULL;
	}

}	

//=== deinizjalizacja ===

void CBankSFX::DeInitializeALL(void)
{
	// tutaj czyscimy zdarzenia

	for (unsigned int uiLicznik=0; uiLicznik < m_uiEventCount; uiLicznik++)
	{
		if (m_pEventsSFX[uiLicznik] != NULL )
		{
			delete m_pEventsSFX[uiLicznik];
			m_pEventsSFX[uiLicznik] = NULL;
		}
	}

	// a tutaj wskaznik na wskazniki

	if (m_pEventsSFX != NULL)
	{
		delete m_pEventsSFX;
		m_pEventsSFX = NULL;
	}
}

//=== ustawia wersje banku dzwiekowego ===

void CBankSFX::SetVersion(unsigned char ucIndex)
{
	m_ucVersion = ucIndex;
}

//=== pobiera wersje banku dzwiekowego ===

unsigned char CBankSFX::GetVersion(void)
{
	return m_ucVersion;
}


//= end =