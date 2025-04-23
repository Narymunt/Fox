// PSprite.cpp: implementation of the CPSprite class.
//
//////////////////////////////////////////////////////////////////////

#include "PSprite.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== konstruktor 

CPSprite::CPSprite(double dMass)
{
	
	// alokuj wektory
	
	m_pvPosition = NULL;	// wspolrzedne
	m_pvPosition = new CVector();

	m_pvSpeed = NULL; // przyspieszenie
	m_pvSpeed = new CVector();

	m_pvActionStrength = NULL;	// si³a wypadkowa
	m_pvActionStrength = new CVector();


}

//=== destruktor zwalnia pamiec i zeruje wskazniki

CPSprite::~CPSprite()
{

	// zwolnij wektory

	if (m_pvPosition != NULL)
	{
		delete m_pvPosition;
		m_pvPosition = NULL;
	}

	if (m_pvSpeed != NULL)
	{
		delete m_pvSpeed;
		m_pvSpeed = NULL;
	}

	if (m_pvActionStrength != NULL)
	{
		delete m_pvActionStrength;
		m_pvActionStrength = NULL;
	}


}
