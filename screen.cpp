#include "screen.h"

CScreen::CScreen(int iX, int iY, int iBpp,bool bFullScreen)
{
	if (bFullScreen)
	{
		m_pScreen = SDL_SetVideoMode(iX, iY, iBpp,(SDL_SWSURFACE|SDL_FULLSCREEN));
	}
	else
	{
		m_pScreen = SDL_SetVideoMode(iX,iY, iBpp,(SDL_SWSURFACE));
	}

	m_iSizeX = iX;
	m_iSizeY = iY;
	m_iBpp = iBpp;
	m_bFullscreen = bFullScreen;

}

CScreen::CScreen(bool bFullScreen)
{
	const	SDL_VideoInfo	*m_pVideoInfo=SDL_GetVideoInfo();
	int i, d, n;
    const char *driver;
    SDL_DisplayMode mode; 
	Uint32 Rmask, Gmask, Bmask, Amask; 

	m_iSizeX = m_pVideoInfo->current_w;
	m_iSizeY = m_pVideoInfo->current_h;
	m_bFullscreen = bFullScreen;

    for (d = 0; d < SDL_GetNumVideoDisplays(); ++d) 
	{
        SDL_Rect bounds;

        SDL_GetDisplayBounds(d, &bounds);

        SDL_GetDesktopDisplayMode(d, &mode);
        SDL_PixelFormatEnumToMasks(mode.format, &m_iBpp, &Rmask, &Gmask, &Bmask,
                                   &Amask);
	}

	if (bFullScreen)
	{
		m_pScreen = SDL_SetVideoMode(m_iSizeX, m_iSizeY, m_iBpp,(SDL_SWSURFACE|SDL_FULLSCREEN));
	}
	else
	{
		m_pScreen = SDL_SetVideoMode(m_iSizeX,m_iSizeY, m_iBpp,(SDL_SWSURFACE));
	}

}

CScreen::~CScreen()
{

}

int CScreen::iGetBpp(void)
{
	return m_iBpp;
}

int CScreen::iGetSizeX(void)
{
	return m_iSizeX;
}

int CScreen::iGetSizeY(void)
{
	return m_iSizeY;
}

bool CScreen::isFullscreen(void)
{
	return m_bFullscreen;
}

void CScreen::Blur(int iCount)
{
	for (int i=0; i<iCount; i++)
	{
		for (int h1=0; h1<m_iSizeY; h1++)
		{
			for (int h2=0; h2<(m_iSizeX-1)<<2; h2++)
			{
				if (((unsigned char*)m_pScreen->pixels)[(m_iSizeX<<2)*h1+h2]>0) ((unsigned char*)m_pScreen->pixels)[(m_iSizeX<<2)*h1+h2] --;
				if (((unsigned char*)m_pScreen->pixels)[(m_iSizeX<<2)*h1+h2+1]>0) ((unsigned char*)m_pScreen->pixels)[(m_iSizeX<<2)*h1+h2+1] --;
				if (((unsigned char*)m_pScreen->pixels)[(m_iSizeX<<2)*h1+h2+2]>0) ((unsigned char*)m_pScreen->pixels)[(m_iSizeX<<2)*h1+h2+2] --;				
				if (((unsigned char*)m_pScreen->pixels)[(m_iSizeX<<2)*h1+h2+3]>0) ((unsigned char*)m_pScreen->pixels)[(m_iSizeX<<2)*h1+h2+3] --;				
			}
		}
	}
}