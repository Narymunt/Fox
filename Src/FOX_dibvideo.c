// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:
// - dib zwalnianie hw surface
// - dib blokowanie i odblokowanie hw surface 

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <windows.h>

// potrzebne ? 

#ifndef CDS_FULLSCREEN
	#define CDS_FULLSCREEN	4
#endif

#include "FOX.h"
#include "FOX_mutex.h"
#include "FOX_syswm.h"
#include "FOX_sysvideo.h"
#include "FOX_sysevents.h"
#include "FOX_events_c.h"
#include "FOX_pixels_c.h"
#include "FOX_dibvideo.h"
#include "FOX_syswm_c.h"
#include "FOX_sysmouse_c.h"
#include "FOX_dibevents_c.h"
#include "FOX_wingl_c.h"

// inicjalizacja

static int DIB_VideoInit(_THIS, FOX_PixelFormat *vformat);
static FOX_Rect **DIB_ListModes(_THIS, FOX_PixelFormat *format, Uint32 flags);
FOX_Surface *DIB_SetVideoMode(_THIS, FOX_Surface *current, int width, int height, int bpp, Uint32 flags);
static int DIB_SetColors(_THIS, int firstcolor, int ncolors,
			 FOX_Color *colors);
static void DIB_CheckGamma(_THIS);
static void DIB_SwapGamma(_THIS);
static void DIB_QuitGamma(_THIS);

#ifndef NO_GAMMA_SUPPORT
	static int DIB_SetGammaRamp(_THIS, Uint16 *ramp);
	static int DIB_GetGammaRamp(_THIS, Uint16 *ramp);
#endif

static void DIB_VideoQuit(_THIS);

// funckje do sprzetowej obslugi surface

static int DIB_AllocHWSurface(_THIS, FOX_Surface *surface);
static int DIB_LockHWSurface(_THIS, FOX_Surface *surface);
static void DIB_UnlockHWSurface(_THIS, FOX_Surface *surface);
static void DIB_FreeHWSurface(_THIS, FOX_Surface *surface);

// obsluga funkcji 

static void DIB_RealizePalette(_THIS);
static void DIB_PaletteChanged(_THIS, HWND window);
static void DIB_WinPAINT(_THIS, HDC hdc);

// pomocnicza

static int DIB_SussScreenDepth();

// driver ?

static int DIB_Available(void)
{
	return(1);
}

static void DIB_DeleteDevice(FOX_VideoDevice *device)
{
	if ( device ) 
	{
		if ( device->hidden ) 
		{
			free(device->hidden);
		}
		
		if ( device->gl_data ) 
		{
			free(device->gl_data);
		}
		free(device);
	}
}

static FOX_VideoDevice *DIB_CreateDevice(int devindex)
{
	FOX_VideoDevice *device;

	// inicjalizuj wszystkie zmienne
	
	device = (FOX_VideoDevice *)malloc(sizeof(FOX_VideoDevice));

	if ( device ) 
	{
		memset(device, 0, (sizeof *device));
		device->hidden = (struct FOX_PrivateVideoData *)
				malloc((sizeof *device->hidden));
		device->gl_data = (struct FOX_PrivateGLData *)
				malloc((sizeof *device->gl_data));
	}
	
	if ( (device == NULL) || (device->hidden == NULL) ||
		                 (device->gl_data == NULL) ) 
	{
		FOX_OutOfMemory();
		DIB_DeleteDevice(device);
		return(NULL);
	}
	
	memset(device->hidden, 0, (sizeof *device->hidden));
	memset(device->gl_data, 0, (sizeof *device->gl_data));

	// ustaw wskazniki
	
	device->VideoInit = DIB_VideoInit;
	device->ListModes = DIB_ListModes;
	device->SetVideoMode = DIB_SetVideoMode;
	device->UpdateMouse = WIN_UpdateMouse;
	device->SetColors = DIB_SetColors;
	device->UpdateRects = NULL;
	device->VideoQuit = DIB_VideoQuit;
	device->AllocHWSurface = DIB_AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = DIB_LockHWSurface;
	device->UnlockHWSurface = DIB_UnlockHWSurface;
	device->FlipHWSurface = NULL;
	device->FreeHWSurface = DIB_FreeHWSurface;

#ifndef NO_GAMMA_SUPPORT
	device->SetGammaRamp = DIB_SetGammaRamp;
	device->GetGammaRamp = DIB_GetGammaRamp;
#endif

	device->SetCaption = WIN_SetWMCaption;
	device->SetIcon = WIN_SetWMIcon;
	device->IconifyWindow = WIN_IconifyWindow;
	device->GrabInput = WIN_GrabInput;
	device->GetWMInfo = WIN_GetWMInfo;
	device->FreeWMCursor = WIN_FreeWMCursor;
	device->CreateWMCursor = WIN_CreateWMCursor;
	device->ShowWMCursor = WIN_ShowWMCursor;
	device->WarpWMCursor = WIN_WarpWMCursor;
	device->CheckMouseMode = WIN_CheckMouseMode;
	device->InitOSKeymap = DIB_InitOSKeymap;
	device->PumpEvents = DIB_PumpEvents;

	// ustaw przechwytywanie bledow

	WIN_RealizePalette = DIB_RealizePalette;
	WIN_PaletteChanged = DIB_PaletteChanged;
	WIN_SwapGamma = DIB_SwapGamma;
	WIN_WinPAINT = DIB_WinPAINT;
	HandleMessage = DIB_HandleMessage;

	device->free = DIB_DeleteDevice;
	
	return device;
}

