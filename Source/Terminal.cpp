// Terminal.cpp: implementation of the CTerminal class.
//
//////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <stdio.h>
#include <Terminal.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== konstruktor, inicjalizuj obiekty

CTerminal::CTerminal()
{
	
	// wyzeruj wskazniki
	
	p_cNazwaBankuSFX = NULL;
	p_cNazwaKoncowki = NULL;
	p_cNazwaZdarzeniaSFX = NULL;
	p_cNazwaZdarzeniaZAN = NULL;


}

//=== destruktor, usun obiekty

CTerminal::~CTerminal()
{

	if (p_cNazwaBankuSFX!=NULL)
	{
		free (p_cNazwaBankuSFX);
		p_cNazwaBankuSFX = NULL;
	}

	if (p_cNazwaKoncowki!=NULL)
	{
		free (p_cNazwaKoncowki);
		p_cNazwaKoncowki = NULL;
	}

	if (p_cNazwaZdarzeniaSFX!=NULL)
	{
		free (p_cNazwaZdarzeniaSFX);
		p_cNazwaZdarzeniaSFX = NULL;
	}

	if (p_cNazwaZdarzeniaZAN!=NULL)
	{
		free (p_cNazwaZdarzeniaZAN);
		p_cNazwaZdarzeniaZAN = NULL;
	}

}

//=== ustaw wersje 

void CTerminal::SetVersion(unsigned char ucValue)
{
	m_ucVersion = ucValue;
}

//=== pobierz wersje

unsigned char CTerminal::GetVersion(void)
{
	return m_ucVersion;
}

//=== ustaw starting true/false

void CTerminal::SetStarting(unsigned char ucValue)
{
	m_ucStarting = ucValue;
}

//=== pobierz starting 

unsigned char CTerminal::GetStarting(void)
{
	return m_ucStarting;
}

//=== ustaw ending true/false

void CTerminal::SetEnding(unsigned char ucValue)
{
	m_ucEnding = ucValue;
}

//=== pobierz ending

unsigned char CTerminal::GetEnding(void)
{
	return m_ucEnding;
}

//=== alokuj pamiec na nazwe koncowki

void CTerminal::AllocNazwaKoncowki(void)
{
	p_cNazwaKoncowki = (char*)malloc(256);
}

//=== zwroc wskaznik

char *CTerminal::pGetNazwaKoncowki(void)
{
	return p_cNazwaKoncowki;
}

//=== alokuj pamiec na nazwe zdarzenia ZAN

void CTerminal::AllocNazwaZdarzeniaZAN(void)
{
	p_cNazwaZdarzeniaZAN = (char*)malloc(256);
}

//=== zwroc wskaznik

char *CTerminal::pGetNazwaZdarzeniaZAN(void)
{
	return p_cNazwaZdarzeniaZAN;
}

//=== alokuj pamiec na nazwe banku sfx

void CTerminal::AllocNazwaZdarzeniaSFX(void)
{
	p_cNazwaZdarzeniaSFX = (char*)malloc(256);
}

//=== zwroc wskaznik

char *CTerminal::pGetNazwaZdarzeniaSFX(void)
{
	return p_cNazwaZdarzeniaSFX;
}

//=== alokuj pamiec

void CTerminal::AllocNazwaBankuSFX(void)
{
	p_cNazwaBankuSFX = (char*) malloc(256);
}

//=== zwroc wskaznik 

char *CTerminal::pGetNazwaBankuSFX(void)
{
	return p_cNazwaBankuSFX;
}


