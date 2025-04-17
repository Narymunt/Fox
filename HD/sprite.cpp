#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sprite.h"
#include "image_tga.h"
#include "zip_image_tga.h"

/*
CSprite::CSprite(char cZipFile[],  char cPassword[],char cFilename[], int iBpp, int iOrgX, int iOrgY, int iDestX, int iDestY )
{
	return;
}
*/

// iBpp okresla do jakiego formatu obraz ma byc przekonwertowany
// powinien byc przekonwertowany do takiego samego ibpp jaki ma ekran
// dane obrazka trzymamy zawsze jako 32bpp, przy wyswietlaniu jako 24bpp robimy alfe na 4 bajcie


CSprite::CSprite(char cZipFile[],  char cPassword[],char cFilename[], int iBpp)
{
	CZipImageTGA	*pImage;

	pImage = new CZipImageTGA(cZipFile,cPassword,cFilename);		// nieznany bpp

	m_pSprite = new char[pImage->iGetSizeX() * pImage->iGetSizeY() *4];	// bierzemy pamiec od razu na 32bpp, dla 24bpp sami wyliczamy kolory na podstawie t³a
	
	for (long h1=0, h3=pImage->iGetSizeY()-1; h1<pImage->iGetSizeY(); h1++,h3--)
	{
		for (long h2=0; h2<pImage->iGetSizeX(); h2++)
		{
			if (pImage->iGetBpp()==24)
			{
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+1] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)+1];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+2] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)+2];
			}

			if (pImage->iGetBpp()==32)
			{
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+1] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+1];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+2] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+2];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+3] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+3];	// dodatkowy alfa
			}

		}
	}

	m_iXSize = pImage->iGetSizeX();
	m_iYSize = pImage->iGetSizeY();

	m_iBpp = iBpp;	// zapamietaj ibpp ekranu

	delete pImage;

	return;
}

// ibpp okresla do jakiego formatu ma byc przekonwertowany obraz

CSprite::CSprite(char cFilename[], int iBpp )
{	
	CImageTGA	*pImage;

	pImage = new CImageTGA("test.tga");		// nieznany bpp

	m_pSprite = new char[pImage->iGetSizeX() * pImage->iGetSizeY() *4];	// bierzemy pamiec od razu na 32bpp
	
	for (long h1=0, h3=pImage->iGetSizeY()-1; h1<pImage->iGetSizeY(); h1++,h3--)
	{
		for (long h2=0; h2<pImage->iGetSizeX(); h2++)
		{
			if (pImage->iGetBpp()==24)
			{
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+1] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)+1];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+2] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)+2];
			}

			if (pImage->iGetBpp()==32)
			{
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+1] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+1];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+2] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+2];
				m_pSprite[(h1*(pImage->iGetSizeX()*4))+(h2*4)+3] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+3];	// dodatkowy alfa
			}

		}
	}

	m_iXSize = pImage->iGetSizeX();
	m_iYSize = pImage->iGetSizeY();

	m_iBpp = iBpp;	// zapamietaj ibpp ekranu

	delete pImage;

	return;
}

