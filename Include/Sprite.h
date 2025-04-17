// Sprite.h: interface for the CSprite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPRITE_H__3545E25B_AF0C_49E1_AA24_F9ADB694C73F__INCLUDED_)
#define AFX_SPRITE_H__3545E25B_AF0C_49E1_AA24_F9ADB694C73F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

	virtual void		SetRotation(float newRotation);	// ustaw rotacje
	virtual void		SetRotationCenter(float x, float y); // ustaw punkt rotacji
	virtual float		GetRotation(void);				// pobierz rotacje
	virtual void		AddRotation(float addValue);	// dodaj lub odejmij 

	virtual void		SetTranslation(float x, float y);	// ustaw przesuniecie
	virtual float		GetTranslationX(void);
	virtual float		GetTranslationY(void);
	virtual void		AddTranslation(float x, float y);

	virtual float		GetDegree(float fVector);		// podaj w katach
	virtual float		GetRadian(float fVector);		// podaj w radianach

private:

	D3DXVECTOR2			m_RotCenter;		// w pixelach
	D3DXVECTOR2			m_Translation;		// przesuniecie we float 
	D3DXVECTOR2			m_Scaling;			// scala we float 



	float				m_Rotation;			// we float
	
	D3DCOLOR			m_ModulateColor;	// color 
	
	LPD3DXSPRITE		m_pSprite;
	
	LPDIRECT3DTEXTURE8	m_pTexture;
	
	BOOL				m_bInitialized;

};

#endif // !defined(AFX_SPRITE_H__3545E25B_AF0C_49E1_AA24_F9ADB694C73F__INCLUDED_)
