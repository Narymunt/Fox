// Fox v0.7
// by Jaroslaw Rozynski
//================================================================================================
// LAST UPDATE:
// 18 march 2002
//================================================================================================
// CHANGES:
// - usuniete definicje dla lock and unlock
// - kilka zmian przy rysowaniu kursora
// - zoptymalizowane rysowanie
// - nowe sprawdzanie blokowania ekranu
//================================================================================================
// TODO:
// - draw_cursor_slow usunac, jezeli nie potrzebne


// mapowanie kursora

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FOX_mutex.h"
#include "FOX_error.h"
#include "FOX_video.h"
#include "FOX_mouse.h"
#include "blit.h"
#include "FOX_events_c.h"
#include "FOX_sysvideo.h"
#include "FOX_sysevents.h"
#include "cursor_c.h"
#include "FOX_pixels_c.h"
#include "dcursor.h"

// statyczne 

volatile int FOX_cursorstate = CURSOR_VISIBLE;

FOX_Cursor *FOX_cursor = NULL;

static FOX_Cursor *FOX_defcursor = NULL;

FOX_mutex *FOX_cursorlock = NULL;

// publiczne, jako takie ;)

void FOX_CursorQuit(void)
{
	if ( FOX_cursor != NULL ) 
	{
		FOX_Cursor *cursor;

		FOX_cursorstate &= ~CURSOR_VISIBLE;
	
		if ( FOX_cursor != FOX_defcursor ) 	FOX_FreeCursor(FOX_cursor);
		
		FOX_cursor = NULL;
		
		if ( FOX_defcursor != NULL ) 
		{
			cursor = FOX_defcursor;
			FOX_defcursor = NULL;
			FOX_FreeCursor(cursor);
		}
	}
	
	if ( FOX_cursorlock != NULL ) 
	{
		FOX_DestroyMutex(FOX_cursorlock);
		FOX_cursorlock = NULL;
	}
}

// zainicjalizuj kursor

int FOX_CursorInit(Uint32 multithreaded)
{
	
	// nie ma jeszcze focus
	
	FOX_cursorstate = CURSOR_VISIBLE;

	// stworz domyslny kursor

	if ( FOX_defcursor == NULL ) 
	{
		FOX_defcursor = FOX_CreateCursor(default_cdata, default_cmask,
					DEFAULT_CWIDTH, DEFAULT_CHEIGHT,
						DEFAULT_CHOTX, DEFAULT_CHOTY);
		FOX_SetCursor(FOX_defcursor);
	}

	// zablokuj jezeli potrzebne
	
	if ( multithreaded ) FOX_cursorlock = FOX_CreateMutex();

	// wszystko

	return(0);
}

// na multiwatek

void FOX_LockCursor(void)
{
	if ( FOX_cursorlock ) FOX_mutexP(FOX_cursorlock);
}

void FOX_UnlockCursor(void)
{
	if ( FOX_cursorlock ) FOX_mutexV(FOX_cursorlock);
}

// rysujemy software

FOX_Cursor * FOX_CreateCursor (Uint8 *data, Uint8 *mask, 
					int w, int h, int hot_x, int hot_y)
{
	FOX_VideoDevice *video = current_video;
	int savelen;
	int i;
	FOX_Cursor *cursor;

	// szerokosc 

	w = ((w+7)&~7);

	// sprawdz hot spot
	
	if ( (hot_x < 0) || (hot_y < 0) || (hot_x >= w) || (hot_y >= h) ) 
	{
		FOX_SetError("[FOX]: goracy punkt kursora myszy poza bitmapa...");
		return(NULL);
	}

	// alokuj pamiec na kursor
	
	cursor = (FOX_Cursor *)malloc(sizeof *cursor);
	
	if ( cursor == NULL ) 
	{
		FOX_OutOfMemory();
		return(NULL);
	}
	savelen = (w*4)*h;
	cursor->area.x = 0;
	cursor->area.y = 0;
	cursor->area.w = w;
	cursor->area.h = h;
	cursor->hot_x = hot_x;
	cursor->hot_y = hot_y;
	cursor->data = (Uint8 *)malloc((w/8)*h*2);
	cursor->mask = cursor->data+((w/8)*h);
	cursor->save[0] = (Uint8 *)malloc(savelen*2);
	cursor->save[1] = cursor->save[0] + savelen;
	cursor->wm_cursor = NULL;
	
	if ( ! cursor->data || ! cursor->save[0] ) 
	{
		FOX_FreeCursor(cursor);
		FOX_OutOfMemory();
		return(NULL);
	}
	
	for ( i=((w/8)*h)-1; i>=0; --i ) 
	{
		cursor->data[i] = data[i];
		cursor->mask[i] = mask[i] | data[i];
	}
	
	memset(cursor->save[0], 0, savelen*2);
	
	if ( video->CreateWMCursor ) 
		cursor->wm_cursor = video->CreateWMCursor(video, data, mask, w, h, hot_x, hot_y);
	else 
		cursor->wm_cursor = NULL;
	
	return(cursor);
}

