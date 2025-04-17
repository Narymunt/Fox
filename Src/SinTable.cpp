// SinTable.cpp: implementation of the CSinTable class.
//
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <math.h>
#include "stdafx.h"
#include "SinTable.h"

//=== nasze pi ;)

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//=== alokacja pamieci i przeliczenie

CSinTable::CSinTable()
{

	// tutaj dodac sprawdzenie do loggera

	p_lData = (long*) malloc(1024*sizeof(long));

	// przeliczenie

	for (double dLicznik=0; dLicznik < 1024; dLicznik++)
	{
		*p_lData++ = (long) (sin(dLicznik*M_PI/256)*16384);
	}

}

//=== zwolnienie pamieci 

CSinTable::~CSinTable()
{

	// zwolnij

	if (p_lData!=NULL)
	{
		free(p_lData);
		p_lData = NULL;
	}

}
