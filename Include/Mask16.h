// Mask16.h: interface for the CMask16 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXMASK16_
#define _FOXMASK16_

#include <windows.h>
#include "YTable.h"

//=== klasa obslugujaca maske 16bpp
//=== nie musi byc jako RGB ale zwykly short
//=== TODO: 
//		- konwersje pomiedzy bitami (16,32, itepe)

class __declspec(dllexport) CMask16  
{
public:
	CMask16(long lXSize, long lYSize);
	virtual ~CMask16();

	// ustawia i pobiera wartosc z tej tablicy, wykorzysuje tablice z przeliczeniami

	virtual void			Put(long lXPosition, long lYPosition, unsigned short ucValue);
	virtual unsigned short	Get(long lXPosition, long lYPosition); 

	// zwraca przeliczony offset dla tych wartosci

	virtual unsigned long	GetOffset(long lXPosition, long lYPosition);

private:

	unsigned short *m_pBuffer;	// jako SHORT !!! 
	
	CYTable		  *m_pYTable;


};

#endif 
