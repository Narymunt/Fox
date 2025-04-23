// Fox v0.7
// by Jaroslaw Rozynski
//================================================================================================
// LAST UPDATE:
// 18 march 2002
//================================================================================================
// CHANGES:
// - nie mozna uruchomic biblioteki bez obslugi video
// - zawsze musi byc flaga wlaczenie video i audio
// - usuniete sprawdzenie blokady przy deinicjalizowaniu audio i video
//================================================================================================
// TODO:
// - czy potrzebne endian ? zapis jest jako 4321
// - po co FOX_leaks.h ? 
// - check_leaks jest zawsze
// - timer zawsze wlaczony, wiec usunac flagi dla disable timers
// - audio zawsze wlaczone, wiec usunac flagi dla disable audio (zamiast tego stosowac stop^play)
// - przy zamykaniu systemu informacje o surface do dziennika, a nie na ekran
// - przy wywolaniu biblioteki - reason_for_call do dziennika

// inicjalizacja biblioteki

#include <stdlib.h>		// dla gettenv, w razie czego usunac
#include <windows.h>

#include "FOX.h"			// glowny
#include "FOX_endian.h"		// 1234 czy 3412
#include "FOX_fatal.h"		// krytyczne bledy 
#include "FOX_leaks.h"		// dodatki 

// inicjalizacja i czyszczenie routinek 

#ifndef DISABLE_TIMERS
	extern void FOX_StartTicks(void);
	extern int  FOX_TimerInit(void);
	extern void FOX_TimerQuit(void);
#endif

// zainicjalizowane podsystemy

static Uint32 FOX_initialized = 0;
static Uint32 ticks_started = 0;

#ifdef CHECK_LEAKS
	int surfaces_allocated = 0;
#endif

int FOX_InitSubSystem(Uint32 flags)
{
	
	// init video i zdarzen	
	
	if ( (flags & FOX_INIT_VIDEO) && !(FOX_initialized & FOX_INIT_VIDEO) ) 
	{
		if ( FOX_VideoInit(getenv("FOX_VIDEODRIVER"),(flags&FOX_INIT_EVENTTHREAD)) < 0 ) 
			return(-1);
		
		FOX_initialized |= FOX_INIT_VIDEO;
	}

	// init audio
	
	if ( (flags & FOX_INIT_AUDIO) && !(FOX_initialized & FOX_INIT_AUDIO) ) 
	{
		if ( FOX_AudioInit(getenv("FOX_AUDIODRIVER")) < 0 ) 
			return(-1);
		
		FOX_initialized |= FOX_INIT_AUDIO;
	}

#ifndef DISABLE_TIMERS

	// init timer
	
	if ( ! ticks_started ) 
	{
		FOX_StartTicks();
		ticks_started = 1;
	}
	
	if ( (flags & FOX_INIT_TIMER) && !(FOX_initialized & FOX_INIT_TIMER) ) 
	{
		if ( FOX_TimerInit() < 0 ) 
		{
			return(-1);
		}
		FOX_initialized |= FOX_INIT_TIMER;
	}
#else
	if ( flags & FOX_INIT_TIMER ) 
	{
		FOX_SetError("[FOX]: brak obslugi timera");
		return(-1);
	}
#endif

	return(0);
}

//=== tutaj init

int FOX_Init(Uint32 flags)
{
	
	FOX_ClearError();		// wyczysc liste bledow

	// zainicjalizj podsystemy, joystick, sound itd.

	if ( FOX_InitSubSystem(flags) < 0 ) return(-1);

	return(0);
}

// zwolnij podsystem - uzywaj flagi

void FOX_QuitSubSystem(Uint32 flags)
{
	
	// wylacz co bylo we flagach

#ifndef DISABLE_TIMERS
	if ( (flags & FOX_initialized & FOX_INIT_TIMER) ) 
	{
		FOX_TimerQuit();
		FOX_initialized &= ~FOX_INIT_TIMER;
	}
#endif

	// deinicjalizacja audio

	if ( (flags & FOX_initialized & FOX_INIT_AUDIO) ) 
	{
		FOX_AudioQuit();
		FOX_initialized &= ~FOX_INIT_AUDIO;
	}

	// deinicjalizacja video

	if ( (flags & FOX_initialized & FOX_INIT_VIDEO) ) 
	{
		FOX_VideoQuit();
		FOX_initialized &= ~FOX_INIT_VIDEO;
	}

}

// sprawdz co bylo zainicjalizowane

Uint32 FOX_WasInit(Uint32 flags)
{
	if ( ! flags ) flags = FOX_INIT_EVERYTHING;
	
	return (FOX_initialized&flags);
}

// wyjscie

void FOX_Quit(void)
{
	
	// zamknij wszystkie podsystemy	

	FOX_QuitSubSystem(FOX_INIT_EVERYTHING);

#ifdef CHECK_LEAKS
	
	if ( surfaces_allocated != 0 ) 
	{
		fprintf(stderr, "[FOX]: %d FOX surfaces \n", 
							surfaces_allocated);
	}
#endif

}

// powinno dzialac

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, 
                       LPVOID lpReserved )
{
	switch (ul_reason_for_call) 
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}
 
// end

