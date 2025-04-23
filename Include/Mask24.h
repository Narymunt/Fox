// Mask24.h: interface for the CMask24 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXMASK24_
#define _FOXMASK24_

#include <windows.h>
#include "YTable.h"

class __declspec(dllexport) CMask24  
{
public:
	CMask24(long lXSize, long lYSize); // konstruktor od razu inicjalizuje
	virtual ~CMask24();

	// ustawia i pobiera wartosc z tej tablicy, wykorzysuje tablice z przeliczeniami

	virtual void			Put(long lXPosition, long lYPosition, unsigned char ucValue);
	virtual unsigned short	Get(long lXPosition, long lYPosition); 

	// zwraca przeliczony offset dla tych wartosci

	virtual unsigned long	GetOffset(long lXPosition, long lYPosition);

private:

	unsigned char *m_pBuffer;		// pomnozyc x3 
	
	CYTable		  *m_pYTable;

};

#endif 
