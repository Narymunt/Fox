// klasa opisuje ekran, rozdzielczosc oraz vaddr

#ifndef _fox_image_tga_
#define _fox_image_tga_

#include <stdio.h>
#include "sdl.h"

class CImageTGA
{

public : 

	CImageTGA(char cFilename[]);	
	~CImageTGA();

	int	iGetSizeX(void);
	int	iGetSizeY(void);
	int	iGetBpp(void);

	
	char	*m_pData;

protected :

	unsigned char	m_pHeader[18];
	int		m_iSizeX, m_iSizeY, m_iBpp;

};

#endif
