// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// stawianie pixeli

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FOX_error.h"
#include "FOX_endian.h"
#include "FOX_video.h"
#include "FOX_sysvideo.h"
#include "blit.h"
#include "FOX_pixels_c.h"
#include "FOX_RLEaccel_c.h"

// pomocnicze

FOX_PixelFormat *FOX_AllocFormat(int bpp,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	FOX_PixelFormat *format;
	Uint32 mask;

	// zaalokuj pusta strukture
	format = malloc(sizeof(*format));
	
	if ( format == NULL ) 
	{
		FOX_OutOfMemory();
		return(NULL);
	}
	memset(format, 0, sizeof(*format));
	format->alpha = FOX_ALPHA_OPAQUE;

	// ustaw format 
	
	format->BitsPerPixel = bpp;
	format->BytesPerPixel = (bpp+7)/8;
	switch (bpp) {
		case 1:
			
			format->palette = (FOX_Palette *)malloc(
							sizeof(FOX_Palette));
			
			if ( format->palette == NULL ) 
			{
				FOX_FreeFormat(format);
				FOX_OutOfMemory();
				return(NULL);
			}
			(format->palette)->ncolors = 2;
			(format->palette)->colors = (FOX_Color *)malloc(
				(format->palette)->ncolors*sizeof(FOX_Color));
			
			if ( (format->palette)->colors == NULL ) 
			{
				FOX_FreeFormat(format);
				FOX_OutOfMemory();
				return(NULL);
			}
			format->palette->colors[0].r = 0xFF;
			format->palette->colors[0].g = 0xFF;
			format->palette->colors[0].b = 0xFF;
			format->palette->colors[1].r = 0x00;
			format->palette->colors[1].g = 0x00;
			format->palette->colors[1].b = 0x00;
			format->Rloss = 8;
			format->Gloss = 8;
			format->Bloss = 8;
			format->Aloss = 8;
			format->Rshift = 0;
			format->Gshift = 0;
			format->Bshift = 0;
			format->Ashift = 0;
			format->Rmask = 0;
			format->Gmask = 0;
			format->Bmask = 0;
			format->Amask = 0;
			break;

		case 4:
			
			format->palette = (FOX_Palette *)malloc(
							sizeof(FOX_Palette));
			if ( format->palette == NULL ) 
			{
				FOX_FreeFormat(format);
				FOX_OutOfMemory();
				return(NULL);
			}
			(format->palette)->ncolors = 16;
			(format->palette)->colors = (FOX_Color *)malloc(
				(format->palette)->ncolors*sizeof(FOX_Color));
			
			if ( (format->palette)->colors == NULL ) 
			{
				FOX_FreeFormat(format);
				FOX_OutOfMemory();
				return(NULL);
			}
			
			memset((format->palette)->colors, 0,
				(format->palette)->ncolors*sizeof(FOX_Color));
			
			format->Rloss = 8;
			format->Gloss = 8;
			format->Bloss = 8;
			format->Aloss = 8;
			format->Rshift = 0;
			format->Gshift = 0;
			format->Bshift = 0;
			format->Ashift = 0;
			format->Rmask = 0;
			format->Gmask = 0;
			format->Bmask = 0;
			format->Amask = 0;
			break;

		case 8:
			// stworz pusta 8bpp palete
			format->palette = (FOX_Palette *)malloc(
							sizeof(FOX_Palette));
			
			if ( format->palette == NULL ) 
			{
				FOX_FreeFormat(format);
				FOX_OutOfMemory();
				return(NULL);
			}
			(format->palette)->ncolors = 256;
			(format->palette)->colors = (FOX_Color *)malloc(
				(format->palette)->ncolors*sizeof(FOX_Color));
			
			if ( (format->palette)->colors == NULL ) 
			{
				FOX_FreeFormat(format);
				FOX_OutOfMemory();
				return(NULL);
			}
			memset((format->palette)->colors, 0,
				(format->palette)->ncolors*sizeof(FOX_Color));

			// palety bez mask info
			format->Rloss = 8;
			format->Gloss = 8;
			format->Bloss = 8;
			format->Aloss = 8;
			format->Rshift = 0;
			format->Gshift = 0;
			format->Bshift = 0;
			format->Ashift = 0;
			format->Rmask = 0;
			format->Gmask = 0;
			format->Bmask = 0;
			format->Amask = 0;
			break;

		default:
			
			// bez palety
			
			format->palette = NULL;
			format->Rshift = 0;
			format->Rloss = 8;
			
			if ( Rmask ) 
			{
				for ( mask = Rmask; !(mask&0x01); mask >>= 1 )
					++format->Rshift;
				for ( ; (mask&0x01); mask >>= 1 )
					--format->Rloss;
			}
			format->Gshift = 0;
			format->Gloss = 8;
			
			if ( Gmask ) 
			{
				for ( mask = Gmask; !(mask&0x01); mask >>= 1 )
					++format->Gshift;
				for ( ; (mask&0x01); mask >>= 1 )
					--format->Gloss;
			}
			format->Bshift = 0;
			format->Bloss = 8;
			
			if ( Bmask ) 
			{
				for ( mask = Bmask; !(mask&0x01); mask >>= 1 )
					++format->Bshift;
				for ( ; (mask&0x01); mask >>= 1 )
					--format->Bloss;
			}
			format->Ashift = 0;
			format->Aloss = 8;
			if ( Amask ) 
			{
				for ( mask = Amask; !(mask&0x01); mask >>= 1 )
					++format->Ashift;
				for ( ; (mask&0x01); mask >>= 1 )
					--format->Aloss;
			}
			format->Rmask = Rmask;
			format->Gmask = Gmask;
			format->Bmask = Bmask;
			format->Amask = Amask;
			break;
	}

	// przelicz standardowe maski 
	
	if ( (bpp > 8) && !format->Rmask && !format->Gmask && !format->Bmask ) 
	{
		//rgb
		if ( bpp > 24 )
			bpp = 24;
		format->Rloss = 8-(bpp/3);
		format->Gloss = 8-(bpp/3)-(bpp%3);
		format->Bloss = 8-(bpp/3);
		format->Rshift = ((bpp/3)+(bpp%3))+(bpp/3);
		format->Gshift = (bpp/3);
		format->Bshift = 0;
		format->Rmask = ((0xFF>>format->Rloss)<<format->Rshift);
		format->Gmask = ((0xFF>>format->Gloss)<<format->Gshift);
		format->Bmask = ((0xFF>>format->Bloss)<<format->Bshift);
	}
	return(format);
}

