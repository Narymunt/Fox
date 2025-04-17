
#include "Track.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== stworz obiekt

CTrack::CTrack(char cFilename[])
{
	FILE	*plik;	
	char	linia[1024];
	int		iCount;
	int		iStartOffset, iEndOffset, iStart, iEnd;

	if ((plik=fopen(cFilename,"r"))==NULL)
	{
		MessageBox(NULL,"ERROR! Brak pliku demo.ini!","ERROR", MB_OK);
		PostQuitMessage( 0 );
		return ;
	}

	iCount =0;

	fscanf(plik,"%s\n",&linia);	// wczytaj linie pliku
	UpOnly(linia);

	if (strcmp(linia,"<TRACK>"))	// czy to plik track ? 
	{
		MessageBox(NULL,"ERROR! Uszkodzony plik track.ini!","ERROR", MB_OK);
		PostQuitMessage( 0 );
		return ;
	}

	// wczytujemy elementy

	while (strcmp(linia,"</TRACK>"))
	{
		fscanf(plik,"%s\n",&linia);	// wczytaj linie pliku
		UpOnly(linia);
		iCount++;		
	}

	iCount--;
	iCount>>=2;

	printf("TRACK %s : size %d\n",cFilename,iCount);

	fclose(plik);

	// teraz parsujemy

	if ((plik=fopen(cFilename,"r"))==NULL)
	{
		MessageBox(NULL,"ERROR! Brak pliku demo.ini!","ERROR", MB_OK);
		PostQuitMessage( 0 );
		return ;
	}

	fscanf(plik,"%s\n",&linia);	// wczytaj linie pliku
	UpOnly(linia);

	// wczytujemy elementy

	for (int i=0; i<iCount; i++)
	{
		fscanf(plik,"%d %d %d %d\n",&iStartOffset, &iStart, &iEnd, &iEndOffset);	// wczytaj linie pliku
		
		m_iOffsetStart.push_back(iStartOffset);
		m_iStart.push_back(iStart);
		m_iEnd.push_back(iEnd);
		m_iOffsetEnd.push_back(iEndOffset);
		
		printf("%d %d %d %d\n",iStartOffset, iStart, iEnd, iEndOffset);	// wczytaj linie pliku
	}

	fclose(plik);

}

//=== usun obiekt === 

CTrack::~CTrack()
{
	m_iOffsetStart.clear();
	m_iStart.clear();
	m_iEnd.clear();
	m_iOffsetEnd.clear();
}

// zamiana literek na duze

void CTrack::UpOnly(char str[])
{
	int i=0;
	char c;
	
	while (str[i])
	{
		c = str[i];
		str[i]= toupper(c);
		i++;
	}
}

int CTrack::iGetSize(void)
{
	return m_iOffsetStart.size();
}

// zwraca maksymalna wartosc startu licznika

int CTrack::iGetStartMax(void)
{
	int iMax;

	iMax = 0;

	for (unsigned int i=0; i<m_iOffsetStart.size(); i++)
	{
		if (m_iStart[i]>iMax) iMax = m_iStart[i];
	}

	return iMax;
}

// zwracanie wartosci

int CTrack::iGetOffsetStart(int i)
{
	return m_iOffsetStart[i];
}

int CTrack::iGetStart(int i)
{
	return m_iStart[i];
}

int CTrack::iGetEnd(int i)
{
	return m_iEnd[i];
}

int CTrack::iGetOffsetEnd(int i)
{
	return m_iOffsetEnd[i];
}

// sprawdzamy czy w danym przedziale czasu powinnismy odtwarzac

bool CTrack::isPlaying(int i)
{

	for (unsigned int h1=0; h1<m_iStart.size(); h1++)
	{
		if ( (m_iStart[h1]<=i) && (m_iEnd[h1]>=i) ) return true;
	}

	return false;
}

// zwraca numer klatki jaki powinnismy odtworzyc w tym przedziale czasu

int CTrack::iGetFrame(int i)
{
	int iFrame;

	for (unsigned int h1=0; h1<m_iStart.size(); h1++)
	{
		if ( (m_iStart[h1]<=i) && (m_iEnd[h1]>=i) ) 
		{
			iFrame = (i - m_iStart[h1]) - m_iOffsetStart[h1] + m_iOffsetEnd[h1];
		}
	}

	return iFrame;
}

