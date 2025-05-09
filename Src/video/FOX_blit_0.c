// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#include <stdio.h>
#include <string.h>

#include "FOX_types.h"
#include "FOX_video.h"
#include "FOX_blit.h"

// blituj bitmapy na inne surface

static void BlitBto1(FOX_BlitInfo *info)
{

	int c;
	int width, height;
	
	Uint8 *src, *map, *dst;
	
	int srcskip, dstskip;

	// ustaw podstawowe zmienne

	width = info->d_width;
	height = info->d_height;
	
	src = info->s_pixels;
	srcskip = info->s_skip;
	
	dst = info->d_pixels;
	dstskip = info->d_skip;
	
	map = info->table;
	
	srcskip += width-(width+7)/8;

	if ( map ) 
	{
		while ( height-- ) 
		{
		        Uint8 byte = 0, bit;
	    		for ( c=0; c<width; ++c ) 
				{
					if ( (c&7) == 0 ) 
					{
					byte = *src++;
					}
				bit = (byte&0x80)>>7;
				
				if ( 1 ) 
				{
				  *dst = map[bit];
				}
				dst++;
				byte <<= 1;
			}
			src += srcskip;
			dst += dstskip;
		}
	} 
	else	
	{
		while ( height-- ) 
		{
		        Uint8 byte = 0, bit;
	    		
				for ( c=0; c<width; ++c ) 
				{
				
					if ( (c&7) == 0 ) 
					{
						byte = *src++;
					}
				
					bit = (byte&0x80)>>7;
				
					if ( 1 ) 
					{
						*dst = bit;
					}
				
					dst++;
					byte <<= 1;
				}
			src += srcskip;
			dst += dstskip;
		}
	}
}

// wersja druga 

static void BlitBto2(FOX_BlitInfo *info)
{
	int c;
	
	int width, height;
	
	Uint8 *src;
	Uint16 *map, *dst;
	
	int srcskip, dstskip;

	// ustaw podstawowe zmienne

	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = (Uint16 *)info->d_pixels;
	dstskip = info->d_skip/2;
	map = (Uint16 *)info->table;
	srcskip += width-(width+7)/8;

	while ( height-- ) 
	{
	        Uint8 byte = 0, bit;
	    	
			for ( c=0; c<width; ++c ) 
			{
				if ( (c&7) == 0 ) 
				{
				byte = *src++;
				}
			
				bit = (byte&0x80)>>7;
			
				if ( 1 ) 
				{
				*dst = map[bit];
				}
			
				byte <<= 1;
			
				dst++;
		}
		
		src += srcskip;
		dst += dstskip;
	}
}

// trzecia wersja 

static void BlitBto3(FOX_BlitInfo *info)
{
	int c, o;
	int width, height;
	Uint8 *src, *map, *dst;
	int srcskip, dstskip;

	// podstawowe zmienne

	width = info->d_width;
	height = info->d_height;
	
	src = info->s_pixels;
	srcskip = info->s_skip;
	
	dst = info->d_pixels;
	dstskip = info->d_skip;
	
	map = info->table;
	
	srcskip += width-(width+7)/8;

	while ( height-- ) 
	{
	        Uint8 byte = 0, bit;
	    	for ( c=0; c<width; ++c ) 
			{
				if ( (c&7) == 0 ) 
				{
				byte = *src++;
				}
			
				bit = (byte&0x80)>>7;
			
				if ( 1 ) 
				{
				o = bit * 4;
				dst[0] = map[o++];
				dst[1] = map[o++];
				dst[2] = map[o++];
				}
			
				byte <<= 1;
			
				dst += 3;
			}
		src += srcskip;
		dst += dstskip;
	}
}

// czwarta wersja

static void BlitBto4(FOX_BlitInfo *info)
{
	int width, height;
	Uint8 *src;
	Uint32 *map, *dst;
	int srcskip, dstskip;
	int c;

	// ustaw podstawowe zmienne 
	
	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = (Uint32 *)info->d_pixels;
	dstskip = info->d_skip/4;
	map = (Uint32 *)info->table;
	srcskip += width-(width+7)/8;

	while ( height-- ) 
	{
	        Uint8 byte = 0, bit;
	
		for ( c=0; c<width; ++c ) 
		{
			
			if ( (c&7) == 0 ) 
			{
				byte = *src++;
			}
			bit = (byte&0x80)>>7;
			
			if ( 1 ) 
			{
				*dst = map[bit];
			}
			byte <<= 1;
			dst++;
		}
		src += srcskip;
		dst += dstskip;
	}
}