CSprite::CSprite(char cFilename[], int iDestX, int iDestY, int iBpp)
{	
	CImageTGA	*pImage;
	char	*pTemp;
	float fDeltaX, fDeltaY, fX, fY;
	float	lAddr;

	pImage = new CImageTGA("test.tga");

	m_pSprite = new char[iDestY * iDestX *4];	// docelowy rozmiar obrazka

	//todo: niech rozroznia bpp, nie potrzeba zawsze 32bpp

	pTemp = new char[pImage->iGetSizeX() * pImage->iGetSizeY() *4];	// bierzemy pamiec od razu na 32bpp

	for (long h1=0, h3=pImage->iGetSizeY()-1; h1<pImage->iGetSizeY(); h1++,h3--)
	{
		for (long h2=0; h2<pImage->iGetSizeX(); h2++)
		{
			if (pImage->iGetBpp()==24)
			{
				pTemp[(h1*(pImage->iGetSizeX()*4))+(h2*4)] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)];
				pTemp[(h1*(pImage->iGetSizeX()*4))+(h2*4)+1] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)+1];
				pTemp[(h1*(pImage->iGetSizeX()*4))+(h2*4)+2] = pImage->m_pData[(h3*(pImage->iGetSizeX()*3))+(h2*3)+2];
			}

			if (pImage->iGetBpp()==32)
			{
				pTemp[(h1*(pImage->iGetSizeX()*4))+(h2*4)] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)];
				pTemp[(h1*(pImage->iGetSizeX()*4))+(h2*4)+1] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+1];
				pTemp[(h1*(pImage->iGetSizeX()*4))+(h2*4)+2] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+2];
				pTemp[(h1*(pImage->iGetSizeX()*4))+(h2*4)+3] = pImage->m_pData[(h3*(pImage->iGetSizeX()*4))+(h2*4)+3];	// dodatkowy alfa
			}
		}
	}

	// skalowanie obrazu do wymaganego rozmiaru

	fDeltaX = (float)pImage->iGetSizeX() / iDestX;
	fDeltaY = (float)pImage->iGetSizeY() / iDestY;

	fY = 0; 
	fX = 0;
	for (long d1=0; d1<iDestY; d1++)
	{
		fX =0; 
		for (long d2=0; d2<iDestX; d2++)
		{			
			lAddr = ((long)fY*(pImage->iGetSizeX()*4)) +((long)fX *4);

			m_pSprite[(d1*(iDestX*4))+(d2*4)] = pTemp[(long)lAddr];
			m_pSprite[(d1*(iDestX*4))+(d2*4)+1] = pTemp[(long)lAddr+1];
			m_pSprite[(d1*(iDestX*4))+(d2*4)+2] = pTemp[(long)lAddr+2];
			m_pSprite[(d1*(iDestX*4))+(d2*4)+3] = pTemp[(long)lAddr+3];

			fX +=fDeltaX;
		}
		fY += fDeltaY;
	}


	m_iXSize = iDestX;
	m_iYSize = iDestY;

	m_iBpp = iBpp;	// zapamietaj ibpp ekranu

	delete pImage;
	delete pTemp;

	return;
}


CSprite::~CSprite()
{
	delete m_pSprite;
	return;
}

//=== ustaw rotacje

void CSprite::SetRotation(float newRotation)
{
	m_Rotation = newRotation;
}
	
//=== pobierz rotacje

float CSprite::GetRotation(void)
{
	return m_Rotation;
}

//=== dodaj lub odejmij rotacje

void CSprite::AddRotation(float addValue)
{
	m_Rotation+=addValue;
}

//=== ustaw przesuniecie obiektu

void CSprite::SetPosition(int x, int y)
{
	m_iXPosition = x;
	m_iYPosition = y;
}

//=== pobierz przesuniecie x

int CSprite::iGetPositionX(void)
{
	return m_iXPosition;
}

//=== pobierz przesuniecie y

int CSprite::iGetPositionY(void)
{
	return m_iYPosition;
}

//=== dodaj lub odejmij przesuniecie

void CSprite::AddPosition(int x, int y)
{
	m_iXPosition += x;
	m_iYPosition += y;
}

//=== ustaw punkt rotacji w obiekcie

void CSprite::SetRotationCenter(int x, int y)
{
	m_iXRotation = x;
	m_iYRotation = y;
}

//=== zwroc wartosc kata w stopniach

float CSprite::GetDegree(float fVector)
{
	return fVector;// * (180.0f / D3DX_PI));
}

//=== zwroc wartosc kata w radianach

float CSprite::GetRadian(float fVector)
{
	return fVector;
}

//=== ustaw skalê obiektu

void CSprite::SetScale(float fValueX, float fValueY)
{
	m_fXScale = fValueX;
	m_fYScale = fValueY;
}

//=== ustaw skalê obiektu - stretching

void CSprite::SetScaleX(float fValueX)
{
	m_fXScale = fValueX;
}

