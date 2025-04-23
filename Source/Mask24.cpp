// Mask24.cpp: implementation of the CMask24 class.
//
//////////////////////////////////////////////////////////////////////

#include "Mask24.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMask24::CMask24(long lXSize, long lYSize)
{
	m_pBuffer = NULL;
	m_pBuffer = (unsigned char*) malloc (3* (lXSize*lYSize)*sizeof(unsigned short));

	m_pYTable = NULL;
	m_pYTable = new CYTable(lXSize, lYSize);

}

//=== destruktor zwalnia pamiec, jezeli zajeta ===

CMask24::~CMask24()
{

	if (m_pBuffer!=NULL)
	{
		free(m_pBuffer);
		m_pBuffer = NULL;
	}

	if (m_pYTable!=NULL)
	{
		delete m_pYTable;
		m_pYTable = NULL;
	}
}


//=== ustawia wartosc w tablicy

void CMask24::Put(long lXPosition, long lYPosition, unsigned char ucValue)
{
	m_pBuffer[m_pYTable->lGetValue(lYPosition)+lXPosition]=ucValue;
}

// pobiera wartosc z tablicy, uzywa przeliczonych tablic dla offsetow

unsigned short CMask24::Get(long lXPosition, long lYPosition)
{
	return m_pBuffer[m_pYTable->lGetValue(lYPosition)+lXPosition];
}

//=== zwraca offset dla podanych wspolrzednych ===

unsigned long CMask24::GetOffset(long lXPosition, long lYPosition)
{
	return (m_pYTable->lGetValue(lYPosition)+lXPosition);
}

//= end =