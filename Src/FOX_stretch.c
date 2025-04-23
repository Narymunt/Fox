// Fox v0.5a
// by Jaroslaw Rozynski
//===
// TODO:

#include "FOX_error.h"
#include "FOX_video.h"
#include "blit.h"


#define PREFIX16	0x66
#define STORE_BYTE	0xAA
#define STORE_WORD	0xAB
#define LOAD_BYTE	0xAC
#define LOAD_WORD	0xAD
#define RETURN		0xC3

static unsigned char copy_row[4096];

static int generate_rowbytes(int src_w, int dst_w, int bpp)
{
	static struct {
		int bpp;
		int src_w;
		int dst_w;
	} last;

	int i;
	int pos, inc;
	unsigned char *eip;
	unsigned char load, store;

	// potrzebujemy bufora

	if ( (src_w == last.src_w) &&
	     (dst_w == last.src_w) && (bpp == last.bpp) ) 
	{
		return(0);
	}
	last.bpp = bpp;
	last.src_w = src_w;
	last.dst_w = dst_w;

	switch (bpp) {
	    case 1:
		load = LOAD_BYTE;
		store = STORE_BYTE;
		break;
	    case 2:
	    case 4:
		load = LOAD_WORD;
		store = STORE_WORD;
		break;
	    default:
		FOX_SetError("[FOX]: stretch %d \n", bpp);
		return(-1);
	}
	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	eip = copy_row;
	
	for ( i=0; i<dst_w; ++i ) 
	{
		while ( pos >= 0x10000L ) 
		{
			if ( bpp == 2 ) 
			{
				*eip++ = PREFIX16;
			}
			*eip++ = load;
			pos -= 0x10000L;
		}
		
		if ( bpp == 2 ) 
		{
			*eip++ = PREFIX16;
		}
		*eip++ = store;
		pos += inc;
	}
	*eip++ = RETURN;
	
	if ( eip > (copy_row+sizeof(copy_row)) ) 
	{
		FOX_SetError("[FOX]: buffer overflow");
		return(-1);
	}
	return(0);
}



#define DEFINE_COPY_ROW(name, type)			\
void name(type *src, int src_w, type *dst, int dst_w)	\
{							\
	int i;						\
	int pos, inc;					\
	type pixel = 0;					\
							\
	pos = 0x10000;					\
	inc = (src_w << 16) / dst_w;			\
	for ( i=dst_w; i>0; --i ) {			\
		while ( pos >= 0x10000L ) {		\
			pixel = *src++;			\
			pos -= 0x10000L;		\
		}					\
		*dst++ = pixel;				\
		pos += inc;				\
	}						\
}
DEFINE_COPY_ROW(copy_row1, Uint8)
DEFINE_COPY_ROW(copy_row2, Uint16)
DEFINE_COPY_ROW(copy_row4, Uint32)



void copy_row3(Uint8 *src, int src_w, Uint8 *dst, int dst_w)
{
	int i;
	int pos, inc;
	Uint8 pixel[3];

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for ( i=dst_w; i>0; --i ) 
	{
		while ( pos >= 0x10000L ) 
		{
			pixel[0] = *src++;
			pixel[1] = *src++;
			pixel[2] = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel[0];
		*dst++ = pixel[1];
		*dst++ = pixel[2];
		pos += inc;
	}
}

// stretchowany blit

int FOX_SoftStretch(FOX_Surface *src, FOX_Rect *srcrect,
                    FOX_Surface *dst, FOX_Rect *dstrect)
{
	int pos, inc;
	int dst_width;
	int dst_maxrow;
	int src_row, dst_row;
	Uint8 *srcp = NULL;
	Uint8 *dstp;
	FOX_Rect full_src;
	FOX_Rect full_dst;

	const int bpp = dst->format->BytesPerPixel;

	if ( src->format->BitsPerPixel != dst->format->BitsPerPixel ) 
	{
		FOX_SetError("[FOX]: stretch tylkko na tych samych formatach");
		return(-1);
	}

	// blituj 
	
	if ( srcrect ) 
	{
		if ( (srcrect->x < 0) || (srcrect->y < 0) ||
		     ((srcrect->x+srcrect->w) > src->w) ||
		     ((srcrect->y+srcrect->h) > src->h) ) 
		{
			FOX_SetError("[FOX]: nieprawidlowe zrodlo dla stretch");
			return(-1);
		}
	} 
	else 
	{
		full_src.x = 0;
		full_src.y = 0;
		full_src.w = src->w;
		full_src.h = src->h;
		srcrect = &full_src;
	}
	
	if ( dstrect ) 
	{
		if ( (dstrect->x < 0) || (dstrect->y < 0) ||
		     ((dstrect->x+dstrect->w) > dst->w) ||
		     ((dstrect->y+dstrect->h) > dst->h) ) 
		{
			FOX_SetError("[FOX]: nieprawidlowy cel dla stretch");
			return(-1);
		}
	} 
	else 
	{
		full_dst.x = 0;
		full_dst.y = 0;
		full_dst.w = dst->w;
		full_dst.h = dst->h;
		dstrect = &full_dst;
	}

	// ustaw dane
	
	pos = 0x10000;
	inc = (srcrect->h << 16) / dstrect->h;
	src_row = srcrect->y;
	dst_row = dstrect->y;
	dst_width = dstrect->w*bpp;

#ifdef USE_ASM_STRETCH
	if ( (bpp != 3) &&
	     (generate_rowbytes(srcrect->w, dstrect->w, bpp) < 0) ) {
		return(-1);
	}
#endif

	// stretch i blit
	
	for ( dst_maxrow = dst_row+dstrect->h; dst_row<dst_maxrow; ++dst_row ) 
	{
		dstp = (Uint8 *)dst->pixels + (dst_row*dst->pitch)
		                            + (dstrect->x*bpp);
		while ( pos >= 0x10000L ) 
		{
			srcp = (Uint8 *)src->pixels + (src_row*src->pitch)
			                            + (srcrect->x*bpp);
			++src_row;
			pos -= 0x10000L;
		}
#ifdef USE_ASM_STRETCH
		switch (bpp) 
		{
		    case 3:
			copy_row3(srcp, srcrect->w, dstp, dstrect->w);
			break;
		    default:
		{ void *code = &copy_row;
			__asm {
				push edi
				push esi
	
				mov edi, dstp
				mov esi, srcp
				call dword ptr code

				pop esi
				pop edi
			}
		}
#endif

		switch (bpp) {
		    case 1:
			copy_row1(srcp, srcrect->w, dstp, dstrect->w);
			break;
		    case 2:
			copy_row2((Uint16 *)srcp, srcrect->w,
			          (Uint16 *)dstp, dstrect->w);
			break;
		    case 3:
			copy_row3(srcp, srcrect->w, dstp, dstrect->w);
			break;
		    case 4:
			copy_row4((Uint32 *)srcp, srcrect->w,
			          (Uint32 *)dstp, dstrect->w);
			break;
		}

		pos += inc;
	}
	return(0);
}

// end
