// Fox v0.5a
// by Jaroslaw Rozynski
//===
// TODO:

/* The high-level video driver subsystem */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FOX.h"
#include "FOX_error.h"
#include "FOX_video.h"
#include "FOX_events.h"
#include "FOX_mutex.h"
#include "FOX_sysvideo.h"
#include "FOX_sysevents.h"
#include "FOX_blit.h"
#include "FOX_pixels_c.h"
#include "FOX_events_c.h"
#include "FOX_cursor_c.h"

/* Available video drivers */
static VideoBootStrap *bootstrap[] = {
//#ifdef ENABLE_DIRECTFB
//	&DirectFB_bootstrap,
//#endif
//#ifdef ENABLE_VGL
//	&VGL_bootstrap,
//#endif
//#ifdef ENABLE_DIRECTX
	&DIRECTX_bootstrap,
//#endif
//#ifdef ENABLE_WINDIB
//	&WINDIB_bootstrap,
//#endif
//#ifdef ENABLE_BWINDOW
//	&BWINDOW_bootstrap,
//#endif
//#ifdef ENABLE_TOOLBOX
//	&TOOLBOX_bootstrap,
//#endif
//#ifdef ENABLE_DRAWSPROCKET
//	&DSp_bootstrap,
//#endif
//#ifdef ENABLE_EPOC
//	&EPOC_bootstrap,
//#endif
//#ifdef ENABLE_DUMMYVIDEO
//	&DUMMY_bootstrap,
//#endif
	NULL
};

FOX_VideoDevice *current_video = NULL;

/* Various local functions */
int FOX_VideoInit(const char *driver_name, Uint32 flags);
void FOX_VideoQuit(void);
void FOX_GL_UpdateRectsLock(FOX_VideoDevice* this, int numrects, FOX_Rect* rects);

static FOX_GrabMode FOX_WM_GrabInputOff(void);


/*
 * Initialize the video and event subsystems -- determine native pixel format
 */
int FOX_VideoInit (const char *driver_name, Uint32 flags)
{
	FOX_VideoDevice *video;
	int index;
	int i;
	FOX_PixelFormat vformat;
	Uint32 video_flags;

	/* Toggle the event thread flags, based on OS requirements */
#if defined(MUST_THREAD_EVENTS)
	flags |= FOX_INIT_EVENTTHREAD;
#elif defined(CANT_THREAD_EVENTS)
	if ( (flags & FOX_INIT_EVENTTHREAD) == FOX_INIT_EVENTTHREAD ) {
		FOX_SetError("OS doesn't support threaded events");
		return(-1);
	}
#endif

	/* Check to make sure we don't overwrite 'current_video' */
	if ( current_video != NULL ) {
		FOX_VideoQuit();
	}

	/* Select the proper video driver */
	index = 0;
	video = NULL;
	if ( driver_name != NULL ) {
#if 0	/* This will be replaced with a better driver selection API */
		if ( strrchr(driver_name, ':') != NULL ) {
			index = atoi(strrchr(driver_name, ':')+1);
		}
#endif
		for ( i=0; bootstrap[i]; ++i ) {
			if ( strncmp(bootstrap[i]->name, driver_name,
			             strlen(bootstrap[i]->name)) == 0 ) {
				if ( bootstrap[i]->available() ) {
					video = bootstrap[i]->create(index);
					break;
				}
			}
		}
	} else {
		for ( i=0; bootstrap[i]; ++i ) {
			if ( bootstrap[i]->available() ) {
				video = bootstrap[i]->create(index);
				if ( video != NULL ) {
					break;
				}
			}
		}
	}
	if ( video == NULL ) {
		FOX_SetError("No available video device");
		return(-1);
	}
	current_video = video;
	current_video->name = bootstrap[i]->name;

	/* Do some basic variable initialization */
	video->screen = NULL;
	video->shadow = NULL;
	video->visible = NULL;
	video->physpal = NULL;
	video->gammacols = NULL;
	video->gamma = NULL;
	video->wm_title = NULL;
	video->wm_icon  = NULL;
	video->offset_x = 0;
	video->offset_y = 0;
	memset(&video->info, 0, (sizeof video->info));
	
	/* Initialize the video subsystem */
	memset(&vformat, 0, sizeof(vformat));
	if ( video->VideoInit(video, &vformat) < 0 ) {
		FOX_VideoQuit();
		return(-1);
	}

	/* Create a zero sized video surface of the appropriate format */
	video_flags = FOX_SWSURFACE;
	FOX_VideoSurface = FOX_CreateRGBSurface(video_flags, 0, 0,
				vformat.BitsPerPixel,
				vformat.Rmask, vformat.Gmask, vformat.Bmask, 0);
	if ( FOX_VideoSurface == NULL ) {
		FOX_VideoQuit();
		return(-1);
	}
	FOX_PublicSurface = NULL;	/* Until FOX_SetVideoMode() */

#if 0 /* Don't change the current palette - may be used by other programs.
       * The application can't do anything with the display surface until
       * a video mode has been set anyway. :)
       */
	/* If we have a palettized surface, create a default palette */
	if ( FOX_VideoSurface->format->palette ) {
	        FOX_PixelFormat *vf = FOX_VideoSurface->format;
		FOX_DitherColors(vf->palette->colors, vf->BitsPerPixel);
		video->SetColors(video,
				 0, vf->palette->ncolors, vf->palette->colors);
	}
#endif
	video->info.vfmt = FOX_VideoSurface->format;

	/* Start the event loop */
	if ( FOX_StartEventLoop(flags) < 0 ) {
		FOX_VideoQuit();
		return(-1);
	}
	FOX_CursorInit(flags & FOX_INIT_EVENTTHREAD);

	/* We're ready to go! */
	return(0);
}

char *FOX_VideoDriverName(char *namebuf, int maxlen)
{
	if ( current_video != NULL ) {
		strncpy(namebuf, current_video->name, maxlen-1);
		namebuf[maxlen-1] = '\0';
		return(namebuf);
	}
	return(NULL);
}

/*
 * Get the current display surface
 */
