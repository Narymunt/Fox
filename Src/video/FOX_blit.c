// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// nasze 

#include "FOX_error.h"
#include "FOX_video.h"
#include "FOX_sysvideo.h"
#include "FOX_blit.h"
#include "FOX_RLEaccel_c.h"
#include "FOX_pixels_c.h"
#include "FOX_memops.h"

// najbezpieczniejszy blit, bez akceleracji

static int FOX_SoftBlit(FOX_Surface *src, FOX_Rect *srcrect,
			FOX_Surface *dst, FOX_Rect *dstrect)
{
	int okay;
	int src_locked;
	int dst_locked;

	// wszystko jest ok

	okay = 1;

	// zablokuj cel jezeli to jest w hardware
	// aha i nie czekaj na plamke, jezeli jest async

	dst_locked = 0;

	if ( dst->flags & (FOX_HWSURFACE|FOX_ASYNCBLIT) ) 
	{
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;
	
		if ( video->LockHWSurface(this, dst) < 0 ) 
		{
			okay = 0;
		} 
		
		else 
		
		{
			dst_locked = 1;
		}
	}
	
	// zablokuj zrodlo jezeli to jest hardware
	// tak samo jak u gory, czy trzeba czekac na plamke

	src_locked = 0;
	
	if ( src->flags & (FOX_HWSURFACE|FOX_ASYNCBLIT) ) 
	{
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;
		
		if ( video->LockHWSurface(this, src) < 0 ) 
		{
			okay = 0;
		} 
		
		else 
		
		{
			src_locked = 1;
		}
	}

	// rozkoduj cel jezeli jest zapisany jako rle
	
	if ( dst->flags & FOX_RLEACCEL ) 
	{
		FOX_UnRLESurface(dst, 1);
		dst->flags |= FOX_RLEACCEL;	// zachowaj stan
	}

	// ustaw wskazniki i kopiuj

	if ( okay  && srcrect->w && srcrect->h ) 
	{
		FOX_BlitInfo info;
		FOX_loblit RunBlit;

		// ustaw info

		
		info.s_pixels = (Uint8 *)src->pixels + src->offset +
				(Uint16)srcrect->y*src->pitch +
				(Uint16)srcrect->x*src->format->BytesPerPixel;
		
		info.s_width = srcrect->w;
		info.s_height = srcrect->h;
		
		info.s_skip=src->pitch-info.s_width*src->format->BytesPerPixel;
		
		info.d_pixels = (Uint8 *)dst->pixels + dst->offset +
				(Uint16)dstrect->y*dst->pitch +
				(Uint16)dstrect->x*dst->format->BytesPerPixel;
		
		info.d_width = dstrect->w;
		info.d_height = dstrect->h;
		
		info.d_skip=dst->pitch-info.d_width*dst->format->BytesPerPixel;
		
		info.aux_data = src->map->sw_data->aux_data;
		info.src = src->format;
		info.table = src->map->table;
		info.dst = dst->format;
		
		RunBlit = src->map->sw_data->blit;

		// software blit
		
		RunBlit(&info);
	}

	// zakoduj od nowa cel jezeli jest kodowany w rle
	
	if ( dst->flags & FOX_RLEACCEL ) 
	{
	        dst->flags &= ~FOX_RLEACCEL;	// stop 
		FOX_RLESurface(dst);
	}

	// odblokuj surface jezeli zablokowany
	
	if ( dst_locked ) 
	{

		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;
		
		video->UnlockHWSurface(this, dst);
	} 
	else
	if ( src_locked ) 
	{
	
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;
		
		video->UnlockHWSurface(this, src);
	}
	
	// gotowe ? 

	return(okay ? 0 : -1);
}

// blit copy

static void FOX_BlitCopy(FOX_BlitInfo *info)
{
	
	Uint8 *src, *dst;
	
	int w, h;
	int srcskip, dstskip;

	// poczatkowe ustawienia

	w = info->d_width*info->dst->BytesPerPixel;
	h = info->d_height;
	
	src = info->s_pixels;
	dst = info->d_pixels;
	
	srcskip = w+info->s_skip;
	dstskip = w+info->d_skip;
	
	while ( h-- ) 
	{
	
		FOX_memcpy(dst, src, w);

		src += srcskip;
		dst += dstskip;

	}
}

// blit z nakladaniem

static void FOX_BlitCopyOverlap(FOX_BlitInfo *info)
{
	
	Uint8 *src, *dst;
	
	int w, h;
	int srcskip, dstskip;

	w = info->d_width*info->dst->BytesPerPixel;
	h = info->d_height;
	
	src = info->s_pixels;
	dst = info->d_pixels;
	
	srcskip = w+info->s_skip;
	dstskip = w+info->d_skip;
	
	if ( dst < src ) 
	{
		while ( h-- ) 
		{
			FOX_memcpy(dst, src, w);
			src += srcskip;
			dst += dstskip;
		}
	} 
	else 
	{
		src += ((h-1) * srcskip);
		dst += ((h-1) * dstskip);

		while ( h-- ) 
		{
			FOX_revcpy(dst, src, w);
			src -= srcskip;
			dst -= dstskip;
		}
	}
}

