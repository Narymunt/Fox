// Mask24.cpp: implementation of the CMask24 class.
//
//////////////////////////////////////////////////////////////////////

#include "Mask24.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMask24::CMask24(long lXSize, long lYSize)
{

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
