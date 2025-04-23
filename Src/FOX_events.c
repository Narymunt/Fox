// Fox v0.5
// by Jaroslaw Rozynski
//===
// *EVENTS*
//===
// TODO:

// obsluga events 

#include <stdio.h>
#include <string.h>

#include "FOX.h"
#include "FOX_thread.h"
#include "FOX_mutex.h"
#include "FOX_events.h"
#include "FOX_events_c.h"
#include "FOX_timer_c.h"

#include "FOX_syswm.h"
#include "FOX_sysevents.h"

// filtr dla zdarzen

FOX_EventFilter FOX_EventOK = NULL;
Uint8 FOX_ProcessEvents[FOX_NUMEVENTS];
static Uint32 FOX_eventstate = 0;

// prywatne 

#define MAXEVENTS	128

static struct 
{

	FOX_mutex *lock;
	
	int active;
	int head;
	int tail;
	
	FOX_Event event[MAXEVENTS];
	
	int wmmsg_next;
	
	struct FOX_SysWMmsg wmmsg[MAXEVENTS];

} FOX_EventQ;

// prywatne, blokowanie struktury dla zdarzenia

static struct 
{

	FOX_mutex *lock;

	int safe;

} FOX_EventLock;

// watki 

static FOX_Thread *FOX_EventThread = NULL;	// handler dla watku (thread)
static Uint32 event_thread;					// event thread id 

// zablokuj 

void FOX_Lock_EventThread(void)
{
	if ( FOX_EventThread && (FOX_ThreadID() != event_thread) ) 
	{
		// grab 
		FOX_mutexP(FOX_EventLock.lock);
		
		while ( ! FOX_EventLock.safe ) 
		{
			FOX_Delay(1);
		}
	}
}

// odblokuj eventthread

void FOX_Unlock_EventThread(void)
{
	if ( FOX_EventThread && (FOX_ThreadID() != event_thread) ) 
	{
		FOX_mutexV(FOX_EventLock.lock);
	}
}

// gobble (???) - wyciete prawie prosto z msdn

static int FOX_GobbleEvents(void *unused)
{
	FOX_SetTimerThreaded(2);
	event_thread = FOX_ThreadID();

	while ( FOX_EventQ.active ) 
	{
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;

		// pobierz events z video subsystem
		
		if ( video ) 
		{
			video->PumpEvents(this);
		}

		// klawisze
		
		FOX_CheckKeyRepeat();

		// ile pozostalo ? 
		
		FOX_EventLock.safe = 1;
		
		if( FOX_timer_running ) 
		{
			FOX_ThreadedTimerCheck();
		}
		
		FOX_Delay(1);

		// sprawdz blokowanie 

		FOX_mutexP(FOX_EventLock.lock);
		FOX_EventLock.safe = 0;
		FOX_mutexV(FOX_EventLock.lock);
	}
	FOX_SetTimerThreaded(0);
	event_thread = 0;
	return(0);
}

// start event thread 

static int FOX_StartEventThread(Uint32 flags)
{
	
	// wyzeruj wszystko 
	
	FOX_EventThread = NULL;
	
	memset(&FOX_EventLock, 0, sizeof(FOX_EventLock));

	// zablokuj 
	
#ifndef DISABLE_THREADS
	FOX_EventQ.lock = FOX_CreateMutex();
	if ( FOX_EventQ.lock == NULL ) 
	{
		return(-1);
	}
#endif /* !DISABLE_THREADS */

	FOX_EventQ.active = 1;

	if ( (flags&FOX_INIT_EVENTTHREAD) == FOX_INIT_EVENTTHREAD ) 
	{
		FOX_EventLock.lock = FOX_CreateMutex();
		
		if ( FOX_EventLock.lock == NULL ) 
		{
			return(-1);
		}
		FOX_EventLock.safe = 0;

		FOX_EventThread = FOX_CreateThread(FOX_GobbleEvents, NULL);
		
		if ( FOX_EventThread == NULL ) 
		{
			return(-1);
		}
	}
	else 
	{
		event_thread = 0;
	}
	return(0);
}

// zatrzymaj 

static void FOX_StopEventThread(void)
{
	FOX_EventQ.active = 0;
	if ( FOX_EventThread ) 
	{
		FOX_WaitThread(FOX_EventThread, NULL);
		FOX_EventThread = NULL;
		FOX_DestroyMutex(FOX_EventLock.lock);
	}
	FOX_DestroyMutex(FOX_EventQ.lock);
}

// pobierz id 

Uint32 FOX_EventThreadID(void)
{
	return(event_thread);
}

