// klasa opisuje ekran, rozdzielczosc oraz vaddr

#ifndef _fox_screen_
#define _fox_screen_

#include "sdl.h"

class CScreen
{

public : 

	CScreen(int iX, int iY, int iBpp,bool bFullScreen);	// inicjalizacja okna
	CScreen(bool bFullScreen);	// inicjalizacja okna w takiej samej rozdzielczosci jaka jest
	~CScreen();

	void Blur(int iCount);

	int	iGetSizeX(void);
	int	iGetSizeY(void);
	int	iGetBpp(void);
	bool isFullscreen(void);

	SDL_Surface	*m_pScreen;	// publiczne, ¿eby u³atwiæ 

protected :

	bool	m_bFullscreen;
	int		m_iSizeX, m_iSizeY, m_iBpp;

};

#endif
