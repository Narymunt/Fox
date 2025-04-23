// Mask16.cpp: implementation of the CMask16 class.
//
//////////////////////////////////////////////////////////////////////

#include "Mask16.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== konstruktor, od razu alokuje pamiec na obiekty ===

CMask16::CMask16(long lXSize, long lYSize)
{
	m_pBuffer = NULL;
	m_pBuffer = (unsigned short*) malloc (lXSize*lYSize*sizeof(unsigned short));

	m_pYTable = NULL;
	m_pYTable = new CYTable(lXSize, lYSize);

}

//=== destruktor - zwalnia pamiec jezeli nadal jest zaalokowana ===

CMask16::~CMask16()
{
	if (m_pBuffer!=NULL)
	{
		free(m_pBuffer);
		m_pBuffer=NULL;
	}

	if (m_pYTable!=NULL)
	{
		delete m_pYTable;
	}
}



// ustawia wartosc w tablicy

void CMask16::Put(long lXPosition, long lYPosition, unsigned short ucValue)
{
	m_pBuffer[m_pYTable->lGetValue(lYPosition)+lXPosition]=ucValue;
}

// pobiera wartosc z tablicy, uzywa przeliczonych tablic dla offsetow

unsigned short CMask16::Get(long lXPosition, long lYPosition)
{
	return m_pBuffer[m_pYTable->lGetValue(lYPosition)+lXPosition];
}

// zwraca offset dla podanych wspolrzednych

unsigned long CMask16::GetOffset(long lXPosition, long lYPosition)
{
	return (m_pYTable->lGetValue(lYPosition)+lXPosition);
}

// end