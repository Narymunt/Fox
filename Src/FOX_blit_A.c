// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#include <stdio.h>

#include "FOX_types.h"
#include "FOX_video.h"
#include "blit.h"

// funkcje do blitowania z alpha blendingiem

static void BlitNto1SurfaceAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	Uint8 *palmap = info->table;
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;

	const unsigned A = srcfmt->alpha;

	while ( height-- ) 
	{
	    DUFFS_LOOP4(
	    {
		Uint32 pixel;
		unsigned sR;
		unsigned sG;
		unsigned sB;
		unsigned dR;
		unsigned dG;
		unsigned dB;
		DISEMBLE_RGB(src, srcbpp, srcfmt, pixel, sR, sG, sB);
		dR = dstfmt->palette->colors[*dst].r;
		dG = dstfmt->palette->colors[*dst].g;
		dB = dstfmt->palette->colors[*dst].b;
		ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
		dR &= 0xff;
		dG &= 0xff;
		dB &= 0xff;
		
		// rgb w 8bpp pixel
		
		if ( palmap == NULL ) 
		{
		    *dst =((dR>>5)<<(3+2))|
			  ((dG>>5)<<(2))|
			  ((dB>>6)<<(0));
		} 
		else 
		{
		    *dst = palmap[((dR>>5)<<(3+2))|
				  ((dG>>5)<<(2))  |
				  ((dB>>6)<<(0))];
		}
		dst++;
		src += srcbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

static void BlitNto1PixelAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	Uint8 *palmap = info->table;
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;

	while ( height-- ) 
	{
	    DUFFS_LOOP4(
	    {
		Uint32 pixel;
		unsigned sR;
		unsigned sG;
		unsigned sB;
		unsigned sA;
		unsigned dR;
		unsigned dG;
		unsigned dB;
		DISEMBLE_RGBA(src,srcbpp,srcfmt,pixel,sR,sG,sB,sA);
		dR = dstfmt->palette->colors[*dst].r;
		dG = dstfmt->palette->colors[*dst].g;
		dB = dstfmt->palette->colors[*dst].b;
		ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
		dR &= 0xff;
		dG &= 0xff;
		dB &= 0xff;
		// pakuj RGB w jeden 8bpp pixel
		
		if ( palmap == NULL ) 
		{
		    *dst =((dR>>5)<<(3+2))|
			  ((dG>>5)<<(2))|
			  ((dB>>6)<<(0));
		} 
		else 
		{
		    *dst = palmap[((dR>>5)<<(3+2))|
				  ((dG>>5)<<(2))  |
				  ((dB>>6)<<(0))  ];
		}
		dst++;
		src += srcbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

static void BlitNto1SurfaceAlphaKey(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	Uint8 *palmap = info->table;
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;
	Uint32 ckey = srcfmt->colorkey;

	const int A = srcfmt->alpha;

	while ( height-- ) 
	{
	    DUFFS_LOOP(
	    {
		Uint32 pixel;
		unsigned sR;
		unsigned sG;
		unsigned sB;
		unsigned dR;
		unsigned dG;
		unsigned dB;
		DISEMBLE_RGB(src, srcbpp, srcfmt, pixel, sR, sG, sB);
		if ( pixel != ckey ) 
		{
		    dR = dstfmt->palette->colors[*dst].r;
		    dG = dstfmt->palette->colors[*dst].g;
		    dB = dstfmt->palette->colors[*dst].b;
		    ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
		    dR &= 0xff;
		    dG &= 0xff;
		    dB &= 0xff;
		    // pakuj rgb w jeden 8bpp pixel
		    if ( palmap == NULL ) 
			{
			*dst =((dR>>5)<<(3+2))|
			      ((dG>>5)<<(2)) |
			      ((dB>>6)<<(0));
		    } 
			else 
			{
			*dst = palmap[((dR>>5)<<(3+2))|
				      ((dG>>5)<<(2))  |
				      ((dB>>6)<<(0))  ];
		    }
		}
		dst++;
		src += srcbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

static void BlitRGBtoRGBSurfaceAlpha128(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint32 *srcp = (Uint32 *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	Uint32 *dstp = (Uint32 *)info->d_pixels;
	int dstskip = info->d_skip >> 2;

	while(height--) 
	{
	    DUFFS_LOOP4({
		    Uint32 s = *srcp++;
		    Uint32 d = *dstp;
		    *dstp++ = ((((s & 0x00fefefe) + (d & 0x00fefefe)) >> 1)
			       + (s & d & 0x00010101)) | 0xff000000;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

static void BlitRGBtoRGBSurfaceAlpha(FOX_BlitInfo *info)
{
	unsigned alpha = info->src->alpha;
	if(alpha == 128) 
	{
		BlitRGBtoRGBSurfaceAlpha128(info);
	} 
	else 
	{
		int width = info->d_width;
		int height = info->d_height;
		Uint32 *srcp = (Uint32 *)info->s_pixels;
		int srcskip = info->s_skip >> 2;
		Uint32 *dstp = (Uint32 *)info->d_pixels;
		int dstskip = info->d_skip >> 2;

		while(height--) 
		{
			DUFFS_LOOP4({
				Uint32 s;
				Uint32 d;
				Uint32 s1;
				Uint32 d1;
				s = *srcp;
				d = *dstp;
				s1 = s & 0xff00ff;
				d1 = d & 0xff00ff;
				d1 = (d1 + ((s1 - d1) * alpha >> 8))
				     & 0xff00ff;
				s &= 0xff00;
				d &= 0xff00;
				d = (d + ((s - d) * alpha >> 8)) & 0xff00;
				*dstp = d1 | d | 0xff000000;
				++srcp;
				++dstp;
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void BlitRGBtoRGBPixelAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint32 *srcp = (Uint32 *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	Uint32 *dstp = (Uint32 *)info->d_pixels;
	int dstskip = info->d_skip >> 2;

	while(height--) 
	{
	    DUFFS_LOOP4({
		Uint32 dalpha;
		Uint32 d;
		Uint32 s1;
		Uint32 d1;
		Uint32 s = *srcp;
		Uint32 alpha = s >> 24;

		if(alpha == FOX_ALPHA_OPAQUE) 
		{
		    *dstp = (s & 0x00ffffff) | (*dstp & 0xff000000);
		} 
		else 
		{
		    d = *dstp;
		    dalpha = d & 0xff000000;
		    s1 = s & 0xff00ff;
		    d1 = d & 0xff00ff;
		    d1 = (d1 + ((s1 - d1) * alpha >> 8)) & 0xff00ff;
		    s &= 0xff00;
		    d &= 0xff00;
		    d = (d + ((s - d) * alpha >> 8)) & 0xff00;
		    *dstp = d1 | d | dalpha;
		}
		++srcp;
		++dstp;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

// blenduj jeden 16bpp pixel

#define BLEND16_50(d, s, mask)						\
	((((s & mask) + (d & mask)) >> 1) + (s & d & (~mask & 0xffff)))

// tak samo tylko ze dwa pixele

#define BLEND2x16_50(d, s, mask)					     \
	(((s & (mask | mask << 16)) >> 1) + ((d & (mask | mask << 16)) >> 1) \
	 + (s & d & (~(mask | mask << 16))))

static void Blit16to16SurfaceAlpha128(FOX_BlitInfo *info, Uint16 mask)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint16 *srcp = (Uint16 *)info->s_pixels;
	int srcskip = info->s_skip >> 1;
	Uint16 *dstp = (Uint16 *)info->d_pixels;
	int dstskip = info->d_skip >> 1;

	while(height--) 
	{
		if(((unsigned long)srcp ^ (unsigned long)dstp) & 2) 
		{
			Uint32 prev_sw;
			int w = width;
			
			if((unsigned long)dstp & 2) 
			{
				Uint16 d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				dstp++;
				srcp++;
				w--;
			}
			srcp++;	
			
			prev_sw = ((Uint32 *)srcp)[-1];

			while(w > 1) 
			{
				Uint32 sw, dw, s;
				sw = *(Uint32 *)srcp;
				dw = *(Uint32 *)dstp;
				if(FOX_BYTEORDER == FOX_BIG_ENDIAN)
					s = (prev_sw << 16) + (sw >> 16);
				else
					s = (prev_sw >> 16) + (sw << 16);
				prev_sw = sw;
				*(Uint32 *)dstp = BLEND2x16_50(dw, s, mask);
				dstp += 2;
				srcp += 2;
				w -= 2;
			}
			
			if(w) 
			{
				Uint16 d = *dstp, s;
				if(FOX_BYTEORDER == FOX_BIG_ENDIAN)
					s = prev_sw;
				else
					s = prev_sw >> 16;
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
			}
			srcp += srcskip - 1;
			dstp += dstskip;
		} 
		else 
		{
			
			int w = width;
			
			if((unsigned long)srcp & 2) 
			{
				Uint16 d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
				w--;
			}
			
			while(w > 1) 
			{
				Uint32 sw = *(Uint32 *)srcp;
				Uint32 dw = *(Uint32 *)dstp;
				*(Uint32 *)dstp = BLEND2x16_50(dw, sw, mask);
				srcp += 2;
				dstp += 2;
				w -= 2;
			}
			
			if(w) 
			{
				Uint16 d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
			}
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void Blit565to565SurfaceAlpha(FOX_BlitInfo *info)
{
	unsigned alpha = info->src->alpha;

	if(alpha == 128) 
	{
		Blit16to16SurfaceAlpha128(info, 0xf7de);
	} 
	else 
	{
		int width = info->d_width;
		int height = info->d_height;
		Uint16 *srcp = (Uint16 *)info->s_pixels;
		int srcskip = info->s_skip >> 1;
		Uint16 *dstp = (Uint16 *)info->d_pixels;
		int dstskip = info->d_skip >> 1;
		alpha >>= 3;	// alpha do 5 bitow

		while(height--) 
		{
			DUFFS_LOOP4({
				Uint32 s = *srcp++;
				Uint32 d = *dstp;
				s = (s | s << 16) & 0x07e0f81f;
				d = (d | d << 16) & 0x07e0f81f;
				d += (s - d) * alpha >> 5;
				d &= 0x07e0f81f;
				*dstp++ = d | d >> 16;
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void Blit555to555SurfaceAlpha(FOX_BlitInfo *info)
{
	unsigned alpha = info->src->alpha; // alpha do 5 bitow

	if(alpha == 128) 
	{
		Blit16to16SurfaceAlpha128(info, 0xfbde);
	} 
	else 
	{
		int width = info->d_width;
		int height = info->d_height;
		Uint16 *srcp = (Uint16 *)info->s_pixels;
		int srcskip = info->s_skip >> 1;
		Uint16 *dstp = (Uint16 *)info->d_pixels;
		int dstskip = info->d_skip >> 1;
		alpha >>= 3;		// alpha do 5 bitow

		while(height--) {
			DUFFS_LOOP4({
				Uint32 s = *srcp++;
				Uint32 d = *dstp;

				s = (s | s << 16) & 0x03e07c1f;
				d = (d | d << 16) & 0x03e07c1f;
				d += (s - d) * alpha >> 5;
				d &= 0x03e07c1f;
				*dstp++ = d | d >> 16;
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void BlitARGBto565PixelAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint32 *srcp = (Uint32 *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	Uint16 *dstp = (Uint16 *)info->d_pixels;
	int dstskip = info->d_skip >> 1;

	while(height--) {
	    DUFFS_LOOP4({
		Uint32 s = *srcp;
		unsigned alpha = s >> 27; // alpha do 5 bitow
		
		if(alpha == (FOX_ALPHA_OPAQUE >> 3)) 
		{
		    *dstp = (s >> 8 & 0xf800) + (s >> 5 & 0x7e0)
			  + (s >> 3  & 0x1f);
		} 
		else
		{
		    Uint32 d = *dstp;
		    s = ((s & 0xfc00) << 11) + (s >> 8 & 0xf800)
		      + (s >> 3 & 0x1f);
		    d = (d | d << 16) & 0x07e0f81f;
		    d += (s - d) * alpha >> 5;
		    d &= 0x07e0f81f;
		    *dstp = d | d >> 16;
		}
		srcp++;
		dstp++;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

static void BlitARGBto555PixelAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint32 *srcp = (Uint32 *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	Uint16 *dstp = (Uint16 *)info->d_pixels;
	int dstskip = info->d_skip >> 1;

	while(height--) {
	    DUFFS_LOOP4({
		unsigned alpha;
		Uint32 s = *srcp;
		alpha = s >> 27; // alpha do 5 bitow
		
		if(alpha == (FOX_ALPHA_OPAQUE >> 3)) 
		{
		    *dstp = (s >> 9 & 0x7c00) + (s >> 6 & 0x3e0)
			  + (s >> 3  & 0x1f);
		} 
		else 
		{
		    Uint32 d = *dstp;
		    s = ((s & 0xf800) << 10) + (s >> 9 & 0x7c00)
		      + (s >> 3 & 0x1f);
		    d = (d | d << 16) & 0x03e07c1f;
		    d += (s - d) * alpha >> 5;
		    d &= 0x03e07c1f;
		    *dstp = d | d >> 16;
		}
		srcp++;
		dstp++;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

static void BlitNtoNSurfaceAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;
	int dstbpp = dstfmt->BytesPerPixel;
	unsigned sA = srcfmt->alpha;
	unsigned dA = dstfmt->Amask ? FOX_ALPHA_OPAQUE : 0;

	while ( height-- ) {
	    DUFFS_LOOP4(
	    {
		Uint32 pixel;
		unsigned sR;
		unsigned sG;
		unsigned sB;
		unsigned dR;
		unsigned dG;
		unsigned dB;
		DISEMBLE_RGB(src, srcbpp, srcfmt, pixel, sR, sG, sB);
		DISEMBLE_RGB(dst, dstbpp, dstfmt, pixel, dR, dG, dB);
		ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
		ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
		src += srcbpp;
		dst += dstbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

// alpha z colorkey 

static void BlitNtoNSurfaceAlphaKey(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;
	Uint32 ckey = srcfmt->colorkey;
	int srcbpp = srcfmt->BytesPerPixel;
	int dstbpp = dstfmt->BytesPerPixel;
	unsigned sA = srcfmt->alpha;
	unsigned dA = dstfmt->Amask ? FOX_ALPHA_OPAQUE : 0;

	while ( height-- ) {
	    DUFFS_LOOP4(
	    {
		Uint32 pixel;
		unsigned sR;
		unsigned sG;
		unsigned sB;
		unsigned dR;
		unsigned dG;
		unsigned dB;
		RETRIEVE_RGB_PIXEL(src, srcbpp, pixel);
		if(pixel != ckey) {
		    RGB_FROM_PIXEL(pixel, srcfmt, sR, sG, sB);
		    DISEMBLE_RGB(dst, dstbpp, dstfmt, pixel, dR, dG, dB);
		    ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
		    ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
		}
		src += srcbpp;
		dst += dstbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

static void BlitNtoNPixelAlpha(FOX_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	Uint8 *src = info->s_pixels;
	int srcskip = info->s_skip;
	Uint8 *dst = info->d_pixels;
	int dstskip = info->d_skip;
	FOX_PixelFormat *srcfmt = info->src;
	FOX_PixelFormat *dstfmt = info->dst;

	int  srcbpp;
	int  dstbpp;

	// ustaw podstawowe zmienne
	
	srcbpp = srcfmt->BytesPerPixel;
	dstbpp = dstfmt->BytesPerPixel;

	while ( height-- ) {
	    DUFFS_LOOP4(
	    {
		Uint32 pixel;
		unsigned sR;
		unsigned sG;
		unsigned sB;
		unsigned dR;
		unsigned dG;
		unsigned dB;
		unsigned sA;
		unsigned dA;
		DISEMBLE_RGBA(src, srcbpp, srcfmt, pixel, sR, sG, sB, sA);
		DISEMBLE_RGBA(dst, dstbpp, dstfmt, pixel, dR, dG, dB, dA);
		ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
		ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
		src += srcbpp;
		dst += dstbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}


FOX_loblit FOX_CalculateAlphaBlit(FOX_Surface *surface, int blit_index)
{
    FOX_PixelFormat *sf = surface->format;
    FOX_PixelFormat *df = surface->map->dst->format;

    if(sf->Amask == 0) 
	{
	if((surface->flags & FOX_SRCCOLORKEY) == FOX_SRCCOLORKEY) 
	{
	    if(df->BytesPerPixel == 1)
		return BlitNto1SurfaceAlphaKey;
	    else
		return BlitNtoNSurfaceAlphaKey;
	} 
	else 
	{
	    
	    switch(df->BytesPerPixel) {
	    case 1:
		return BlitNto1SurfaceAlpha;

	    case 2:
		if(surface->map->identity) {
		    if(df->Gmask == 0x7e0)
			return Blit565to565SurfaceAlpha;
		    else if(df->Gmask == 0x3e0)
			return Blit555to555SurfaceAlpha;
		}
		return BlitNtoNSurfaceAlpha;

	    case 4:
		if(sf->Rmask == df->Rmask
		   && sf->Gmask == df->Gmask
		   && sf->Bmask == df->Bmask
		   && (sf->Rmask | sf->Gmask | sf->Bmask) == 0xffffff
		   && sf->BytesPerPixel == 4)
		    return BlitRGBtoRGBSurfaceAlpha;
		else
		    return BlitNtoNSurfaceAlpha;

	    case 3:
	    default:
		return BlitNtoNSurfaceAlpha;
	    }
	}
    }
	else
	{
	
	switch(df->BytesPerPixel) {
	case 1:
	    return BlitNto1PixelAlpha;

	case 2:
	    if(sf->BytesPerPixel == 4 && sf->Amask == 0xff000000
	       && sf->Gmask == 0xff00
	       && ((sf->Rmask == 0xff && df->Rmask == 0x1f)
		   || (sf->Bmask == 0xff && df->Bmask == 0x1f))) {
		if(df->Gmask == 0x7e0)
		    return BlitARGBto565PixelAlpha;
		else if(df->Gmask == 0x3e0)
		    return BlitARGBto555PixelAlpha;
	    }
	    return BlitNtoNPixelAlpha;

	case 4:
	    if(sf->Amask == 0xff000000
	       && sf->Rmask == df->Rmask
	       && sf->Gmask == df->Gmask
	       && sf->Bmask == df->Bmask
	       && sf->BytesPerPixel == 4)
		return BlitRGBtoRGBPixelAlpha;
	    return BlitNtoNPixelAlpha;

	case 3:
	default:
	    return BlitNtoNPixelAlpha;
	}
    }
}

// end

