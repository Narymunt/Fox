// SinTable.h: interface for the CSinTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SINTABLE_H__DAD291DB_5C58_4697_8B7F_C7ECD79E5105__INCLUDED_)
#define AFX_SINTABLE_H__DAD291DB_5C58_4697_8B7F_C7ECD79E5105__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// tablica z przeliczeniami sinusow

class CSinTable  
{

public:
	CSinTable();
	virtual ~CSinTable();

private:

	long	*p_lData;	// tutaj przeliczenia, 1kb

};

#endif // !defined(AFX_SINTABLE_H__DAD291DB_5C58_4697_8B7F_C7ECD79E5105__INCLUDED_)