//=== ustaw skalê obiektu - stretching

void CSprite::SetScaleY(float fValueY)
{
	m_fYScale = fValueY;
}

//=== pobierz skalê X

float CSprite::GetScaleX(void)
{
	return m_fXScale;
}
 
//=== pobierz skalê Y
	
float CSprite::GetScaleY(void)
{
	return m_fYScale;
}

//=== dodaj skalê

void CSprite::AddScale(float fValueX, float fValueY)
{
	m_fXScale += fValueX;
	m_fYScale += fValueY;
}

//=== dodaj skalê X

void CSprite::AddScaleX(float fValueX)
{
	m_fXScale += fValueX;
}

//=== dodaj skalê Y

void CSprite::AddScaleY(float fValueY)
{
	m_fYScale += fValueY;
}

//=== ustawia widocznosc ===

void CSprite::SetVisible(bool bState)
{
	m_bVisible = bState;
}

//=== zwraca znacznik widocznosci ===

bool CSprite::GetVisible(void)
{
	return m_bVisible; 
}

//=== rysowanie na zablokowanej texturze

void CSprite::Paint(int iX, int iY, int iA, int iR, int iG, int iB)
{
	long offset = iX + iY;
}

void CSprite::PaintAdd(int iX, int iY, int iA, int iR, int iG, int iB)
{
	long offset = iX + iY;// * (LockedRect.Pitch>>2);
	unsigned char a,r,g,b;
	unsigned long color;

//	color = m_pTextureBuffer[offset];
	a= (unsigned char)color;
	r= (unsigned char)color>>8;
	g= (unsigned char)color>>16;
	b= (unsigned char)color>>24;

	iR+=r; iG+=g; iB+=b; iA+=a;

	if (iA>255) iA=255;
	if (iR>255) iR=255;
	if (iG>255) iG=255;
	if (iB>255) iB=255;

//	m_pTextureBuffer[offset] = D3DCOLOR_XRGB(iR,iG,iB);
}
//=== pobieranie skladowych na zablokowanej texturze

unsigned char CSprite::ucGetR(int iX, int iY)
{
	long offset = iX + iY;
	long color;

	return (unsigned char)color>>8;
}

unsigned char CSprite::ucGetG(int iX, int iY)
{
	long offset = iX + iY;
	long color;

	return (unsigned char)color>>16;
}

unsigned char CSprite::ucGetB(int iX, int iY)
{
	long offset = iX + iY;
	long color;

	return (unsigned char)color>>24;
}


int CSprite::iGetYSize(void)
{
	return m_iYSize;
}

int CSprite::iGetXSize(void)
{
	return m_iXSize;
}

unsigned char CSprite::ucGetAlfa(void)
{
	return m_ucAlfa;
}

unsigned char CSprite::ucGetAlfaR(void)
{
	return m_ucAlfaR;
}

unsigned char CSprite::ucGetAlfaG(void)
{
	return m_ucAlfaG;
}

unsigned char CSprite::ucGetAlfaB(void)
{
	return m_ucAlfaB;
}

// renderowanie na ekranie na podstawie zapamietanych wspolrzednych

void CSprite::Render(CScreen *pScreen)
{

	if (m_iBpp==32)
	{
		for (int h1=0; h1<m_iYSize; h1++)
		{
			for (int h2=0; h2<m_iXSize<<2; h2++)
			{
				((char*)pScreen->m_pScreen->pixels)[(pScreen->iGetSizeX()<<2)*h1+h2] = 
					m_pSprite[(m_iXSize<<2)*h1+h2];
			}
		}

		return;
	}

	if (m_iBpp==24)
	{
		for (int h1=0; h1<m_iYSize; h1++)
		{
			for (int h2=0; h2<m_iXSize*3; h2++)
			{
				((char*)pScreen->m_pScreen->pixels)[(pScreen->iGetSizeX()*3)*h1+h2] = 
					m_pSprite[(m_iXSize*3)*h1+h2];
			}
		}

		return;

	}


}