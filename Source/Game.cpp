// Game.cpp: implementation of the CGame class.
//
//////////////////////////////////////////////////////////////////////

#include "Game.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGame::CGame()
{

}

CGame::~CGame()
{

}

//=== ustawia wersje pliku ===

void CGame::SetFileVersion(unsigned char cVersion)
{
	m_ucFileVersion = cVersion;
}

//=== zwraca wersje pliku jezeli zostala ustawiona ===
//=== NULL oznacza, ze nie bylo jeszcze ustawione ===

unsigned char CGame::GetFileVersion(void)
{
	return m_ucFileVersion;
}

//=== ustaw liczbe wlasciwosc - opis w pliku *.h ===

void CGame::SetPropertiesCount(unsigned int iCount)
{
	m_uiPropertiesCount = iCount; 
}

//=== zwroc liczbe wlasciwosci - opis w pliku *.h ===

unsigned int CGame::GetPropertiesCount(void)
{
	return m_uiPropertiesCount;
}

//=== ustaw liczbe zmiennych ===

void CGame::SetVariablesCount(unsigned int iCount)
{
	m_uiVariablesCount = iCount;
}

//=== pobierz liczbe zmiennych ===

unsigned int CGame::GetVariablesCount(void)
{
	return m_uiVariablesCount;
}

//=== ustaw liczbe procedur na cykl ===

void CGame::SetCycleProcedureCount(unsigned int iCount)
{
	m_uiCycleProcedureCount = iCount;
}

//=== pobierz liczbe procedur na cykl ===

unsigned int CGame::GetCycleProcedureCount(void)
{
	return m_uiCycleProcedureCount;
}

//=== ustaw liczbe procedur na tik ===

void CGame::SetTikProcedureCount(unsigned int iCount)
{
	m_uiTikProcedureCount = iCount;
}

//=== pobierz liczbe procedur na tik ====

unsigned int CGame::GetTikProcedureCount(void)
{
	return m_uiTikProcedureCount;
}

//=== ustaw liczbe warunkow === 

void CGame::SetIfCount(unsigned int uiCount)
{
	m_uiIfCount = uiCount;
}

//=== zwroc liczbe warunkow ===

unsigned int CGame::GetIfCount(void)
{
	return m_uiIfCount;
}
