// Event.cpp: implementation of the CEvent class.
//
//////////////////////////////////////////////////////////////////////

#include <Event.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== konstruktor, stworzenie obiektu i ustawienie wskaznikow ===

CEvent::CEvent()
{
	p_ucNazwaZdarzenia = NULL;

	m_iPositionX = NULL;
	m_iPositionY = NULL;

	// alokacja

	p_ucNazwaZdarzenia= (unsigned char*) malloc(256);	

	// wyzeruj wskazniki

	p_ucFileNazwaBanku = NULL;
	p_ucFileNazwaZdarzenia = NULL;
}

//=== usuniecie obiektu i wyzerowanie wskaznikow ===

CEvent::~CEvent()
{
	
	if (p_ucNazwaZdarzenia!=NULL)
	{
		free(p_ucNazwaZdarzenia);
		p_ucNazwaZdarzenia = NULL;
	}

	if (p_ucFileNazwaBanku!=NULL)
	{
		free(p_ucFileNazwaBanku);
		p_ucFileNazwaBanku = NULL;
	}

	if (p_ucFileNazwaZdarzenia!=NULL)
	{
		free(p_ucFileNazwaZdarzenia);
		p_ucFileNazwaZdarzenia = NULL;
	}

}

// zwraca wskaznik na nazwe zdarzenia

unsigned char *CEvent::pGetNazwaZdarzenia(void)
{
	return p_ucNazwaZdarzenia;
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
	m_iPositionX=(int*) malloc(m_uiLiczbaKlatek*sizeof(int));
	m_iPositionY=(int*) malloc(m_uiLiczbaKlatek*sizeof(int));
}

//=== pobierz wspolrzedne X

int CEvent::GetPositionX(unsigned long ulIndex)
{
	return m_iPositionX[ulIndex];
}

//=== ustaw wspolrzedne X

void CEvent::SetPositionX(unsigned long ulIndex, int iValue)
{
	m_iPositionX[ulIndex]=iValue;
}

//=== pobierz wspolrzedne Y

int CEvent::GetPositionY(unsigned long ulIndex)
{
	return m_iPositionY[ulIndex];
}

//=== ustaw wspolrzedne Y

void CEvent::SetPositionY(unsigned long ulIndex, int iValue)
{
	m_iPositionY[ulIndex]=iValue;
}

//=== pobierz nr klatki z listy

unsigned int CEvent::GetNrKlatki(unsigned long ulIndex)
{
	return m_uiNrKlatki[ulIndex];
}

//=== ustaw nr klatki na liscie

void CEvent::SetNrKlatki(unsigned long ulIndex, unsigned int uiNrKlatki)
{
	m_uiNrKlatki[ulIndex]=uiNrKlatki;
}

//=== zaalokuj pamiec na liste klatek

void CEvent::AllocKlatki(void)
{
	m_uiNrKlatki = (unsigned int*) malloc(m_uiLiczbaKlatek*sizeof(unsigned int));
}

//=== czy odtwarzac wave dla danej klatki

unsigned char CEvent::GetPlayWaveInfo(unsigned long ulIndex)
{
	return m_bPlayWave[ulIndex];
}

//=== ustaw znacznik wave dla danej klatki

void CEvent::SetPlayWaveInfo(unsigned long ulIndex, unsigned char ucValue)
{
	m_bPlayWave[ulIndex]=ucValue;
}

//=== zaalokuj pamiec na info o czymstam

void CEvent::AllocWaveInfo(void)
{
	m_bPlayWave = (unsigned char*) malloc(m_uiLiczbaKlatek);
}

//=== zaalokuj pamiec na nazwy bankow

void CEvent::AllocSFXBankNames(void)
{
	p_ucFileNazwaBanku = (unsigned char*) malloc (256);
	p_ucFileNazwaZdarzenia = (unsigned char*) malloc (256);
}

//=== zwraca wskaznik na zaalokowane nazwy zdarzen

unsigned char *CEvent::pGetFileNazwaBanku(void)
{
	return p_ucFileNazwaBanku;
}

//=== zwraca wskaznik na zaalokowane nazwy zdarzen

unsigned char *CEvent::pGetFileNazwaZdarzenia(void)
{
	return p_ucFileNazwaZdarzenia;
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
