// Sprite.h: interface for the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXSPRITE_
#define _FOXSPRITE_

#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>

// dll interface

#pragma warning(disable: 4251)

class __declspec(dllexport) CSprite  
{

public:
	
	CSprite(unsigned char alpha, unsigned char r, unsigned char g, unsigned char b);
	virtual ~CSprite();

	virtual HRESULT		Initialize(char pCurrentDirectory[200],IDirect3DDevice8 *g_pDevice, char *path);
	virtual HRESULT		InitializeTGAinMemory(unsigned int *pSource, unsigned long m_ulSize, IDirect3DDevice8 *g_pDevice);
	
	virtual HRESULT		Render();

	//-=- obrót -=-

	virtual void		SetRotation(float newRotation);	// ustaw rotacje
	virtual void		SetRotationCenter(float x, float y); // ustaw punkt rotacji
	virtual float		GetRotation(void);				// pobierz rotacje
	virtual void		AddRotation(float addValue);	// dodaj lub odejmij 

	//-=- przesuniêcie -=-

	virtual void		SetTranslation(float x, float y);	// ustaw przesuniecie
	virtual float		GetTranslationX(void);
	virtual float		GetTranslationY(void);
	virtual void		AddTranslation(float x, float y);

	//-=- konwersje k¹tów -=-

	virtual float		GetDegree(float fVector);		// podaj w katach
	virtual float		GetRadian(float fVector);		// podaj w radianach

	//-=- skalowanie -=-

	virtual void		SetScale(float fValueX, float fValueY);	// ustaw skalê obiektu
	
	virtual void		SetScaleX(float fValueX);	// ustaw skalê
	virtual void		SetScaleY(float fValueY);

	virtual float		GetScaleX(void);	// pobierz wspó³rzêdne
	virtual float		GetScaleY(void);		

	virtual void		AddScale(float fValueX, float fValueY);	// dodaj skalê

	virtual void		AddScaleX(float fValueX);
	virtual void		AddScaleY(float fValueY);

	//=== widzialnosci

	virtual void		SetVisible(bool bState);
	virtual bool		GetVisible(void);

	//=== stan inicjalizacji (czy byl zainicjalizowany, czy tylko klasa)

	virtual void		SetState(bool bState);
	virtual bool		GetState(void);

private:
	
	bool				m_bVisible;			// czy widoczny, czy ma byc renderowany
	
	D3DXVECTOR2			m_RotCenter;		// w pixelach
	D3DXVECTOR2			m_Translation;		// przesuniecie we float 
	D3DXVECTOR2			m_Scaling;			// scala we float 

	float				m_Rotation;			// we float
	
	D3DCOLOR			m_ModulateColor;	// color 
	
	LPD3DXSPRITE		m_pSprite;
	
	LPDIRECT3DTEXTURE8	m_pTexture;
	
	bool				m_bInitialized;

};

#endif // !defined(AFX_SPRITE_H__3545E25B_AF0C_49E1_AA24_F9ADB694C73F__INCLUDED_)
 