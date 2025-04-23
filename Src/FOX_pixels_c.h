// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// do blitowania itd

#include "blit.h"

// do formatu pixela

extern FOX_PixelFormat *FOX_AllocFormat(int bpp,
		Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern FOX_PixelFormat *FOX_ReallocFormat(FOX_Surface *surface, int bpp,
		Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern void FOX_FormatChanged(FOX_Surface *surface);
extern void FOX_FreeFormat(FOX_PixelFormat *format);

// mapowanie przy blitowaniu

extern FOX_BlitMap *FOX_AllocBlitMap(void);
extern void FOX_InvalidateMap(FOX_BlitMap *map);
extern int FOX_MapSurface (FOX_Surface *src, FOX_Surface *dst);
extern void FOX_FreeBlitMap(FOX_BlitMap *map);

// dodatkowo

extern Uint16 FOX_CalculatePitch(FOX_Surface *surface);
extern void FOX_DitherColors(FOX_Color *colors, int bpp);
extern Uint8 FOX_FindColor(FOX_Palette *pal, Uint8 r, Uint8 g, Uint8 b);
extern void FOX_ApplyGamma(Uint16 *gamma, FOX_Color *colors, FOX_Color *output, int ncolors);

// end
