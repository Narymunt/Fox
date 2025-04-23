// Fox v0.7
// by Jaroslaw Rozynski
//================================================================================================
// LAST UPDATE:
// 18 march 2002
//================================================================================================
// CHANGES:
// - usuniete fork_hack
//================================================================================================
// TODO:
// - sprawdzanie ile w danym momencie zajmuje bufor audio

// funkcje do alokowania bufora na dzwieku 

#include <stdlib.h>
#include "audiomem.h"

// alokuj pamiec ktora bedzie dzielona pomiedzy watkami i zwalniana na wyjsciu

void *FOX_AllocAudioMem(int size)
{
	void *chunk;
	chunk = malloc(size);	// i to wlasnie wyeksponowac do info
	return((void *)chunk);
}

// zwolnij pamiec audio 

void FOX_FreeAudioMem(void *chunk)
{
	free(chunk);		// bez forkow
}

// end

