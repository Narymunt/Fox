// PSprite.h: interface for the CPSprite class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXPSPRITE_
#define _FOXPSPRITE_

#include <windows.h>
#include <fox.h>

// fizyczny obiekt

class __declspec(dllexport) CPSprite  
{
public:
	CPSprite(double dMass);
	virtual ~CPSprite();

private:

	double	m_dMass;	// masa obiektu
	double	m_dHard;	// tarcie ;) 

	long	m_lFlag;	// typ obiektu

	CVector	*m_pvPosition;   // wspo³rzêdne na p³aszczyŸnie
	CVector *m_pvSpeed;	   // wektor przyœpieszenia
	CVector *m_pvActionStrength;	// si³a wypadkowa


};

#endif // !defined(AFX_PSPRITE_H__A1E06858_0EBA_42C0_AA69_4934136632E4__INCLUDED_)
