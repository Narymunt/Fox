// Variable.h: interface for the CVariable class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXVARIABLE_
#define _FOXVARIABLE_

#include <windows.h>
#include <fox.h>

// zmienna wykorzystywana jest przy warunkach i ustawieniach

class __declspec(dllexport) CVariable  
{
public:
	CVariable();
	virtual ~CVariable();

private:

	int		m_iType;				// typ zmiennej
	char	m_cName[256];			// nazwa zmiennej
	double  m_dValue;


};

#endif // !defined(AFX_VARIABLE_H__6631E8AA_64C2_4579_8240_26F4155F2DDA__INCLUDED_)