// tak samo tylko z ck

static void BlitBto1Key(FOX_BlitInfo *info)
{
    
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	Uint8 *dst = info->d_pixels;
	
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	
	Uint32 ckey = info->src->colorkey;
	Uint8 *palmap = info->table;
	
	int c;

	// podstawowe info
	
	srcskip += width-(width+7)/8;

	if ( palmap ) 
	{
		while ( height-- ) 
		{
		        Uint8  byte = 0, bit;
	    		
				for ( c=0; c<width; ++c ) 
				{
					if ( (c&7) == 0 ) 
					{
					byte = *src++;
					}

				bit = (byte&0x80)>>7;
				
				if ( bit != ckey ) 
				{
				  *dst = palmap[bit];
				}
				
				dst++;
				byte <<= 1;
			}
			
				src += srcskip;
				dst += dstskip;
		}
	} 
	else 
	{
		while ( height-- ) 
		{
		        Uint8  byte = 0, bit;

	    		for ( c=0; c<width; ++c ) 
				{
					if ( (c&7) == 0 ) 
					{
						byte = *src++;
					}
				
					bit = (byte&0x80)>>7;
				
					if ( bit != ckey ) 
					{
						*dst = bit;
					}
				
					dst++;
				
					byte <<= 1;
			}
			
				src += srcskip;
				dst += dstskip;
		}
	}
}

// tak samo - druga wersja z ck

static void BlitBto2Key(FOX_BlitInfo *info)
{
    
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	Uint16 *dstp = (Uint16 *)info->d_pixels;
	
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	
	Uint32 ckey = info->src->colorkey;
	Uint8 *palmap = info->table;
	
	int c;

	// ustaw podstawowe info
	
	srcskip += width-(width+7)/8;
	dstskip /= 2;

	while ( height-- ) 
	{
	        Uint8 byte = 0, bit;
	    	for ( c=0; c<width; ++c ) 
			{
				if ( (c&7) == 0 ) 
				{
				byte = *src++;
				}
			
				bit = (byte&0x80)>>7;
			
				if ( bit != ckey ) 
				{
				*dstp=((Uint16 *)palmap)[bit];
				}
			
				byte <<= 1;
				dstp++;
		}
		src += srcskip;
		dstp += dstskip;
	}
}

// trzecia wersja 

static void BlitBto3Key(FOX_BlitInfo *info)
{
    
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	Uint8 *dst = info->d_pixels;
	
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	
	Uint32 ckey = info->src->colorkey;
	Uint8 *palmap = info->table;
	
	int c;

	// podstawowe info
	
	srcskip += width-(width+7)/8;

	while ( height-- ) 
	{
	        Uint8  byte = 0, bit;
	    	
		for ( c=0; c<width; ++c ) 
		{
			
			if ( (c&7) == 0 ) 
			{
				byte = *src++;
			}
			
			bit = (byte&0x80)>>7;
			
			if ( bit != ckey ) 
			{
				memcpy(dst, &palmap[bit*4], 3);
			}
			byte <<= 1;
			dst += 3;
		}
		src += srcskip;
		dst += dstskip;
	}
}

// czwarta wersja 

static void BlitBto4Key(FOX_BlitInfo *info)
{
    
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	
	Uint32 *dstp = (Uint32 *)info->d_pixels;
	
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	
	Uint32 ckey = info->src->colorkey;
	Uint8 *palmap = info->table;
	
	int c;

	// podstawowe info
	
	srcskip += width-(width+7)/8;
	dstskip /= 4;

	while ( height-- ) 
	{
	        Uint8 byte = 0, bit;
	    	for ( c=0; c<width; ++c ) 
			{
				if ( (c&7) == 0 ) 
				{
				byte = *src++;
				}
			
				bit = (byte&0x80)>>7;
			
				if ( bit != ckey ) 
				{
				*dstp=((Uint32 *)palmap)[bit];
				}
			
				byte <<= 1;
				dstp++;
			}
		src += srcskip;
		dstp += dstskip;
	}
}

