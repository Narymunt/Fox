// Fox v0.5
// by Jaroslaw Rozynski
//===
// *TIMER*
//===
// TODO:
// - usunac debug


#include <stdlib.h>
#include <stdio.h>			// NULL 

#include "FOX_error.h"
#include "FOX_timer.h"
#include "FOX_timer_c.h"
#include "FOX_mutex.h"
#include "FOX_systimer.h"

// trudno sprawdzic

// #define DEBUG_TIMERS 

int FOX_timer_started = 0;
int FOX_timer_running = 0;

// dla jednego alarmu 

Uint32 FOX_alarm_interval = 0;
FOX_TimerCallback FOX_alarm_callback;

static FOX_bool list_changed = FOX_FALSE;

// timer oparty na watku 

static int FOX_timer_threaded = 0;

struct _FOX_TimerID 
{

	Uint32 interval;
	
	FOX_NewTimerCallback cb;
	
	void *param;
	
	Uint32 last_alarm;
	
	struct _FOX_TimerID *next;

};

static FOX_TimerID FOX_timers = NULL;
static Uint32 num_timers = 0;
static FOX_mutex *FOX_timer_mutex;

// timer na watku ? 

int FOX_SetTimerThreaded(int value)
{
	int retval;

	if ( FOX_timer_started ) 
	{
		FOX_SetError("[FOX]: zegar juz zainicjalizowany");
		retval = -1;
	} 
	else 
	{
		retval = 0;
		FOX_timer_threaded = value;
	}
	return retval;
}

// inicjalizuj 

int FOX_TimerInit(void)
{
	int retval;

	FOX_timer_running = 0;
	
	FOX_SetTimer(0, NULL);
	
	retval = 0;
	
	if ( ! FOX_timer_threaded ) 
	{
		retval = FOX_SYS_TimerInit();
	}
	
	if ( FOX_timer_threaded ) 
	{
		FOX_timer_mutex = FOX_CreateMutex();
	}
	FOX_timer_started = 1;
	return(retval);
}

// wyjscie 

void FOX_TimerQuit(void)
{
	FOX_SetTimer(0, NULL);
	
	if ( FOX_timer_threaded < 2 ) 
	{
		FOX_SYS_TimerQuit();
	}
	
	if ( FOX_timer_threaded ) 
	{
		FOX_DestroyMutex(FOX_timer_mutex);
	}
	FOX_timer_started = 0;
	FOX_timer_threaded = 0;
}

// timer trzymany na watku 

void FOX_ThreadedTimerCheck(void)
{
	Uint32 now, ms;
	FOX_TimerID t, prev, next;
	int removed;

	now = FOX_GetTicks();

	FOX_mutexP(FOX_timer_mutex);
	
	for ( prev = NULL, t = FOX_timers; t; t = next ) 
	{
		removed = 0;
		ms = t->interval - FOX_TIMESLICE;
		next = t->next;
		
		if ( (t->last_alarm < now) && ((now - t->last_alarm) > ms) ) 
		{
			if ( (now - t->last_alarm) < t->interval ) 
			{
				t->last_alarm += t->interval;
			} 
			else 
			{
				t->last_alarm = now;
			}
			list_changed = FOX_FALSE;
#ifdef DEBUG_TIMERS
			printf("[FOX]: Executing timer %p (thread = %d)\n",
						t, FOX_ThreadID());
#endif
			FOX_mutexV(FOX_timer_mutex);
			ms = t->cb(t->interval, t->param);
			FOX_mutexP(FOX_timer_mutex);
			
			if ( list_changed ) 
			{
				// lista zmieniona 
				break;
			}
			
			if ( ms != t->interval ) 
			{
				if ( ms ) 
				{
					t->interval = ROUND_RESOLUTION(ms);
				} 
				else 
				{ 
#ifdef DEBUG_TIMERS
					printf("[FOX]: usuwam zegar %p\n", t);
#endif
					if ( prev ) 
					{
						prev->next = next;
					} 
					else 
					{
						FOX_timers = next;
					}
					free(t);
					-- num_timers;
					removed = 1;
				}
			}
		}
		
		// nie aktualizuj 
		
		if ( ! removed ) 
		{
			prev = t;
		}
	}
	FOX_mutexV(FOX_timer_mutex);
}

