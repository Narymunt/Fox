#include "image_tga.h"

CImageTGA::CImageTGA(char cFilename[])
{
	FILE	*plik;
	unsigned short	tmp;

	plik = fopen(cFilename,"rb");
	fread(m_pHeader,1,18,plik);

	tmp = (m_pHeader[13]<<8)|m_pHeader[12];
	m_iSizeX = (int)tmp;

	tmp = (m_pHeader[15]<<8)|m_pHeader[14];
	m_iSizeY = (int)tmp;

	m_iBpp = m_pHeader[16];

	switch (m_iBpp)
	{
		case 24:
				m_pData = new char[m_iSizeX*m_iSizeY*3];
				fread(m_pData,1,m_iSizeX*m_iSizeY*3,plik);
		break;

		case 32:
				m_pData = new char[m_iSizeX*m_iSizeY*4];
				fread(m_pData,1,m_iSizeX*m_iSizeY*4,plik);
		break;
	}

	fclose(plik);

}

CImageTGA::~CImageTGA()
{
	delete m_pData;
}

int CImageTGA::iGetBpp(void)
{
	return m_iBpp;
}

int CImageTGA::iGetSizeX(void)
{
	return m_iSizeX;
}

int CImageTGA::iGetSizeY(void)
{
	return m_iSizeY;
}