FOX_Surface *FOX_GetVideoSurface(void)
{
	FOX_Surface *visible;

	visible = NULL;
	if ( current_video ) {
		visible = current_video->visible;
	}
	return(visible);
}

/*
 * Get the current information about the video hardware
 */
const FOX_VideoInfo *FOX_GetVideoInfo(void)
{
	const FOX_VideoInfo *info;

	info = NULL;
	if ( current_video ) {
		info = &current_video->info;
	}
	return(info);
}

/*
 * Return a pointer to an array of available screen dimensions for the
 * given format, sorted largest to smallest.  Returns NULL if there are
 * no dimensions available for a particular format, or (FOX_Rect **)-1
 * if any dimension is okay for the given format.  If 'format' is NULL,
 * the mode list will be for the format given by FOX_GetVideoInfo()->vfmt
 */
FOX_Rect ** FOX_ListModes (FOX_PixelFormat *format, Uint32 flags)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;
	FOX_Rect **modes;

	modes = NULL;
	if ( FOX_VideoSurface ) {
		if ( format == NULL ) {
			format = FOX_VideoSurface->format;
		}
		modes = video->ListModes(this, format, flags);
	}
	return(modes);
}

/*
 * Check to see if a particular video mode is supported.
 * It returns 0 if the requested mode is not supported under any bit depth,
 * or returns the bits-per-pixel of the closest available mode with the
 * given width and height.  If this bits-per-pixel is different from the
 * one used when setting the video mode, FOX_SetVideoMode() will succeed,
 * but will emulate the requested bits-per-pixel with a shadow surface.
 */
static Uint8 FOX_closest_depths[4][8] = {
	/* 8 bit closest depth ordering */
	{ 0, 8, 16, 15, 32, 24, 0, 0 },
	/* 15,16 bit closest depth ordering */
	{ 0, 16, 15, 32, 24, 8, 0, 0 },
	/* 24 bit closest depth ordering */
	{ 0, 24, 32, 16, 15, 8, 0, 0 },
	/* 32 bit closest depth ordering */
	{ 0, 32, 16, 15, 24, 8, 0, 0 }
};

