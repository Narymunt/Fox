// YTable.cpp: implementation of the CYTable class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "stdafx.h"
#include "YTable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== ten konstruktor nic nie robi

CYTable::CYTable()
{
	p_ulLine = NULL;
}

//=== destruktor zwalnia pamiec jezeli jest zaalokowana

CYTable::~CYTable()
{
	if (p_ulLine!=NULL)	// zwolnij pamiec
	{
		free(p_ulLine);
		p_ulLine = NULL;
	}

}

//=== zaalokuj tablice offsetow dla danej rozdzielczosci

CYTable::CYTable(unsigned long ulSize, unsigned long ulResolution)
{
	
	// to poprawic na logger
	
	if ( (p_ulLine=(unsigned long*)malloc(ulSize))==NULL )
	{
		MessageBox(0,"B³¹d!","Za ma³o pamiêci dla tablicy przeliczeñ!",MB_OK);
	}

	// przygotuj tablice offsetow

	for (unsigned long ulLicznik=0; ulLicznik<ulSize; ulLicznik++)
	{
		p_ulLine[ulLicznik] = ulResolution * ulLicznik;	// i.e. 1*800, 2*800, 3*800
	}

}

//=== zwroc wartosc 

long CYTable::lGetValue(unsigned long ulIndex)
{
	return p_ulLine[ulIndex];
}

//end