FOX_PixelFormat *FOX_ReallocFormat(FOX_Surface *surface, int bpp,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	if ( surface->format ) 
	{
		FOX_FreeFormat(surface->format);
		FOX_FormatChanged(surface);
	}
	surface->format = FOX_AllocFormat(bpp, Rmask, Gmask, Bmask, Amask);
	return surface->format;
}

// zmien maski i format 

void FOX_FormatChanged(FOX_Surface *surface)
{
	surface->format_version++;
	FOX_InvalidateMap(surface->map);
}

// zwolnij strukture 

void FOX_FreeFormat(FOX_PixelFormat *format)
{
	if ( format ) 
	{
		if ( format->palette ) 
		{
			if ( format->palette->colors ) 
			{
				free(format->palette->colors);
			}
			free(format->palette);
		}
		free(format);
	}
}

// 8bpp ditheres

void FOX_DitherColors(FOX_Color *colors, int bpp)
{
	int i;
	if(bpp != 8)
		return;		

	for(i = 0; i < 256; i++) 
	{
		int r, g, b;
		r = i & 0xe0;
		r |= r >> 3 | r >> 6;
		colors[i].r = r;
		g = (i << 3) & 0xe0;
		g |= g >> 3 | g >> 6;
		colors[i].g = g;
		b = i & 0x3;
		b |= b << 2;
		b |= b << 4;
		colors[i].b = b;
	}
}

Uint16 FOX_CalculatePitch(FOX_Surface *surface)
{
	Uint16 pitch;

	// surface powinien byc zaalignowany na granicy 4 bajtow 
	
	pitch = surface->w*surface->format->BytesPerPixel;
	switch (surface->format->BitsPerPixel) 
	{
		case 1:
			pitch = (pitch+7)/8;
			break;
		case 4:
			pitch = (pitch+1)/2;
			break;
		default:
			break;
	}
	pitch = (pitch + 3) & ~3;	
	return(pitch);
}

Uint8 FOX_FindColor(FOX_Palette *pal, Uint8 r, Uint8 g, Uint8 b)
{
	// policz 
	unsigned int smallest;
	unsigned int distance;
	int rd, gd, bd;
	int i;
	Uint8 pixel=0;
		
	smallest = ~0;
	
	for ( i=0; i<pal->ncolors; ++i ) 
	{
		rd = pal->colors[i].r - r;
		gd = pal->colors[i].g - g;
		bd = pal->colors[i].b - b;
		distance = (rd*rd)+(gd*gd)+(bd*bd);
		if ( distance < smallest ) 
		{
			pixel = i;
			
			if ( distance == 0 ) 
			{ 
				break;
			}
			smallest = distance;
		}
	}
	return(pixel);
}

