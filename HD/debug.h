#ifndef _fox_debug_
#define _fox_debug_

#include <stdlib.h>
#include <stdio.h>

void fox_debug(char cInfo[])
{

	FILE *plik;
	
	plik = fopen("debug.log","at");
	fprintf(plik,cInfo);
	
	fclose(plik);
	
}

#endif