VideoBootStrap WINDIB_bootstrap = {
	"windib", "Win95/98/NT/2000 GDI",
	DIB_Available, DIB_CreateDevice
};

#ifndef NO_CHANGEDISPLAYSETTINGS

static int cmpmodes(const void *va, const void *vb)
{
    FOX_Rect *a = *(FOX_Rect **)va;
    FOX_Rect *b = *(FOX_Rect **)vb;
    if(a->w > b->w)
        return -1;
    return b->h - a->h;
}

static int DIB_AddMode(_THIS, int bpp, int w, int h)
{
	FOX_Rect *mode;
	int i, index;
	int next_mode;

	// sprawdz czy juz to mamy
	
	if ( bpp < 8 ) 
	{ 
		return(0);
	}
	index = ((bpp+7)/8)-1;
	
	for ( i=0; i<FOX_nummodes[index]; ++i ) 
	{
		mode = FOX_modelist[index][i];
		if ( (mode->w == w) && (mode->h == h) ) 
		{
			return(0);
		}
	}
	
	mode = (FOX_Rect *)malloc(sizeof *mode);
	
	if ( mode == NULL ) 
	{
		FOX_OutOfMemory();
		return(-1);
	}
	mode->x = 0;
	mode->y = 0;
	mode->w = w;
	mode->h = h;

	// alokuj
	
	next_mode = FOX_nummodes[index];
	FOX_modelist[index] = (FOX_Rect **)
	       realloc(FOX_modelist[index], (1+next_mode+1)*sizeof(FOX_Rect *));
	
	if ( FOX_modelist[index] == NULL ) 
	{
		FOX_OutOfMemory();
		FOX_nummodes[index] = 0;
		free(mode);
		return(-1);
	}
	FOX_modelist[index][next_mode] = mode;
	FOX_modelist[index][next_mode+1] = NULL;
	FOX_nummodes[index]++;

	return(0);
}

#endif /* !NO_CHANGEDISPLAYSETTINGS */

static HPALETTE DIB_CreatePalette(int bpp)
{

	HPALETTE handle = NULL;
	
	if ( bpp <= 8 )
	{
		LOGPALETTE *palette;
		HDC hdc;
		int ncolors;
		int i;

		ncolors = 1;

		for ( i=0; i<bpp; ++i ) 
		{
			ncolors *= 2;
		}
		palette = (LOGPALETTE *)malloc(sizeof(*palette)+
					ncolors*sizeof(PALETTEENTRY));
		palette->palVersion = 0x300;
		palette->palNumEntries = ncolors;
		hdc = GetDC(FOX_Window);
		GetSystemPaletteEntries(hdc, 0, ncolors, palette->palPalEntry);
		ReleaseDC(FOX_Window, hdc);
		handle = CreatePalette(palette);
		free(palette);
	}
	
	return handle;
}

