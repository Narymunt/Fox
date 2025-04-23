// Scene.h: interface for the CScene class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXSCENE_
#define _FOXSCENE_

// scenka - modu³ sklada sie ze scenek
// glowna klasa obslugujaca postacie
// TODO:
//	- dynamiczna tablica zmiennych
//  - stala tablica wlasciwosci
//	- tablica procedur per tick
//  - tablica procedur per frame
//	- tablica warunkow per tick
//	- tablica warunkow per frame

//----------------------------------------------------------------------
// liczba sprites w konstruktorze tak naprawde oznacza WOLNE SLOTY
// potem trzeba recznie zaladowac i aktywowac sloty 
// czyli mozna przelaczac pomiedzy aktywnoscia obiektow
// slot pusty automatycznie nie jest rysowany
// slot pelen musi byc recznie aktywowany przez play(nazwa_zdarzenia)
//----------------------------------------------------------------------

#include <windows.h>
#include <d3d8.h>
#include <d3dx8.h>
#include "Sprite.h"
#include "FileSystem.h"

class __declspec(dllexport) CScene  
{
public:
	CScene(unsigned long ulSpritesCount);
	virtual ~CScene();

	

	//=== aktywne ? 

	virtual void SetState(bool bState);
	virtual bool GetState(void);

	//=== gramy czy ogladamy ? 

	virtual void SetPlayable(bool bState);
	virtual bool GetPlayable(void);

	//=== obsluga slotow

	virtual void AddSpriteToScene(unsigned long ulSlotCount, 
								  IDirect3DDevice8* pDevice, 
								  char cFilesystemName[],
								  char cSpriteName[]);

	virtual void RemoveSpriteFromScene(unsigned long ulSlotCount); 
	
	virtual void ShowSprite(unsigned long ulSlotCount); // rysowany i pracuje
	virtual void HideSprite(unsigned long ulSlotCount); // nie rysowany ALE pracuje


	//=== aktualizacja 

	virtual long Upgrade(long lTimer, 
						 float fMouseX, float fMouseY,
						 bool bLeftButton, bool bRightButton, bool bCenterButton
						);

	//=== do formatu z edytora === 

	virtual void SetFileVersion(char cVersion);	// ustawia
	virtual char GetFileVersion(void); // zwraca

	virtual void			SetPropertiesCount(unsigned int iCount);
	virtual unsigned int	GetPropertiesCount(void);

	virtual void			SetVariablesCount(unsigned int iCount);
	virtual unsigned int	GetVariablesCount(void);

	virtual void			SetCycleProcedureCount(unsigned int iCount);
	virtual unsigned int	GetCycleProcedureCount(void);

	virtual void			SetTikProcedureCount(unsigned int iCount);
	virtual unsigned int	GetTikProcedureCount(void);

	virtual void			SetIfCount(unsigned int uiCount);
	virtual unsigned int	GetIfCount(void);



private:

	char	m_cFileVersion;		// wersja pliku importowanego z edytora
	char	m_cSceneName[256];	// nazwa sceny importowane z edytora

	// pierwsze to liczba wlasciwosci
	// drugie to liczba wlasciwosci + zmienne
	// czyli zeby uzyskac zmienne to trzeba odjac jedno od drugiego

	unsigned int m_uiPropertiesCount;	// lista zmiennych wlasciwosci importowanych
	unsigned int m_uiVariablesCount;

	unsigned int m_uiCycleProcedureCount;	// lista procedur eksportowanych (cykl)
	unsigned int m_uiTikProcedureCount;	// lista procedur eksportowanych (zegar)

	unsigned int m_uiIfCount;			// lista / liczba warunkow eksportowanych
	unsigned int m_uiSpritesCount;	// lista sprites eksportowanych (ile ich jest)

	

	bool m_bActive;		// czy sprawdzany w petli 

	bool m_bPlayable;	// gramy czy ogladamy ? 

	unsigned long	m_ulSpritesCount;	// ile sprite w scene ? 

	CSprite			**m_pSprites;		// tutaj obiekty

	// CTree	**pTreeList;


};

#endif 
