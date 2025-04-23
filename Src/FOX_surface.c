// Fox v0.5a
// by Jaroslaw Rozynski

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FOX_error.h"
#include "FOX_video.h"
#include "FOX_sysvideo.h"
#include "cursor_c.h"
#include "blit.h"
#include "FOX_RLEaccel_c.h"
#include "FOX_pixels_c.h"
#include "FOX_memops.h"
#include "FOX_leaks.h"

/* Public routines */
/*
 * Create an empty RGB surface of the appropriate depth
 */
FOX_Surface * FOX_CreateRGBSurface (Uint32 flags,
			int width, int height, int depth,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;
	FOX_Surface *screen;
	FOX_Surface *surface;

	/* Check to see if we desire the surface in video memory */
	if ( video ) {
		screen = FOX_PublicSurface;
	} else {
		screen = NULL;
	}
	if ( screen && ((screen->flags&FOX_HWSURFACE) == FOX_HWSURFACE) ) {
		if ( (flags&(FOX_SRCCOLORKEY|FOX_SRCALPHA)) != 0 ) {
			flags |= FOX_HWSURFACE;
		}
		if ( (flags & FOX_SRCCOLORKEY) == FOX_SRCCOLORKEY ) {
			if ( ! current_video->info.blit_hw_CC ) {
				flags &= ~FOX_HWSURFACE;
			}
		}
		if ( (flags & FOX_SRCALPHA) == FOX_SRCALPHA ) {
			if ( ! current_video->info.blit_hw_A ) {
				flags &= ~FOX_HWSURFACE;
			}
		}
	} else {
		flags &= ~FOX_HWSURFACE;
	}

	/* Allocate the surface */
	surface = (FOX_Surface *)malloc(sizeof(*surface));
	if ( surface == NULL ) {
		FOX_OutOfMemory();
		return(NULL);
	}
	surface->flags = FOX_SWSURFACE;
	if ( (flags & FOX_HWSURFACE) == FOX_HWSURFACE ) {
		depth = screen->format->BitsPerPixel;
		Rmask = screen->format->Rmask;
		Gmask = screen->format->Gmask;
		Bmask = screen->format->Bmask;
		Amask = screen->format->Amask;
	}
	surface->format = FOX_AllocFormat(depth, Rmask, Gmask, Bmask, Amask);
	if ( surface->format == NULL ) {
		free(surface);
		return(NULL);
	}
	if ( Amask ) {
		surface->flags |= FOX_SRCALPHA;
	}
	surface->w = width;
	surface->h = height;
	surface->pitch = FOX_CalculatePitch(surface);
	surface->pixels = NULL;
	surface->offset = 0;
	surface->hwdata = NULL;
	surface->locked = 0;
	surface->map = NULL;
	surface->format_version = 0;
	FOX_SetClipRect(surface, NULL);

	/* Get the pixels */
	if ( ((flags&FOX_HWSURFACE) == FOX_SWSURFACE) || 
				(video->AllocHWSurface(this, surface) < 0) ) {
		if ( surface->w && surface->h ) {
			surface->pixels = malloc(surface->h*surface->pitch);
			if ( surface->pixels == NULL ) {
				FOX_FreeSurface(surface);
				FOX_OutOfMemory();
				return(NULL);
			}
			/* This is important for bitmaps */
			memset(surface->pixels, 0, surface->h*surface->pitch);
		}
	}

	/* Allocate an empty mapping */
	surface->map = FOX_AllocBlitMap();
	if ( surface->map == NULL ) {
		FOX_FreeSurface(surface);
		return(NULL);
	}

	/* The surface is ready to go */
	surface->refcount = 1;
#ifdef CHECK_LEAKS
	++surfaces_allocated;
#endif
	return(surface);
}
/*
 * Create an RGB surface from an existing memory buffer
 */
FOX_Surface * FOX_CreateRGBSurfaceFrom (void *pixels,
			int width, int height, int depth, int pitch,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
	FOX_Surface *surface;

	surface = FOX_CreateRGBSurface(FOX_SWSURFACE, 0, 0, depth,
	                               Rmask, Gmask, Bmask, Amask);
	if ( surface != NULL ) {
		surface->flags |= FOX_PREALLOC;
		surface->pixels = pixels;
		surface->w = width;
		surface->h = height;
		surface->pitch = pitch;
		FOX_SetClipRect(surface, NULL);
	}
	return(surface);
}
/*
 * Set the color key in a blittable surface
 */
