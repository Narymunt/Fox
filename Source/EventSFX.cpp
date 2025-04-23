// EventSFX.cpp: implementation of the CEventSFX class.
//
//////////////////////////////////////////////////////////////////////

#include "EventSFX.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEventSFX::CEventSFX()
{

}

//=== destruktor zwalnia pamiec i zeruje wskazniki jezeli sa uzywane

CEventSFX::~CEventSFX()
{

}

//=== czy wave w zdarzeniu maja byc losowane ? ===

void CEventSFX::SetLosowanie(unsigned char ucValue)
{
	m_ucLosowanie = ucValue;
}

//=== zwroc flage czy wave ma byc losowany ===

unsigned char CEventSFX::GetLosowanie(void)
{
	return m_ucLosowanie;
}

//=== ustaw flage czy te wave w tym zdarzeniu (index) maja byc odtwarzane po kolei

void CEventSFX::SetPoKolei(unsigned char ucValue)
{
	m_ucPoKolei = ucValue;
}

//=== zwroc flage czy wave maja byc odtwarzane po kolei

unsigned char CEventSFX::GetPoKolei(void)
{
	return m_ucPoKolei;
}

//=== ustaw liczbe wave, ktore znajduja sie w tym zdarzeniu

void CEventSFX::SetWaveCount(unsigned int uiValue)
{
	m_uiLiczbaWave = uiValue;
}

//=== zwroc liczbe wave w tym zdarzeniu  

unsigned int CEventSFX::GetWaveCount(void)
{
	return m_uiLiczbaWave;
}

//=== ustawia nazwe pliku wave dla danego zdarzenia

void CEventSFX::SetWaveFilename(unsigned int uiWaveIndex, char cName[])
{
	strcpy(&m_pNazwaPliku[uiWaveIndex], cName);
}

//=== pobiera nazwe pliku wave dla danego zdarzenia 

char *CEventSFX::GetWaveFilename(unsigned int uiWaveIndex)
{
	return &m_pNazwaPliku[uiWaveIndex];
}

//=== ustawia opis wave w zdarzeniu dzwiekowym

void CEventSFX::SetFilenameDesc(unsigned int uiWaveIndex, char cName[])
{
	strcpy(&m_pOpis[uiWaveIndex], cName);
}

//=== pobiera opis wave w zdarzeniu dzwiekowym

char *CEventSFX::GetFilenameDesc(unsigned int uiWaveIndex)
{
	return &m_pOpis[uiWaveIndex];
}

//=== ustawia nazwe zdarzenia dzwiekowego

void CEventSFX::SetEventName(unsigned int uiEventIndex, char cName[])
{
	strcpy(m_pNazwaZdarzenia, cName);
}

//=== pobiera nazwe zdarzenia dzwiekoweg

char *CEventSFX::GetEventName(unsigned int uiEventIndex)
{
	return m_pNazwaZdarzenia;
}


//= end = 