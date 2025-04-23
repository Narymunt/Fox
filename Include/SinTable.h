// SinTable.h: interface for the CSinTable class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _FOXSINTABLE_
#define _FOXSINTABLE_

// tablica z przeliczeniami sinusow

// TODO:
//	- uporzadkowac metody sinusow


class CSinTable  
{

public:
	CSinTable();
	virtual ~CSinTable();

private:

	long	*p_lData;	// tutaj przeliczenia, 1kb

};

#endif 