int FOX_SetColorKey (FOX_Surface *surface, Uint32 flag, Uint32 key)
{
	/* Sanity check the flag as it gets passed in */
	if ( flag & FOX_SRCCOLORKEY ) {
		if ( flag & (FOX_RLEACCEL|FOX_RLEACCELOK) ) {
			flag = (FOX_SRCCOLORKEY | FOX_RLEACCELOK);
		} else {
			flag = FOX_SRCCOLORKEY;
		}
	} else {
		flag = 0;
	}

	/* Optimize away operations that don't change anything */
	if ( (flag == (surface->flags & (FOX_SRCCOLORKEY|FOX_RLEACCELOK))) &&
	     (key == surface->format->colorkey) ) {
		return(0);
	}

	/* UnRLE surfaces before we change the colorkey */
	if ( surface->flags & FOX_RLEACCEL ) {
	        FOX_UnRLESurface(surface, 1);
	}

	if ( flag ) {
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;


		surface->flags |= FOX_SRCCOLORKEY;
		surface->format->colorkey = key;
		if ( (surface->flags & FOX_HWACCEL) == FOX_HWACCEL ) {
			if ( (video->SetHWColorKey == NULL) ||
			     (video->SetHWColorKey(this, surface, key) < 0) ) {
				surface->flags &= ~FOX_HWACCEL;
			}
		}
		if ( flag & FOX_RLEACCELOK ) {
			surface->flags |= FOX_RLEACCELOK;
		} else {
			surface->flags &= ~FOX_RLEACCELOK;
		}
	} else {
		surface->flags &= ~(FOX_SRCCOLORKEY|FOX_RLEACCELOK);
		surface->format->colorkey = 0;
	}
	FOX_InvalidateMap(surface->map);
	return(0);
}
int FOX_SetAlpha (FOX_Surface *surface, Uint32 flag, Uint8 value)
{
	Uint32 oldflags = surface->flags;
	Uint32 oldalpha = surface->format->alpha;

	/* Sanity check the flag as it gets passed in */
	if ( flag & FOX_SRCALPHA ) {
		if ( flag & (FOX_RLEACCEL|FOX_RLEACCELOK) ) {
			flag = (FOX_SRCALPHA | FOX_RLEACCELOK);
		} else {
			flag = FOX_SRCALPHA;
		}
	} else {
		flag = 0;
	}

	/* Optimize away operations that don't change anything */
	if ( (flag == (surface->flags & (FOX_SRCALPHA|FOX_RLEACCELOK))) &&
	     (!flag || value == oldalpha) ) {
		return(0);
	}

	if(!(flag & FOX_RLEACCELOK) && (surface->flags & FOX_RLEACCEL))
		FOX_UnRLESurface(surface, 1);

	if ( flag ) {
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;

		surface->flags |= FOX_SRCALPHA;
		surface->format->alpha = value;
		if ( (surface->flags & FOX_HWACCEL) == FOX_HWACCEL ) {
			if ( (video->SetHWAlpha == NULL) ||
			     (video->SetHWAlpha(this, surface, value) < 0) ) {
				surface->flags &= ~FOX_HWACCEL;
			}
		}
		if ( flag & FOX_RLEACCELOK ) {
		        surface->flags |= FOX_RLEACCELOK;
		} else {
		        surface->flags &= ~FOX_RLEACCELOK;
		}
	} else {
		surface->flags &= ~FOX_SRCALPHA;
		surface->format->alpha = FOX_ALPHA_OPAQUE;
	}
	/*
	 * The representation for software surfaces is independent of
	 * per-surface alpha, so no need to invalidate the blit mapping
	 * if just the alpha value was changed. (If either is 255, we still
	 * need to invalidate.)
	 */
	if((surface->flags & FOX_HWACCEL) == FOX_HWACCEL
	   || oldflags != surface->flags
	   || (((oldalpha + 1) ^ (value + 1)) & 0x100))
		FOX_InvalidateMap(surface->map);
	return(0);
}

/*
 * A function to calculate the intersection of two rectangles:
 * return true if the rectangles intersect, false otherwise
 */