// dodaj 

FOX_TimerID FOX_AddTimer(Uint32 interval, FOX_NewTimerCallback callback, void *param)
{
	FOX_TimerID t;
	
	if ( ! FOX_timer_mutex ) 
	{
		if ( FOX_timer_started ) 
		{
			FOX_SetError("[FOX]: nie mozna zainicjalizowac kilku zegarow");
		} 
		else 
		{
			FOX_SetError("[FOX]: zegar nie zostal zainicjalizowany");
		}
		return NULL;
	}
	
	if ( ! FOX_timer_threaded ) 
	{
		FOX_SetError("[FOX]: wiecej zagaerow wymaga TEV");
		return NULL;
	}
	
	FOX_mutexP(FOX_timer_mutex);
	t = (FOX_TimerID) malloc(sizeof(struct _FOX_TimerID));
	
	if ( t ) 
	{
		t->interval = ROUND_RESOLUTION(interval);
		t->cb = callback;
		t->param = param;
		t->last_alarm = FOX_GetTicks();
		t->next = FOX_timers;
		FOX_timers = t;
		++ num_timers;
		list_changed = FOX_TRUE;
		FOX_timer_running = 1;
	}
#ifdef DEBUG_TIMERS
	printf("[FOX]: FOX_AddTimer(%d) = %08x num_timers = %d\n", interval, (Uint32)t, num_timers);
#endif
	FOX_mutexV(FOX_timer_mutex);
	return t;
}

// usun timer 

FOX_bool FOX_RemoveTimer(FOX_TimerID id)
{
	FOX_TimerID t, prev = NULL;
	FOX_bool removed;

	removed = FOX_FALSE;
	FOX_mutexP(FOX_timer_mutex);
	
	// szukaj id na liscie timerow 
		
	for (t = FOX_timers; t; prev=t, t = t->next ) 
	{
		if ( t == id ) 
		{
			if(prev) 
			{
				prev->next = t->next;
			} 
			else 
			{
				FOX_timers = t->next;
			}
			free(t);
			-- num_timers;
			removed = FOX_TRUE;
			list_changed = FOX_TRUE;
			break;
		}
	}
#ifdef DEBUG_TIMERS
	printf("[FOX]: FOX_RemoveTimer(%08x) = %d num_timers = %d thread = %d\n", (Uint32)id, removed, num_timers, FOX_ThreadID());
#endif
	FOX_mutexV(FOX_timer_mutex);
	return removed;
}

// usun wszystkie timery

static void FOX_RemoveAllTimers(FOX_TimerID t)
{
	FOX_TimerID freeme;

	while ( t ) 
	{
		freeme = t;
		t = t->next;
		free(freeme);
	}
}

// oldschool callback ;) 

static Uint32 callback_wrapper(Uint32 ms, void *param)
{
	FOX_TimerCallback func = (FOX_TimerCallback) param;
	return (*func)(ms);
}

// ustaw timer 

int FOX_SetTimer(Uint32 ms, FOX_TimerCallback callback)
{
	int retval;

#ifdef DEBUG_TIMERS
	printf("[FOX]: FOX_SetTimer(%d)\n", ms);
#endif
	retval = 0;
	
	if ( FOX_timer_running ) 
	{
		FOX_timer_running = 0;
	
		if ( FOX_timer_threaded ) 
		{
			FOX_mutexP(FOX_timer_mutex);
			FOX_RemoveAllTimers(FOX_timers);
			FOX_timers = NULL;
			FOX_mutexV(FOX_timer_mutex);
		} 
		else 
		{
			FOX_SYS_StopTimer();
		}
	}
	
	if ( ms ) 
	{
		if ( FOX_timer_threaded ) 
		{
			retval = (FOX_AddTimer(ms, callback_wrapper,
					       (void *)callback) != NULL);
		} 
		else 
		{
			FOX_timer_running = 1;
			FOX_alarm_interval = ms;
			FOX_alarm_callback = callback;
			retval = FOX_SYS_StartTimer();
		}
	}
	return retval;
}

// end
