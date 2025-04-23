// Fox v0.5
// by Jaroslaw Rozynski

// czas, timer i dodatkowe smieci
// generalnie dla synchronizacji

#ifndef _FOX_mutex_h
#define _FOX_mutex_h

#include "FOX_main.h"		// glowne funkcje 
#include "FOX_types.h"

#include "FOX_begin.h"		// poczatkowe smieci

// traktuj funkcje jako C 

#ifdef __cplusplus
extern "C" {
#endif

// synchro

#define FOX_MUTEX_TIMEDOUT	1

// timeout 

#define FOX_MUTEX_MAXWAIT	(~(Uint32)0)

// struktura mutex, szczegoly w FOX_mutex.c

struct FOX_mutex;

typedef struct FOX_mutex FOX_mutex;

// stworz mutex, inicjalizacja zmiennych

extern DECLSPEC FOX_mutex * FOX_CreateMutex(void);

// zablokuj mutex, zwraca 0 lub -1 jezeli blad

#define FOX_LockMutex(m)	FOX_mutexP(m)

extern DECLSPEC int FOX_mutexP(FOX_mutex *mutex);

// odblokuj mutex, 0 ok, -1 error

#define FOX_UnlockMutex(m)	FOX_mutexV(m)

extern DECLSPEC int FOX_mutexV(FOX_mutex *mutex);

// zwolnij 

extern DECLSPEC void FOX_DestroyMutex(FOX_mutex *mutex);

// struktura 

struct FOX_semaphore;

typedef struct FOX_semaphore FOX_sem;

// zainicjalizuj, NULL jezeli blad

extern DECLSPEC FOX_sem * FOX_CreateSemaphore(Uint32 initial_value);

// usun 

extern DECLSPEC void FOX_DestroySemaphore(FOX_sem *sem);

// delay

extern DECLSPEC int FOX_SemWait(FOX_sem *sem);

// czekaj bez blokowania

extern DECLSPEC int FOX_SemTryWait(FOX_sem *sem);

// czekaj i blokuj na x milicekund

extern DECLSPEC int FOX_SemWaitTimeout(FOX_sem *sem, Uint32 ms);

// zwieksz limit czekania, zwroc 0 ok, -1 error

extern DECLSPEC int FOX_SemPost(FOX_sem *sem);

// zwroc aktualny czas oczekiwania

extern DECLSPEC Uint32 FOX_SemValue(FOX_sem *sem);

// szczegoly w c

struct FOX_cond;

typedef struct FOX_cond FOX_cond;

// dodanie zmiennej

extern DECLSPEC FOX_cond * FOX_CreateCond(void);

// usuniecie zmiennej 

extern DECLSPEC void FOX_DestroyCond(FOX_cond *cond);

// zrestartuj watek, zwraca 0 lub -1 jezeli error

extern DECLSPEC int FOX_CondSignal(FOX_cond *cond);

// zrestartuj wszystkie watki, 0 ok, -1 error

extern DECLSPEC int FOX_CondBroadcast(FOX_cond *cond);

// czekaj na warunek, 0 gdy jest, -1 jezeli blad

extern DECLSPEC int FOX_CondWait(FOX_cond *cond, FOX_mutex *mut);

// czekaj warunkowo kilka ms, 0 jezeli warunek jest, -1 jezeli blad

extern DECLSPEC int FOX_CondWaitTimeout(FOX_cond *cond, FOX_mutex *mutex, Uint32 ms);

// koniec funkcji C 

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end