// set cursor

void FOX_SetCursor (FOX_Cursor *cursor)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;

	// czy napewno tryb graficzny?

	if ( ! video ) return;

	// blokuj
	
	FOX_LockCursor();

	// ustaw nowy kursor

	if ( cursor && (cursor != FOX_cursor) ) 
	{
	
		if ( SHOULD_DRAWCURSOR(FOX_cursorstate) ) FOX_EraseCursor(FOX_VideoSurface);
		else if ( video->MoveWMCursor && FOX_cursor ) 
		 video->ShowWMCursor(this, NULL);
			
		FOX_cursor = cursor;
	}

	// rysuj nowy kursor

	if ( FOX_cursor && (FOX_cursorstate&CURSOR_VISIBLE) ) 
	{
		// uzyj wm kursor jezeli mozna

		if ( FOX_cursor->wm_cursor && 
	             video->ShowWMCursor(this, FOX_cursor->wm_cursor) )
			FOX_cursorstate &= ~CURSOR_USINGSW;
		else 
		{
			FOX_cursorstate |= CURSOR_USINGSW;
			
			if ( video->ShowWMCursor ) video->ShowWMCursor(this, NULL);
			
			{ int x, y;
				FOX_GetMouseState(&x, &y);
				FOX_cursor->area.x = (x - FOX_cursor->hot_x);
				FOX_cursor->area.y = (y - FOX_cursor->hot_y);
			}
			FOX_DrawCursor(FOX_VideoSurface);
		}
	} 
	else 
	{
		
		if ( FOX_cursor && (FOX_cursorstate & CURSOR_USINGSW) ) 
			FOX_EraseCursor(FOX_VideoSurface);
		else if ( video ) 
			video->ShowWMCursor(this, NULL);
			
		
	}
	FOX_UnlockCursor();
}

// zwracaj wartosci

FOX_Cursor * FOX_GetCursor (void)
{
	return(FOX_cursor);
}

// zwolnij kursor

void FOX_FreeCursor (FOX_Cursor *cursor)
{
	if ( cursor ) 
	{
		if ( cursor == FOX_cursor ) FOX_SetCursor(FOX_defcursor);
		
		if ( cursor != FOX_defcursor ) 
		{
			FOX_VideoDevice *video = current_video;
			FOX_VideoDevice *this  = current_video;

			if ( cursor->data ) free(cursor->data);
			if ( cursor->save[0] ) free(cursor->save[0]);
			
			if ( video && cursor->wm_cursor ) video->FreeWMCursor(this, cursor->wm_cursor);
			
			free(cursor);
		}
	}
}

// pokaz kursor

int FOX_ShowCursor (int toggle)
{
	int showing;

	showing = (FOX_cursorstate & CURSOR_VISIBLE);
	
	if ( toggle >= 0 ) 
	{
		FOX_LockCursor();
		
		if ( toggle ) FOX_cursorstate |= CURSOR_VISIBLE;
		else FOX_cursorstate &= ~CURSOR_VISIBLE;
		
		FOX_UnlockCursor();
		
		if ( (FOX_cursorstate & CURSOR_VISIBLE) != showing ) 
		{
			FOX_VideoDevice *video = current_video;
			FOX_VideoDevice *this  = current_video;

			FOX_SetCursor(NULL);
		
			if ( video && video->CheckMouseMode ) video->CheckMouseMode(this);
			
		}
	} 
	else 
		fprintf(stderr,"[FOX]: showcursor! toggle < 0 "); // sprawdz, co teraz?
	
	return(showing ? 1 : 0);
}

// zmien ksztalt, raczej nie uzywac

void FOX_WarpMouse (Uint16 x, Uint16 y)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;

	// wygeneruj

	if ( video->WarpWMCursor ) video->WarpWMCursor(this, x, y);
	else 
	{
		x += (this->screen->offset % this->screen->pitch) /
		      this->screen->format->BytesPerPixel;
		y += (this->screen->offset / this->screen->pitch);
		FOX_PrivateMouseMotion(0, 0, x, y);
	}
}

