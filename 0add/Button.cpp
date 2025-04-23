// Button.cpp: implementation of the CButton class.
//
//////////////////////////////////////////////////////////////////////

#include <Button.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== tworzy przycisk na podstawie trzech bitmap ===

CButton::CButton(char cFileSystem[], char clicked_name[], 
		char onbutton_name[], char pimage_name[],
		IDirect3DDevice8	*pDevice)
{
	CFileSystem	*pTempFilesystem;	// tylko do wczytania danych

	pTempFilesystem = new CFileSystem(cFileSystem);

	pTempFilesystem->Load(clicked_name);
	
	pClicked = new CSprite(255,255,255,255);
	pClicked->InitializeTGAinMemory((unsigned int*)pTempFilesystem->pDataBuffer,
		pTempFilesystem->Search(clicked_name),pDevice);

	pTempFilesystem->Load(onbutton_name);
	
	pOnButton = new CSprite(255,255,255,255);
	pOnButton->InitializeTGAinMemory((unsigned int*)pTempFilesystem->pDataBuffer,
		pTempFilesystem->Search(onbutton_name),pDevice);


	pTempFilesystem->Load(pimage_name);

	pImage = new CSprite(255,255,255,255);
	pImage->InitializeTGAinMemory((unsigned int*)pTempFilesystem->pDataBuffer,
		pTempFilesystem->Search(pimage_name),pDevice);

}

//=== destruktor zwalnia pamiec po obiektach ===

CButton::~CButton()
{
	if (pClicked!=NULL)
	{
		delete (pClicked);
		pClicked = NULL;
	}

	if (pImage!=NULL) 
	{
		delete (pImage);
		pImage = NULL;
	}

	if (pOnButton!=NULL) 
	{
		delete (pOnButton);
		pOnButton = NULL;
	}
}

//=== rysuje przycisk w zaleznosci od stanu myszy ===

int CButton::Draw(float mousex, float mousey, bool left, bool center, bool right)
{

	if ((mousex>pOnButton->GetTranslationX())&&
	   (mousex<pOnButton->GetTranslationX()+128)&&
	   (mousey>pOnButton->GetTranslationY())&&
	   (mousey<pOnButton->GetTranslationY()+128)&&
	   left
	   )
	{
		pClicked->Render();
		return 2;
	}
	else if ((mousex>pOnButton->GetTranslationX())&&
	   (mousex<pOnButton->GetTranslationX()+128)&&
	   (mousey>pOnButton->GetTranslationY())&&
	   (mousey<pOnButton->GetTranslationY()+128)
	   )
	{
		pOnButton->Render();
		return 1;
	}
	else
	{
		pImage->Render();
		return 0; 
	}

}

//=== ustawia wspolrzedne przycisku ===

void CButton::SetPosition(long lX, long lY)
{
	pClicked->SetTranslation((float)lX,(float)lY);
	pImage->SetTranslation((float)lX,(float)lY);
	pOnButton->SetTranslation((float)lX,(float)lY);
}

//=== end ===