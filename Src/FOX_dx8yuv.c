// Fox v0.5
// by Jaroslaw Rozynski
//===
// *DIRECTX*
//===
// TODO:

// implementacja YUV na directdraw

#include <stdlib.h>
#include <string.h>

#include "FOX_error.h"
#include "FOX_video.h"
#include "FOX_dx8yuv.h"
#include "FOX_yuvfuncs.h"

#define USE_DIRECTX_OVERLAY

// funkcja do manipulowania 

static struct private_yuvhwfuncs dx8_yuvfuncs = {
	DX8_LockYUVOverlay,
	DX8_UnlockYUVOverlay,
	DX8_DisplayYUVOverlay,
	DX8_FreeYUVOverlay
};

struct private_yuvhwdata {
	LPDIRECTDRAWSURFACE3 surface;
	
	Uint16 pitches[3];
	Uint8 *planes[3];
};


static LPDIRECTDRAWSURFACE3 CreateYUVSurface(_THIS,
                                         int width, int height, Uint32 format)
{
	HRESULT result;
	LPDIRECTDRAWSURFACE  dd_surface1;
	LPDIRECTDRAWSURFACE3 dd_surface3;
	DDSURFACEDESC ddsd;

	// ustaw opis surface
	
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = (DDSD_WIDTH|DDSD_HEIGHT|DDSD_CAPS|DDSD_PIXELFORMAT);
	ddsd.dwWidth = width;
	ddsd.dwHeight= height;
#ifdef USE_DIRECTX_OVERLAY
	ddsd.ddsCaps.dwCaps = (DDSCAPS_OVERLAY|DDSCAPS_VIDEOMEMORY);
#else
	ddsd.ddsCaps.dwCaps = (DDSCAPS_OFFSCREENPLAIN|DDSCAPS_VIDEOMEMORY);
#endif
	ddsd.ddpfPixelFormat.dwSize = sizeof(ddsd.ddpfPixelFormat);
	ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
	ddsd.ddpfPixelFormat.dwFourCC = format;

	// stworz surface directdraw 
	
	result = IDirectDraw2_CreateSurface(ddraw2, &ddsd, &dd_surface1, NULL); 
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDraw2::CreateSurface", result);
		return(NULL);
	}
	
	result = IDirectDrawSurface_QueryInterface(dd_surface1,
			&IID_IDirectDrawSurface3, (LPVOID *)&dd_surface3);
	IDirectDrawSurface_Release(dd_surface1);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface::QueryInterface", result);
		return(NULL);
	}

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	result = IDirectDrawSurface3_Lock(dd_surface3, NULL,
					  &ddsd, DDLOCK_NOSYSLOCK, NULL);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::Lock", result);
		IDirectDrawSurface_Release(dd_surface3);
		return(NULL);
	}
	
	IDirectDrawSurface3_Unlock(dd_surface3, NULL);

	if ( !(ddsd.ddpfPixelFormat.dwFlags & DDPF_FOURCC) ||
	      (ddsd.ddpfPixelFormat.dwFourCC != format) ) 
	{
		FOX_SetError("[FOX]: DDraw nie moze ustawic wlasciwego formatu przy YUV");
		IDirectDrawSurface_Release(dd_surface3);
		return(NULL);
	}

	return(dd_surface3);
}

#ifdef DEBUG_YUV
static char *PrintFOURCC(Uint32 code)
{
	static char buf[5];

	buf[3] = code >> 24;
	buf[2] = (code >> 16) & 0xFF;
	buf[1] = (code >> 8) & 0xFF;
	buf[0] = (code & 0xFF);
	return(buf);
}
#endif

