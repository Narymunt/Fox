// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#include <stdio.h>
#include <malloc.h>
#include <windows.h>

#include "FOX_version.h"
#include "FOX_error.h"
#include "FOX_video.h"
#include "FOX_syswm.h"
#include "FOX_syswm_c.h"
#include "FOX_pixels_c.h"

//#ifdef _WIN32_WCE
//#define DISABLE_ICON_SUPPORT
//#endif

#include "cursor_c.h"

// ikonka ekranu

HICON   screen_icn = NULL;

void WIN_SetWMIcon(_THIS, FOX_Surface *icon, Uint8 *mask)
{
#ifdef DISABLE_ICON_SUPPORT
	return;
#else
	FOX_Palette *pal_256;
	FOX_Surface *icon_256;
	Uint8 *pdata, *pwin32;
	Uint8 *mdata, *mwin32, m = 0;
	int icon_len;
	int icon_plen;
	int icon_mlen;
	int icon_pitch;
	int mask_pitch;
	FOX_Rect bounds;
	int i, skip;
	int row, col;
	struct /* quasi-BMP format */ Win32Icon {
		Uint32 biSize;
		Sint32 biWidth;
		Sint32 biHeight;
		Uint16 biPlanes;
		Uint16 biBitCount;
		Uint32 biCompression;
		Uint32 biSizeImage;
		Sint32 biXPelsPerMeter;
		Sint32 biYPelsPerMeter;
		Uint32 biClrUsed;
		Uint32 biClrImportant;
		struct /* RGBQUAD -- note it's BGR ordered */ {
			Uint8 rgbBlue;
			Uint8 rgbGreen;
			Uint8 rgbRed;
			Uint8 rgbReserved;
		} biColors[256];
		/* Pixels:
		Uint8 pixels[]
		*/
		/* Mask:
		Uint8 mask[]
		*/
	} *icon_win32;
	
	// alokacja 
	
	icon_pitch = ((icon->w+3)&~3);
	mask_pitch = ((icon->w+7)/8);
	icon_plen = icon->h*icon_pitch;
	icon_mlen = icon->h*mask_pitch;
	icon_len = sizeof(*icon_win32)+icon_plen+icon_mlen;
	icon_win32 = (struct Win32Icon *)alloca(icon_len);

	if ( icon_win32 == NULL ) 
	{
		return;
	}
	memset(icon_win32, 0, icon_len);

	// ustaw podstawowe parametry bmp
	
	icon_win32->biSize = sizeof(*icon_win32)-sizeof(icon_win32->biColors);
	icon_win32->biWidth = icon->w;
	icon_win32->biHeight = icon->h*2;
	icon_win32->biPlanes = 1;
	icon_win32->biBitCount = 8;
	icon_win32->biSizeImage = icon_plen+icon_mlen;

	// ustaw 256 color icon
	
	icon_256 = FOX_CreateRGBSurface(FOX_SWSURFACE, icon->w, icon->h,
					 icon_win32->biBitCount, 0, 0, 0, 0);
	if ( icon_256 == NULL ) 
	{
		return;
	}
	pal_256 = icon_256->format->palette;
	
	if (icon->format->palette && 
		(icon->format->BitsPerPixel == icon_256->format->BitsPerPixel))
	{
		Uint8 black;
		memcpy(pal_256->colors, icon->format->palette->colors,
					pal_256->ncolors*sizeof(FOX_Color));
		
		// na pewno 0 jest czarny?
		
		black = FOX_FindColor(pal_256, 0x00, 0x00, 0x00);
		pal_256->colors[black] = pal_256->colors[0];
		pal_256->colors[0].r = 0x00;
		pal_256->colors[0].g = 0x00;
		pal_256->colors[0].b = 0x00;
	} 
	else 
	{
		FOX_DitherColors(pal_256->colors,
					icon_256->format->BitsPerPixel);
	}

	// teraz kopiuj kolorowe dane na ikonke bmp
	
	for ( i=0; i<(1<<icon_win32->biBitCount); ++i ) 
	{
		icon_win32->biColors[i].rgbRed = pal_256->colors[i].r;
		icon_win32->biColors[i].rgbGreen = pal_256->colors[i].g;
		icon_win32->biColors[i].rgbBlue = pal_256->colors[i].b;
	}

	// konwertuj ikone na surface
	
        bounds.x = 0;
        bounds.y = 0;
        bounds.w = icon->w;
        bounds.h = icon->h;

        if ( FOX_LowerBlit(icon, &bounds, icon_256, &bounds) < 0 ) 
		{
		FOX_FreeSurface(icon_256);
                return;
	}

	// kopiuj pixele od dolu (bmp)
	
	if ( FOX_MUSTLOCK(icon_256) || (icon_256->pitch != icon_pitch) ) 
	{
		FOX_FreeSurface(icon_256);
		FOX_SetError("[FOX]: nie spodziewany icon_256 w kursorze myszy");
		return;
	}
	pdata = (Uint8 *)icon_256->pixels;
	mdata = mask;
	pwin32 = (Uint8 *)icon_win32+sizeof(*icon_win32)+icon_plen-icon_pitch;
	skip = icon_pitch - icon->w;

	for ( row=0; row<icon->h; ++row ) 
	{
		for ( col=0; col<icon->w; ++col ) 
		{
			if ( (col%8) == 0 ) 
			{
				m = *mdata++;
			}
			
			if ( (m&0x80) != 0x00 ) 
			{
				*pwin32 = *pdata;
			}
			m <<= 1;
			++pdata;
			++pwin32;
		}
		pdata  += skip;
		pwin32 += skip;
		pwin32 -= 2*icon_pitch;
	}
	FOX_FreeSurface(icon_256);

	// od dolu <bmp>

	mdata = mask;
	mwin32 = (Uint8 *)icon_win32
			+sizeof(*icon_win32)+icon_plen+icon_mlen-mask_pitch;
	for ( row=0; row<icon->h; ++row ) 
	{
		for ( col=0; col<mask_pitch; ++col ) 
		{
			*mwin32++ = ~*mdata++;
		}
		mwin32 -= 2*mask_pitch;
	}

	// stworz 
	
	screen_icn = CreateIconFromResourceEx((Uint8 *)icon_win32, icon_len,
			TRUE, 0x00030000, icon->w, icon->h, LR_DEFAULTCOLOR);
	
	if ( screen_icn == NULL ) 
	{
		FOX_SetError("[FOX]: nie mozna stworzy kursora");
	} else {
		SetClassLong(FOX_Window, GCL_HICON, (LONG)screen_icn);
	}
#endif /* DISABLE_ICON_SUPPORT */
}