static __inline__
FOX_bool FOX_IntersectRect(const FOX_Rect *A, const FOX_Rect *B, FOX_Rect *intersection)
{
	int Amin, Amax, Bmin, Bmax;

	/* Horizontal intersection */
	Amin = A->x;
	Amax = Amin + A->w;
	Bmin = B->x;
	Bmax = Bmin + B->w;
	if(Bmin > Amin)
	        Amin = Bmin;
	intersection->x = Amin;
	if(Bmax < Amax)
	        Amax = Bmax;
	intersection->w = Amax - Amin > 0 ? Amax - Amin : 0;

	/* Vertical intersection */
	Amin = A->y;
	Amax = Amin + A->h;
	Bmin = B->y;
	Bmax = Bmin + B->h;
	if(Bmin > Amin)
	        Amin = Bmin;
	intersection->y = Amin;
	if(Bmax < Amax)
	        Amax = Bmax;
	intersection->h = Amax - Amin > 0 ? Amax - Amin : 0;

	return (intersection->w && intersection->h);
}
/*
 * Set the clipping rectangle for a blittable surface
 */
FOX_bool FOX_SetClipRect(FOX_Surface *surface, const FOX_Rect *rect)
{
	FOX_Rect full_rect;

	/* Don't do anything if there's no surface to act on */
	if ( ! surface ) {
		return FOX_FALSE;
	}

	/* Set up the full surface rectangle */
	full_rect.x = 0;
	full_rect.y = 0;
	full_rect.w = surface->w;
	full_rect.h = surface->h;

	/* Set the clipping rectangle */
	if ( ! rect ) {
		surface->clip_rect = full_rect;
		return 1;
	}
	return FOX_IntersectRect(rect, &full_rect, &surface->clip_rect);
}
void FOX_GetClipRect(FOX_Surface *surface, FOX_Rect *rect)
{
	if ( surface && rect ) {
		*rect = surface->clip_rect;
	}
}
/* 
 * Set up a blit between two surfaces -- split into three parts:
 * The upper part, FOX_UpperBlit(), performs clipping and rectangle 
 * verification.  The lower part is a pointer to a low level
 * accelerated blitting function.
 *
 * These parts are separated out and each used internally by this 
 * library in the optimimum places.  They are exported so that if
 * you know exactly what you are doing, you can optimize your code
 * by calling the one(s) you need.
 */
int FOX_LowerBlit (FOX_Surface *src, FOX_Rect *srcrect,
				FOX_Surface *dst, FOX_Rect *dstrect)
{
	FOX_blit do_blit;

	/* Check to make sure the blit mapping is valid */
	if ( (src->map->dst != dst) ||
             (src->map->dst->format_version != src->map->format_version) ) {
		if ( FOX_MapSurface(src, dst) < 0 ) {
			return(-1);
		}
	}

	/* Figure out which blitter to use */
	if ( (src->flags & FOX_HWACCEL) == FOX_HWACCEL ) {
		do_blit = src->map->hw_blit;
	} else {
		do_blit = src->map->sw_blit;
	}
	return(do_blit(src, srcrect, dst, dstrect));
}


int FOX_UpperBlit (FOX_Surface *src, FOX_Rect *srcrect,
		   FOX_Surface *dst, FOX_Rect *dstrect)
{
        FOX_Rect fulldst;
	int srcx, srcy, w, h;

	/* Make sure the surfaces aren't locked */
	if ( ! src || ! dst ) {
		FOX_SetError("FOX_UpperBlit: passed a NULL surface");
		return(-1);
	}
	if ( src->locked || dst->locked ) {
		FOX_SetError("Surfaces must not be locked during blit");
		return(-1);
	}

	/* If the destination rectangle is NULL, use the entire dest surface */
	if ( dstrect == NULL ) {
	        fulldst.x = fulldst.y = 0;
		dstrect = &fulldst;
	}

	/* clip the source rectangle to the source surface */
	if(srcrect) {
	        int maxw, maxh;
	
		srcx = srcrect->x;
		w = srcrect->w;
		if(srcx < 0) {
		        w += srcx;
			dstrect->x -= srcx;
			srcx = 0;
		}
		maxw = src->w - srcx;
		if(maxw < w)
			w = maxw;

		srcy = srcrect->y;
		h = srcrect->h;
		if(srcy < 0) {
		        h += srcy;
			dstrect->y -= srcy;
			srcy = 0;
		}
		maxh = src->h - srcy;
		if(maxh < h)
			h = maxh;
	    
	} else {
	        srcx = srcy = 0;
		w = src->w;
		h = src->h;
	}

	/* clip the destination rectangle against the clip rectangle */
	{
	        FOX_Rect *clip = &dst->clip_rect;
		int dx, dy;

		dx = clip->x - dstrect->x;
		if(dx > 0) {
			w -= dx;
			dstrect->x += dx;
			srcx += dx;
		}
		dx = dstrect->x + w - clip->x - clip->w;
		if(dx > 0)
			w -= dx;

		dy = clip->y - dstrect->y;
		if(dy > 0) {
			h -= dy;
			dstrect->y += dy;
			srcy += dy;
		}
		dy = dstrect->y + h - clip->y - clip->h;
		if(dy > 0)
			h -= dy;
	}

	if(w > 0 && h > 0) {
	        FOX_Rect sr;
	        sr.x = srcx;
		sr.y = srcy;
		sr.w = dstrect->w = w;
		sr.h = dstrect->h = h;
		return FOX_LowerBlit(src, &sr, dst, dstrect);
	}
	dstrect->w = dstrect->h = 0;
	return 0;
}