int DIB_VideoInit(_THIS, FOX_PixelFormat *vformat)
{
#ifndef NO_CHANGEDISPLAYSETTINGS
	int i;
	DEVMODE settings;
#endif

	// stworz okno

	if ( DIB_CreateWindow(this) < 0 ) 
	{
		return(-1);
	}
#ifndef DISABLE_AUDIO
	DX8_SoundFocus(FOX_Window);
#endif

	// sprawdz rozdzielczosc

	vformat->BitsPerPixel = DIB_SussScreenDepth();
	switch (vformat->BitsPerPixel) 
	{
		case 15:
			vformat->Rmask = 0x00007c00;
			vformat->Gmask = 0x000003e0;
			vformat->Bmask = 0x0000001f;
			vformat->BitsPerPixel = 16;
			break;
		case 16:
			vformat->Rmask = 0x0000f800;
			vformat->Gmask = 0x000007e0;
			vformat->Bmask = 0x0000001f;
			break;
		case 24:
		case 32:
	
			vformat->Rmask = 0x00ff0000;
			vformat->Gmask = 0x0000ff00;
			vformat->Bmask = 0x000000ff;
			break;
		default:
			break;
	}
	
	DIB_CheckGamma(this);

#ifndef NO_CHANGEDISPLAYSETTINGS

	for ( i=0; EnumDisplaySettings(NULL, i, &settings); ++i ) 
	{
		DIB_AddMode(this, settings.dmBitsPerPel,
			settings.dmPelsWidth, settings.dmPelsHeight);
	}
	
	// sortuj liste trybow
	
	for ( i=0; i<NUM_MODELISTS; ++i ) 
	{
		if ( FOX_nummodes[i] > 0 ) 
		{
			qsort(FOX_modelist[i], FOX_nummodes[i], sizeof *FOX_modelist[i], cmpmodes);
		}
	}
#endif /* !NO_CHANGEDISPLAYSETTINGS */

	if ( vformat->BitsPerPixel <= 8 ) 
	{
		screen_pal = DIB_CreatePalette(vformat->BitsPerPixel);
	}

	this->info.wm_available = 1;

	return(0);
}

