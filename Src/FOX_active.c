// Fox v0.5
// by Jaroslaw Rozynski
//===
// *EVENTS*
//===
// TODO:

// focus, iconify itd. 

#include <stdio.h>
#include <string.h>

#include "FOX_events.h"
#include "FOX_events_c.h"

// statyczne, lapiemy sobie stan naszej aplikacji

static Uint8 FOX_appstate = 0;

// funkcje publiczne 

int FOX_AppActiveInit(void)
{
	
	// hmmm... tutaj mozna zmienic
	
	FOX_appstate = (FOX_APPACTIVE|FOX_APPINPUTFOCUS|FOX_APPMOUSEFOCUS);
	
	return(0);
}

// pobierz stan naszej aplikacji

Uint8 FOX_GetAppState(void)
{
	return(FOX_appstate);
}

// szczegoly w FOX_eventloop.c

int FOX_PrivateAppActive(Uint8 gain, Uint8 state)
{
	int posted;
	Uint8 new_state;

	// zmien stan uzywajac maski
	
	if ( gain ) 
	{
		new_state = (FOX_appstate | state);
	} 
	else 
	{
		new_state = (FOX_appstate & ~state);
	}

	// olewamy events ktore nie zmieniaja stanu

	if ( new_state == FOX_appstate ) 
	{
		return(0);
	}

	// aktualizacja stanu
	
	FOX_appstate = new_state;

	// pchnij event
	
	posted = 0;
	
	if ( FOX_ProcessEvents[FOX_ACTIVEEVENT] == FOX_ENABLE ) 
	{
		FOX_Event event;
		memset(&event, 0, sizeof(event));
		event.type = FOX_ACTIVEEVENT;
		event.active.gain = gain;
		event.active.state = state;

		if ( (FOX_EventOK == NULL) || (*FOX_EventOK)(&event) ) 
		{
			posted = 1;
			FOX_PushEvent(&event);
		}
	}

	// keyboard focus
	
	if ( (state & FOX_APPINPUTFOCUS) && !gain ) 
	{
		FOX_ResetKeyboard();
	}
	return(posted);
}

// end