// publiczne

void FOX_StopEventLoop(void)
{
	
	FOX_StopEventThread();	// jezeli wogole dziala
	
	FOX_EventQ.head = 0;	// wyczysc eventq
	FOX_EventQ.tail = 0;
	FOX_EventQ.wmmsg_next = 0;
}

// mozna wywolywac wiecej niz jeden raz

int FOX_StartEventLoop(Uint32 flags)
{
	int retcode;

	// wyczysc 
	
	FOX_EventThread = NULL;
	FOX_EventQ.lock = NULL;
	FOX_StopEventLoop();

	// brak filtru dla startu
	
	FOX_EventOK = NULL;
	memset(FOX_ProcessEvents,FOX_ENABLE,sizeof(FOX_ProcessEvents));

	FOX_eventstate = ~0;
	
	// lepiej nie wywolywac FOX_EventState()
		
	FOX_eventstate &= ~(0x00000001 << FOX_SYSWMEVENT);
	FOX_ProcessEvents[FOX_SYSWMEVENT] = FOX_IGNORE;

	// inicjalizuj handler dla zdarzenia
	
	retcode = 0;
	retcode += FOX_AppActiveInit();
	retcode += FOX_KeyboardInit();
	retcode += FOX_MouseInit();
	retcode += FOX_QuitInit();

	if ( retcode < 0 ) 
	{
		// nie powinien sie wysypac, ale na wszelki wypadek 
				
		return(-1);
	}

	// zrob blokade i zdarzenie 
	
	if ( FOX_StartEventThread(flags) < 0 ) 
	{
		FOX_StopEventLoop();
		return(-1);
	}
	return(0);
}

// dodaj watek i event

static int FOX_AddEvent(FOX_Event *event)
{
	int tail, added;

	tail = (FOX_EventQ.tail+1)%MAXEVENTS;

	if ( tail == FOX_EventQ.head ) 
	{
		// przeladowanie - porzuc
		added = 0;
	} 
	else 
	{
		FOX_EventQ.event[FOX_EventQ.tail] = *event;
		
		if (event->type == FOX_SYSWMEVENT) 
		{
			// mozna zgubic to zdarzenie
			int next = FOX_EventQ.wmmsg_next;
			
			FOX_EventQ.wmmsg[next] = *event->syswm.msg;
		        FOX_EventQ.event[FOX_EventQ.tail].syswm.msg =
						&FOX_EventQ.wmmsg[next];
			
			FOX_EventQ.wmmsg_next = (next+1)%MAXEVENTS;
		}
		FOX_EventQ.tail = tail;
		added = 1;
	}
	return(added);
}

// obetnij zdarzenie

static int FOX_CutEvent(int spot)
{
	if ( spot == FOX_EventQ.head ) 
	{
		FOX_EventQ.head = (FOX_EventQ.head+1)%MAXEVENTS;
		return(FOX_EventQ.head);
	} else
	
	if ( (spot+1)%MAXEVENTS == FOX_EventQ.tail ) 
	{
		FOX_EventQ.tail = spot;
		return(FOX_EventQ.tail);
	} 
	else
	{
		int here, next;
			
		// moznaby to zrobic memcpy ;) 
		
		if ( --FOX_EventQ.tail < 0 ) 
		{
			FOX_EventQ.tail = MAXEVENTS-1;
		}
		
		for ( here=spot; here != FOX_EventQ.tail; here = next ) 
		{
			next = (here+1)%MAXEVENTS;
			FOX_EventQ.event[here] = FOX_EventQ.event[next];
		}
		return(spot);
	}
	
	// hmmm? 

}

// zablokuj kolejke zdarzen

int FOX_PeepEvents(FOX_Event *events, int numevents, FOX_eventaction action,
								Uint32 mask)
{
	int i, used;

	// nie blokuj jezeli wyjscie

	if ( ! FOX_EventQ.active ) 
	{
		return(0);
	}

	// zablokuj kolejke zdarzen
	
	used = 0;
	
	if ( FOX_mutexP(FOX_EventQ.lock) == 0 ) 
	{
		if ( action == FOX_ADDEVENT ) 
		{
			for ( i=0; i<numevents; ++i ) 
			{
				used += FOX_AddEvent(&events[i]);
			}
		} 
		else 
		{
			FOX_Event tmpevent;
			int spot;

			// jezeli events = NULL zobacz czy wogole jakies sa
			
			if ( events == NULL ) 
			{
				action = FOX_PEEKEVENT;
				numevents = 1;
				events = &tmpevent;
			}
			
			spot = FOX_EventQ.head;
			
			while ((used < numevents)&&(spot != FOX_EventQ.tail)) 
			{
				if ( mask & FOX_EVENTMASK(FOX_EventQ.event[spot].type) ) 
				{
					events[used++] = FOX_EventQ.event[spot];
					if ( action == FOX_GETEVENT ) 
					{
						spot = FOX_CutEvent(spot);
					} 
					else 
					{
						spot = (spot+1)%MAXEVENTS;
					}
				} 
				else 
				{
					spot = (spot+1)%MAXEVENTS;
				}
			}
		}
		FOX_mutexV(FOX_EventQ.lock);
	} 
	else 
	{
		FOX_SetError("[FOX]: nie mozna zablokowac kolejki zdarzen");
		used = -1;
	}
	return(used);
}