/* 
 * This function performs a fast fill of the given rectangle with 'color'
 */
int FOX_FillRect(FOX_Surface *dst, FOX_Rect *dstrect, Uint32 color)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;
	int x, y;
	Uint8 *row;

	/* If 'dstrect' == NULL, then fill the whole surface */
	if ( dstrect ) {
		/* Perform clipping */
		if ( !FOX_IntersectRect(dstrect, &dst->clip_rect, dstrect) ) {
			return(0);
		}
	} else {
		dstrect = &dst->clip_rect;
	}

	/* Check for hardware acceleration */
	if ( ((dst->flags & FOX_HWSURFACE) == FOX_HWSURFACE) &&
					video->info.blit_fill ) {
		return(video->FillHWRect(this, dst, dstrect, color));
	}

	/* Perform software fill */
	if ( FOX_LockSurface(dst) != 0 ) {
		return(-1);
	}
	row = (Uint8 *)dst->pixels+dstrect->y*dst->pitch+
			dstrect->x*dst->format->BytesPerPixel;
	if ( dst->format->palette || (color == 0) ) {
		x = dstrect->w*dst->format->BytesPerPixel;
		if ( !color && !((long)row&3) && !(x&3) && !(dst->pitch&3) ) {
			int n = x >> 2;
			for ( y=dstrect->h; y; --y ) {
				FOX_memset4(row, 0, n);
				row += dst->pitch;
			}
		} else {
#ifdef __powerpc__
			/*
			 * memset() on PPC (both glibc and codewarrior) uses
			 * the dcbz (Data Cache Block Zero) instruction, which
			 * causes an alignment exception if the destination is
			 * uncachable, so only use it on software surfaces
			 */
			if((dst->flags & FOX_HWSURFACE) == FOX_HWSURFACE) {
				if(dstrect->w >= 8) {
					/*
					 * 64-bit stores are probably most
					 * efficient to uncached video memory
					 */
					double fill;
					memset(&fill, color, (sizeof fill));
					for(y = dstrect->h; y; y--) {
						Uint8 *d = row;
						unsigned n = x;
						unsigned nn;
						Uint8 c = color;
						double f = fill;
						while((unsigned long)d
						      & (sizeof(double) - 1)) {
							*d++ = c;
							n--;
						}
						nn = n / (sizeof(double) * 4);
						while(nn) {
							((double *)d)[0] = f;
							((double *)d)[1] = f;
							((double *)d)[2] = f;
							((double *)d)[3] = f;
							d += 4*sizeof(double);
							nn--;
						}
						n &= ~(sizeof(double) * 4 - 1);
						nn = n / sizeof(double);
						while(nn) {
							*(double *)d = f;
							d += sizeof(double);
							nn--;
						}
						n &= ~(sizeof(double) - 1);
						while(n) {
							*d++ = c;
							n--;
						}
						row += dst->pitch;
					}
				} else {
					/* narrow boxes */
					for(y = dstrect->h; y; y--) {
						Uint8 *d = row;
						Uint8 c = color;
						int n = x;
						while(n) {
							*d++ = c;
							n--;
						}
						row += dst->pitch;
					}
				}
			} else
#endif /* __powerpc__ */
			{
				for(y = dstrect->h; y; y--) {
					memset(row, color, x);
					row += dst->pitch;
				}
			}
		}
	} else {
		switch (dst->format->BytesPerPixel) {
		    case 2:
			for ( y=dstrect->h; y; --y ) {
				Uint16 *pixels = (Uint16 *)row;
				Uint16 c = color;
				Uint32 cc = (Uint32)c << 16 | c;
				int n = dstrect->w;
				if((unsigned long)pixels & 3) {
					*pixels++ = c;
					n--;
				}
				if(n >> 1)
					FOX_memset4(pixels, cc, n >> 1);
				if(n & 1)
					pixels[n - 1] = c;
				row += dst->pitch;
			}
			break;

		    case 3:
			if(FOX_BYTEORDER == FOX_BIG_ENDIAN)
				color <<= 8;
			for ( y=dstrect->h; y; --y ) {
				Uint8 *pixels = row;
				for ( x=dstrect->w; x; --x ) {
					memcpy(pixels, &color, 3);
					pixels += 3;
				}
				row += dst->pitch;
			}
			break;

		    case 4:
			for(y = dstrect->h; y; --y) {
				FOX_memset4(row, color, dstrect->w);
				row += dst->pitch;
			}
			break;
		}
	}
	FOX_UnlockSurface(dst);

	/* We're done! */
	return(0);
}

