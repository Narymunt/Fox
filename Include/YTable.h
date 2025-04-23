// YTable.h: interface for the CYTable class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXYTABLE_
#define _FOXYTABLE_

#include <windows.h>

// xsize * y

class CYTable  
{
public:
	
	CYTable();
	virtual ~CYTable();

	CYTable(unsigned long ulSize, unsigned long ulResolution);	// konstruktor

	long	lGetValue(unsigned long ulIndex);

private:

	unsigned long	*p_ulLine;		// tablica z offsetami


};

#endif // !defined(AFX_YTABLE_H__148BB383_BB62_4653_A18E_19AA5F81A7A0__INCLUDED_)
