// Fox v0.5
// by Jaroslaw Rozynski
//===
// *EVENTS*
//===
// TODO:


// wyjscie wywolane przez luser'a 

#include <stdio.h>

// domyslnie bez

#ifndef NO_SIGNAL_H
	#include <signal.h>
#endif

#include "FOX_events.h"
#include "FOX_events_c.h"

#ifndef NO_SIGNAL_H

	static void FOX_HandleSIG(int sig)
	{
		
		signal(sig, FOX_HandleSIG);
		FOX_PrivateQuit();

	}

#endif 

// funkcje publiczne ;)

int FOX_QuitInit(void)
{

#ifndef NO_SIGNAL_H
	void (*ohandler)(int);


	ohandler = signal(SIGINT,  FOX_HandleSIG);

	if ( ohandler != SIG_DFL )
		signal(SIGINT, ohandler);
	ohandler = signal(SIGTERM, FOX_HandleSIG);
	
	if ( ohandler != SIG_DFL )
		signal(SIGTERM, ohandler);

#endif 
	
	return(0);
}

// zwraca 1 jezeli bez problemu udalo sie zamknac okno applikacji 

int FOX_PrivateQuit(void)
{
	int posted;

	posted = 0;
	if ( FOX_ProcessEvents[FOX_QUIT] == FOX_ENABLE ) 
	{
		FOX_Event event;
		event.type = FOX_QUIT;
		
		if ( (FOX_EventOK == NULL) || (*FOX_EventOK)(&event) ) 
		{
			posted = 1;
			FOX_PushEvent(&event);
		}
	}
	return(posted);
}

// end