// poruszenie kursora

void FOX_MoveCursor(int x, int y)
{
	FOX_VideoDevice *video = current_video;

	// usun i rysuj od nowa

	if ( SHOULD_DRAWCURSOR(FOX_cursorstate) ) 
	{
		FOX_LockCursor();
		FOX_EraseCursor(FOX_VideoSurface);
		FOX_cursor->area.x = (x - FOX_cursor->hot_x);
		FOX_cursor->area.y = (y - FOX_cursor->hot_y);
		FOX_DrawCursor(FOX_VideoSurface);
		FOX_UnlockCursor();
	} 
	else if ( video->MoveWMCursor ) video->MoveWMCursor(video, x, y);
	
}

// kolory 

static int palette_changed = 1;
static Uint32 pixels8[2];

void FOX_CursorPaletteChanged(void)
{
	palette_changed = 1;
}

// prostokat przcinajacy ksztalt myszy

void FOX_MouseRect(FOX_Rect *area)
{
	int clip_diff;

	*area = FOX_cursor->area;
	
	if ( area->x < 0 ) 
	{
		area->w += area->x;
		area->x = 0;
	}
	
	if ( area->y < 0 ) 
	{
		area->h += area->y;
		area->y = 0;
	}
	
	clip_diff = (area->x+area->w)-FOX_VideoSurface->w;
	
	if ( clip_diff > 0 ) area->w = area->w < clip_diff ? 0 : area->w-clip_diff;
	
	clip_diff = (area->y+area->h)-FOX_VideoSurface->h;
	
	if ( clip_diff > 0 ) area->h = area->h < clip_diff ? 0 : area->h-clip_diff;
	
}

// szybkie rysowanie kursora

static void FOX_DrawCursorFast(FOX_Surface *screen, FOX_Rect *area)
{
	const Uint32 pixels[2] = { 0xFFFFFFFF, 0x00000000 };
	int i, w, h;
	Uint8 *data, datab;
	Uint8 *mask, maskb;

	data = FOX_cursor->data + area->y * FOX_cursor->area.w/8;
	mask = FOX_cursor->mask + area->y * FOX_cursor->area.w/8;
	
	switch (screen->format->BytesPerPixel) 
	{

	    case 1: {
		Uint8 *dst;
		int dstskip;

		if ( palette_changed ) 
		{
			pixels8[0] = FOX_MapRGB(screen->format, 255, 255, 255);
			pixels8[1] = FOX_MapRGB(screen->format, 0, 0, 0);
			palette_changed = 0;
		}
		
		dst = (Uint8 *)screen->pixels +
                       (FOX_cursor->area.y+area->y)*screen->pitch +
                       FOX_cursor->area.x;
		
		dstskip = screen->pitch-area->w;

		for ( h=area->h; h; h-- ) 
		{
			for ( w=area->w/8; w; w-- ) 
			{
				maskb = *mask++;
				datab = *data++;
				for ( i=0; i<8; ++i ) 
				{
					if ( maskb & 0x80 ) *dst = pixels8[datab>>7];
					
					maskb <<= 1;
					datab <<= 1;
					dst++;
				}
			}
			dst += dstskip;
		}
	    }
	    break;

	    case 2: {
		Uint16 *dst;
		int dstskip;

		dst = (Uint16 *)screen->pixels +
                       (FOX_cursor->area.y+area->y)*screen->pitch/2 +
                       FOX_cursor->area.x;
		
		dstskip = (screen->pitch/2)-area->w;

		for ( h=area->h; h; h-- ) 
		{
			for ( w=area->w/8; w; w-- ) 
			{
				maskb = *mask++;
				datab = *data++;
				for ( i=0; i<8; ++i ) 
				{
					if ( maskb & 0x80 ) *dst = pixels[datab>>7];
					
					maskb <<= 1;
					datab <<= 1;
					dst++;
				}
			}
			dst += dstskip;
		}
	    }
	    break;

	    case 3: {
		Uint8 *dst;
		int dstskip;

		dst = (Uint8 *)screen->pixels +
                       (FOX_cursor->area.y+area->y)*screen->pitch +
                       FOX_cursor->area.x*3;
		
		dstskip = screen->pitch-area->w*3;

		for ( h=area->h; h; h-- ) 
		{
			for ( w=area->w/8; w; w-- ) 
			{
				maskb = *mask++;
				datab = *data++;
				for ( i=0; i<8; ++i ) 
				{
					if ( maskb & 0x80 ) memset(dst,pixels[datab>>7],3);
					
					maskb <<= 1;
					datab <<= 1;
					dst += 3;
				}
			}
			dst += dstskip;
		}
	    }
	    break;

	    case 4: {
		Uint32 *dst;
		int dstskip;

		dst = (Uint32 *)screen->pixels +
                       (FOX_cursor->area.y+area->y)*screen->pitch/4 +
                       FOX_cursor->area.x;
		
		dstskip = (screen->pitch/4)-area->w;

		for ( h=area->h; h; h-- ) 
		{
			for ( w=area->w/8; w; w-- ) 
			{
				maskb = *mask++;
				datab = *data++;
			
				for ( i=0; i<8; ++i ) 
				{
					if ( maskb & 0x80 ) *dst = pixels[datab>>7];
					maskb <<= 1;
					datab <<= 1;
					dst++;
				}
			}
			dst += dstskip;
		}
	    }
	    break;
	}
}

