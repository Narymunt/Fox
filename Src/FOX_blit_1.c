// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#include <stdio.h>

#include "FOX_types.h"
#include "FOX_video.h"
#include "blit.h"
#include "FOX_sysvideo.h"
#include "FOX_endian.h"

// funkcje do blitowanie 8bpp sf na inne

static void Blit1to1(FOX_BlitInfo *info)
{

#ifndef USE_DUFFS_LOOP
	int c;
#endif

	int width, height;

	Uint8 *src, *map, *dst;

	int srcskip, dstskip;

	// podstawowe ustawienia

	width = info->d_width;
	height = info->d_height;
	
	src = info->s_pixels;
	srcskip = info->s_skip;
	
	dst = info->d_pixels;
	dstskip = info->d_skip;
	
	map = info->table;

	while ( height-- ) 
	{

	#ifdef USE_DUFFS_LOOP
		DUFFS_LOOP(
			{
			  *dst = map[*src];
			}
			dst++;
			src++;
		, width);
	#else
		for ( c=width; c; --c ) 
		{
		    *dst = map[*src];
			dst++;
			src++;
		}
#endif
		src += srcskip;
		dst += dstskip;
	}
}

// zalezy od endian

#if ( FOX_BYTEORDER == FOX_LIL_ENDIAN )

	#define HI	1
	#define LO	0

#else /* ( FOX_BYTEORDER == FOX_BIG_ENDIAN ) */

	#define HI	0
	#define LO	1

#endif

