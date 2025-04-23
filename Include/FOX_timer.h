// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// zegarek 

#ifndef _FOX_timer_h
#define _FOX_timer_h

#include "FOX_main.h"
#include "FOX_types.h"

#include "FOX_begin.h"

// traktuj funkcje jako C

#ifdef __cplusplus
extern "C" {
#endif

// slice w ms 

#define FOX_TIMESLICE		10

// res w ms

#define TIMER_RESOLUTION	10	

// podaj status zegara

extern DECLSPEC Uint32 FOX_GetTicks(void);

// czekaj ms

extern DECLSPEC void FOX_Delay(Uint32 ms);

// prototyp dla callback 

typedef Uint32 (*FOX_TimerCallback)(Uint32 interval);

// ustaw callback po uplywie ilus milisekund

extern DECLSPEC int FOX_SetTimer(Uint32 interval, FOX_TimerCallback callback);

typedef Uint32 (*FOX_NewTimerCallback)(Uint32 interval, void *param);

// definicja identyfikatora timera

typedef struct _FOX_TimerID *FOX_TimerID;

// dodaj nowy timer do tych ktore juz dzialaja

extern DECLSPEC FOX_TimerID FOX_AddTimer(Uint32 interval, FOX_NewTimerCallback callback, void *param);

// usun timer na podstawie jego identyfikatora

extern DECLSPEC FOX_bool FOX_RemoveTimer(FOX_TimerID t);

// koniec funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end