void WIN_SetWMCaption(_THIS, const char *title, const char *icon)
{
#ifdef _WIN32_WCE
	/* WinCE uses the UNICODE version */
	int nLen = strlen(title)+1;
	LPWSTR lpszW = alloca(nLen*2);
	MultiByteToWideChar(CP_ACP, 0, title, -1, lpszW, nLen);
	SetWindowText(FOX_Window, lpszW);
#else
	SetWindowText(FOX_Window, title);
#endif
}

int WIN_IconifyWindow(_THIS)
{
	ShowWindow(FOX_Window, SW_MINIMIZE);
	return(1);
}

FOX_GrabMode WIN_GrabInput(_THIS, FOX_GrabMode mode)
{
	if ( mode == FOX_GRAB_OFF ) 
	{
		ClipCursor(NULL);

		if ( !(FOX_cursorstate & CURSOR_VISIBLE) ) 
		{
			POINT pt;
			int x, y;
			FOX_GetMouseState(&x,&y);
			pt.x = x;
			pt.y = y;
			ClientToScreen(FOX_Window, &pt);
			SetCursorPos(pt.x,pt.y);
		}
	} 
	else 
	{
		ClipCursor(&FOX_bounds);
		if ( !(FOX_cursorstate & CURSOR_VISIBLE) ) 
		{
			POINT pt;
			pt.x = (FOX_VideoSurface->w/2);
			pt.y = (FOX_VideoSurface->h/2);
			ClientToScreen(FOX_Window, &pt);
			SetCursorPos(pt.x, pt.y);
		}
	}
	return(mode);
}

// jezeli info jest wlasciwe, zwraca 1, jezeli nie to -1

int WIN_GetWMInfo(_THIS, FOX_SysWMinfo *info)
{
	if ( info->version.major <= FOX_MAJOR_VERSION ) 
	{
		info->window = FOX_Window;
		return(1);
	} 
	else 
	{
		FOX_SetError("[FOX]: aplikacja nie kompiluje sie z FOX %d.%d\n",
					FOX_MAJOR_VERSION, FOX_MINOR_VERSION);
		return(-1);
	}
}

// end