int FOX_VideoModeOK (int width, int height, int bpp, Uint32 flags) 
{
	int table, b, i;
	int supported;
	FOX_PixelFormat format;
	FOX_Rect **sizes;

	/* Currently 1 and 4 bpp are not supported */
	if ( bpp < 8 || bpp > 32 ) {
		return(0);
	}
	if ( (width == 0) || (height == 0) ) {
		return(0);
	}

	/* Search through the list valid of modes */
	memset(&format, 0, sizeof(format));
	supported = 0;
	table = ((bpp+7)/8)-1;
	FOX_closest_depths[table][0] = bpp;
	FOX_closest_depths[table][7] = 0;
	for ( b = 0; !supported && FOX_closest_depths[table][b]; ++b ) {
		format.BitsPerPixel = FOX_closest_depths[table][b];
		sizes = FOX_ListModes(&format, flags);
		if ( sizes == (FOX_Rect **)0 ) {
			/* No sizes supported at this bit-depth */
			continue;
		} else 
#ifdef macintosh /* MPW optimization bug? */
		if ( (sizes == (FOX_Rect **)0xFFFFFFFF) ||
#else
		if ( (sizes == (FOX_Rect **)-1) ||
#endif
		     current_video->handles_any_size ) {
			/* Any size supported at this bit-depth */
			supported = 1;
			continue;
		} else
		for ( i=0; sizes[i]; ++i ) {
			if ((sizes[i]->w == width) && (sizes[i]->h == height)) {
				supported = 1;
				break;
			}
		}
	}
	if ( supported ) {
		--b;
		return(FOX_closest_depths[table][b]);
	} else {
		return(0);
	}
}

/*
 * Get the closest non-emulated video mode to the one requested
 */
static int FOX_GetVideoMode (int *w, int *h, int *BitsPerPixel, Uint32 flags)
{
	int table, b, i;
	int supported;
	int native_bpp;
	FOX_PixelFormat format;
	FOX_Rect **sizes;

	/* Try the original video mode, get the closest depth */
	native_bpp = FOX_VideoModeOK(*w, *h, *BitsPerPixel, flags);
	if ( native_bpp == *BitsPerPixel ) {
		return(1);
	}
	if ( native_bpp > 0 ) {
		*BitsPerPixel = native_bpp;
		return(1);
	}

	/* No exact size match at any depth, look for closest match */
	memset(&format, 0, sizeof(format));
	supported = 0;
	table = ((*BitsPerPixel+7)/8)-1;
	FOX_closest_depths[table][0] = *BitsPerPixel;
	FOX_closest_depths[table][7] = FOX_VideoSurface->format->BitsPerPixel;
	for ( b = 0; !supported && FOX_closest_depths[table][b]; ++b ) {
		format.BitsPerPixel = FOX_closest_depths[table][b];
		sizes = FOX_ListModes(&format, flags);
		if ( sizes == (FOX_Rect **)0 ) {
			/* No sizes supported at this bit-depth */
			continue;
		}
		for ( i=0; sizes[i]; ++i ) {
			if ((sizes[i]->w < *w) || (sizes[i]->h < *h)) {
				if ( i > 0 ) {
					--i;
					*w = sizes[i]->w;
					*h = sizes[i]->h;
					*BitsPerPixel = FOX_closest_depths[table][b];
					supported = 1;
				} else {
					/* Largest mode too small... */;
				}
				break;
			}
		}
		if ( (i > 0) && ! sizes[i] ) {
			/* The smallest mode was larger than requested, OK */
			--i;
			*w = sizes[i]->w;
			*h = sizes[i]->h;
			*BitsPerPixel = FOX_closest_depths[table][b];
			supported = 1;
		}
	}
	if ( ! supported ) {
		FOX_SetError("No video mode large enough for %dx%d", *w, *h);
	}
	return(supported);
}

/* This should probably go somewhere else -- like FOX_surface.c */
static void FOX_ClearSurface(FOX_Surface *surface)
{
	Uint32 black;

	black = FOX_MapRGB(surface->format, 0, 0, 0);
	FOX_FillRect(surface, NULL, black);
	if ((surface->flags&FOX_HWSURFACE) && (surface->flags&FOX_DOUBLEBUF)) {
		FOX_Flip(surface);
		FOX_FillRect(surface, NULL, black);
	}
	FOX_Flip(surface);
}

/*
 * Create a shadow surface suitable for fooling the app. :-)
 */
static void FOX_CreateShadowSurface(int depth)
{
	Uint32 Rmask, Gmask, Bmask;

	/* Allocate the shadow surface */
	if ( depth == (FOX_VideoSurface->format)->BitsPerPixel ) {
		Rmask = (FOX_VideoSurface->format)->Rmask;
		Gmask = (FOX_VideoSurface->format)->Gmask;
		Bmask = (FOX_VideoSurface->format)->Bmask;
	} else {
		Rmask = Gmask = Bmask = 0;
	}
	FOX_ShadowSurface = FOX_CreateRGBSurface(FOX_SWSURFACE,
				FOX_VideoSurface->w, FOX_VideoSurface->h,
						depth, Rmask, Gmask, Bmask, 0);
	if ( FOX_ShadowSurface == NULL ) {
		return;
	}

	/* 8-bit shadow surfaces report that they have exclusive palette */
	if ( FOX_ShadowSurface->format->palette ) {
		FOX_ShadowSurface->flags |= FOX_HWPALETTE;
		if ( depth == (FOX_VideoSurface->format)->BitsPerPixel ) {
			memcpy(FOX_ShadowSurface->format->palette->colors,
				FOX_VideoSurface->format->palette->colors,
				FOX_VideoSurface->format->palette->ncolors*
							sizeof(FOX_Color));
		} else {
			FOX_DitherColors(
			FOX_ShadowSurface->format->palette->colors, depth);
		}
	}

	/* If the video surface is resizable, the shadow should say so */
	if ( (FOX_VideoSurface->flags & FOX_RESIZABLE) == FOX_RESIZABLE ) {
		FOX_ShadowSurface->flags |= FOX_RESIZABLE;
	}
	/* If the video surface has no frame, the shadow should say so */
	if ( (FOX_VideoSurface->flags & FOX_NOFRAME) == FOX_NOFRAME ) {
		FOX_ShadowSurface->flags |= FOX_NOFRAME;
	}
	/* If the video surface is fullscreen, the shadow should say so */
	if ( (FOX_VideoSurface->flags & FOX_FULLSCREEN) == FOX_FULLSCREEN ) {
		FOX_ShadowSurface->flags |= FOX_FULLSCREEN;
	}
	/* If the video surface is flippable, the shadow should say so */
	if ( (FOX_VideoSurface->flags & FOX_DOUBLEBUF) == FOX_DOUBLEBUF ) {
		FOX_ShadowSurface->flags |= FOX_DOUBLEBUF;
	}
	return;
}

/*
 * Set the requested video mode, allocating a shadow buffer if necessary.
 */
FOX_Surface * FOX_SetVideoMode (int width, int height, int bpp, Uint32 flags)
{
	FOX_VideoDevice *video, *this;
	FOX_Surface *prev_mode, *mode;
	int video_w;
	int video_h;
	int video_bpp;
	int is_opengl;
	FOX_GrabMode saved_grab;

	/* Start up the video driver, if necessary..
	   WARNING: This is the only function protected this way!
	 */
	if ( ! current_video ) {
		if ( FOX_Init(FOX_INIT_VIDEO|FOX_INIT_NOPARACHUTE) < 0 ) {
			return(NULL);
		}
	}
	this = video = current_video;

	/* Default to the current video bpp */
	if ( bpp == 0 ) {
		flags |= FOX_ANYFORMAT;
		bpp = FOX_VideoSurface->format->BitsPerPixel;
	}

	/* Get a good video mode, the closest one possible */
	video_w = width;
	video_h = height;
	video_bpp = bpp;
	if ( ! FOX_GetVideoMode(&video_w, &video_h, &video_bpp, flags) ) {
		return(NULL);
	}

	/* Check the requested flags */
	/* There's no palette in > 8 bits-per-pixel mode */
	if ( video_bpp > 8 ) {
		flags &= ~FOX_HWPALETTE;
	}
#if 0
	if ( (flags&FOX_FULLSCREEN) != FOX_FULLSCREEN ) {
		/* There's no windowed double-buffering */
		flags &= ~FOX_DOUBLEBUF;
	}
#endif
	if ( (flags&FOX_DOUBLEBUF) == FOX_DOUBLEBUF ) {
		/* Use hardware surfaces when double-buffering */
		flags |= FOX_HWSURFACE;
	}

	is_opengl = ( ( flags & FOX_OPENGL ) == FOX_OPENGL );
	if ( is_opengl ) {
		/* These flags are for 2D video modes only */
		flags &= ~(FOX_HWSURFACE|FOX_DOUBLEBUF);
	}

	/* Reset the keyboard here so event callbacks can run */
	FOX_ResetKeyboard();

	/* Clean up any previous video mode */
	if ( FOX_PublicSurface != NULL ) {
		FOX_PublicSurface = NULL;
	}
	if ( FOX_ShadowSurface != NULL ) {
		FOX_Surface *ready_to_go;
		ready_to_go = FOX_ShadowSurface;
		FOX_ShadowSurface = NULL;
		FOX_FreeSurface(ready_to_go);
	}
	if ( video->physpal ) {
		free(video->physpal->colors);
		free(video->physpal);
		video->physpal = NULL;
	}
	if( video->gammacols) {
	        free(video->gammacols);
		video->gammacols = NULL;
	}

	/* Save the previous grab state and turn off grab for mode switch */
	saved_grab = FOX_WM_GrabInputOff();

	/* Try to set the video mode, along with offset and clipping */
	prev_mode = FOX_VideoSurface;
	FOX_LockCursor();
	FOX_VideoSurface = NULL;	/* In case it's freed by driver */
	mode = video->SetVideoMode(this, prev_mode,video_w,video_h,video_bpp,flags);
	if ( mode ) { /* Prevent resize events from mode change */
	    FOX_PrivateResize(mode->w, mode->h);

	    /* Sam - If we asked for OpenGL mode, and didn't get it, fail */
	    if ( is_opengl && !(mode->flags & FOX_OPENGL) ) {
		mode = NULL;
	    }
        }
	/*
	 * rcg11292000
	 * If you try to set an FOX_OPENGL surface, and fail to find a
	 * matching  visual, then the next call to FOX_SetVideoMode()
	 * will segfault, since  we no longer point to a dummy surface,
	 * but rather NULL.
	 * Sam 11/29/00
	 * WARNING, we need to make sure that the previous mode hasn't
	 * already been freed by the video driver.  What do we do in
	 * that case?  Should we call FOX_VideoInit() again?
	 */
	FOX_VideoSurface = (mode != NULL) ? mode : prev_mode;

	if ( (mode != NULL) && (!is_opengl) ) {
		/* Sanity check */
		if ( (mode->w < width) || (mode->h < height) ) {
			FOX_SetError("Video mode smaller than requested");
			return(NULL);
		}

		/* If we have a palettized surface, create a default palette */
		if ( mode->format->palette ) {
	        	FOX_PixelFormat *vf = mode->format;
			FOX_DitherColors(vf->palette->colors, vf->BitsPerPixel);
			video->SetColors(this, 0, vf->palette->ncolors,
			                           vf->palette->colors);
		}

		/* Clear the surface to black */
		video->offset_x = 0;
		video->offset_y = 0;
		mode->offset = 0;
		FOX_SetClipRect(mode, NULL);
		FOX_ClearSurface(mode);

		/* Now adjust the offsets to match the desired mode */
		video->offset_x = (mode->w-width)/2;
		video->offset_y = (mode->h-height)/2;
		mode->offset = video->offset_y*mode->pitch +
				video->offset_x*mode->format->BytesPerPixel;
#ifdef DEBUG_VIDEO
  fprintf(stderr,
	"Requested mode: %dx%dx%d, obtained mode %dx%dx%d (offset %d)\n",
		width, height, bpp,
		mode->w, mode->h, mode->format->BitsPerPixel, mode->offset);
#endif
		mode->w = width;
		mode->h = height;
		FOX_SetClipRect(mode, NULL);
	}
	FOX_ResetCursor();
	FOX_UnlockCursor();

	/* If we failed setting a video mode, return NULL... (Uh Oh!) */
	if ( mode == NULL ) {
		return(NULL);
	}

	/* If there is no window manager, set the FOX_NOFRAME flag */
	if ( ! video->info.wm_available ) {
		mode->flags |= FOX_NOFRAME;
	}

	/* Reset the mouse cursor and grab for new video mode */
	FOX_SetCursor(NULL);
	if ( video->UpdateMouse ) {
		video->UpdateMouse(this);
	}
	FOX_WM_GrabInput(saved_grab);
	FOX_GetRelativeMouseState(NULL, NULL); /* Clear first large delta */


	/* Set up a fake FOX surface for OpenGL "blitting" */
	if ( (flags & FOX_OPENGLBLIT) == FOX_OPENGLBLIT ) {
		/* Load GL functions for performing the texture updates */
#ifdef HAVE_OPENGL
#define FOX_PROC(ret,func,params) \
do { \
	video->func = FOX_GL_GetProcAddress(#func); \
	if ( ! video->func ) { \
		FOX_SetError("Couldn't load GL function: %s\n", #func); \
		return(NULL); \
	} \
} while ( 0 );
#include "FOX_glfuncs.h"
#undef FOX_PROC	

		/* Create a software surface for blitting */
#ifdef GL_VERSION_1_2
		/* If the implementation either supports the packed pixels
		   extension, or implements the core OpenGL 1.2 API, it will
		   support the GL_UNSIGNED_SHORT_5_6_5 texture format.
		 */
		if ( (bpp == 16) &&
		     (strstr((const char *)video->glGetString(GL_EXTENSIONS),
		                           "GL_EXT_packed_pixels") ||
		     (strncmp((const char *)video->glGetString(GL_VERSION),
		              "1.2", 3) == 0)) )
		{
			video->is_32bit = 0;
			FOX_VideoSurface = FOX_CreateRGBSurface(
				flags, 
				width, 
				height,  
				16,
				31 << 11,
				63 << 5,
				31,
				0
				);
		}
		else
#endif /* OpenGL 1.2 */
		{
			video->is_32bit = 1;
			FOX_VideoSurface = FOX_CreateRGBSurface(
				flags, 
				width, 
				height, 
				32, 
#if FOX_BYTEORDER == FOX_LIL_ENDIAN
				0x000000FF,
				0x0000FF00,
				0x00FF0000,
				0xFF000000
#else
				0xFF000000,
				0x00FF0000,
				0x0000FF00,
				0x000000FF
#endif
				);
		}
		if ( ! FOX_VideoSurface ) {
			return(NULL);
		}
		FOX_VideoSurface->flags = mode->flags | FOX_OPENGLBLIT;

		/* Free the original video mode surface (is this safe?) */
		FOX_FreeSurface(mode);

                /* Set the surface completely opaque & white by default */
		memset( FOX_VideoSurface->pixels, 255, FOX_VideoSurface->h * FOX_VideoSurface->pitch );
		video->glGenTextures( 1, &video->texture );
		video->glBindTexture( GL_TEXTURE_2D, video->texture );
		video->glTexImage2D(
			GL_TEXTURE_2D,
			0,
			video->is_32bit ? GL_RGBA : GL_RGB,
			256,
			256,
			0,
			video->is_32bit ? GL_RGBA : GL_RGB,
#ifdef GL_VERSION_1_2
			video->is_32bit ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT_5_6_5,
#else
			GL_UNSIGNED_BYTE,
#endif
			NULL);

		video->UpdateRects = FOX_GL_UpdateRectsLock;
#else
		FOX_SetError("Somebody forgot to #define HAVE_OPENGL");
		return(NULL);
#endif
	}

	/* Create a shadow surface if necessary */
	/* There are three conditions under which we create a shadow surface:
		1.  We need a particular bits-per-pixel that we didn't get.
		2.  We need a hardware palette and didn't get one.
		3.  We need a software surface and got a hardware surface.
	*/
	if ( !(FOX_VideoSurface->flags & FOX_OPENGL) &&
	     (
	     (  !(flags&FOX_ANYFORMAT) &&
			(FOX_VideoSurface->format->BitsPerPixel != bpp)) ||
	     (   (flags&FOX_HWPALETTE) && 
				!(FOX_VideoSurface->flags&FOX_HWPALETTE)) ||
		/* If the surface is in hardware, video writes are visible
		   as soon as they are performed, so we need to buffer them
		 */
	     (   ((flags&FOX_HWSURFACE) == FOX_SWSURFACE) &&
				(FOX_VideoSurface->flags&FOX_HWSURFACE))
	     ) ) {
		FOX_CreateShadowSurface(bpp);
		if ( FOX_ShadowSurface == NULL ) {
			FOX_SetError("Couldn't create shadow surface");
			return(NULL);
		}
		FOX_PublicSurface = FOX_ShadowSurface;
	} else {
		FOX_PublicSurface = FOX_VideoSurface;
	}
	video->info.vfmt = FOX_VideoSurface->format;

	/* We're done! */
	return(FOX_PublicSurface);
}

/* 
 * Convert a surface into the video pixel format.
 */
FOX_Surface * FOX_DisplayFormat (FOX_Surface *surface)
{
	Uint32 flags;

	if ( ! FOX_PublicSurface ) {
		FOX_SetError("No video mode has been set");
		return(NULL);
	}
	/* Set the flags appropriate for copying to display surface */
	flags  = (FOX_PublicSurface->flags&FOX_HWSURFACE);
#ifdef AUTORLE_DISPLAYFORMAT
	flags |= (surface->flags & (FOX_SRCCOLORKEY|FOX_SRCALPHA));
	flags |= FOX_RLEACCELOK;
#else
	flags |= surface->flags & (FOX_SRCCOLORKEY|FOX_SRCALPHA|FOX_RLEACCELOK);
#endif
	return(FOX_ConvertSurface(surface, FOX_PublicSurface->format, flags));
}

/*
 * Convert a surface into a format that's suitable for blitting to
 * the screen, but including an alpha channel.
 */
FOX_Surface *FOX_DisplayFormatAlpha(FOX_Surface *surface)
{
	FOX_PixelFormat *vf;
	FOX_PixelFormat *format;
	FOX_Surface *converted;
	Uint32 flags;
	/* default to ARGB8888 */
	Uint32 amask = 0xff000000;
	Uint32 rmask = 0x00ff0000;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x000000ff;

	if ( ! FOX_PublicSurface ) {
		FOX_SetError("No video mode has been set");
		return(NULL);
	}
	vf = FOX_PublicSurface->format;

	switch(vf->BytesPerPixel) {
	    case 2:
		/* For XGY5[56]5, use, AXGY8888, where {X, Y} = {R, B}.
		   For anything else (like ARGB4444) it doesn't matter
		   since we have no special code for it anyway */
		if ( (vf->Rmask == 0x1f) &&
		     (vf->Bmask == 0xf800 || vf->Bmask == 0x7c00)) {
			rmask = 0xff;
			bmask = 0xff0000;
		}
		break;

	    case 3:
	    case 4:
		/* Keep the video format, as long as the high 8 bits are
		   unused or alpha */
		if ( (vf->Rmask == 0xff) && (vf->Bmask == 0xff0000) ) {
			rmask = 0xff;
			bmask = 0xff0000;
		}
		break;

	    default:
		/* We have no other optimised formats right now. When/if a new
		   optimised alpha format is written, add the converter here */
		break;
	}
	format = FOX_AllocFormat(32, rmask, gmask, bmask, amask);
	flags = FOX_PublicSurface->flags & FOX_HWSURFACE;
	flags |= surface->flags & (FOX_SRCALPHA | FOX_RLEACCELOK);
	converted = FOX_ConvertSurface(surface, format, flags);
	FOX_FreeFormat(format);
	return(converted);
}

/*
 * Update a specific portion of the physical screen
 */
void FOX_UpdateRect(FOX_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
	if ( screen ) {
		FOX_Rect rect;

		/* Perform some checking */
		if ( w == 0 )
			w = screen->w;
		if ( h == 0 )
			h = screen->h;
		if ( (int)(x+w) > screen->w )
			return;
		if ( (int)(y+h) > screen->h )
			return;

		/* Fill the rectangle */
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		FOX_UpdateRects(screen, 1, &rect);
	}
}
void FOX_UpdateRects (FOX_Surface *screen, int numrects, FOX_Rect *rects)
{
	int i;
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this = current_video;

	if ( screen == FOX_ShadowSurface ) {
		/* Blit the shadow surface using saved mapping */
	        FOX_Palette *pal = screen->format->palette;
		FOX_Color *saved_colors = NULL;
	        if ( pal && !(FOX_VideoSurface->flags & FOX_HWPALETTE) ) {
			/* simulated 8bpp, use correct physical palette */
			saved_colors = pal->colors;
			if ( video->gammacols ) {
				/* gamma-corrected palette */
				pal->colors = video->gammacols;
			} else if ( video->physpal ) {
				/* physical palette different from logical */
				pal->colors = video->physpal->colors;
			}
		}
		if ( SHOULD_DRAWCURSOR(FOX_cursorstate) ) {
			FOX_LockCursor();
			FOX_DrawCursor(FOX_ShadowSurface);
			for ( i=0; i<numrects; ++i ) {
				FOX_LowerBlit(FOX_ShadowSurface, &rects[i], 
						FOX_VideoSurface, &rects[i]);
			}
			FOX_EraseCursor(FOX_ShadowSurface);
			FOX_UnlockCursor();
		} else {
			for ( i=0; i<numrects; ++i ) {
				FOX_LowerBlit(FOX_ShadowSurface, &rects[i], 
						FOX_VideoSurface, &rects[i]);
			}
		}
		if ( saved_colors )
			pal->colors = saved_colors;

		/* Fall through to video surface update */
		screen = FOX_VideoSurface;
	}
	if ( screen == FOX_VideoSurface ) {
		/* Update the video surface */
		if ( screen->offset ) {
			for ( i=0; i<numrects; ++i ) {
				rects[i].x += video->offset_x;
				rects[i].y += video->offset_y;
			}
			video->UpdateRects(this, numrects, rects);
			for ( i=0; i<numrects; ++i ) {
				rects[i].x -= video->offset_x;
				rects[i].y -= video->offset_y;
			}
		} else {
			video->UpdateRects(this, numrects, rects);
		}
	}
}

/*
 * Performs hardware double buffering, if possible, or a full update if not.
 */
int FOX_Flip(FOX_Surface *screen)
{
	FOX_VideoDevice *video = current_video;
	/* Copy the shadow surface to the video surface */
	if ( screen == FOX_ShadowSurface ) {
		FOX_Rect rect;
	        FOX_Palette *pal = screen->format->palette;
		FOX_Color *saved_colors = NULL;
	        if ( pal && !(FOX_VideoSurface->flags & FOX_HWPALETTE) ) {
			/* simulated 8bpp, use correct physical palette */
			saved_colors = pal->colors;
			if ( video->gammacols ) {
				/* gamma-corrected palette */
				pal->colors = video->gammacols;
			} else if ( video->physpal ) {
				/* physical palette different from logical */
				pal->colors = video->physpal->colors;
			}
		}

		rect.x = 0;
		rect.y = 0;
		rect.w = screen->w;
		rect.h = screen->h;
		FOX_LowerBlit(FOX_ShadowSurface,&rect, FOX_VideoSurface,&rect);

		if ( saved_colors )
			pal->colors = saved_colors;
		screen = FOX_VideoSurface;
	}
	if ( (screen->flags & FOX_DOUBLEBUF) == FOX_DOUBLEBUF ) {
		FOX_VideoDevice *this  = current_video;
		return(video->FlipHWSurface(this, FOX_VideoSurface));
	} else {
		FOX_UpdateRect(screen, 0, 0, 0, 0);
	}
	return(0);
}

static void SetPalette_logical(FOX_Surface *screen, FOX_Color *colors,
			       int firstcolor, int ncolors)
{
        FOX_Palette *pal = screen->format->palette;
	FOX_Palette *vidpal;

	if ( colors != (pal->colors + firstcolor) ) {
	        memcpy(pal->colors + firstcolor, colors,
		       ncolors * sizeof(*colors));
	}

	vidpal = FOX_VideoSurface->format->palette;
	if ( (screen == FOX_ShadowSurface) && vidpal ) {
	        /*
		 * This is a shadow surface, and the physical
		 * framebuffer is also indexed. Propagate the
		 * changes to its logical palette so that
		 * updates are always identity blits
		 */
		memcpy(vidpal->colors + firstcolor, colors,
		       ncolors * sizeof(*colors));
	}
	FOX_FormatChanged(screen);
}

static int SetPalette_physical(FOX_Surface *screen,
                               FOX_Color *colors, int firstcolor, int ncolors)
{
	FOX_VideoDevice *video = current_video;
	int gotall = 1;

	if ( video->physpal ) {
		/* We need to copy the new colors, since we haven't
		 * already done the copy in the logical set above.
		 */
		memcpy(video->physpal->colors + firstcolor,
		       colors, ncolors * sizeof(*colors));
	}
	if ( screen == FOX_ShadowSurface ) {
		if ( FOX_VideoSurface->flags & FOX_HWPALETTE ) {
			/*
			 * The real screen is also indexed - set its physical
			 * palette. The physical palette does not include the
			 * gamma modification, we apply it directly instead,
			 * but this only happens if we have hardware palette.
			 */
			screen = FOX_VideoSurface;
		} else {
			/*
			 * The video surface is not indexed - invalidate any
			 * active shadow-to-video blit mappings.
			 */
			if ( screen->map->dst == FOX_VideoSurface ) {
				FOX_InvalidateMap(screen->map);
			}
			if ( video->gamma ) {
				if( ! video->gammacols ) {
					FOX_Palette *pp = video->physpal;
					if(!pp)
						pp = screen->format->palette;
					video->gammacols = malloc(pp->ncolors
							  * sizeof(FOX_Color));
					FOX_ApplyGamma(video->gamma,
						       pp->colors,
						       video->gammacols,
						       pp->ncolors);
				} else {
					FOX_ApplyGamma(video->gamma, colors,
						       video->gammacols
						       + firstcolor,
						       ncolors);
				}
			}
			FOX_UpdateRect(screen, 0, 0, 0, 0);
		}
	}

	if ( screen == FOX_VideoSurface ) {
		FOX_Color gcolors[256];

	        if ( video->gamma ) {
		        FOX_ApplyGamma(video->gamma, colors, gcolors, ncolors);
			colors = gcolors;
		}
		gotall = video->SetColors(video, firstcolor, ncolors, colors);
		if ( ! gotall ) {
			/* The video flags shouldn't have FOX_HWPALETTE, and
			   the video driver is responsible for copying back the
			   correct colors into the video surface palette.
			*/
			;
		}
		FOX_CursorPaletteChanged();
	}
	return gotall;
}

/*
 * Set the physical and/or logical colormap of a surface:
 * Only the screen has a physical colormap. It determines what is actually
 * sent to the display.
 * The logical colormap is used to map blits to/from the surface.
 * 'which' is one or both of FOX_LOGPAL, FOX_PHYSPAL
 *
 * Return nonzero if all colours were set as requested, or 0 otherwise.
 */
int FOX_SetPalette(FOX_Surface *screen, int which,
		   FOX_Color *colors, int firstcolor, int ncolors)
{
        FOX_Palette *pal;
	int gotall;
	int palsize;

	if ( ! current_video ) {
		return 0;
	}
	if ( screen != FOX_PublicSurface ) {
	        /* only screens have physical palettes */
	        which &= ~FOX_PHYSPAL;
	} else if( (screen->flags & FOX_HWPALETTE) != FOX_HWPALETTE ) {
	        /* hardware palettes required for split colormaps */
	        which |= FOX_PHYSPAL | FOX_LOGPAL;
	}

	/* Verify the parameters */
	pal = screen->format->palette;
	if( !pal ) {
	        return 0;	/* not a palettized surface */
	}
	gotall = 1;
	palsize = 1 << screen->format->BitsPerPixel;
	if ( ncolors > (palsize - firstcolor) ) {
	        ncolors = (palsize - firstcolor);
		gotall = 0;
	}

	if ( which & FOX_LOGPAL ) {
		/*
		 * Logical palette change: The actual screen isn't affected,
		 * but the internal colormap is altered so that the
		 * interpretation of the pixel values (for blits etc) is
		 * changed.
		 */
	        SetPalette_logical(screen, colors, firstcolor, ncolors);
	}
	if ( which & FOX_PHYSPAL ) {
		FOX_VideoDevice *video = current_video;
	        /*
		 * Physical palette change: This doesn't affect the
		 * program's idea of what the screen looks like, but changes
		 * its actual appearance.
		 */
	        if(!video)
		        return gotall;	/* video not yet initialized */
		if(!video->physpal && !(which & FOX_LOGPAL) ) {
			/* Lazy physical palette allocation */
		        int size;
			FOX_Palette *pp = malloc(sizeof(*pp));
			current_video->physpal = pp;
			pp->ncolors = pal->ncolors;
			size = pp->ncolors * sizeof(FOX_Color);
			pp->colors = malloc(size);
			memcpy(pp->colors, pal->colors, size);
		}
		if ( ! SetPalette_physical(screen,
		                           colors, firstcolor, ncolors) ) {
			gotall = 0;
		}
	}
	return gotall;
}

int FOX_SetColors(FOX_Surface *screen, FOX_Color *colors, int firstcolor,
		  int ncolors)
{
        return FOX_SetPalette(screen, FOX_LOGPAL | FOX_PHYSPAL,
			      colors, firstcolor, ncolors);
}

/*
 * Clean up the video subsystem
 */
void FOX_VideoQuit (void)
{
	FOX_Surface *ready_to_go;

	if ( current_video ) {
		FOX_VideoDevice *video = current_video;
		FOX_VideoDevice *this  = current_video;

		/* Halt event processing before doing anything else */
		FOX_StopEventLoop();

		/* Clean up allocated window manager items */
		if ( FOX_PublicSurface ) {
			FOX_PublicSurface = NULL;
		}
		FOX_CursorQuit();

		/* Just in case... */
		FOX_WM_GrabInputOff();

		/* Clean up the system video */
		video->VideoQuit(this);

		/* Free any lingering surfaces */
		ready_to_go = FOX_ShadowSurface;
		FOX_ShadowSurface = NULL;
		FOX_FreeSurface(ready_to_go);
		if ( FOX_VideoSurface != NULL ) {
			ready_to_go = FOX_VideoSurface;
			FOX_VideoSurface = NULL;
			FOX_FreeSurface(ready_to_go);
		}
		FOX_PublicSurface = NULL;

		/* Clean up miscellaneous memory */
		if ( video->physpal ) {
			free(video->physpal->colors);
			free(video->physpal);
			video->physpal = NULL;
		}
		if ( video->gammacols ) {
			free(video->gammacols);
			video->gammacols = NULL;
		}
		if ( video->gamma ) {
			free(video->gamma);
			video->gamma = NULL;
		}
		if ( video->wm_title != NULL ) {
			free(video->wm_title);
			video->wm_title = NULL;
		}
		if ( video->wm_icon != NULL ) {
			free(video->wm_icon);
			video->wm_icon = NULL;
		}

		/* Finish cleaning up video subsystem */
		video->free(this);
		current_video = NULL;
	}
	return;
}


/*
 * Sets/Gets the title and icon text of the display window, if any.
 */
void FOX_WM_SetCaption (const char *title, const char *icon)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;

	if ( video ) {
		if ( title ) {
			if ( video->wm_title ) {
				free(video->wm_title);
			}
			video->wm_title = (char *)malloc(strlen(title)+1);
			if ( video->wm_title != NULL ) {
				strcpy(video->wm_title, title);
			}
		}
		if ( icon ) {
			if ( video->wm_icon ) {
				free(video->wm_icon);
			}
			video->wm_icon = (char *)malloc(strlen(icon)+1);
			if ( video->wm_icon != NULL ) {
				strcpy(video->wm_icon, icon);
			}
		}
		if ( (title || icon) && (video->SetCaption != NULL) ) {
			video->SetCaption(this, video->wm_title,video->wm_icon);
		}
	}
}
void FOX_WM_GetCaption (char **title, char **icon)
{
	FOX_VideoDevice *video = current_video;

	if ( video ) {
		if ( title ) {
			*title = video->wm_title;
		}
		if ( icon ) {
			*icon = video->wm_icon;
		}
	}
}

/* Utility function used by FOX_WM_SetIcon() */
static void CreateMaskFromColorKey(FOX_Surface *icon, Uint8 *mask)
{
	int x, y;
	Uint32 colorkey;
#define SET_MASKBIT(icon, x, y, mask) \
	mask[(y*((icon->w+7)/8))+(x/8)] &= ~(0x01<<(7-(x%8)))

	colorkey = icon->format->colorkey;
	switch (icon->format->BytesPerPixel) {
		case 1: { Uint8 *pixels;
			for ( y=0; y<icon->h; ++y ) {
				pixels = (Uint8 *)icon->pixels + y*icon->pitch;
				for ( x=0; x<icon->w; ++x ) {
					if ( *pixels++ == colorkey ) {
						SET_MASKBIT(icon, x, y, mask);
					}
				}
			}
		}
		break;

		case 2: { Uint16 *pixels;
			for ( y=0; y<icon->h; ++y ) {
				pixels = (Uint16 *)icon->pixels +
				                   y*icon->pitch/2;
				for ( x=0; x<icon->w; ++x ) {
					if ( *pixels++ == colorkey ) {
						SET_MASKBIT(icon, x, y, mask);
					}
				}
			}
		}
		break;

		case 4: { Uint32 *pixels;
			for ( y=0; y<icon->h; ++y ) {
				pixels = (Uint32 *)icon->pixels +
				                   y*icon->pitch/4;
				for ( x=0; x<icon->w; ++x ) {
					if ( *pixels++ == colorkey ) {
						SET_MASKBIT(icon, x, y, mask);
					}
				}
			}
		}
		break;
	}
}

/*
 * Sets the window manager icon for the display window.
 */
void FOX_WM_SetIcon (FOX_Surface *icon, Uint8 *mask)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;

	if ( icon && video->SetIcon ) {
		/* Generate a mask if necessary, and create the icon! */
		if ( mask == NULL ) {
			int mask_len = icon->h*(icon->w+7)/8;
			mask = (Uint8 *)malloc(mask_len);
			if ( mask == NULL ) {
				return;
			}
			memset(mask, ~0, mask_len);
			if ( icon->flags & FOX_SRCCOLORKEY ) {
				CreateMaskFromColorKey(icon, mask);
			}
			video->SetIcon(video, icon, mask);
			free(mask);
		} else {
			video->SetIcon(this, icon, mask);
		}
	}
}

/*
 * Grab or ungrab the keyboard and mouse input.
 * This function returns the final grab mode after calling the
 * driver dependent function.
 */
static FOX_GrabMode FOX_WM_GrabInputRaw(FOX_GrabMode mode)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;

	/* Only do something if we have support for grabs */
	if ( video->GrabInput == NULL ) {
		return(video->input_grab);
	}

	/* If the final grab mode if off, only then do we actually grab */
#ifdef DEBUG_GRAB
  printf("FOX_WM_GrabInputRaw(%d) ... ", mode);
#endif
	if ( mode == FOX_GRAB_OFF ) {
		if ( video->input_grab != FOX_GRAB_OFF ) {
			mode = video->GrabInput(this, mode);
		}
	} else {
		if ( video->input_grab == FOX_GRAB_OFF ) {
			mode = video->GrabInput(this, mode);
		}
	}
	if ( mode != video->input_grab ) {
		video->input_grab = mode;
		if ( video->CheckMouseMode ) {
			video->CheckMouseMode(this);
		}
	}
#ifdef DEBUG_GRAB
  printf("Final mode %d\n", video->input_grab);
#endif

	/* Return the final grab state */
	if ( mode >= FOX_GRAB_FULLSCREEN ) {
		mode -= FOX_GRAB_FULLSCREEN;
	}
	return(mode);
}
FOX_GrabMode FOX_WM_GrabInput(FOX_GrabMode mode)
{
	FOX_VideoDevice *video = current_video;

	/* If the video isn't initialized yet, we can't do anything */
	if ( ! video ) {
		return FOX_GRAB_OFF;
	}

	/* Return the current mode on query */
	if ( mode == FOX_GRAB_QUERY ) {
		mode = video->input_grab;
		if ( mode >= FOX_GRAB_FULLSCREEN ) {
			mode -= FOX_GRAB_FULLSCREEN;
		}
		return(mode);
	}

#ifdef DEBUG_GRAB
  printf("FOX_WM_GrabInput(%d) ... ", mode);
#endif
	/* If the video surface is fullscreen, we always grab */
	if ( mode >= FOX_GRAB_FULLSCREEN ) {
		mode -= FOX_GRAB_FULLSCREEN;
	}
	if ( FOX_VideoSurface && (FOX_VideoSurface->flags & FOX_FULLSCREEN) ) {
		mode += FOX_GRAB_FULLSCREEN;
	}
	return(FOX_WM_GrabInputRaw(mode));
}
static FOX_GrabMode FOX_WM_GrabInputOff(void)
{
	FOX_GrabMode mode;

	/* First query the current grab state */
	mode = FOX_WM_GrabInput(FOX_GRAB_QUERY);

	/* Now explicitly turn off input grab */
	FOX_WM_GrabInputRaw(FOX_GRAB_OFF);

	/* Return the old state */
	return(mode);
}

