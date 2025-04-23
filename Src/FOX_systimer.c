// Fox v0.5
// by Jaroslaw Rozynski
//===
// *TIMER*
//===
// TODO:

#include <windows.h>
#include <mmsystem.h>

#include "FOX_timer.h"
#include "FOX_timer_c.h"
#include "FOX_error.h"

#define TIME_WRAP_VALUE	(~(DWORD)0)

// start

static DWORD start;

#ifndef USE_GETTICKCOUNT
	static BOOL hires_timer_available;
	static LARGE_INTEGER hires_start_ticks;
	static LARGE_INTEGER hires_ticks_per_second;
#endif

// uruchom zegarek 

void FOX_StartTicks(void)
{
	
#ifdef USE_GETTICKCOUNT
	start = GetTickCount();
#else
	
	if (QueryPerformanceFrequency(&hires_ticks_per_second) == TRUE)
	{
		hires_timer_available = TRUE;
		QueryPerformanceCounter(&hires_start_ticks);
	}
	else
	{
		hires_timer_available = FALSE;
		timeBeginPeriod(1);		// precyzja 1ms 
		start = timeGetTime();
	}
#endif
}

Uint32 FOX_GetTicks(void)
{
	DWORD now, ticks;

#ifndef USE_GETTICKCOUNT
	LARGE_INTEGER hires_now;
#endif

#ifdef USE_GETTICKCOUNT
	now = GetTickCount();
#else

	if (hires_timer_available)
	{
		QueryPerformanceCounter(&hires_now);

		hires_now.QuadPart -= hires_start_ticks.QuadPart;
		hires_now.QuadPart *= 1000;
		hires_now.QuadPart /= hires_ticks_per_second.QuadPart;

		return (DWORD)hires_now.QuadPart;
	}
	else
	{
		now = timeGetTime();
	}
#endif

	if ( now < start ) 
	{
		ticks = (TIME_WRAP_VALUE-start) + now;
	} 
	else 
	{
		ticks = (now - start);
	}
	return(ticks);
}

// opoznienie

void FOX_Delay(Uint32 ms)
{
	Sleep(ms);
}

#ifdef USE_SETTIMER

static UINT WIN_timer;

int FOX_SYS_TimerInit(void)
{
	return(0);
}

void FOX_SYS_TimerQuit(void)
{
	return;
}

// tak samo deklaracje

int FOX_SYS_StartTimer(void);

static VOID CALLBACK TimerCallbackProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	Uint32 ms;

	ms = FOX_alarm_callback(FOX_alarm_interval);

	if ( ms != FOX_alarm_interval ) 
	{
		KillTimer(NULL, idEvent);
		
		if ( ms ) 
		{
			FOX_alarm_interval = ROUND_RESOLUTION(ms);
			FOX_SYS_StartTimer();
		} 
		else
		{
			FOX_alarm_interval = 0;
		}
	}
}

// wystartuj 

int FOX_SYS_StartTimer(void)
{
	int retval;

	WIN_timer = SetTimer(NULL, 0, FOX_alarm_interval, TimerCallbackProc);
	
	if ( WIN_timer ) 
	{
		retval = 0;
	} 
	else 
	{
		retval = -1;
	}
	return retval;
}

// zatrzymaj 

void FOX_SYS_StopTimer(void)
{
	if ( WIN_timer ) 
	{
		KillTimer(NULL, WIN_timer);
		WIN_timer = 0;
	}
}

#else 

static UINT timerID = 0;

static void CALLBACK HandleAlarm(UINT uID,  UINT uMsg, DWORD dwUser,
						DWORD dw1, DWORD dw2)
{
	FOX_ThreadedTimerCheck();
}

// inicjalizuj 

int FOX_SYS_TimerInit(void)
{
	MMRESULT result;

	// dokladnosc 
	
	result = timeBeginPeriod(TIMER_RESOLUTION);

	if ( result != TIMERR_NOERROR ) 
	{
		FOX_SetError("[FOX]: nie mozna ustawic zegara %d ms",
							TIMER_RESOLUTION);
	}
	
	timerID = timeSetEvent(TIMER_RESOLUTION,1,HandleAlarm,0,TIME_PERIODIC);
	
	if ( ! timerID ) 
	{
		FOX_SetError("[FOX]: timeSetEvent() failed");
		return(-1);
	}
	return(FOX_SetTimerThreaded(1));
}

// zatrzymaj 

void FOX_SYS_TimerQuit(void)
{
	if ( timerID ) 
	{
		timeKillEvent(timerID);
	}
	timeEndPeriod(TIMER_RESOLUTION);
}

// startuj 

int FOX_SYS_StartTimer(void)
{
	FOX_SetError("[FOX]: blad wewnetrzny zegara");
	return(-1);
}

// hmm ? 

void FOX_SYS_StopTimer(void)
{
	return;
}

#endif 

// end

