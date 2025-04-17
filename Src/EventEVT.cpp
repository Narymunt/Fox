// EventEVT.cpp: implementation of the CEventEVT class.
//
//////////////////////////////////////////////////////////////////////

#include <EventEVT.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// inicjalizuj
// wczytaj 

CEventEVT::CEventEVT(char m_SourceFOX[], char m_cEventName[], IDirect3DDevice8 *pDevice)
{
	int		m_iOffset;		// offset w pliku
	CFileSystem	*pFOXFile;	// plik spakowany
	char	*p_cFilename;	// nazwa pliku *.001

	char	znakA, znakB, znakC;	// do generowania nazw

	pFOXFile = new CFileSystem(m_SourceFOX);	// otworz plik *.fox
	pFOXFile->Load(m_cEventName);				// wczytaj *.evt

	m_iOffset=0;	// wyzeruj offset

	m_ucVersion = pFOXFile->pDataBuffer[0];	// odczytaj offset
	
	m_iOffset++;	// zwieksz offset

	m_uiIloscKlatek = (unsigned long)((pFOXFile->pDataBuffer[m_iOffset+3]<<24)+
					(pFOXFile->pDataBuffer[m_iOffset+2]<<16)+
					(pFOXFile->pDataBuffer[m_iOffset+1]<<8)+
					(pFOXFile->pDataBuffer[m_iOffset+0]));	// odczytaj ilosc klatek
	
	m_iOffset+=4;	// zwieksz offset

	m_uiIloscZdarzen = (unsigned long)((pFOXFile->pDataBuffer[m_iOffset+3]<<24)+
					(pFOXFile->pDataBuffer[m_iOffset+2]<<16)+
					(pFOXFile->pDataBuffer[m_iOffset+1]<<8)+
					(pFOXFile->pDataBuffer[m_iOffset+0]));	// odczytaj ilosc zdarzen

	m_iOffset+=4;	// zwieksz offset

	// alokuj ilosc klatek

	pEventFrames = new CSprite * [m_uiIloscKlatek];
	
	// alokuj eventy

	pEvent = new CEvent * [m_uiIloscZdarzen];

	// odczytaj eventy

	for (unsigned int h1=0; h1<m_uiIloscZdarzen; h1++)
	{
		pEvent[h1]=new CEvent();
		
		memcpy(pEvent[h1]->pGetNazwaZdarzenia(),&pFOXFile->pDataBuffer[m_iOffset],256);	// pobierz nazwe
		
		m_iOffset+=256;	// zwieksz offset

		pEvent[h1]->SetLiczbaKlatek( (unsigned int)
			((pFOXFile->pDataBuffer[m_iOffset+3]<<24)+
			 (pFOXFile->pDataBuffer[m_iOffset+2]<<16)+
			 (pFOXFile->pDataBuffer[m_iOffset+1]<<8)+
			 (pFOXFile->pDataBuffer[m_iOffset+0])));	// ustaw liczbe klatek

		m_iOffset+=4;	// zwieksz offset

		// zaalokuj pamiec na klatki

		pEvent[h1]->AllocPositions();
		
		// zaalokuj pamiec na numery klatek

		pEvent[h1]->AllocKlatki();

		// zaalokuj pamiec na znaczki czy odtwarzac wave

		pEvent[h1]->AllocWaveInfo();

		// odczytaj zdarzenia

		for (unsigned int h2=0; h2<pEvent[h1]->GetLiczbaKlatek(); h2++)
		{

		pEvent[h1]->SetPositionX((unsigned long)h2, (int)
			(pFOXFile->pDataBuffer[m_iOffset+3]<<24) +
			(pFOXFile->pDataBuffer[m_iOffset+2]<<16) +
			(pFOXFile->pDataBuffer[m_iOffset+1]<<8) +
			(pFOXFile->pDataBuffer[m_iOffset+0]));	// ustaw x

		pEvent[h1]->SetPositionY((unsigned long)h2, (int)
			(pFOXFile->pDataBuffer[m_iOffset+7]<<24) +
			(pFOXFile->pDataBuffer[m_iOffset+6]<<16) +
			(pFOXFile->pDataBuffer[m_iOffset+5]<<8) +
			(pFOXFile->pDataBuffer[m_iOffset+4]));	// ustaw y

		pEvent[h1]->SetNrKlatki((unsigned long)h2,
			(pFOXFile->pDataBuffer[m_iOffset+11]<<24) +
			(pFOXFile->pDataBuffer[m_iOffset+10]<<16) +
			(pFOXFile->pDataBuffer[m_iOffset+9]<<8) +
			(pFOXFile->pDataBuffer[m_iOffset+8]));	// ustaw nr klatki dla tych wspolrzednych

		m_iOffset+=12;	// zwieksz offset

		pEvent[h1]->SetPlayWaveInfo(h2,pFOXFile->pDataBuffer[m_iOffset]);	// ustaw znacznik

		m_iOffset++;	// zwieksz offset

		// jezeli sa dzwieki to zaladuj nazwy plikow

		if (pEvent[h1]->GetPlayWaveInfo(h2)!=0)
		{
			pEvent[h1]->AllocSFXBankNames();	

			memcpy(pEvent[h1]->pGetFileNazwaBanku(),&pFOXFile->pDataBuffer[m_iOffset],256);
			memcpy(pEvent[h1]->pGetFileNazwaZdarzenia(),&pFOXFile->pDataBuffer[m_iOffset+256],256);
		}
		
		m_iOffset+=512;	// zwieksz offset 
		
		}

	}

	// na koncu czytanie klatek, bo tracimy bufor

	p_cFilename = (char*) malloc(strlen(m_cEventName));	

	strcpy(p_cFilename,m_cEventName);

	znakA=znakB=znakC='0';		// wyzeruj

	for (h1=0; h1<m_uiIloscKlatek; h1++)
	{
		pEventFrames[h1] = new CSprite(255,255,255,255);
	
		p_cFilename[strlen(p_cFilename)-1]=znakA;
		p_cFilename[strlen(p_cFilename)-2]=znakB;
		p_cFilename[strlen(p_cFilename)-3]=znakC;

		pFOXFile->Load(p_cFilename);	// wczytaj animacje

		pEventFrames[h1]->InitializeTGAinMemory((unsigned int*)pFOXFile->pDataBuffer,
			pFOXFile->Search(p_cFilename),pDevice);

		znakA++;

		if (znakA==':')
		{
			znakA='0';
			znakB++;
		}

		if (znakB==':')
		{
			znakB='0';
			znakC++;
		}

	}

	delete pFOXFile;
}	

