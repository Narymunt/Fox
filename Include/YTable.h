// YTable.h: interface for the CYTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YTABLE_H__148BB383_BB62_4653_A18E_19AA5F81A7A0__INCLUDED_)
#define AFX_YTABLE_H__148BB383_BB62_4653_A18E_19AA5F81A7A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

// xsize * y

class CYTable  
{
public:
	
	CYTable();
	virtual ~CYTable();

	CYTable(unsigned long ulSize, unsigned long ulResolution);	// konstruktor

private:

	unsigned long	*p_ulLine;		// tablica z offsetami


};

#endif // !defined(AFX_YTABLE_H__148BB383_BB62_4653_A18E_19AA5F81A7A0__INCLUDED_)
