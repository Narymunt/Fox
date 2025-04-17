// BankSFX.cpp: implementation of the CBankSFX class.
//
//////////////////////////////////////////////////////////////////////

#include <BankSFX.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// stworzenie obiektu i wyzerowanie wskaznikow

CBankSFX::CBankSFX()
{
	p_cNazwaPliku = NULL;
	p_cOpis = NULL;
	
}

// usuniecie obiektu i wyzerowanie wskaznikow

CBankSFX::~CBankSFX()
{
	if (p_cNazwaPliku!=NULL)
	{
		free(p_cNazwaPliku);
		p_cNazwaPliku = NULL;
	}

	if (p_cOpis!=NULL)
	{
		free(p_cOpis);
		p_cNazwaPliku = NULL;
	}

}