Uint32 FOX_MapRGB(FOX_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b)
{
	if ( format->palette == NULL ) 
	{
		return (r >> format->Rloss) << format->Rshift
		       | (g >> format->Gloss) << format->Gshift
		       | (b >> format->Bloss) << format->Bshift
		       | format->Amask;
	} 
	else 
	{
		return FOX_FindColor(format->palette, r, g, b);
	}
}

// znajdz ten pixel 

Uint32 FOX_MapRGBA(FOX_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	if ( format->palette == NULL ) 
	{
	        return (r >> format->Rloss) << format->Rshift
		    | (g >> format->Gloss) << format->Gshift
		    | (b >> format->Bloss) << format->Bshift
		    | ((a >> format->Aloss) << format->Ashift & format->Amask);
	} 
	else 
	{
		return FOX_FindColor(format->palette, r, g, b);
	}
}

void FOX_GetRGBA(Uint32 pixel, FOX_PixelFormat *fmt,
		 Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
	if ( fmt->palette == NULL ) 
	{
        unsigned rv, gv, bv, av;

		rv = (pixel & fmt->Rmask) >> fmt->Rshift;
		*r = (rv << fmt->Rloss) + (rv >> (8 - fmt->Rloss));
		gv = (pixel & fmt->Gmask) >> fmt->Gshift;
		*g = (gv << fmt->Gloss) + (gv >> (8 - fmt->Gloss));
		bv = (pixel & fmt->Bmask) >> fmt->Bshift;
		*b = (bv << fmt->Bloss) + (bv >> (8 - fmt->Bloss));
		
		if(fmt->Amask) 
		{
		        av = (pixel & fmt->Amask) >> fmt->Ashift;
			*a = (av << fmt->Aloss) + (av >> (8 - fmt->Aloss));
		}
		else
		        *a = FOX_ALPHA_OPAQUE;
	} 
	else 
	{
		*r = fmt->palette->colors[pixel].r;
		*g = fmt->palette->colors[pixel].g;
		*b = fmt->palette->colors[pixel].b;
		*a = FOX_ALPHA_OPAQUE;
	}
}

void FOX_GetRGB(Uint32 pixel, FOX_PixelFormat *fmt, Uint8 *r,Uint8 *g,Uint8 *b)
{
	if ( fmt->palette == NULL ) 
	{
        unsigned rv, gv, bv;

		rv = (pixel & fmt->Rmask) >> fmt->Rshift;
		*r = (rv << fmt->Rloss) + (rv >> (8 - fmt->Rloss));
		gv = (pixel & fmt->Gmask) >> fmt->Gshift;
		*g = (gv << fmt->Gloss) + (gv >> (8 - fmt->Gloss));
		bv = (pixel & fmt->Bmask) >> fmt->Bshift;
		*b = (bv << fmt->Bloss) + (bv >> (8 - fmt->Bloss));
	} 
	else 
	{
		*r = fmt->palette->colors[pixel].r;
		*g = fmt->palette->colors[pixel].g;
		*b = fmt->palette->colors[pixel].b;
	}
}

void FOX_ApplyGamma(Uint16 *gamma, FOX_Color *colors, FOX_Color *output,
							int ncolors)
{
	int i;

	for ( i=0; i<ncolors; ++i ) 
	{
		output[i].r = gamma[0*256 + colors[i].r] >> 8;
		output[i].g = gamma[1*256 + colors[i].g] >> 8;
		output[i].b = gamma[2*256 + colors[i].b] >> 8;
	}
}

// mapuj z palety do palety 

static Uint8 *Map1to1(FOX_Palette *src, FOX_Palette *dst, int *identical)
{
	Uint8 *map;
	int i;

	if ( identical ) 
	{
		if ( src->ncolors <= dst->ncolors ) 
		{
			
			if ( memcmp(src->colors, dst->colors, src->ncolors*
						sizeof(FOX_Color)) == 0 ) 
			{
				*identical = 1;
				return(NULL);
			}
		}
		*identical = 0;
	}
	map = (Uint8 *)malloc(src->ncolors);
	
	if ( map == NULL ) 
	{
		FOX_OutOfMemory();
		return(NULL);
	}
	
	for ( i=0; i<src->ncolors; ++i ) 
	{
		map[i] = FOX_FindColor(dst,
			src->colors[i].r, src->colors[i].g, src->colors[i].b);
	}
	return(map);
}

