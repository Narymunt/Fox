// Ytable.cpp: implementation of the CYtable class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "ytable.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CYtable::CYtable()
{

}

CYtable::~CYtable()
{
	if (m_ulLine!=NULL) delete m_ulLine;
}

CYtable::CYtable(unsigned long ulSize, unsigned long ulResolution)
{

	unsigned long t1;

	if ((m_ulLine= new unsigned long[ulSize+1])==NULL)
	{
		MessageBox(0,"error","Za duza Ytable",MB_OK);
		exit(0);
	}

	for (t1=0;t1<ulSize+1;t1++)
	{
		m_ulLine[t1]= ulResolution * t1;		// 1*800, 2*800, 3*800
	}

}