// wolne, rysowanie kursora

static void FOX_DrawCursorSlow(FOX_Surface *screen, FOX_Rect *area)
{
	const Uint32 pixels[2] = { 0xFFFFFF, 0x000000 };
	int h;
	int x, minx, maxx;
	Uint8 *data, datab = 0;
	Uint8 *mask, maskb = 0;
	Uint8 *dst;
	int dstbpp, dstskip;

	data = FOX_cursor->data + area->y * FOX_cursor->area.w/8;
	mask = FOX_cursor->mask + area->y * FOX_cursor->area.w/8;
	
	dstbpp = screen->format->BytesPerPixel;
	
	dst = (Uint8 *)screen->pixels +
                       (FOX_cursor->area.y+area->y)*screen->pitch +
                       FOX_cursor->area.x*dstbpp;
	
	dstskip = screen->pitch-FOX_cursor->area.w*dstbpp;

	minx = area->x;
	maxx = area->x+area->w;
	
	if ( screen->format->BytesPerPixel == 1 ) 
	{
		if ( palette_changed ) 
		{
			pixels8[0] = FOX_MapRGB(screen->format, 255, 255, 255);
			pixels8[1] = FOX_MapRGB(screen->format, 0, 0, 0);
			palette_changed = 0;
		}
		
		for ( h=area->h; h; h-- ) 
		{
			for ( x=0; x<FOX_cursor->area.w; ++x ) 
			{
				if ( (x%8) == 0 ) 
				{
					maskb = *mask++;
					datab = *data++;
				}
				
				if ((x >= minx)&&(x < maxx)&&(maskb&0x80)) 
					 memset(dst, pixels8[datab>>7], dstbpp);
				
				maskb <<= 1;
				datab <<= 1;
				dst += dstbpp;
			}
			dst += dstskip;
		}
	} 
	else 
	{
		for ( h=area->h; h; h-- ) 
		{
			for ( x=0; x<FOX_cursor->area.w; ++x ) 
			{
				if ( (x%8) == 0 ) 
				{
					maskb = *mask++;
					datab = *data++;
				}
				
				if ((x>=minx)&&(x<maxx)&&(maskb&0x80)) memset(dst, pixels[datab>>7], dstbpp);
			
				maskb <<= 1;
				datab <<= 1;
				dst += dstbpp;
			}
			dst += dstskip;
		}
	}
}

// handler

static void FOX_ConvertCursorSave(FOX_Surface *screen, int w, int h)
{
	FOX_BlitInfo info;
	FOX_loblit RunBlit;

	if ( screen->map->dst != FOX_VideoSurface ) return;

	// ustaw info dla blit

	info.s_pixels = FOX_cursor->save[1];
	info.s_width = w;
	info.s_height = h;
	info.s_skip = 0;
	info.d_pixels = FOX_cursor->save[0];
	info.d_width = w;
	info.d_height = h;
	info.d_skip = 0;
	info.aux_data = screen->map->sw_data->aux_data;
	info.src = screen->format;
	info.table = screen->map->table;
	info.dst = FOX_VideoSurface->format;
	RunBlit = screen->map->sw_data->blit;

	// blituj

	RunBlit(&info);
}

// postaw kursor na niezablokowanym

