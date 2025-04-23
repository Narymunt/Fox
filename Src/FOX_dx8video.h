// Fox v0.5
// by Jaroslaw Rozynski
//===
// *DIRECTX*
//===
// TODO:

#ifndef _FOX_dx8video_h
#define _FOX_dx8video_h

#include "directx.h"

// prywatne dane

struct FOX_PrivateVideoData {
    LPDIRECTDRAW2 ddraw2;
    LPDIRECTDRAWSURFACE3 FOX_primary;
    LPDIRECTDRAWCLIPPER FOX_clipper;
    LPDIRECTDRAWPALETTE FOX_palette;
    PALETTEENTRY FOX_colors[256];
    int colorchange_expected;

#define NUM_MODELISTS	4		// 8, 16, 24, 32 bps
    int FOX_nummodes[NUM_MODELISTS];
    FOX_Rect **FOX_modelist[NUM_MODELISTS];
    int FOX_modeindex[NUM_MODELISTS];
};

// stare nazwy zmiennych

#define ddraw2					(this->hidden->ddraw2)
#define FOX_primary				(this->hidden->FOX_primary)
#define FOX_clipper				(this->hidden->FOX_clipper)
#define FOX_palette				(this->hidden->FOX_palette)
#define FOX_colors				(this->hidden->FOX_colors)
#define colorchange_expected	(this->hidden->colorchange_expected)
#define FOX_nummodes			(this->hidden->FOX_nummodes)
#define FOX_modelist			(this->hidden->FOX_modelist)
#define FOX_modeindex			(this->hidden->FOX_modeindex)

// wskazniki directx do funkcji

extern HRESULT (WINAPI *DDrawCreate)( GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter );
extern HRESULT (WINAPI *DInputCreate)(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA *ppDI, LPUNKNOWN punkOuter);

// directdraw error

extern void SetDDerror(const char *function, int code);

#endif 

// end