static void Blit1to2(FOX_BlitInfo *info)
{

#ifndef USE_DUFFS_LOOP
	int c;
#endif

	int width, height;

	Uint8 *src, *dst;
	Uint16 *map;
	
	int srcskip, dstskip;

	// podstawowe ustawienia
		
	width = info->d_width;
	height = info->d_height;
	
	src = info->s_pixels;
	srcskip = info->s_skip;
	
	dst = info->d_pixels;
	dstskip = info->d_skip;
	
	map = (Uint16 *)info->table;

#ifdef USE_DUFFS_LOOP
	while ( height-- ) 
	{
		DUFFS_LOOP(
		{
			*(Uint16 *)dst = map[*src++];
			dst += 2;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
#else
	
	// jezeli sie da to align na 4 bajty

	if ( (long)dst & 0x03 ) 
	{
		if ( width == 0 ) 
		{
			return;
		}
		--width;

		while ( height-- ) 
		{
			*(Uint16 *)dst = map[*src++];
			dst += 2;
		
			// kopiuj po 4 bajty
			
			for ( c=width/4; c; --c ) 
			{
				*(Uint32 *)dst =
					(map[src[HI]]<<16)|(map[src[LO]]);
				src += 2;
				dst += 4;
				*(Uint32 *)dst =
					(map[src[HI]]<<16)|(map[src[LO]]);
				src += 2;
				dst += 4;
			}
			
			// zostalo cos ?
						
			switch (width & 3) 
			{
				case 3:
					*(Uint16 *)dst = map[*src++];
					dst += 2;
				case 2:
					*(Uint32 *)dst =
					  (map[src[HI]]<<16)|(map[src[LO]]);
					src += 2;
					dst += 4;
					break;
				case 1:
					*(Uint16 *)dst = map[*src++];
					dst += 2;
					break;
			}
			src += srcskip;
			dst += dstskip;
		}
	} 
	else 
	{
		while ( height-- ) 
		{
		
			// kopiuj jako 4 bajty 
			
			for ( c=width/4; c; --c ) 
			{
				*(Uint32 *)dst =
					(map[src[HI]]<<16)|(map[src[LO]]);
				src += 2;
				dst += 4;
				*(Uint32 *)dst =
					(map[src[HI]]<<16)|(map[src[LO]]);
				src += 2;
				dst += 4;
			}
			
			// cos pozostalo ?
			
			switch (width & 3) 
			{
				case 3:
					*(Uint16 *)dst = map[*src++];
					dst += 2;
				case 2:
					*(Uint32 *)dst =
					  (map[src[HI]]<<16)|(map[src[LO]]);
					src += 2;
					dst += 4;
					break;
				case 1:
					*(Uint16 *)dst = map[*src++];
					dst += 2;
					break;
			}
			src += srcskip;
			dst += dstskip;
		}
	}
#endif /* USE_DUFFS_LOOP */
}

static void Blit1to3(FOX_BlitInfo *info)
{
#ifndef USE_DUFFS_LOOP
	int c;
#endif

	int o;
	
	int width, height;
	
	Uint8 *src, *map, *dst;
	
	int srcskip, dstskip;

	// ustaw poczatkowe wartosci
	
	width = info->d_width;
	height = info->d_height;
	
	src = info->s_pixels;
	srcskip = info->s_skip;
	
	dst = info->d_pixels;
	dstskip = info->d_skip;
	
	map = info->table;

	while ( height-- ) 
	{
#ifdef USE_DUFFS_LOOP
		DUFFS_LOOP(
			{
				o = *src * 4;
				dst[0] = map[o++];
				dst[1] = map[o++];
				dst[2] = map[o++];
			}
			src++;
			dst += 3;
		, width);
#else
		for ( c=width; c; --c ) 
		{
			o = *src * 4;
			dst[0] = map[o++];
			dst[1] = map[o++];
			dst[2] = map[o++];
			src++;
			dst += 3;
		}
#endif /* USE_DUFFS_LOOP */
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1to4(FOX_BlitInfo *info)
{
#ifndef USE_DUFFS_LOOP
	int c;
#endif
	
	int width, height;
	
	Uint8 *src;
	Uint32 *map, *dst;
	
	int srcskip, dstskip;

	// ustaw poczatkowe wartosci
		
	width = info->d_width;
	height = info->d_height;
	
	src = info->s_pixels;
	srcskip = info->s_skip;
	
	dst = (Uint32 *)info->d_pixels;
	dstskip = info->d_skip/4;
	
	map = (Uint32 *)info->table;

	while ( height-- ) 
	{
#ifdef USE_DUFFS_LOOP
		DUFFS_LOOP(
			*dst++ = map[*src++];
		, width);
#else
		for ( c=width/4; c; --c ) 
		{
			*dst++ = map[*src++];
			*dst++ = map[*src++];
			*dst++ = map[*src++];
			*dst++ = map[*src++];
		}
		switch ( width & 3 ) 
		{
			case 3:
				*dst++ = map[*src++];
			case 2:
				*dst++ = map[*src++];
			case 1:
				*dst++ = map[*src++];
		}
#endif /* USE_DUFFS_LOOP */
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1to1Key(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	
	Uint8 *palmap = info->table;
	Uint32 ckey = info->src->colorkey;
        
	if ( palmap ) 
	{
		while ( height-- ) 
		{
			DUFFS_LOOP(
			{
				if ( *src != ckey ) 
				{
				  *dst = palmap[*src];
				}
				dst++;
				src++;
			},
			width);
			src += srcskip;
			dst += dstskip;
		}
	} else {
		while ( height-- ) 
		{
			DUFFS_LOOP(
			{
				if ( *src != ckey ) 
				{
				  *dst = *src;
				}
				dst++;
				src++;
			},
			width);
			src += srcskip;
			dst += dstskip;
		}
	}
}

static void Blit1to2Key(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	
	Uint16 *dstp = (Uint16 *)info->d_pixels;
	int dstskip = info->d_skip;
	
	Uint16 *palmap = (Uint16 *)info->table;
	
	Uint32 ckey = info->src->colorkey;

	// ustawienia poczatkowe 
	
	dstskip /= 2;

	while ( height-- ) 
	{
		DUFFS_LOOP(
		{
			if ( *src != ckey ) 
			{
				*dstp=palmap[*src];
			}
			src++;
			dstp++;
		},
		width);
		src += srcskip;
		dstp += dstskip;
	}
}

static void Blit1to3Key(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	
	Uint8 *palmap = info->table;
	
	Uint32 ckey = info->src->colorkey;
	
	int o;

	while ( height-- ) 
	{
		DUFFS_LOOP(
		{
			if ( *src != ckey ) 
			{
				o = *src * 4;
				dst[0] = palmap[o++];
				dst[1] = palmap[o++];
				dst[2] = palmap[o++];
			}
			src++;
			dst += 3;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1to4Key(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	
	Uint32 *dstp = (Uint32 *)info->d_pixels;
	int dstskip = info->d_skip;
	
	Uint32 *palmap = (Uint32 *)info->table;
	
	Uint32 ckey = info->src->colorkey;

	// ustawienia poczatkowe
	
	dstskip /= 4;

	while ( height-- ) 
	{
		DUFFS_LOOP(
		{
			if ( *src != ckey ) 
			{
				*dstp = palmap[*src];
			}
			src++;
			dstp++;
		},
		width);
		src += srcskip;
		dstp += dstskip;
	}
}

static void Blit1toNAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	
	FOX_PixelFormat *dstfmt = info->dst;
	
	const FOX_Color *srcpal	= info->src->palette->colors;
	
	int dstbpp;
	
	const int A = info->src->alpha;

	// ustawienia poczatkowe
	
	dstbpp = dstfmt->BytesPerPixel;

	while ( height-- ) 
	{
	        int sR, sG, sB;
		int dR, dG, dB;
	    	DUFFS_LOOP4(
			{
			        Uint32 pixel;
				sR = srcpal[*src].r;
				sG = srcpal[*src].g;
				sB = srcpal[*src].b;
				DISEMBLE_RGB(dst, dstbpp, dstfmt,
					     pixel, dR, dG, dB);
				ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
			  	ASSEMBLE_RGB(dst, dstbpp, dstfmt, dR, dG, dB);
				src++;
				dst += dstbpp;
			},
			width);
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1toNAlphaKey(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	
	const FOX_Color *srcpal	= info->src->palette->colors;
	Uint32 ckey = srcfmt->colorkey;
	int dstbpp;
	const int A = srcfmt->alpha;

	// ustawienia poczatkowe

	dstbpp = dstfmt->BytesPerPixel;

	while ( height-- ) 
	{
	        int sR, sG, sB;
		int dR, dG, dB;
		DUFFS_LOOP(
		{
			if ( *src != ckey ) 
			{
			        Uint32 pixel;
				sR = srcpal[*src].r;
				sG = srcpal[*src].g;
				sB = srcpal[*src].b;
				DISEMBLE_RGB(dst, dstbpp, dstfmt,
							pixel, dR, dG, dB);
				ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
			  	ASSEMBLE_RGB(dst, dstbpp, dstfmt, dR, dG, dB);
			}
			src++;
			dst += dstbpp;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

static FOX_loblit one_blit[] = {
	NULL, Blit1to1, Blit1to2, Blit1to3, Blit1to4
};

static FOX_loblit one_blitkey[] = {
        NULL, Blit1to1Key, Blit1to2Key, Blit1to3Key, Blit1to4Key
};

FOX_loblit FOX_CalculateBlit1(FOX_Surface *surface, int blit_index)
{
	int which;
	FOX_PixelFormat *dstfmt;

	dstfmt = surface->map->dst->format;
	if ( dstfmt->BitsPerPixel < 8 ) 
	{
		which = 0;
	} 
	else 
	{
		which = dstfmt->BytesPerPixel;
	}
	
	switch(blit_index) 
	{
	case 0:			// kopiuj
	    return one_blit[which];

	case 1:			// ck
	    return one_blitkey[which];

	case 2:			// alpha
	
	    return which >= 2 ? Blit1toNAlpha : NULL;

	case 3:			// alpha + colorkey 
	    return which >= 2 ? Blit1toNAlphaKey : NULL;

	}
	return NULL;
}

// end