// uruchom petle obslugujaca zdarzenia

void FOX_PumpEvents(void)
{
	if ( !FOX_EventThread ) 
	{
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;

		// pobierz zdarzenia dla video 
		
		if ( video ) 
		{
			video->PumpEvents(this);
		}

		// wcisniety klawisz ? 
		
		FOX_CheckKeyRepeat();
	}
}

// publiczne 

int FOX_PollEvent (FOX_Event *event)
{
	FOX_PumpEvents();

	return(FOX_PeepEvents(event, 1, FOX_GETEVENT, FOX_ALLEVENTS));
}

// zaczekaj 

int FOX_WaitEvent (FOX_Event *event)
{
	while ( 1 ) 
	{
		FOX_PumpEvents();
		switch(FOX_PeepEvents(event, 1, FOX_GETEVENT, FOX_ALLEVENTS)) 
		{
		    case -1: return -1; 
		    case 1: return 1;
		    case 0: FOX_Delay(10);
		}
	}
}

// pchnij 

int FOX_PushEvent(FOX_Event *event)
{
	return(FOX_PeepEvents(event, 1, FOX_ADDEVENT, 0));
}

// filtr dla zdarzenia

void FOX_SetEventFilter (FOX_EventFilter filter)
{
	FOX_Event bitbucket;

	// ustaw
	
	FOX_EventOK = filter;
	while ( FOX_PollEvent(&bitbucket) > 0 )
		;
}

// pobierz filtr zdarzenia

FOX_EventFilter FOX_GetEventFilter(void)
{
	return(FOX_EventOK);
}

// stan zdarzenia

Uint8 FOX_EventState (Uint8 type, int state)
{
	FOX_Event bitbucket;
	Uint8 current_state;

	// jezeli FOX_ALLEVENTS
	
	if ( type == 0xFF ) 
	{
		current_state = FOX_IGNORE;
		for ( type=0; type<FOX_NUMEVENTS; ++type ) 
		{
			if ( FOX_ProcessEvents[type] != FOX_IGNORE ) 
			{
				current_state = FOX_ENABLE;
			}
			
			FOX_ProcessEvents[type] = state;
			
			if ( state == FOX_ENABLE ) 
			{
				FOX_eventstate |= (0x00000001 << (type));
			} 
			else 
			{
				FOX_eventstate &= ~(0x00000001 << (type));
			}
		}

		while ( FOX_PollEvent(&bitbucket) > 0 )
			;
		return(current_state);
	}

	// ustaw stan dla zdarzenia
		
	current_state = FOX_ProcessEvents[type];
	switch (state) 
	{
		case FOX_IGNORE:
		case FOX_ENABLE:
			// napewno ? 
			FOX_ProcessEvents[type] = state;
			
			if ( state == FOX_ENABLE ) 
			{
				FOX_eventstate |= (0x00000001 << (type));
			} 
			else 
			{
				FOX_eventstate &= ~(0x00000001 << (type));
			}
			
			while ( FOX_PollEvent(&bitbucket) > 0 )
				;
			break;
		default:
			// i co teraz ? 
			break;
	}
	return(current_state);
}

// glowna obsluga zdarzen 

int FOX_PrivateSysWMEvent(FOX_SysWMmsg *message)
{
	int posted;

	posted = 0;

	if ( FOX_ProcessEvents[FOX_SYSWMEVENT] == FOX_ENABLE ) 
	{
		FOX_Event event;
		memset(&event, 0, sizeof(event));
		event.type = FOX_SYSWMEVENT;
		event.syswm.msg = message;

		if ( (FOX_EventOK == NULL) || (*FOX_EventOK)(&event) ) 
		{
			posted = 1;
			FOX_PushEvent(&event);
		}
	}
	
	// aktualizuj stan

	return(posted);
}

// end