FOX_Rect **DIB_ListModes(_THIS, FOX_PixelFormat *format, Uint32 flags)
{
#ifdef NO_CHANGEDISPLAYSETTINGS
	return((FOX_Rect **)-1);
#else
	if ( (flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
	{
		return(FOX_modelist[((format->BitsPerPixel+7)/8)-1]);
	} 
	else 
	{
		return((FOX_Rect **)-1);
	}
#endif
}

static int DIB_SussScreenDepth()
{
#ifdef NO_GETDIBITS
	int depth;
	HDC hdc;

	hdc = GetDC(FOX_Window);
	depth = GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(FOX_Window, hdc);

	if ( depth == 16 ) 
	{
		depth = 15;	
	}

	return(depth);
#else
    int dib_size;
    LPBITMAPINFOHEADER dib_hdr;
    HDC hdc;
    HBITMAP hbm;

    dib_size = sizeof(BITMAPINFOHEADER) + 256 * sizeof (RGBQUAD);
    dib_hdr = (LPBITMAPINFOHEADER) malloc(dib_size);
    memset(dib_hdr, 0, dib_size);
    dib_hdr->biSize = sizeof(BITMAPINFOHEADER);
    
    hdc = GetDC(NULL);
    hbm = CreateCompatibleBitmap( hdc, 1, 1 );

    GetDIBits(hdc, hbm, 0, 1, NULL, (LPBITMAPINFO) dib_hdr, DIB_RGB_COLORS);
    GetDIBits(hdc, hbm, 0, 1, NULL, (LPBITMAPINFO) dib_hdr, DIB_RGB_COLORS);
    DeleteObject(hbm);
    ReleaseDC(NULL, hdc);

    switch( dib_hdr->biBitCount )
    {
    case 8:     return 8;
    case 24:    return 24;
    case 32:    return 32;
    case 16:
        if( dib_hdr->biCompression == BI_BITFIELDS ) 
		{
                      switch( ((DWORD*)((char*)dib_hdr + dib_hdr->biSize))[0] ) 
			{
                case 0xf800: return 16;    /* 565 */
                case 0x7c00: return 15;    /* 555 */
            }
        }
    }
    return 0;
#endif /* NO_GETDIBITS */
}

static void DIB_NormalUpdate(_THIS, int numrects, FOX_Rect *rects);

FOX_Surface *DIB_SetVideoMode(_THIS, FOX_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
	FOX_Surface *video;
	Uint32 prev_flags;
	DWORD style;
	const DWORD directstyle =
			(WS_POPUP);
	const DWORD windowstyle = 
			(WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX);

	const DWORD resizestyle =
			(WS_THICKFRAME|WS_MAXIMIZEBOX);

	int binfo_size;
	BITMAPINFO *binfo;
	HDC hdc;
	RECT bounds;
	int x, y;
	BOOL was_visible;
	Uint32 Rmask, Gmask, Bmask;

	// wycentrowac ?
	
	was_visible = IsWindowVisible(FOX_Window);
	
	if ( bpp == current->format->BitsPerPixel ) 
	{
		video = current;
	} 
	else 
	{
		switch (bpp) {
			case 15:
			case 16:
				if ( DIB_SussScreenDepth() == 15 ) 
				{
					/* 5-5-5 */
					Rmask = 0x00007c00;
					Gmask = 0x000003e0;
					Bmask = 0x0000001f;
				} 
				else 
				{
					/* 5-6-5 */
					Rmask = 0x0000f800;
					Gmask = 0x000007e0;
					Bmask = 0x0000001f;
				}
				break;
			case 24:
			case 32:
				Rmask = 0x00ff0000;
				Gmask = 0x0000ff00;
				Bmask = 0x000000ff;
				break;
			default:
				Rmask = 0x00000000;
				Gmask = 0x00000000;
				Bmask = 0x00000000;
				break;
		}
		video = FOX_CreateRGBSurface(FOX_SWSURFACE,
					0, 0, bpp, Rmask, Gmask, Bmask, 0);
		if ( video == NULL ) 
		{
			FOX_OutOfMemory();
			return(NULL);
		}
	}
	
	prev_flags = video->flags;
	video->flags = 0;
	video->w = width;
	video->h = height;
	video->pitch = FOX_CalculatePitch(video);

#ifndef NO_CHANGEDISPLAYSETTINGS
	
	if ( (flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
	{
		DEVMODE settings;

		memset(&settings, 0, sizeof(DEVMODE));
		settings.dmSize = sizeof(DEVMODE);
		settings.dmBitsPerPel = video->format->BitsPerPixel;
		settings.dmPelsWidth = width;
		settings.dmPelsHeight = height;
		settings.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
		
		if ( ChangeDisplaySettings(&settings, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL ) 
		{
			video->flags |= FOX_FULLSCREEN;
		}
	}
#endif /* !NO_CHANGEDISPLAYSETTINGS */

	// reset ppalety
	
	if ( screen_pal != NULL ) 
	{
		DeleteObject(screen_pal);
		screen_pal = NULL;
	}

	if ( bpp <= 8 )
	{
		screen_pal = DIB_CreatePalette(bpp);
	}

	style = GetWindowLong(FOX_Window, GWL_STYLE);

	style &= ~(resizestyle|WS_MAXIMIZE);

	if ( (video->flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
	{
		style &= ~windowstyle;
		style |= directstyle;
	} 
	else 
	{
#ifndef NO_CHANGEDISPLAYSETTINGS
		if ( (prev_flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) 
		{
			ChangeDisplaySettings(NULL, 0);
		}
#endif
		if ( flags & FOX_NOFRAME ) 
		{
			style &= ~windowstyle;
			style |= directstyle;
			video->flags |= FOX_NOFRAME;
		} 
		else 
		{
			style &= ~directstyle;
			style |= windowstyle;
			if ( flags & FOX_RESIZABLE ) 
			{

				style |= resizestyle;

				video->flags |= FOX_RESIZABLE;
			}
		}

		if (IsZoomed(FOX_Window)) style |= WS_MAXIMIZE;

	}
   
   if (!FOX_windowid)
	   SetWindowLong(FOX_Window, GWL_STYLE, style);
	
	if ( screen_bmp != NULL ) 
	{
		DeleteObject(screen_bmp);
	}
	
	if ( ! (flags & FOX_OPENGL) ) 
	{
		BOOL is16bitmode = (video->format->BytesPerPixel == 2);
	
		binfo_size = sizeof(*binfo);
		if( is16bitmode ) 
		{
			// 16bpp
			binfo_size += 3*sizeof(DWORD);
		} 
		else 
			if ( video->format->palette ) 
			{
			binfo_size += video->format->palette->ncolors *
							sizeof(RGBQUAD);
		}
		binfo = (BITMAPINFO *)malloc(binfo_size);
		
		if ( ! binfo ) 
		{
			if ( video != current ) 
			{
				FOX_FreeSurface(video);
			}
			FOX_OutOfMemory();
			return(NULL);
		}

		binfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		binfo->bmiHeader.biWidth = video->w;
		binfo->bmiHeader.biHeight = -video->h;	
		binfo->bmiHeader.biPlanes = 1;
		binfo->bmiHeader.biSizeImage = video->h * video->pitch;
		binfo->bmiHeader.biXPelsPerMeter = 0;
		binfo->bmiHeader.biYPelsPerMeter = 0;
		binfo->bmiHeader.biClrUsed = 0;
		binfo->bmiHeader.biClrImportant = 0;
		binfo->bmiHeader.biBitCount = video->format->BitsPerPixel;

		if ( is16bitmode ) 
		{
			
			binfo->bmiHeader.biCompression = BI_BITFIELDS;
			((Uint32*)binfo->bmiColors)[0] = video->format->Rmask;
			((Uint32*)binfo->bmiColors)[1] = video->format->Gmask;
			((Uint32*)binfo->bmiColors)[2] = video->format->Bmask;
		} 
		else 
		{
			binfo->bmiHeader.biCompression = BI_RGB;	
			
			if ( video->format->palette ) 
			{
				memset(binfo->bmiColors, 0,
					video->format->palette->ncolors*sizeof(RGBQUAD));
			}
		}

		// stworz bufor
		
		hdc = GetDC(FOX_Window);
		screen_bmp = CreateDIBSection(hdc, binfo, DIB_RGB_COLORS,
					(void **)(&video->pixels), NULL, 0);
		ReleaseDC(FOX_Window, hdc);
		free(binfo);
		
		if ( screen_bmp == NULL ) 
		{
			if ( video != current ) 
			{
				FOX_FreeSurface(video);
			}
			FOX_SetError("[FOX]: nie moge stworzyc dib");
			return(NULL);
		}
		this->UpdateRects = DIB_NormalUpdate;

		if ( bpp <= 8 ) 
		{
			video->flags |= FOX_HWPALETTE;
		}
	}

	// zmien rozmiar okna
	
	if ( FOX_windowid == NULL ) 
	{
		UINT swp_flags;

		FOX_resizing = 1;
		bounds.left = 0;
		bounds.top = 0;
		bounds.right = video->w;
		bounds.bottom = video->h;

		AdjustWindowRect(&bounds, GetWindowLong(FOX_Window, GWL_STYLE), FALSE);

		width = bounds.right-bounds.left;
		height = bounds.bottom-bounds.top;
		x = (GetSystemMetrics(SM_CXSCREEN)-width)/2;
		y = (GetSystemMetrics(SM_CYSCREEN)-height)/2;
		
		if ( y < 0 ) 
		{ 
			y -= GetSystemMetrics(SM_CYCAPTION)/2;
		}

		swp_flags = (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW);

		if ( was_visible && !(flags & FOX_FULLSCREEN) ) 
		{
			swp_flags |= SWP_NOMOVE;
		}
		SetWindowPos(FOX_Window, NULL, x, y, width, height, swp_flags);
		FOX_resizing = 0;
		SetForegroundWindow(FOX_Window);
	}
	
	return(video);
}

// bez dostepu do hardware surface przez dib

static int DIB_AllocHWSurface(_THIS, FOX_Surface *surface)
{
	return(-1);
}

// tutaj dopisac 

static void DIB_FreeHWSurface(_THIS, FOX_Surface *surface)
{
	return;
}

static int DIB_LockHWSurface(_THIS, FOX_Surface *surface)
{
	return(0);
}

static void DIB_UnlockHWSurface(_THIS, FOX_Surface *surface)
{
	return;
}

static void DIB_NormalUpdate(_THIS, int numrects, FOX_Rect *rects)
{
	HDC hdc, mdc;
	int i;

	hdc = GetDC(FOX_Window);

	if ( screen_pal ) 
	{
		SelectPalette(hdc, screen_pal, FALSE);
	}
	mdc = CreateCompatibleDC(hdc);
	SelectObject(mdc, screen_bmp);
	
	for ( i=0; i<numrects; ++i ) 
	{
		BitBlt(hdc, rects[i].x, rects[i].y, rects[i].w, rects[i].h,
					mdc, rects[i].x, rects[i].y, SRCCOPY);
	}
	DeleteDC(mdc);
	ReleaseDC(FOX_Window, hdc);
}

int DIB_SetColors(_THIS, int firstcolor, int ncolors, FOX_Color *colors)
{
	RGBQUAD *pal;
	int i;

	HDC hdc, mdc;
	
	// aktualizuj palete
	
	hdc = GetDC(FOX_Window);
	if ( screen_pal ) 
	{
		PALETTEENTRY *entries;

		entries = (PALETTEENTRY *)alloca(ncolors*sizeof(PALETTEENTRY));
		for ( i=0; i<ncolors; ++i ) {
			entries[i].peRed   = colors[i].r;
			entries[i].peGreen = colors[i].g;
			entries[i].peBlue  = colors[i].b;

			entries[i].peFlags = PC_NOCOLLAPSE;
		}
		SetPaletteEntries(screen_pal, firstcolor, ncolors, entries);
		SelectPalette(hdc, screen_pal, FALSE);
		RealizePalette(hdc);
	}

	// kopia palety do palety dib
	
	pal = (RGBQUAD *)alloca(ncolors*sizeof(RGBQUAD));
	
	for ( i=0; i<ncolors; ++i ) 
	{
		pal[i].rgbRed = colors[i].r;
		pal[i].rgbGreen = colors[i].g;
		pal[i].rgbBlue = colors[i].b;
		pal[i].rgbReserved = 0;
	}

	// ustaw dib_palette

	mdc = CreateCompatibleDC(hdc);
	SelectObject(mdc, screen_bmp);
	SetDIBColorTable(mdc, firstcolor, ncolors, pal);
	BitBlt(hdc, 0, 0, this->screen->w, this->screen->h,
	       mdc, 0, 0, SRCCOPY);
	DeleteDC(mdc);

	ReleaseDC(FOX_Window, hdc);
	return(1);
}

static void DIB_CheckGamma(_THIS)
{
#ifndef NO_GAMMA_SUPPORT
	HDC hdc;
	WORD ramp[3*256];

	// jezeli nie uda sie ustawic gammy, zablokuj zupelnie gamma_control

	hdc = GetDC(FOX_Window);
	
	if ( ! GetDeviceGammaRamp(hdc, ramp) ) 
	{
		this->GetGammaRamp = NULL;
		this->SetGammaRamp = NULL;
	}
	ReleaseDC(FOX_Window, hdc);
#endif /* !NO_GAMMA_SUPPORT */
}
static void DIB_SwapGamma(_THIS)
{
#ifndef NO_GAMMA_SUPPORT
	HDC hdc;

	if ( gamma_saved ) 
	{
		hdc = GetDC(FOX_Window);
		if ( FOX_GetAppState() & FOX_APPINPUTFOCUS ) 
		{
			// zostaw aktualny stan, odtworz gamme
			SetDeviceGammaRamp(hdc, gamma_saved);
		} 
		else 
		{
			// przed wejsciem ustaw gamme
			GetDeviceGammaRamp(hdc, gamma_saved);
			SetDeviceGammaRamp(hdc, this->gamma);
		}
		ReleaseDC(FOX_Window, hdc);
	}
#endif /* !NO_GAMMA_SUPPORT */
}
static void DIB_QuitGamma(_THIS)
{
#ifndef NO_GAMMA_SUPPORT
	if ( gamma_saved ) 
	{
		// odtworz orginalna gamme jesli potrzebne
		
		if ( FOX_GetAppState() & FOX_APPINPUTFOCUS ) 
		{
			HDC hdc;

			hdc = GetDC(FOX_Window);
			SetDeviceGammaRamp(hdc, gamma_saved);
			ReleaseDC(FOX_Window, hdc);
		}

		// zwolnij zapisana pamiec gammy
		
		free(gamma_saved);
		gamma_saved = 0;
	}
#endif /* !NO_GAMMA_SUPPORT */
}

#ifndef NO_GAMMA_SUPPORT

static int DIB_SetGammaRamp(_THIS, Uint16 *ramp)
{
	HDC hdc;
	BOOL succeeded;

	// ustaw rampe
	
	if ( ! gamma_saved ) 
	{
		gamma_saved = (WORD *)malloc(3*256*sizeof(*gamma_saved));
		
		if ( ! gamma_saved ) 
		{
			FOX_OutOfMemory();
			return -1;
		}
		hdc = GetDC(FOX_Window);
		GetDeviceGammaRamp(hdc, gamma_saved);
		ReleaseDC(FOX_Window, hdc);
	}

	if ( FOX_GetAppState() & FOX_APPINPUTFOCUS ) 
	{
		hdc = GetDC(FOX_Window);
		succeeded = SetDeviceGammaRamp(hdc, ramp);
		ReleaseDC(FOX_Window, hdc);
	} 
	else 
	{
		succeeded = TRUE;
	}
	return succeeded ? 0 : -1;
}

static int DIB_GetGammaRamp(_THIS, Uint16 *ramp)
{
	HDC hdc;
	BOOL succeeded;
	
	// pobierz rampe z display
	
	hdc = GetDC(FOX_Window);
	succeeded = GetDeviceGammaRamp(hdc, ramp);
	ReleaseDC(FOX_Window, hdc);
	return succeeded ? 0 : -1;
}

#endif /* !NO_GAMMA_SUPPORT */

void DIB_VideoQuit(_THIS)
{
	
	// zamknij okno i zwolnij wszystko
	
	if ( FOX_Window ) 
	{
		// usun bitmape ekranu, screen->pixels) 
		if ( this->screen ) 
		{
#ifndef NO_CHANGEDISPLAYSETTINGS
			if ( this->screen->flags & FOX_FULLSCREEN ) 
			{
				ChangeDisplaySettings(NULL, 0);
			}
#endif
			this->screen->pixels = NULL;
		}
		
		if ( screen_bmp ) 
		{
			DeleteObject(screen_bmp);
			screen_bmp = NULL;
		}
		
		if ( screen_icn ) 
		{
			DestroyIcon(screen_icn);
			screen_icn = NULL;
		}
		DIB_QuitGamma(this);
		DIB_DestroyWindow(this);

		FOX_Window = NULL;
	}
}

// tylko dla petli okien

static void DIB_FocusPalette(_THIS, int foreground)
{
	if ( screen_pal != NULL ) 
	{
		HDC hdc;

		hdc = GetDC(FOX_Window);
		SelectPalette(hdc, screen_pal, FALSE);
		if ( RealizePalette(hdc) )
			InvalidateRect(FOX_Window, NULL, FALSE);
		ReleaseDC(FOX_Window, hdc);
	}
}

static void DIB_RealizePalette(_THIS)
{
	DIB_FocusPalette(this, 1);
}

static void DIB_PaletteChanged(_THIS, HWND window)
{
	if ( window != FOX_Window ) 
	{
		DIB_FocusPalette(this, 0);
	}
}

// wyeksportowane

static void DIB_WinPAINT(_THIS, HDC hdc)
{
	HDC mdc;

	if ( screen_pal ) 
	{
		SelectPalette(hdc, screen_pal, FALSE);
	}
	mdc = CreateCompatibleDC(hdc);
	SelectObject(mdc, screen_bmp);
	BitBlt(hdc, 0, 0, FOX_VideoSurface->w, FOX_VideoSurface->h,
							mdc, 0, 0, SRCCOPY);
	DeleteDC(mdc);
}

// w razie gdyby directx nie byl dostepny

#ifndef ENABLE_DIRECTX
void DX8_SoundFocus(HWND hwnd)
{
	return;
}
#endif

//end
