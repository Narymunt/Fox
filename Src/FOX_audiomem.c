// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - sprawdzanie ile w danym momencie zajmuje bufor audio
// - bez forkow

// funkcje do alokowania bufora na dzwieku 

#include <stdlib.h>
#include "FOX_audiomem.h"

// alokuj pamiec ktora bedzie dzielona pomiedzy watkami i zwalniana na wyjsciu

void *FOX_AllocAudioMem(int size)
{
	void *chunk;

#ifdef FORK_HACK
	int   semid;

	// stworz i pobierz adres wspoldzielonego segmentu
	
	semid = shmget(IPC_PRIVATE, size, (IPC_CREAT|0600));

	if ( semid < 0 ) 
	{
		return(NULL);
	}
	
	chunk = shmat(semid, NULL, 0);
	
	// pobierz segment jezeli zwolniony 
	
	shmctl(semid, IPC_RMID, NULL);	// usuwa semid jezeli shmat nie dziala
#else

	chunk = malloc(size);	// i to wlasnie wyesponowac do info

#endif
	return((void *)chunk);
}

// zwolnij pamiec audio 

void FOX_FreeAudioMem(void *chunk)
{
	free(chunk);		// bez forkow
}

// end