/*
 * Iconify the window in window managed environments.
 * A successful iconification will result in an FOX_APPACTIVE loss event.
 */
int FOX_WM_IconifyWindow(void)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;
	int retval;

	retval = 0;
	if ( video->IconifyWindow ) {
		retval = video->IconifyWindow(this);
	}
	return(retval);
}

/*
 * Toggle fullscreen mode
 */
int FOX_WM_ToggleFullScreen(FOX_Surface *surface)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;
	int toggled;

	toggled = 0;
	if ( FOX_PublicSurface && (surface == FOX_PublicSurface) &&
	     video->ToggleFullScreen ) {
		if ( surface->flags & FOX_FULLSCREEN ) {
			toggled = video->ToggleFullScreen(this, 0);
			if ( toggled ) {
				FOX_VideoSurface->flags &= ~FOX_FULLSCREEN;
				FOX_PublicSurface->flags &= ~FOX_FULLSCREEN;
			}
		} else {
			toggled = video->ToggleFullScreen(this, 1);
			if ( toggled ) {
				FOX_VideoSurface->flags |= FOX_FULLSCREEN;
				FOX_PublicSurface->flags |= FOX_FULLSCREEN;
			}
		}
		/* Double-check the grab state inside FOX_WM_GrabInput() */
		if ( toggled ) {
			FOX_WM_GrabInput(video->input_grab);
		}
	}
	return(toggled);
}

/*
 * Get some platform dependent window manager information
 */
int FOX_GetWMInfo (FOX_SysWMinfo *info)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;

	if ( video && video->GetWMInfo ) {
		return(video->GetWMInfo(this, info));
	} else {
		return(0);
	}
}

// end
