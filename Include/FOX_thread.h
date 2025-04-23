// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:
// - watki uporzadkowane w klasy / struktury

// obsluga watkow

#ifndef _FOX_thread_h
#define _FOX_thread_h

#include "FOX_main.h"
#include "FOX_types.h"

#include "FOX_mutex.h"		// synchronizacja watkow

#include "FOX_begin.h"		// poczatkowe smieci

// traktuj funkcje jako C

#ifdef __cplusplus
extern "C" {
#endif

// struktura, reszta w pliku cpp

struct FOX_Thread;
typedef struct FOX_Thread FOX_Thread;

// stworz watek

extern DECLSPEC FOX_Thread * FOX_CreateThread(int (*fn)(void *), void *data);

// pobierz 32bitowy identyfikator aktualnego watka

extern DECLSPEC Uint32 FOX_ThreadID(void);

// szczegoly

extern DECLSPEC Uint32 FOX_GetThreadID(FOX_Thread *thread);

// czekaj na zakonczenie watka 

extern DECLSPEC void FOX_WaitThread(FOX_Thread *thread, int *status);

// zabij watek bez czekania 

extern DECLSPEC void FOX_KillThread(FOX_Thread *thread);

// koniec funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end