//=== destruktor, usun obiekty 

CEventEVT::~CEventEVT()
{

	for (unsigned int h1=0; h1<m_uiIloscZdarzen; h1++)
	{
		delete pEvent[h1];
	}

	delete pEvent;

	for (h1=0; h1<m_uiIloscKlatek; h1++)
	{
		delete pEventFrames[h1];
	}

	delete pEventFrames;

}

//=== odtwarzaj wg trasy

void CEventEVT::Put(unsigned int m_uiNrKlatki, unsigned int m_uiNrEventu)
{
	// jezeli klatka nie miesci sie w przedziale eventu, to wyswietl ostatnia
	if (m_uiNrKlatki>=pEvent[m_uiNrEventu]->GetLiczbaKlatek())
	{
		m_uiNrKlatki=pEvent[m_uiNrEventu]->GetLiczbaKlatek()-1;
	}

	pEventFrames[pEvent[m_uiNrEventu]->GetNrKlatki(m_uiNrKlatki)]->
		SetTranslation((float)pEvent[m_uiNrEventu]->GetPositionX((unsigned long)m_uiNrKlatki),
					   (float)pEvent[m_uiNrEventu]->GetPositionY((unsigned long)m_uiNrKlatki));

	pEventFrames[pEvent[m_uiNrEventu]->GetNrKlatki(m_uiNrKlatki)]->Render();
}


//=== odtwarzaj na wspolrzednych

void CEventEVT::Put(int m_iXScreen, int m_iYScreen, 
		unsigned int m_uiNrKlatki, unsigned int m_uiNrEventu)
{
	
	// jezeli klatka nie miesci sie w przedziale eventu, to wyswietl ostatnia

	if (m_uiNrKlatki>=pEvent[m_uiNrEventu]->GetLiczbaKlatek())
	{
		m_uiNrKlatki=pEvent[m_uiNrEventu]->GetLiczbaKlatek()-1;
	}

	pEventFrames[pEvent[m_uiNrEventu]->GetNrKlatki(m_uiNrKlatki)]->
		SetTranslation((float)m_iXScreen, (float)m_iYScreen);

	pEventFrames[pEvent[m_uiNrEventu]->GetNrKlatki(m_uiNrKlatki)]->Render();

}

//=== odtwarzaj wg trasy z dzwiekiem

void CEventEVT::Play(unsigned int m_uiNrKlatki, unsigned int m_uiNrEventu)
{
	int h1;
	
	// jezeli klatka nie miesci sie w przedziale eventu, to wyswietl ostatnia
	if (m_uiNrKlatki>=pEvent[m_uiNrEventu]->GetLiczbaKlatek())
	{
		m_uiNrKlatki=pEvent[m_uiNrEventu]->GetLiczbaKlatek()-1;
	}

	// odtwarzamy dzwiek dla tej klatki ? 

	if (pEvent[m_uiNrEventu]->GetPlayWaveInfo(m_uiNrKlatki)!=0)
	{

		// sprawdz czy juz nie jest odtwarzany

		if (pEvent[m_uiNrEventu]->GetIsPlayingSound())
		{
			h1=1; // jest odtwarzany
		}
		else
		{
			h1=0; // nie jest odtwarzany, no to play
		}
	}

	pEventFrames[pEvent[m_uiNrEventu]->GetNrKlatki(m_uiNrKlatki)]->
		SetTranslation((float)pEvent[m_uiNrEventu]->GetPositionX((unsigned long)m_uiNrKlatki),
					   (float)pEvent[m_uiNrEventu]->GetPositionY((unsigned long)m_uiNrKlatki));

	pEventFrames[pEvent[m_uiNrEventu]->GetNrKlatki(m_uiNrKlatki)]->Render();
}
