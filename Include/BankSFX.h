// BankSFX.h: interface for the CBankSFX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BANKSFX_H__E1D8FD93_88BE_42B8_BFB7_B20B1D2F6CED__INCLUDED_)
#define AFX_BANKSFX_H__E1D8FD93_88BE_42B8_BFB7_B20B1D2F6CED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// w banku sfx znajduja sie CSound, ktore sa odtwarzane losowo, albo z parametrami

#include "Sound.h"

class CBankSFX  
{

public:
	CBankSFX();
	virtual ~CBankSFX();

private:

	unsigned char		m_ucVersion;		// wersja 0... shinji, 128... fremen

	// petla #1

	unsigned char		m_ucLosowanie;		// czy wave ma byc wylosowany
	unsigned char		m_ucPoKolei;		// leci jeden za drugim

	unsigned int		m_uiLiczbaWave;		// ile dzwiekow jest w tym banku

	char				*p_cNazwaPliku;		// 256 
	char				*p_cOpis;			// 1024 opis dŸwiêku - potrzebne ? 

	CSound				**pWave;			// tutaj te wave

	// koniec - petla #1


};

#endif // !defined(AFX_BANKSFX_H__E1D8FD93_88BE_42B8_BFB7_B20B1D2F6CED__INCLUDED_)