void FOX_DrawCursorNoLock(FOX_Surface *screen)
{
	FOX_Rect area;

	// pobierz prostokat obcinajacy dla kursora myszy, ale obcinaj go rozmiarem ekranu
	
	FOX_MouseRect(&area);

	if ( (area.w == 0) || (area.h == 0) ) return;
	
	// kopiuj tlo
	{ 
	  int w, h, screenbpp;
	  Uint8 *src, *dst;

	  // ustaw wskazniki

	  screenbpp = screen->format->BytesPerPixel;
	  
	  if ((screen == FOX_VideoSurface)||FORMAT_EQUAL(screen->format,FOX_VideoSurface->format)) 
		dst = FOX_cursor->save[0];
	  else 
		dst = FOX_cursor->save[1];
	  
	  src = (Uint8 *)screen->pixels + area.y * screen->pitch + area.x * screenbpp;

	  // ok, kopiowanie

	  w = area.w*screenbpp;
	  h = area.h;
	  
	  while ( h-- ) 
	  {
		  memcpy(dst, src, w);
		  dst += w;
		  src += screen->pitch;
	  }
	}

	// rysuj kursor

	area.x -= FOX_cursor->area.x;
	area.y -= FOX_cursor->area.y;
	
	if ( (area.x == 0) && (area.w == FOX_cursor->area.w) ) FOX_DrawCursorFast(screen, &area);
	else FOX_DrawCursorSlow(screen, &area);
	
}

// rysuj kursor

void FOX_DrawCursor(FOX_Surface *screen)
{
	// blokuj ekran jezeli konieczne 

	if ( screen == NULL ) return;
	
	if ( FOX_MUSTLOCK(screen) && (FOX_LockSurface(screen)<0)) return;

	FOX_DrawCursorNoLock(screen);

	// odblokuj screen i aktualizuj jezeli konieczne
	
	if ( FOX_MUSTLOCK(screen) ) FOX_UnlockSurface(screen);
	
	if ( (screen == FOX_VideoSurface) && ((screen->flags & FOX_HWSURFACE) != FOX_HWSURFACE) ) 
	{
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;
		FOX_Rect area;

		FOX_MouseRect(&area);

		// to mozna wywolac przed ustawienie trybu graficznego
		
		if ( video->UpdateRects ) video->UpdateRects(this, 1, &area);
		
	}
}

// usun kursor 

void FOX_EraseCursorNoLock(FOX_Surface *screen)
{
	FOX_Rect area;

	// pobierz rect
	
	FOX_MouseRect(&area);

	if ( (area.w == 0) || (area.h == 0) ) return;
	
	// kopiuj tlo 
	{
	 int w, h, screenbpp;
	 Uint8 *src, *dst;

	  // ustaw wskazniki do kopiowania
	  
	  screenbpp = screen->format->BytesPerPixel;
	  if ((screen==FOX_VideoSurface)||FORMAT_EQUAL(screen->format,FOX_VideoSurface->format) ) 
		src = FOX_cursor->save[0];
	  else src = FOX_cursor->save[1];
	  
	  dst = (Uint8 *)screen->pixels+area.y*screen->pitch+area.x * screenbpp;

	  // kopiowanie 
	  
	  w = area.w*screenbpp;
	  h = area.h;
	  
	  while ( h-- ) 
	  {
		  memcpy(dst, src, w);
		  src += w;
		  dst += screen->pitch;
	  }
	
	  // konwersja
	  
	  if ( src > FOX_cursor->save[1] ) 
		FOX_ConvertCursorSave(screen, area.w, area.h);
	  
	}
}

// wyczysc 

void FOX_EraseCursor(FOX_Surface *screen)
{
	
	// blokuj jezeli konieczne

	if ( screen == NULL ) return;
	
	if ( FOX_MUSTLOCK(screen) && (FOX_LockSurface(screen)<0)) return;

	FOX_EraseCursorNoLock(screen);

	// odblokuj i aktualizuj
	
	if ( FOX_MUSTLOCK(screen) ) FOX_UnlockSurface(screen);
	
	if ((screen==FOX_VideoSurface)&&((screen->flags&FOX_HWSURFACE)!=FOX_HWSURFACE) ) 
	{
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;
		FOX_Rect area;

		FOX_MouseRect(&area);
		if ( video->UpdateRects ) video->UpdateRects(this, 1, &area);
		
	}
}

// zresetuj ustawienia

void FOX_ResetCursor(void)
{
	int savelen;

	if ( FOX_cursor ) 
	{
		savelen = FOX_cursor->area.w*4*FOX_cursor->area.h;
		FOX_cursor->area.x = 0;
		FOX_cursor->area.y = 0;
		memset(FOX_cursor->save[0], 0, savelen);
	}
}

// end
