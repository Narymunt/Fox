// BankSFX.cpp: implementation of the CBankSFX class.
//
//////////////////////////////////////////////////////////////////////

#include "BankSFX.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== stworzenie obiektu i wyzerowanie wskaznikow ===

CBankSFX::CBankSFX()
{
	m_pNazwaPliku = NULL;
	m_pOpis = NULL;
	
}

//=== usuniecie obiektu i wyzerowanie wskaznikow ===

CBankSFX::~CBankSFX()
{
	if (m_pNazwaPliku!=NULL)
	{
		free(m_pNazwaPliku);
		m_pNazwaPliku = NULL;
	}

	if (m_pOpis!=NULL)
	{
		free(m_pOpis);
		m_pNazwaPliku = NULL;
	}

}

//=== deinizjalizacja ===

void CBankSFX::DeInitialize(void)
{
	if (m_pNazwaPliku!=NULL)
	{
		free(m_pNazwaPliku);
		m_pNazwaPliku = NULL;
	}

	if (m_pOpis!=NULL)
	{
		free(m_pOpis);
		m_pNazwaPliku = NULL;
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

//=== czy wave w zdarzeniu maja byc losowane ? ===
//=== index oznacza tak naprawde numer zdarzenia dzwiekowego

void CBankSFX::SetLosowanie(unsigned int uiEventIndex, unsigned char ucValue)
{
	m_pLosowanie[uiEventIndex] = ucValue;
}

//=== zwroc flage czy wave ma byc losowany ===
//=== index oznacza tak naprawde numer zdarzenia dzwiekowego

unsigned char CBankSFX::GetLosowanie(unsigned int uiEventIndex)
{
	return m_pLosowanie[uiEventIndex];
}

//=== ustaw flage czy te wave w tym zdarzeniu (index) maja byc odtwarzane po kolei
//=== index oznacza numer zdarzenia dzwiekowego

void CBankSFX::SetPoKolei(unsigned int uiEventIndex, unsigned char ucValue)
{
	m_pPoKolei[uiEventIndex] = ucValue;
}

//=== zwroc flage czy wave maja byc odtwarzane po kolei
//=== index oznacza numer zdarzenia dzwiekowego

unsigned char CBankSFX::GetPoKolei(unsigned int uiEventIndex)
{
	return m_pPoKolei[uiEventIndex];
}

//=== ustaw liczbe wave, ktore znajduja sie w tym zdarzeniu
//=== index oznacza numer zdarzenia dla ktorego ustawiamy

void CBankSFX::SetWaveCount(unsigned int uiEventIndex, unsigned int uiValue)
{
	m_pLiczbaWave[uiEventIndex] = uiValue;
}

//=== zwroc liczbe wave w tym zdarzeniu
//=== index oznacza numer zdarzenia dla ktorego ustawiamy

unsigned int CBankSFX::GetWaveCount(unsigned int uiEventIndex)
{
	return m_pLiczbaWave[uiEventIndex];
}

//= end =