/*
 * Lock a surface to directly access the pixels
 * -- Do not call this from any blit function, as FOX_DrawCursor() may recurse
 *    Instead, use:
 *    if ( (surface->flags & FOX_HWSURFACE) == FOX_HWSURFACE )
 *               video->LockHWSurface(video, surface);
 */
int FOX_LockSurface (FOX_Surface *surface)
{
	if ( ! surface->locked ) {
		/* Perform the lock */
		if ( surface->flags & (FOX_HWSURFACE|FOX_ASYNCBLIT) ) {
			FOX_VideoDevice *video = current_video;
			FOX_VideoDevice *this  = current_video;
			if ( video->LockHWSurface(this, surface) < 0 ) {
				return(-1);
			}
		}
		if ( surface->flags & FOX_RLEACCEL ) {
			FOX_UnRLESurface(surface, 1);
			surface->flags |= FOX_RLEACCEL;	/* save accel'd state */
		}
		/* This needs to be done here in case pixels changes value */
		surface->pixels = (Uint8 *)surface->pixels + surface->offset;
	}

	/* Increment the surface lock count, for recursive locks */
	++surface->locked;

	/* Ready to go.. */
	return(0);
}
/*
 * Unlock a previously locked surface
 * -- Do not call this from any blit function, as FOX_DrawCursor() may recurse
 *    Instead, use:
 *    if ( (surface->flags & FOX_HWSURFACE) == FOX_HWSURFACE )
 *               video->UnlockHWSurface(video, surface);
 */
void FOX_UnlockSurface (FOX_Surface *surface)
{
	/* Only perform an unlock if we are locked */
	if ( ! surface->locked || (--surface->locked > 0) ) {
		return;
	}

	/* Perform the unlock */
	surface->pixels = (Uint8 *)surface->pixels - surface->offset;

	/* Unlock hardware or accelerated surfaces */
	if ( surface->flags & (FOX_HWSURFACE|FOX_ASYNCBLIT) ) {
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;
		video->UnlockHWSurface(this, surface);
	} else {
		/* Update RLE encoded surface with new data */
		if ( (surface->flags & FOX_RLEACCEL) == FOX_RLEACCEL ) {
		        surface->flags &= ~FOX_RLEACCEL; /* stop lying */
			FOX_RLESurface(surface);
		}
	}
}

/* 
 * Convert a surface into the specified pixel format.
 */
