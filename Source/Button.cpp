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
	if ((mousex>m_pOnButton->GetTranslationX())&&
	   (mousex<m_pOnButton->GetTranslationX()+128)&&
	   (mousey>m_pOnButton->GetTranslationY())&&
	   (mousey<m_pOnButton->GetTranslationY()+128)&&
	   right
	   )
	{
		m_pClicked->Render();
		return 4;
	}
	else 

	if ((mousex>m_pOnButton->GetTranslationX())&&
	   (mousex<m_pOnButton->GetTranslationX()+128)&&
	   (mousey>m_pOnButton->GetTranslationY())&&
	   (mousey<m_pOnButton->GetTranslationY()+128)&&
	   center
	   )
	{
		m_pClicked->Render();
		return 3;
	}
	else 
	if ((mousex>m_pOnButton->GetTranslationX())&&
	   (mousex<m_pOnButton->GetTranslationX()+128)&&
	   (mousey>m_pOnButton->GetTranslationY())&&
	   (mousey<m_pOnButton->GetTranslationY()+128)&&
	   left
	   )
	{
		m_pClicked->Render();
		return 2;
	}
	else 
	if ((mousex>m_pOnButton->GetTranslationX())&&
	   (mousex<m_pOnButton->GetTranslationX()+128)&&
	   (mousey>m_pOnButton->GetTranslationY())&&
	   (mousey<m_pOnButton->GetTranslationY()+128)
	   )
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

//= end =