static Uint8 *Map1toN(FOX_Palette *src, FOX_PixelFormat *dst)
{
	Uint8 *map;
	int i;
	int  bpp;
	unsigned alpha;

	bpp = ((dst->BytesPerPixel == 3) ? 4 : dst->BytesPerPixel);
	map = (Uint8 *)malloc(src->ncolors*bpp);

	if ( map == NULL ) 
	{
		FOX_OutOfMemory();
		return(NULL);
	}

	alpha = dst->Amask ? FOX_ALPHA_OPAQUE : 0;
	
	for ( i=0; i<src->ncolors; ++i ) 
	{
		ASSEMBLE_RGBA(&map[i*bpp], dst->BytesPerPixel, dst,
			      src->colors[i].r, src->colors[i].g,
			      src->colors[i].b, alpha);
	}
	return(map);
}

// mapuj z bitfield na palete
// generuj dither 8bpp

static Uint8 *MapNto1(FOX_PixelFormat *src, FOX_Palette *dst, int *identical)
{
	
	FOX_Palette dithered;
	FOX_Color colors[256];

	dithered.ncolors = 256;
	FOX_DitherColors(colors, 8);
	dithered.colors = colors;
	return(Map1to1(&dithered, dst, identical));
}

FOX_BlitMap *FOX_AllocBlitMap(void)
{
	FOX_BlitMap *map;

	// alokuj pusta mape
	
	map = (FOX_BlitMap *)malloc(sizeof(*map));
	
	if ( map == NULL ) 
	{
		FOX_OutOfMemory();
		return(NULL);
	}
	memset(map, 0, sizeof(*map));

	// alokuj 
	
	map->sw_data = (struct private_swaccel *)malloc(sizeof(*map->sw_data));
	
	if ( map->sw_data == NULL ) 
	{
		FOX_FreeBlitMap(map);
		FOX_OutOfMemory();
		return(NULL);
	}
	memset(map->sw_data, 0, sizeof(*map->sw_data));
	
	return(map);
}

void FOX_InvalidateMap(FOX_BlitMap *map)
{
	if ( ! map ) 
	{
		return;
	}
	map->dst = NULL;
	map->format_version = (unsigned int)-1;
	if ( map->table ) 
	{
		free(map->table);
		map->table = NULL;
	}
}
int FOX_MapSurface (FOX_Surface *src, FOX_Surface *dst)
{
	FOX_PixelFormat *srcfmt;
	FOX_PixelFormat *dstfmt;
	FOX_BlitMap *map;
	
	map = src->map;
	
	if ( (src->flags & FOX_RLEACCEL) == FOX_RLEACCEL ) 
	{
		FOX_UnRLESurface(src, 1);
	}
	FOX_InvalidateMap(map);

	// sprawdz ktory mapping 
	
	map->identity = 0;
	srcfmt = src->format;
	dstfmt = dst->format;

	switch (srcfmt->BytesPerPixel) 
	{
	    case 1:
		switch (dstfmt->BytesPerPixel) 
		{
		    case 1:
			
			if ( ((src->flags & FOX_HWSURFACE) == FOX_HWSURFACE) &&
			     ((dst->flags & FOX_HWSURFACE) == FOX_HWSURFACE) ) 
			{
				map->identity = 1;
			} 
			else 
			{
				map->table = Map1to1(srcfmt->palette,
					dstfmt->palette, &map->identity);
			}
			
			if ( ! map->identity ) 
			{
				if ( map->table == NULL ) 
				{
					return(-1);
				}
			}
			if (srcfmt->BitsPerPixel!=dstfmt->BitsPerPixel)
				map->identity = 0;
			break;

		    default:
			
			// z palety na bity 

			map->table = Map1toN(srcfmt->palette, dstfmt);
			
			if ( map->table == NULL ) 
			{
				return(-1);
			}
			break;
		}
		break;
	default:
		switch (dstfmt->BytesPerPixel) 
		{
		    case 1:
		
			map->table = MapNto1(srcfmt,
					dstfmt->palette, &map->identity);
		
			if ( ! map->identity ) 
			{
				if ( map->table == NULL ) 
				{
					return(-1);
				}
			}
			map->identity = 0;	
			break;
		    default:
			
			if ( FORMAT_EQUAL(srcfmt, dstfmt) )
				map->identity = 1;
			break;
		}
		break;
	}

	map->dst = dst;
	map->format_version = dst->format_version;
	
	return(FOX_CalculateBlit(src));
}

void FOX_FreeBlitMap(FOX_BlitMap *map)
{
	if ( map ) 
	{
		FOX_InvalidateMap(map);
		if ( map->sw_data != NULL ) 
		{
			free(map->sw_data);
		}
		free(map);
	}
}

// end
