// Mask8.h: interface for the CMask8 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXMASK8_
#define _FOXMASK8_

#include <windows.h>
#include "YTable.h"

// TODO:
//	- alpha
//	- clipping (?)
//	- hline
//	- vline
//	- line
//	- pointed divided bezier line
//	- box filled
//	- rectangle
//	- gouraud line
//	- blur
//	- put mask on mask
//  - gouraud shaded line
//	- bpp conversion 8>>16 8>>24 8>>32
//	- circle + ellipse
//  - triangle (?)



// maska 8bpp czyli jeden bajt oznacza kolor

class __declspec(dllexport) CMask8  
{
public:
	CMask8(long lXSize, long lYSize);
	virtual ~CMask8();

	// ustawia i pobiera wartosc z tej tablicy, wykorzysuje tablice z przeliczeniami

	virtual void			Put(long lXPosition, long lYPosition, unsigned char ucValue);
	virtual unsigned char	Get(long lXPosition, long lYPosition); 

	// zwraca przeliczony offset dla tych wartosci

	virtual unsigned long	GetOffset(long lXPosition, long lYPosition);
	

private:

	unsigned char *m_pBuffer;
	
	CYTable		  *m_pYTable;

};

#endif 
