// klasa opisuje ekran, rozdzielczosc oraz vaddr

#ifndef _fox_zip_image_tga_
#define _fox_zip_image_tga_

#include <stdio.h>
#include "sdl.h"
#include "zip.h"
#include "unzip.h"

class CZipImageTGA
{

public : 

	CZipImageTGA(char cZipFile[], char cPassword[], char cFilename[]);	
	~CZipImageTGA();

	int	iGetSizeX(void);
	int	iGetSizeY(void);
	int	iGetBpp(void);

	
	char	*m_pData;

protected :

	unsigned char	m_pHeader[18];
	int		m_iSizeX, m_iSizeY, m_iBpp;

};

#endif