// ktorego uzywamy ? 

int FOX_CalculateBlit(FOX_Surface *surface)
{

	int blit_index;

	// wyczysc wszystko 

	if ( (surface->flags & FOX_RLEACCEL) == FOX_RLEACCEL ) 
	{
		FOX_UnRLESurface(surface, 1);
	}
	
	surface->map->sw_blit = NULL;

	// sprawdz czy mozna sprzetowo
		
	surface->flags &= ~FOX_HWACCEL;
	
	if ( surface->map->identity ) 
	{
		int hw_blit_ok;

		if ( (surface->flags & FOX_HWSURFACE) == FOX_HWSURFACE ) 
		{
			if ( surface->map->dst->flags & FOX_HWSURFACE ) 
			{
				hw_blit_ok = current_video->info.blit_hw;
			} 
			else 
			{
				hw_blit_ok = 0;
			}
			
			if (hw_blit_ok && (surface->flags & FOX_SRCCOLORKEY)) 
			{
				hw_blit_ok = current_video->info.blit_hw_CC;
			}
			
			if ( hw_blit_ok && (surface->flags & FOX_SRCALPHA) ) 
			{
				hw_blit_ok = current_video->info.blit_hw_A;
			}
		
		} 
		else 
		{
			// tylko sprzetowy blit
			if ( surface->map->dst->flags & FOX_HWSURFACE ) 
			{
				hw_blit_ok = current_video->info.blit_sw;
			} 
			else 
			{
				hw_blit_ok = 0;
			}
			
			if (hw_blit_ok && (surface->flags & FOX_SRCCOLORKEY)) 
			{
				hw_blit_ok = current_video->info.blit_sw_CC;
			}
			
			if ( hw_blit_ok && (surface->flags & FOX_SRCALPHA) ) 
			{
				hw_blit_ok = current_video->info.blit_sw_A;
			}
		}
		
		// mozna - blituj 

		if ( hw_blit_ok ) 
		{
		
			FOX_VideoDevice *video = current_video;
			FOX_VideoDevice *this  = current_video;

			video->CheckHWBlit(this, surface, surface->map->dst);
		
		}
	}

	// pobierz indeks funkcji do blitowania
	// 0 - nic
	// 1 - ck (colorkey)
	// 2 - alpha
	// 3 - ck + alpha

	blit_index = 0;
	blit_index |= (!!(surface->flags & FOX_SRCCOLORKEY))      << 0;

	if ( surface->flags & FOX_SRCALPHA
	     && (surface->format->alpha != FOX_ALPHA_OPAQUE
		 || surface->format->Amask) ) 
		{
	        blit_index |= 2;
		}

	// sprawdz ident
	
	if ( surface->map->identity && blit_index == 0 ) 
	{
	        surface->map->sw_data->blit = FOX_BlitCopy;

			// zlap czy nie blitujemy w to samo miejsce
		
		if ( surface == surface->map->dst ) 
		{
		        surface->map->sw_data->blit = FOX_BlitCopyOverlap;
		}
	} 
	else 
	{
		if ( surface->format->BitsPerPixel < 8 ) 
		{
			surface->map->sw_data->blit =
			    FOX_CalculateBlit0(surface, blit_index);
		} 
		else 
		{
			switch ( surface->format->BytesPerPixel ) 
			{
			    case 1:
				surface->map->sw_data->blit =
				    FOX_CalculateBlit1(surface, blit_index);
				break;
			
				case 2:
			    
				case 3:
			    
				case 4:
				surface->map->sw_data->blit =
				    FOX_CalculateBlitN(surface, blit_index);
				break;
			    
				default:
				surface->map->sw_data->blit = NULL;
				
				break;
			}
		}
	}
	
	// sprawdz czy napewno jest funkcja blit
	
	if ( surface->map->sw_data->blit == NULL ) 
	{
		FOX_InvalidateMap(surface->map);
	
		// wyrzucic to do plikow z asercja
		
		FOX_SetError("[FOX]: kombinacja przy blitowaniu nie obslugiwana");
		return(-1); // 255, ffff
	}

	// software blit 
	
	if(surface->flags & FOX_RLEACCELOK
	   && (surface->flags & FOX_HWACCEL) != FOX_HWACCEL) 
	{

	        if(surface->map->identity
				&& (blit_index == 1
		       || (blit_index == 3 && !surface->format->Amask))) 
			{
		        if ( FOX_RLESurface(surface) == 0 )
					  surface->map->sw_blit = FOX_RLEBlit;
			} 
			
			else 
			
			if(blit_index == 2 && surface->format->Amask) 
			{
		        if ( FOX_RLESurface(surface) == 0 )
			        surface->map->sw_blit = FOX_RLEAlphaBlit;
		}
	}
	
	if ( surface->map->sw_blit == NULL ) 
	{
		surface->map->sw_blit = FOX_SoftBlit;
	}
	
	return(0);

}

// end