// blit bitmapy bez alpha 

static void BlitBtoNAlpha(FOX_BlitInfo *info)
{
    
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	Uint8 *dst = info->d_pixels;
	
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	
	const FOX_Color *srcpal	= info->src->palette->colors;
	
	FOX_PixelFormat *dstfmt = info->dst;
	
	int  dstbpp;
	
	int c;
	
	const int A = info->src->alpha;

	// ustawienie podstawowego info
	
	dstbpp = dstfmt->BytesPerPixel;
	
	srcskip += width-(width+7)/8;

	while ( height-- ) 
	{
	        Uint8 byte = 0, bit;
	    	for ( c=0; c<width; ++c ) 
			{
				if ( (c&7) == 0 ) 
				{
				byte = *src++;
				}
			
				bit = (byte&0x80)>>7;
			
				if ( 1 ) 
				{
			    
				Uint32 pixel;
			    
				unsigned sR, sG, sB;
				unsigned dR, dG, dB;
				
				sR = srcpal[bit].r;
				sG = srcpal[bit].g;
				sB = srcpal[bit].b;
				
				DISEMBLE_RGB(dst, dstbpp, dstfmt,
							pixel, dR, dG, dB);
				
				ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
			  	
				ASSEMBLE_RGB(dst, dstbpp, dstfmt, dR, dG, dB);
				}

			byte <<= 1;
			dst += dstbpp;
		}
		src += srcskip;
		dst += dstskip;
	}
}

// blit z bitmapy z ck bez alphy

static void BlitBtoNAlphaKey(FOX_BlitInfo *info)
{
    
	int width = info->d_width;
	int height = info->d_height;
	
	Uint8 *src = info->s_pixels;
	Uint8 *dst = info->d_pixels;
	
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	
	const FOX_Color *srcpal	= srcfmt->palette->colors;
	
	int dstbpp;
	
	int c;
	
	const int A = srcfmt->alpha;
	Uint32 ckey = srcfmt->colorkey;

	// ustaw zmienne podstawowe 
		
	dstbpp = dstfmt->BytesPerPixel;
	srcskip += width-(width+7)/8;

	while ( height-- ) 
	{
	        Uint8  byte = 0, bit;
	    	
			for ( c=0; c<width; ++c ) 
			{
				if ( (c&7) == 0 ) 
				{
				byte = *src++;
				}
			
				bit = (byte&0x80)>>7;
			
				if ( bit != ckey ) 
				{
			    
				int sR, sG, sB;
				int dR, dG, dB;
				
				Uint32 pixel;
				
				sR = srcpal[bit].r;
				sG = srcpal[bit].g;
				sB = srcpal[bit].b;
				
				DISEMBLE_RGB(dst, dstbpp, dstfmt,
							pixel, dR, dG, dB);
				
				ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
			  	
				ASSEMBLE_RGB(dst, dstbpp, dstfmt, dR, dG, dB);
			}
			byte <<= 1;
			dst += dstbpp;
		}
		src += srcskip;
		dst += dstskip;
	}
}

// tricky ;) 

static FOX_loblit bitmap_blit[] = {
	NULL, BlitBto1, BlitBto2, BlitBto3, BlitBto4
};

static FOX_loblit colorkey_blit[] = {
    NULL, BlitBto1Key, BlitBto2Key, BlitBto3Key, BlitBto4Key
};

FOX_loblit FOX_CalculateBlit0(FOX_Surface *surface, int blit_index)
{
	int which;

	if ( surface->map->dst->format->BitsPerPixel < 8 ) 
	{
		which = 0;
	} 
	else 
	{
		which = surface->map->dst->format->BytesPerPixel;
	}

	switch(blit_index) 
	{
	case 0:						// kopiuj
	    return bitmap_blit[which];

	case 1:						// colorkey
	    return colorkey_blit[which];

	case 2:						// alpha
	    return which >= 2 ? BlitBtoNAlpha : NULL;

	case 4:						// alpha + colorkey
	    return which >= 2 ? BlitBtoNAlphaKey : NULL;
	}
	return NULL;
}

// end