FOX_Overlay *DX8_CreateYUVOverlay(_THIS, int width, int height, Uint32 format, FOX_Surface *display)
{
	FOX_Overlay *overlay;
	struct private_yuvhwdata *hwdata;

#ifdef DEBUG_YUV
	DWORD numcodes;
	DWORD *codes;

	printf("FOURCC format requested: 0x%x\n", PrintFOURCC(format));
	IDirectDraw2_GetFourCCCodes(ddraw2, &numcodes, NULL);

	if ( numcodes ) 
	{
		DWORD i;
		codes = malloc(numcodes*sizeof(*codes));
		
		if ( codes ) 
		{
			IDirectDraw2_GetFourCCCodes(ddraw2, &numcodes, codes);
			for ( i=0; i<numcodes; ++i ) 
			{
				fprintf(stderr, "Code %d: 0x%x\n", i, PrintFOURCC(codes[i]));
			}
			free(codes);
		}
	} 
	else 
	{
		fprintf(stderr, "[FOX]: kod FOURCC nie obslugiwany\n");
	}
#endif

	// stworz strukture
	
	overlay = (FOX_Overlay *)malloc(sizeof *overlay);

	if ( overlay == NULL ) 
	{
		FOX_OutOfMemory();
		return(NULL);
	}
	memset(overlay, 0, (sizeof *overlay));
	
	overlay->format = format;
	overlay->w = width;
	overlay->h = height;

	// ustaw surface YUV
	
	overlay->hwfuncs = &dx8_yuvfuncs;

	// stworz tablice
	
	hwdata = (struct private_yuvhwdata *)malloc(sizeof *hwdata);
	overlay->hwdata = hwdata;
	
	if ( hwdata == NULL ) 
	{
		FOX_OutOfMemory();
		FOX_FreeYUVOverlay(overlay);
		return(NULL);
	}

	hwdata->surface = CreateYUVSurface(this, width, height, format);
	
	if ( hwdata->surface == NULL ) 
	{
		FOX_FreeYUVOverlay(overlay);
		return(NULL);
	}
	overlay->hw_overlay = 1;

	// ustaw wskazniki 
	
	overlay->pitches = hwdata->pitches;
	overlay->pixels = hwdata->planes;
	switch (format) {
	    case FOX_YV12_OVERLAY:
	    case FOX_IYUV_OVERLAY:
		overlay->planes = 3;
		break;
	    default:
		overlay->planes = 1;
		break;
	}
	
	return(overlay);
}

int DX8_LockYUVOverlay(_THIS, FOX_Overlay *overlay)
{
	HRESULT result;
	LPDIRECTDRAWSURFACE3 surface;
	DDSURFACEDESC ddsd;

	surface = overlay->hwdata->surface;
	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	result = IDirectDrawSurface3_Lock(surface, NULL,
					  &ddsd, DDLOCK_NOSYSLOCK, NULL);

	if ( result == DDERR_SURFACELOST ) 
	{
		result = IDirectDrawSurface3_Restore(surface);
		result = IDirectDrawSurface3_Lock(surface, NULL, &ddsd, 
					(DDLOCK_NOSYSLOCK|DDLOCK_WAIT), NULL);
	}
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::Lock", result);
		return(-1);
	}
	
#if defined(NONAMELESSUNION)
	overlay->pitches[0] = (Uint16)ddsd.u1.lPitch;
#else
	overlay->pitches[0] = (Uint16)ddsd.lPitch;
#endif
	overlay->pixels[0] = (Uint8 *)ddsd.lpSurface;
	switch (overlay->format) {
	    case FOX_YV12_OVERLAY:
	    case FOX_IYUV_OVERLAY:
		// dodaj dwa platy
		overlay->pitches[1] = overlay->pitches[0] / 2;
		overlay->pitches[2] = overlay->pitches[0] / 2;
	        overlay->pixels[1] = overlay->pixels[0] +
		                     overlay->pitches[0] * overlay->h;
	        overlay->pixels[2] = overlay->pixels[1] +
		                     overlay->pitches[1] * overlay->h / 2;
	        break;
	    default:
		// tylko jeden plan
		break;
	}
	return(0);
}

// nakladka yuv

void DX8_UnlockYUVOverlay(_THIS, FOX_Overlay *overlay)
{
	LPDIRECTDRAWSURFACE3 surface;

	surface = overlay->hwdata->surface;
	IDirectDrawSurface3_Unlock(surface, NULL);
}

int DX8_DisplayYUVOverlay(_THIS, FOX_Overlay *overlay, FOX_Rect *dstrect)
{
	HRESULT result;
	LPDIRECTDRAWSURFACE3 surface;
	RECT src, dst;

	surface = overlay->hwdata->surface;
	src.top = 0;
	src.bottom = overlay->h;
	src.left = 0;
	src.right = overlay->w;
	dst.top = FOX_bounds.top+dstrect->y;
	dst.left = FOX_bounds.left+dstrect->x;
	dst.bottom = dst.top+dstrect->h;
	dst.right = dst.left+dstrect->w;
#ifdef USE_DIRECTX_OVERLAY
	result = IDirectDrawSurface3_UpdateOverlay(surface, &src,
				FOX_primary, &dst, DDOVER_SHOW, NULL);
	
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::UpdateOverlay", result);
		return(-1);
	}
#else
	result = IDirectDrawSurface3_Blt(FOX_primary, &dst, surface, &src,
							DDBLT_WAIT, NULL);
	if ( result != DD_OK ) 
	{
		SetDDerror("DirectDrawSurface3::Blt", result);
		return(-1);
	}
#endif
	return(0);
}

void DX8_FreeYUVOverlay(_THIS, FOX_Overlay *overlay)
{
	struct private_yuvhwdata *hwdata;

	hwdata = overlay->hwdata;
	if ( hwdata ) 
	{
		if ( hwdata->surface ) 
		{
			IDirectDrawSurface_Release(hwdata->surface);
		}
		free(hwdata);
	}
}

// end

