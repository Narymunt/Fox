// Fox v0.5
// by Jaroslaw Rozynski
//===
// *EVENTS*
//===
// TODO:

// zmiana rozmiaru okna 

#include "FOX_events.h"
#include "FOX_events_c.h"
#include "FOX_sysvideo.h"

// trzymaj ostatni stan

static struct {
	int w;
	int h;
} last_resize;

// szczegoly w FOX_eventloop.c 

int FOX_PrivateResize(int w, int h)
{
	int posted;
	FOX_Event events[32];

	// zobacz czy przypadkiem nie zmieniono nam rozmiaru okna 
	
	if ( !w || !h ||
	     ((last_resize.w == w) && (last_resize.h == h)) ) 
	{
		return(0);
	}
        last_resize.w = w;
        last_resize.h = h;
	
		if ( ! FOX_VideoSurface ||
	     ((w == FOX_VideoSurface->w) && (h == FOX_VideoSurface->h)) ) 
		{
		return(0);
		}

	// hmm... a co z poprzednimi ?
	
	FOX_PeepEvents(events, sizeof(events)/sizeof(events[0]),
	                    FOX_GETEVENT, FOX_VIDEORESIZEMASK);

	// pchnij 
		
	posted = 0;
	
	if ( FOX_ProcessEvents[FOX_VIDEORESIZE] == FOX_ENABLE ) 
	{
		FOX_Event event;
		event.type = FOX_VIDEORESIZE;
		event.resize.w = w;
		event.resize.h = h;
	
		if ( (FOX_EventOK == NULL) || (*FOX_EventOK)(&event) ) 
		{
			posted = 1;
			FOX_PushEvent(&event);
		}
	}
	return(posted);
}

// end

