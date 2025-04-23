// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#ifndef _FOX_dibvideo_h
#define _FOX_dibvideo_h

#include <windows.h>

// dane prywatne, wewnetrzne, nie ruszac !

struct FOX_PrivateVideoData {
    HBITMAP screen_bmp;
    HPALETTE screen_pal;

#define NUM_MODELISTS	4		// 8,16,24,32 bpp
    int FOX_nummodes[NUM_MODELISTS];
    FOX_Rect **FOX_modelist[NUM_MODELISTS];

    WORD *gamma_saved;
};

// stare nazwy zmiennych

#define screen_bmp		(this->hidden->screen_bmp)
#define screen_pal		(this->hidden->screen_pal)
#define FOX_nummodes		(this->hidden->FOX_nummodes)
#define FOX_modelist		(this->hidden->FOX_modelist)
#define gamma_saved		(this->hidden->gamma_saved)

#endif 

//end
