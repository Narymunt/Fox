// Episode.cpp: implementation of the CEpisode class.
//
//////////////////////////////////////////////////////////////////////

#include "Episode.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== konstruktor ustawia od razu znaczniki

CEpisode::CEpisode()
{
	m_bActive = false;	// domyslnie wylaczony
	m_pWorlds = NULL;	// zerowane, bo nie jeste zaalokowany
}

CEpisode::~CEpisode()
{
	m_bActive = false;	// wylaczony

	if (m_pWorlds != NULL)
	{
		delete m_pWorlds;
		m_pWorlds = NULL;
	}
}

//=== ustaw stan episodu === 

void CEpisode::SetState(bool bState)
{
	m_bActive = bState;
}

//=== pobierz stan epizodu ===

bool CEpisode::GetState(void)
{
	return m_bActive;
}