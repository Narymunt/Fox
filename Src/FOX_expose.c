// Fox v0.5
// by Jaroslaw Rozynski
//===
// *EVENTS*
//===
// TODO:

// odswiezanie zdarzen 

#include "FOX_events.h"
#include "FOX_events_c.h"

// glowna dla FOX_eventloop.c

int FOX_PrivateExpose(void)
{
	int posted;

	FOX_Event events[32];		// !!! ostroznie

	// na razie wszystkie out
	
	FOX_PeepEvents(events, sizeof(events)/sizeof(events[0]),
	                    FOX_GETEVENT, FOX_VIDEOEXPOSEMASK);

	// pchnij 

	posted = 0;
	
	if ( FOX_ProcessEvents[FOX_VIDEOEXPOSE] == FOX_ENABLE ) 
	{
		FOX_Event event;
		event.type = FOX_VIDEOEXPOSE;

		if ( (FOX_EventOK == NULL) || (*FOX_EventOK)(&event) ) 
		{
			posted = 1;
			FOX_PushEvent(&event);
		}
	}
	return(posted);
}

// end
