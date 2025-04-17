#ifndef _fox_sprite_
#define _fox_sprite_

#include "screen.h"

// orgx,orgy - dla jakiej rozdzielczosci sprite
// destx, desty - jaka rozdzielczosc docelowa

class CSprite
{
	
public : 
	
	CSprite(char cFilename[], int iBpp);
	CSprite(char cFilename[], int iDestX, int iDestY, int iBpp);

//	CSprite(char cZipFile[], char cPassword[], char cFilename[], int iBpp, int iOrgX, int iOrgY, int iDestX, int iDestY ); 
	CSprite(char cZipFile[], char cPassword[], char cFilename[], int iBpp); 


	~CSprite();
	
	// === settery
	
	void	SetAlfa(unsigned char ucA);
	
	void 	SetAlfaR(unsigned char ucA);
	void	SetAlfaG(unsigned char ucA);
	void 	SetAlfaB(unsigned char ucA);

	void	SetScale(float fValueX, float fValueY);	// ustaw skalê obiektu
	
	void	SetScaleX(float fValueX);	// ustaw skalê
	void	SetScaleY(float fValueY);
	
	//-=- obrót -=-

	void	SetRotation(float newRotation);	// ustaw rotacje
	void	SetRotationCenter(int x, int y); // ustaw punkt rotacji
	void	AddRotation(float addValue);	// dodaj lub odejmij 

	int		iGetRotationX(void);
	int		iGetRotationY(void);

	float	fGetRotation(void);

	void	AddScale(float fValueX, float fValueY);	// dodaj skalê

	void	AddScaleX(float fValueX);
	void	AddScaleY(float fValueY);	
	
	//-=- przesuniêcie -=-

	void	SetPosition(int x, int y);	// ustaw przesuniecie

	int		iGetPositionX(void);
	int		iGetPositionY(void);
	
	void	AddPosition(int x, int y);
	
	// === gettery	
	
	unsigned char	ucGetAlfa(void);
	
	unsigned char 	ucGetAlfaR(void);
	unsigned char	ucGetAlfaG(void);
	unsigned char	ucGetAlfaB(void);

	float	GetRotation(void);				// pobierz rotacje

	//-=- konwersje k¹tów -=-

	float	GetDegree(float fVector);		// podaj w katach
	float	GetRadian(float fVector);		// podaj w radianach

	//-=- skalowanie -=-

	float	GetScaleX(void);	// pobierz wspó³rzêdne
	float	GetScaleY(void);		

	//=== widzialnosci

	void	SetVisible(bool bState);
	bool	GetVisible(void);

	//=== parametry textury

	int		iGetXSize(void);
	int		iGetYSize(void);

	//=== rysowanie na texturze

	void	Paint(int iX, int iY, int iA, int iR, int iG, int iB);
	void	PaintAdd(int iX, int iY, int iA, int iR, int iG, int iB);
	void	Erase(int iX, int iY, int iA, int iR, int iG, int iB);

	unsigned char ucGetR(int iX, int iY);
	unsigned char ucGetG(int iX, int iY);
	unsigned char ucGetB(int iX, int iY);
	
	//=== renderowanie

	void Render(CScreen *pScreen);

protected :

	char	*m_pSprite;			// dane obrazu
	
	bool	m_bVisible;
	
	int		m_iBpp;		// w jakim formacie trzymamy dane ?

	int		m_iXPosition, m_iYPosition;		// wspolprzedne na ekranie	
	int		m_iXRotation, m_iYRotation;		// wspolrzedne srodka obrotu

	unsigned char	m_ucAlfa;
	
	unsigned char	m_ucAlfaR, m_ucAlfaG, m_ucAlfaB;	
	
	float			m_Rotation;			// we float	
	
	int		m_iXSize;			// jaki jest oryginalny rozmiar
	int		m_iYSize;

	float	m_fXScale;	// skala X			
	float	m_fYScale;	// skala Y
	
};


#endif