FOX_Surface * FOX_ConvertSurface (FOX_Surface *surface,
					FOX_PixelFormat *format, Uint32 flags)
{
	FOX_Surface *convert;
	Uint32 colorkey = 0;
	Uint8 alpha = 0;
	Uint32 surface_flags;
	FOX_Rect bounds;

	/* Check for empty destination palette! (results in empty image) */
	if ( format->palette != NULL ) {
		int i;
		for ( i=0; i<format->palette->ncolors; ++i ) {
			if ( (format->palette->colors[i].r != 0) ||
			     (format->palette->colors[i].g != 0) ||
			     (format->palette->colors[i].b != 0) )
				break;
		}
		if ( i == format->palette->ncolors ) {
			FOX_SetError("Empty destination palette");
			return(NULL);
		}
	}

	/* Create a new surface with the desired format */
	convert = FOX_CreateRGBSurface(flags,
				surface->w, surface->h, format->BitsPerPixel,
		format->Rmask, format->Gmask, format->Bmask, format->Amask);
	if ( convert == NULL ) {
		return(NULL);
	}

	/* Copy the palette if any */
	if ( format->palette && convert->format->palette ) {
		memcpy(convert->format->palette->colors,
				format->palette->colors,
				format->palette->ncolors*sizeof(FOX_Color));
		convert->format->palette->ncolors = format->palette->ncolors;
	}

	/* Save the original surface color key and alpha */
	surface_flags = surface->flags;
	if ( (surface_flags & FOX_SRCCOLORKEY) == FOX_SRCCOLORKEY ) {
		/* Convert colourkeyed surfaces to RGBA if requested */
		if((flags & FOX_SRCCOLORKEY) != FOX_SRCCOLORKEY
		   && format->Amask) {
			surface_flags &= ~FOX_SRCCOLORKEY;
		} else {
			colorkey = surface->format->colorkey;
			FOX_SetColorKey(surface, 0, 0);
		}
	}
	if ( (surface_flags & FOX_SRCALPHA) == FOX_SRCALPHA ) {
		alpha = surface->format->alpha;
		FOX_SetAlpha(surface, 0, 0);
	}

	/* Copy over the image data */
	bounds.x = 0;
	bounds.y = 0;
	bounds.w = surface->w;
	bounds.h = surface->h;
	FOX_LowerBlit(surface, &bounds, convert, &bounds);

	/* Clean up the original surface, and update converted surface */
	if ( convert != NULL ) {
		FOX_SetClipRect(convert, &surface->clip_rect);
	}
	if ( (surface_flags & FOX_SRCCOLORKEY) == FOX_SRCCOLORKEY ) {
		Uint32 cflags = surface_flags&(FOX_SRCCOLORKEY|FOX_RLEACCELOK);
		if ( convert != NULL ) {
			Uint8 keyR, keyG, keyB;

			FOX_GetRGB(colorkey,surface->format,&keyR,&keyG,&keyB);
			FOX_SetColorKey(convert, cflags|(flags&FOX_RLEACCELOK),
				FOX_MapRGB(convert->format, keyR, keyG, keyB));
		}
		FOX_SetColorKey(surface, cflags, colorkey);
	}
	if ( (surface_flags & FOX_SRCALPHA) == FOX_SRCALPHA ) {
		Uint32 aflags = surface_flags&(FOX_SRCALPHA|FOX_RLEACCELOK);
		if ( convert != NULL ) {
		        FOX_SetAlpha(convert, aflags|(flags&FOX_RLEACCELOK),
				alpha);
		}
		FOX_SetAlpha(surface, aflags, alpha);
	}

	/* We're ready to go! */
	return(convert);
}

/*
 * Free a surface created by the above function.
 */
void FOX_FreeSurface (FOX_Surface *surface)
{
	/* Free anything that's not NULL, and not the screen surface */
	if ((surface == NULL) ||
	    (current_video &&
	    ((surface == FOX_ShadowSurface)||(surface == FOX_VideoSurface)))) {
		return;
	}
	if ( --surface->refcount > 0 ) {
		return;
	}
	if ( (surface->flags & FOX_RLEACCEL) == FOX_RLEACCEL ) {
	        FOX_UnRLESurface(surface, 0);
	}
	if ( surface->format ) {
		FOX_FreeFormat(surface->format);
		surface->format = NULL;
	}
	if ( surface->map != NULL ) {
		FOX_FreeBlitMap(surface->map);
		surface->map = NULL;
	}
	if ( (surface->flags & FOX_HWSURFACE) == FOX_HWSURFACE ) {
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;
		video->FreeHWSurface(this, surface);
	}
	if ( surface->pixels &&
	     ((surface->flags & FOX_PREALLOC) != FOX_PREALLOC) ) {
		free(surface->pixels);
	}
	free(surface);
#ifdef CHECK_LEAKS
	--surfaces_allocated;
#endif
}

// end
