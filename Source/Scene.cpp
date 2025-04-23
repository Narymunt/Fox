// Scene.cpp: implementation of the CScene class.
//
//////////////////////////////////////////////////////////////////////

#include <Scene.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== konstruktor, docelowo nie aktywny ===

CScene::CScene(unsigned long ulSpritesCount)
{
	m_ulSpritesCount = ulSpritesCount;	// zapamietaj
	
	m_pSprites = new CSprite * [ulSpritesCount];	// nowe sprites

	m_bActive = false;	// nie aktywny
}

//=== destruktor === 

CScene::~CScene()
{
	// zwolnij sprites

	for (unsigned long ulLicznik=0; ulLicznik<m_ulSpritesCount; ulLicznik++)
	{
		
		delete m_pSprites[ulLicznik];
	}
	
	delete m_pSprites;

	m_bActive = false;	// nie aktywny

}

//=== do ustawiania stanu aktywnosci

void CScene::SetState(bool bState)
{
	m_bActive = bState;
}

//=== do sprawdzania stanu aktywnosci

bool CScene::GetState(void)
{
	return m_bActive;
}

//=== gramy czy ogladamy === 

void CScene::SetPlayable(bool bState)
{
	m_bPlayable = bState;
}

//=== zwraca znacznik === 

bool CScene::GetPlayable(void)
{
	return m_bPlayable; 
}

//=== aktualizuje scenke - rysuje sprite

long CScene::Upgrade(long lTimer, 
						 float fMouseX, float fMouseY,
						 bool bLeftButton, bool bRightButton, bool bCenterButton
						)
{
	
	// tutaj petla procedur i warunkow

	if (m_bActive == false) return 0;	// nie rysujemy

	// jezeli sprite widoczny, to rysujemy go

	for (unsigned long ulLicznik=0; ulLicznik<m_ulSpritesCount; ulLicznik++)
	{
		if (m_pSprites[ulLicznik]->GetVisible())
		{
			// tutaj procedury i warunki dla sprite 
			// ...
			// m_pSprites[ulLicznik]->Upgrade();
			
			m_pSprites[ulLicznik]->Render();	// rysuj
		}
	}	

	return -1; // cos narysowal... potem powinien zwracac liste narysowanych obiektow

}

//=== dodaje obiekt do sceny, od razu jest on widoczny na ekranie (aktywowany)

void CScene::AddSpriteToScene(unsigned long ulSlotCount, 
								  IDirect3DDevice8* pDevice, 
								  char cFilesystemName[],
								  char cSpriteName[])
{
	CFileSystem	*pTempFileSystem;

	pTempFileSystem = new CFileSystem(cFilesystemName);	// otworz archiwum
	pTempFileSystem->Load(cSpriteName);	// wczytaj obiekt 


	m_pSprites[ulSlotCount] = new CSprite(255,255,255,255);	// nowy obiekt

	// aktywacja

	m_pSprites[ulSlotCount]->InitializeTGAinMemory((unsigned int*)pTempFileSystem->pDataBuffer,
		pTempFileSystem->Search(cSpriteName),pDevice);

	// domyslnie nie widoczny 
	
	m_pSprites[ulSlotCount]->SetVisible(false);

	delete pTempFileSystem;

}

//=== usuwa obiekt ze sceny, fizycznie, tj nie chowa go ;) 

void CScene::RemoveSpriteFromScene(unsigned long ulSlotCount)
{
	if (ulSlotCount>m_ulSpritesCount) return; // podano za duzy numer

	m_pSprites[ulSlotCount]->SetVisible(false);	// ukryj
	
	delete m_pSprites[ulSlotCount];	// usuwa i deaktywuje
	
}

//=== sprite jest rysowany i petla jest uruchamiana

void CScene::ShowSprite(unsigned long ulSlotCount)
{
	m_pSprites[ulSlotCount]->SetVisible(true);
}

//=== sprite nie jest rysowany, ale petla jest uruchamiana

void CScene::HideSprite(unsigned long ulSlotCount)
{
	m_pSprites[ulSlotCount]->SetVisible(false);
}

//=== ustawia wersje pliku ===

void CScene::SetFileVersion(char cVersion)
{
	m_cFileVersion = cVersion;
}

//=== zwraca wersje pliku jezeli zostala ustawiona ===
//=== NULL oznacza, ze nie bylo jeszcze ustawione ===

char CScene::GetFileVersion(void)
{
	return m_cFileVersion;
}

//=== ustaw liczbe wlasciwosc - opis w pliku *.h ===

void CScene::SetPropertiesCount(unsigned int iCount)
{
	m_uiPropertiesCount = iCount; 
}

//=== zwroc liczbe wlasciwosci - opis w pliku *.h ===

unsigned int CScene::GetPropertiesCount(void)
{
	return m_uiPropertiesCount;
}

//=== ustaw liczbe zmiennych ===

void CScene::SetVariablesCount(unsigned int iCount)
{
	m_uiVariablesCount = iCount;
}

//=== pobierz liczbe zmiennych ===

unsigned int CScene::GetVariablesCount(void)
{
	return m_uiVariablesCount;
}

//=== ustaw liczbe procedur na cykl ===

void CScene::SetCycleProcedureCount(unsigned int iCount)
{
	m_uiCycleProcedureCount = iCount;
}

//=== pobierz liczbe procedur na cykl ===

unsigned int CScene::GetCycleProcedureCount(void)
{
	return m_uiCycleProcedureCount;
}

//=== ustaw liczbe procedur na tik ===

void CScene::SetTikProcedureCount(unsigned int iCount)
{
	m_uiTikProcedureCount = iCount;
}

//=== pobierz liczbe procedur na tik ====

unsigned int CScene::GetTikProcedureCount(void)
{
	return m_uiTikProcedureCount;
}

//=== ustaw liczbe warunkow === 

void CScene::SetIfCount(unsigned int uiCount)
{
	m_uiIfCount = uiCount;
}

//=== zwroc liczbe warunkow ===

unsigned int CScene::GetIfCount(void)
{
	return m_uiIfCount;
}

// end