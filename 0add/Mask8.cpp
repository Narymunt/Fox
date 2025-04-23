// Mask8.cpp: implementation of the CMask8 class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "Mask8.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// konstruktor od razu alokuje pamiec i od razy tworzy tablice offsetow

CMask8::CMask8(long lXSize, long lYSize)
{
	m_pBuffer = NULL;
	m_pBuffer = (unsigned char*) malloc (lXSize*lYSize*sizeof(unsigned char));

	m_pYTable = NULL;
	m_pYTable = new CYTable(lXSize, lYSize);

}

// destruktor zwalnia bufor

CMask8::~CMask8()
{
	if (m_pBuffer!=NULL)
	{
		free(m_pBuffer);
		m_pBuffer=NULL;
	}

	if (m_pYTable!=NULL)
	{
		delete m_pYTable;
		m_pYTable = NULL;
	}

}

// ustawia wartosc w tablicy

void CMask8::Put(long lXPosition, long lYPosition, unsigned char ucValue)
{
	m_pBuffer[m_pYTable->lGetValue(lYPosition)+lXPosition]=ucValue;
}

// pobiera wartosc z tablicy, uzywa przeliczonych tablic dla offsetow

unsigned char CMask8::Get(long lXPosition, long lYPosition)
{
	return m_pBuffer[m_pYTable->lGetValue(lYPosition)+lXPosition];
}

// zwraca offset dla podanych wspolrzednych

unsigned long CMask8::GetOffset(long lXPosition, long lYPosition)
{
	return (m_pYTable->lGetValue(lYPosition)+lXPosition);
}


//=== end