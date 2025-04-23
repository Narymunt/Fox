// Event.cpp: implementation of the CEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "Event.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== konstruktor, stworzenie obiektu i ustawienie wskaznikow ===

CEvent::CEvent()
{
	m_pNazwaZdarzenia = NULL;

	m_pPositionX = NULL;
	m_pPositionY = NULL;

	// alokacja

	m_pNazwaZdarzenia= (unsigned char*) malloc(256);	

	// wyzeruj wskazniki

	m_pFileNazwaBanku = NULL;
	m_pFileNazwaZdarzenia = NULL;
}

//=== usuniecie obiektu i wyzerowanie wskaznikow ===

CEvent::~CEvent()
{
	
	if (m_pNazwaZdarzenia!=NULL)	// zwolnij obiekt jezeli jest zaalokowany
	{
		free(m_pNazwaZdarzenia);
		m_pNazwaZdarzenia = NULL;
	}

	if (m_pFileNazwaBanku!=NULL)	// zwolnij obiekt jezeli jest zaalokowany
	{
		free(m_pFileNazwaBanku);
		m_pFileNazwaBanku = NULL;
	}

	if (m_pFileNazwaZdarzenia!=NULL)
	{
		free(m_pFileNazwaZdarzenia);
		m_pFileNazwaZdarzenia = NULL;
	}

}

// zwraca wskaznik na nazwe zdarzenia

unsigned char *CEvent::pGetNazwaZdarzenia(void)
{
	return m_pNazwaZdarzenia;
}

//=== ustaw liczbe klatek w zdarzeniu

void CEvent::SetLiczbaKlatek(unsigned int m_uiIle)
{
	m_uiLiczbaKlatek=m_uiIle;
}

//=== pobierz liczbe klatek w zdarzeniu

unsigned int CEvent::GetLiczbaKlatek(void)
{
	return m_uiLiczbaKlatek;
}

//=== zaalokuj pamiec na wspolrzedne

void CEvent::AllocPositions(void)
{
	m_pPositionX=(int*) malloc(m_uiLiczbaKlatek*sizeof(int));
	m_pPositionY=(int*) malloc(m_uiLiczbaKlatek*sizeof(int));
}

//=== pobierz wspolrzedne X

int CEvent::GetPositionX(unsigned long ulIndex)
{
	return m_pPositionX[ulIndex];
}

//=== ustaw wspolrzedne X

void CEvent::SetPositionX(unsigned long ulIndex, int iValue)
{
	m_pPositionX[ulIndex]=iValue;
}

//=== pobierz wspolrzedne Y

int CEvent::GetPositionY(unsigned long ulIndex)
{
	return m_pPositionY[ulIndex];
}

//=== ustaw wspolrzedne Y

void CEvent::SetPositionY(unsigned long ulIndex, int iValue)
{
	m_pPositionY[ulIndex]=iValue;
}

//=== pobierz nr klatki z listy

unsigned int CEvent::GetNrKlatki(unsigned long ulIndex)
{
	return m_pNrKlatki[ulIndex];
}

//=== ustaw nr klatki na liscie

void CEvent::SetNrKlatki(unsigned long ulIndex, unsigned int uiNrKlatki)
{
	m_pNrKlatki[ulIndex]=uiNrKlatki;
}

//=== zaalokuj pamiec na liste klatek

void CEvent::AllocKlatki(void)
{
	m_pNrKlatki = (unsigned int*) malloc(m_uiLiczbaKlatek*sizeof(unsigned int));
}

//=== czy odtwarzac wave dla danej klatki

unsigned char CEvent::GetPlayWaveInfo(unsigned long ulIndex)
{
	return m_pPlayWave[ulIndex];
}

//=== ustaw znacznik wave dla danej klatki

void CEvent::SetPlayWaveInfo(unsigned long ulIndex, unsigned char ucValue)
{
	m_pPlayWave[ulIndex]=ucValue;
}

//=== zaalokuj pamiec na info o czymstam

void CEvent::AllocWaveInfo(void)
{
	m_pPlayWave = (unsigned char*) malloc(m_uiLiczbaKlatek);
}

//=== zaalokuj pamiec na nazwy bankow

void CEvent::AllocSFXBankNames(void)
{
	m_pFileNazwaBanku = (unsigned char*) malloc (256);
	m_pFileNazwaZdarzenia = (unsigned char*) malloc (256);
}

//=== zwraca wskaznik na zaalokowane nazwy zdarzen

unsigned char *CEvent::pGetFileNazwaBanku(void)
{
	return m_pFileNazwaBanku;
}

//=== zwraca wskaznik na zaalokowane nazwy zdarzen

unsigned char *CEvent::pGetFileNazwaZdarzenia(void)
{
	return m_pFileNazwaZdarzenia;
}

//=== ustaw informacje czy w tej chwili jest odtwarzany dzwiek

void CEvent::SetIsPlayingSound(bool bState)
{
	m_bIsPlayingSound = bState;
}

//=== pobierz informacje czy w tej chwili jest odtwarzany dzwiek

bool CEvent::GetIsPlayingSound(void)
{
	return m_bIsPlayingSound;
}

//=== end ===
