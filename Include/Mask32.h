// Mask32.h: interface for the CMask32 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXMASK32_
#define _FOXMASK32_

// maska w 32bpp bgra
// TODO:
//		- alpha
//		- clipping

#include <windows.h>
#include "YTable.h"

class __declspec(dllexport) CMask32  
{
public:
	CMask32(long lXSize, long lYSize);
	virtual ~CMask32();

private:

	unsigned char *m_pBuffer;	// x4
	
	CYTable		  *m_pYTable;


};

#endif 
