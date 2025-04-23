// BankSFX.h: interface for the CBankSFX class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXBANKSFX_
#define _FOXBANKSFX_

//=== bank dzwiekowy

//=== TODO:
//===	- znaczniki 

// w banku sfx znajduja sie CSound, ktore sa odtwarzane losowo, albo z parametrami

#include "Sound.h"

class __declspec(dllexport) CBankSFX  
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

	char				*m_pNazwaPliku;		// 256 
	char				*m_pOpis;			// 1024 opis dŸwiêku - potrzebne ? 

	CSound				**m_pWave;			// tutaj te wave

	// koniec - petla #1


};

#endif 

//= end =
