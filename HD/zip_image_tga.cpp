#include "zip_image_tga.h"

CZipImageTGA::CZipImageTGA(char cZipFile[], char cPassword[], char cFilename[])
{
	
	HZIP hZipFile;	// z tego pliku wczytujemy
	ZIPENTRY ze;	
	int i;
	unsigned short	tmp;
	unsigned char	*pTempBuffer;	// tu rozpakujemy dane

	if ((hZipFile = OpenZip(cZipFile,cPassword))==NULL)
	{
		return;	
	};	// otwieramy plik zip z haslem
	FindZipItem(hZipFile,cFilename,true,&i,&ze); // znajdz plik do wczytania

	pTempBuffer = new unsigned char[ze.unc_size]; // przydziel w pamieci miejsce na rozpakowane dane

	UnzipItem(hZipFile,i,pTempBuffer,ze.unc_size);

	tmp = (pTempBuffer[13]<<8)|pTempBuffer[12];
	m_iSizeX = (int)tmp;

	tmp = (pTempBuffer[15]<<8)|pTempBuffer[14];
	m_iSizeY = (int)tmp;

	m_iBpp = pTempBuffer[16];

	switch (m_iBpp)
	{
		case 24:
				m_pData = new char[m_iSizeX*m_iSizeY*3];
				memcpy(m_pData,pTempBuffer+18,m_iSizeX*m_iSizeY*3);
		break;

		case 32:
				m_pData = new char[m_iSizeX*m_iSizeY*4];
				memcpy(m_pData,pTempBuffer+18,m_iSizeX*m_iSizeY*4);
		break;
	}


	CloseZip(hZipFile);
	delete pTempBuffer;
}

CZipImageTGA::~CZipImageTGA()
{
	delete m_pData;
}

int CZipImageTGA::iGetBpp(void)
{
	return m_iBpp;
}

int CZipImageTGA::iGetSizeX(void)
{
	return m_iSizeX;
}

int CZipImageTGA::iGetSizeY(void)
{
	return m_iSizeY;
}

