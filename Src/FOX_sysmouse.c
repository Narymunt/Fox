// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#include <stdlib.h>
#include <windows.h>

#include "FOX_error.h"
#include "FOX_mouse.h"
#include "FOX_sysmouse_c.h"
#include "FOX_events_c.h"
#include "cursor_c.h"
#include "FOX_lowvideo.h"

#define USE_STATIC_CURSOR	// to tylko dla wce, w razie czego usunac


HCURSOR	FOX_hcursor = NULL;		// export do FOX_eventloop.c

struct WMcursor {
	HCURSOR curs;
#ifndef USE_STATIC_CURSOR
	Uint8 *ands;
	Uint8 *xors;
#endif
};

#define PAD_BITS(bits)	((bits+7)/8)

#ifdef CURSOR_DEBUG
static void PrintBITMAP(FILE *out, char *bits, int w, int h)
{
	int i;
	unsigned char ch;

	while ( h-- > 0 ) 
	{
		for ( i=0; i<w; ++i ) 
		{
			if ( (i%8) == 0 )
				ch = *bits++;
			if ( ch&0x80 )
				fprintf(out, "X");
			else
				fprintf(out, " ");
			ch <<= 1;
		}
		fprintf(out, "\n");
	}
}
#endif

#ifndef USE_STATIC_CURSOR

static void memnot(Uint8 *dst, Uint8 *src, int len)
{
	while ( len-- > 0 )
		*dst++ = ~*src++;
}
static void memxor(Uint8 *dst, Uint8 *src1, Uint8 *src2, int len)
{
	while ( len-- > 0 )
		*dst++ = (*src1++)^(*src2++);
}
#endif /* !USE_STATIC_CURSOR */

void WIN_FreeWMCursor(_THIS, WMcursor *cursor)
{
#ifndef USE_STATIC_CURSOR
	if ( cursor->curs != NULL )
		DestroyCursor(cursor->curs);
	if ( cursor->ands != NULL )
		free(cursor->ands);
	if ( cursor->xors != NULL )
		free(cursor->xors);
#endif /* !USE_STATIC_CURSOR */
	free(cursor);
}

WMcursor *WIN_CreateWMCursor(_THIS,
		Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y)
{
#ifdef USE_STATIC_CURSOR
	WMcursor *cursor;

	// alokuj kursor
	cursor = (WMcursor *)malloc(sizeof(*cursor));
	
	if ( cursor ) 
	{
		cursor->curs = LoadCursor(NULL, IDC_ARROW);
	}
	return(cursor);
#else
	WMcursor *cursor;
	int allowed_x;
	int allowed_y;
	int run, pad, i;
	Uint8 *aptr, *xptr;
	
	allowed_x = GetSystemMetrics(SM_CXCURSOR);
	allowed_y = GetSystemMetrics(SM_CYCURSOR);
	
	if ( (w > allowed_x) || (h > allowed_y) ) 
	{
		FOX_SetError("[FOX]: za duzy kursor, max to:dx%d",
							allowed_x, allowed_y);
		return(NULL);
	}

	// alokuj kursor

	cursor = (WMcursor *)malloc(sizeof(*cursor));
	
	if ( cursor == NULL ) 
	{
		FOX_SetError("[FOX]: za malo pamieci");
		return(NULL);
	}
	cursor->curs = NULL;
	cursor->ands = NULL;
	cursor->xors = NULL;

	// zmien na normalny rozmiar
	
	run = PAD_BITS(w);
	pad = PAD_BITS(allowed_x)-run;
	aptr = cursor->ands = (Uint8 *)malloc((run+pad)*allowed_y);
	xptr = cursor->xors = (Uint8 *)malloc((run+pad)*allowed_y);

	if ( (aptr == NULL) || (xptr == NULL) ) 
	{
		WIN_FreeWMCursor(NULL, cursor);
		FOX_OutOfMemory();
		return(NULL);
	}
	
	for ( i=0; i<h; ++i ) 
	{
		memxor(xptr, data, mask, run);
		xptr += run;
		data += run;
		memnot(aptr, mask, run);
		mask += run;
		aptr += run;
		memset(xptr,  0, pad);
		xptr += pad;
		memset(aptr, ~0, pad);
		aptr += pad;
	}
	pad += run;
	
	for ( ; i<allowed_y; ++i ) 
	{
		memset(xptr,  0, pad);
		xptr += pad;
		memset(aptr, ~0, pad);
		aptr += pad;
	}

	// stworz kusor
	
	cursor->curs = CreateCursor(
			(HINSTANCE)GetWindowLong(FOX_Window, GWL_HINSTANCE),
					hot_x, hot_y, allowed_x, allowed_y, 
						cursor->ands, cursor->xors);
	
	if ( cursor->curs == NULL ) 
	{
		WIN_FreeWMCursor(NULL, cursor);
		FOX_SetError("[FOX]: windows nie moze stworzyc kursora myszy");
		return(NULL);
	}
	return(cursor);
#endif /* USE_STATIC_CURSOR */
}

int WIN_ShowWMCursor(_THIS, WMcursor *cursor)
{
	POINT mouse_pos;

	// kursor myszy w trybie fullscreen musi byc wyswietlony softwarewo
	
	if ( !this->screen || DDRAW_FULLSCREEN() ) 
	{
		return(0);
	}

	// ustaw kursor na nasz
	
	if ( cursor != NULL ) 
	{
		FOX_hcursor = cursor->curs;
	} 
	else 
	{
		FOX_hcursor = NULL;
	}
	GetCursorPos(&mouse_pos);
	
	if ( PtInRect(&FOX_bounds, mouse_pos) ) 
	{
		SetCursor(FOX_hcursor);
	}
	return(1);
}

void WIN_WarpWMCursor(_THIS, Uint16 x, Uint16 y)
{
	POINT pt;

	if ( DDRAW_FULLSCREEN() ) 
	{
		x += (this->screen->offset % this->screen->pitch) /
		      this->screen->format->BytesPerPixel;
		y += (this->screen->offset / this->screen->pitch);
		FOX_PrivateMouseMotion(0, 0, x, y);
	} else if ( mouse_relative) 
	{
		FOX_PrivateMouseMotion(0, 0, x, y);
	} 
	else 
	{
		pt.x = x;
		pt.y = y;
		ClientToScreen(FOX_Window, &pt);
		SetCursorPos(pt.x, pt.y);
	}
}

// aktualizuj aktualny stan myszy

void WIN_UpdateMouse(_THIS)
{
	RECT rect;
	POINT pt;

	if ( ! DDRAW_FULLSCREEN() ) 
	{
		GetClientRect(FOX_Window, &rect);
		GetCursorPos(&pt);
		MapWindowPoints(NULL, FOX_Window, &pt, 1);
		
		if (PtInRect(&rect, pt) && (WindowFromPoint(pt) == FOX_Window))
		{
			FOX_PrivateAppActive(1, FOX_APPMOUSEFOCUS);
			FOX_PrivateMouseMotion(0,0, (Sint16)pt.x, (Sint16)pt.y);
		} 
		else 
		{
			FOX_PrivateAppActive(0, FOX_APPMOUSEFOCUS);
		}
	}
}

// sprawdz czy pozycje sa relatywne

void WIN_CheckMouseMode(_THIS)
{
        // czy mysz jest ukryta
        
	if ( !(FOX_cursorstate & CURSOR_VISIBLE) &&
             (this->input_grab != FOX_GRAB_OFF) ) 
	{
                mouse_relative = 1;
        } 
	else 
	{
                mouse_relative = 0;
        }
}

// end
