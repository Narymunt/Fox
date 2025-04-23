// Fox v0.5
// by Jaroslaw Rozynski
//===
// *EVENTS*
//===
// TODO:

// obsluga myszy

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FOX_events.h"
#include "FOX_events_c.h"
#include "FOX_cursor_c.h"
#include "FOX_sysvideo.h"

// statyczne, wypelniane przez handlery

static Sint16 FOX_MouseX = 0;
static Sint16 FOX_MouseY = 0;
static Sint16 FOX_DeltaX = 0;
static Sint16 FOX_DeltaY = 0;
static Uint8  FOX_ButtonState = 0;

// publiczne funkcje 

int FOX_MouseInit(void)
{
	
	FOX_MouseX = 0;
	FOX_MouseY = 0;
	FOX_DeltaX = 0;
	FOX_DeltaY = 0;
	FOX_ButtonState = 0;
	
	return(0);
}

// pobierz stan myszy, x, y i stan  

Uint8 FOX_GetMouseState (int *x, int *y)
{
	if ( x )
		*x = FOX_MouseX;
	if ( y )
		*y = FOX_MouseY;
	return(FOX_ButtonState);
}

// pobierz relatywny stan myszy czyli roznice od ostatniego stanu

Uint8 FOX_GetRelativeMouseState (int *x, int *y)
{
	if ( x )
		*x = FOX_DeltaX;
	if ( y )
		*y = FOX_DeltaY;
	FOX_DeltaX = 0;
	FOX_DeltaY = 0;
	return(FOX_ButtonState);
}

// obcinanie 

static void ClipOffset(Sint16 *x, Sint16 *y)
{

	if ( FOX_VideoSurface->offset ) 
	{
		*y -= FOX_VideoSurface->offset/FOX_VideoSurface->pitch;
		*x -= (FOX_VideoSurface->offset%FOX_VideoSurface->pitch)/
				FOX_VideoSurface->format->BytesPerPixel;
	}

}

// to dla FOX_eventloop.c 

int FOX_PrivateMouseMotion(Uint8 buttonstate, int relative, Sint16 x, Sint16 y)
{
	int posted;
	Uint16 X, Y;
	Sint16 Xrel;
	Sint16 Yrel;

	// jezeli nie ma kursora nie obslugujemy zdarzen myszy
	
	if ( FOX_VideoSurface == NULL ) 
	{
		return(0);
	}

	// stan klawiszy

	if ( ! buttonstate ) 
	{
		buttonstate = FOX_ButtonState;
	}

	Xrel = x;
	Yrel = y;
	
	if ( relative ) 
	{
		x = (FOX_MouseX+x);
		y = (FOX_MouseY+y);
	} 
	else 
	{
		// moze przyciac ? ;) 
		ClipOffset(&x, &y);
	}

	// podaj wspolrzedne myszy
	
	if ( x < 0 )
		X = 0;
	else
	if ( x >= FOX_VideoSurface->w )
		X = FOX_VideoSurface->w-1;
	else
		X = (Uint16)x;

	if ( y < 0 )
		Y = 0;
	else
	if ( y >= FOX_VideoSurface->h )
		Y = FOX_VideoSurface->h-1;
	else
		Y = (Uint16)y;

	// jezeli nie relatywnie to normalnie 
	
	if ( ! relative ) 
	{
		Xrel = X-FOX_MouseX;
		Yrel = Y-FOX_MouseY;
	}

	// aktualizuj sobie 
		
	FOX_ButtonState = buttonstate;
	FOX_MouseX = X;
	FOX_MouseY = Y;
	FOX_DeltaX += Xrel;
	FOX_DeltaY += Yrel;
	FOX_MoveCursor(FOX_MouseX, FOX_MouseY);

	// zdarzenie
	
	posted = 0;

	if ( FOX_ProcessEvents[FOX_MOUSEMOTION] == FOX_ENABLE ) 
	{
		FOX_Event event;
		memset(&event, 0, sizeof(event));
		event.type = FOX_MOUSEMOTION;
		event.motion.state = buttonstate;
		event.motion.x = X;
		event.motion.y = Y;
		event.motion.xrel = Xrel;
		event.motion.yrel = Yrel;
		if ( (FOX_EventOK == NULL) || (*FOX_EventOK)(&event) ) 
		{
			posted = 1;
			FOX_PushEvent(&event);
		}
	}
	return(posted);
}

// hmmm... przyciski 

int FOX_PrivateMouseButton(Uint8 state, Uint8 button, Sint16 x, Sint16 y)
{
	
	FOX_Event event;
	
	int posted;
	int move_mouse;
	
	Uint8 buttonstate;

	memset(&event, 0, sizeof(event));

	// sprawdz parametry 
		
	if ( x || y ) 
	{
		ClipOffset(&x, &y);
		move_mouse = 1;
	
		if ( x < 0 )
			x = 0;
		else
		if ( x >= FOX_VideoSurface->w )
			x = FOX_VideoSurface->w-1;

		if ( y < 0 )
			y = 0;
		else
		if ( y >= FOX_VideoSurface->h )
			y = FOX_VideoSurface->h-1;
	} 
	else 
	{
		move_mouse = 0;
	}
	
	if ( ! x )
		x = FOX_MouseX;
	
	if ( ! y )
		y = FOX_MouseY;

	// ktore zdarzenie ? 
	
	buttonstate = FOX_ButtonState;
	
	switch ( state ) 
	{
		case FOX_PRESSED:
			event.type = FOX_MOUSEBUTTONDOWN;
			buttonstate |= FOX_BUTTON(button);
			break;
		
		case FOX_RELEASED:
			event.type = FOX_MOUSEBUTTONUP;
			buttonstate &= ~FOX_BUTTON(button);
			break;
		
		default:
			// to by bylo dosyc ciekawe ;)
			return(0);
	}

	// aktualizuj stan myszy
	
	FOX_ButtonState = buttonstate;

	if ( move_mouse ) 
	{
		FOX_MouseX = x;
		FOX_MouseY = y;
		FOX_MoveCursor(FOX_MouseX, FOX_MouseY);
	}

	// zdarzenie 
	
	posted = 0;

	if ( FOX_ProcessEvents[event.type] == FOX_ENABLE ) 
	{
		event.button.state = state;
		event.button.button = button;
		event.button.x = x;
		event.button.y = y;
		
		if ( (FOX_EventOK == NULL) || (*FOX_EventOK)(&event) ) 
		{
			posted = 1;
			FOX_PushEvent(&event);
		}
	}
	return(posted);
}

// end
