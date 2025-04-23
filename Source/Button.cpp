// Button.cpp: implementation of the CButton class.
//
//////////////////////////////////////////////////////////////////////

#include "Button.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== tworzy przycisk na podstawie trzech bitmap ===

CButton::CButton(char cFileSystem[], char clicked_name[], 
		char onbutton_name[], char m_pImage_name[],
		IDirect3DDevice8	*pDevice)
{
	CFileSystem	*pTempFilesystem;	// tylko do wczytania danych

	pTempFilesystem = new CFileSystem(cFileSystem);

	pTempFilesystem->Load(clicked_name);
	
	m_pClicked = new CSprite(255,255,255,255);
	m_pClicked->InitializeTGAinMemory((unsigned int*)pTempFilesystem->pDataBuffer,
		pTempFilesystem->Search(clicked_name),pDevice);

	pTempFilesystem->Load(onbutton_name);
	
	m_pOnButton = new CSprite(255,255,255,255);
	m_pOnButton->InitializeTGAinMemory((unsigned int*)pTempFilesystem->pDataBuffer,
		pTempFilesystem->Search(onbutton_name),pDevice);


	pTempFilesystem->Load(m_pImage_name);

	m_pImage = new CSprite(255,255,255,255);
	m_pImage->InitializeTGAinMemory((unsigned int*)pTempFilesystem->pDataBuffer,
		pTempFilesystem->Search(m_pImage_name),pDevice);

	// bez hotx i hoty

	m_lXStart = 0;
	m_lYStart = 0;
	m_lXEnd = 0; 
	m_lYEnd = 0;

}

//=== destruktor zwalnia pamiec po obiektach ===

CButton::~CButton()
{
	if (m_pClicked!=NULL)
	{
		delete (m_pClicked);
		m_pClicked = NULL;
	}

	if (m_pImage!=NULL) 
	{
		delete (m_pImage);
		m_pImage = NULL;
	}

	if (m_pOnButton!=NULL) 
	{
		delete (m_pOnButton);
		m_pOnButton = NULL;
	}
}

//=== rysuje przycisk w zaleznosci od stanu myszy ===

int CButton::Draw(float mousex, float mousey, bool left, bool center, bool right)
{
	if ((mousex>m_lXStart)&&
	   (mousex<m_lXEnd)&&
	   (mousey>m_lYStart)&&
	   (mousey<m_lYEnd)&&
	   right
	   )
	{
		m_pClicked->Render();
		return 4;
	}
	else 

	if ((mousex>m_lXStart)&&
	   (mousex<m_lXEnd)&&
	   (mousey>m_lYStart)&&
	   (mousey<m_lYEnd)&&
	   center
	   )
	{
		m_pClicked->Render();
		return 3;
	}
	else 
	
	if ((mousex>m_lXStart)&&
	   (mousex<m_lXEnd)&&
	   (mousey>m_lYStart)&&
	   (mousey<m_lYEnd)&&
	   left
	   )
	{
		m_pClicked->Render();
		return 2;
	}
	else 

	if ((mousex>m_lXStart)&&
	   (mousex<m_lXEnd)&&
	   (mousey>m_lYStart)&&
	   (mousey<m_lYEnd))
	{
		m_pOnButton->Render();
		return 1;
	}
	else
	{
		m_pImage->Render();
		return 0; 
	}

}

//=== ustawia wspolrzedne przycisku ===

void CButton::SetPosition(long lX, long lY)
{
	m_pClicked->SetTranslation((float)lX,(float)lY);
	m_pImage->SetTranslation((float)lX,(float)lY);
	m_pOnButton->SetTranslation((float)lX,(float)lY);
}

//=== ustawia wspolrzedne w obrebie ktorych przycisk jest aktywny

void CButton::SetHotCoords(long lXStart, long lYStart, long lXEnd, long lYEnd)
{
	m_lXStart = lXStart;
	m_lYStart = lYStart;
	m_lXEnd = lXEnd;
	m_lYEnd = lYEnd;

}

//= end =