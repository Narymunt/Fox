// Fox v0.5a
// by Jaroslaw Rozynski
//===
// TODO:

#ifndef _FOX_sysvideo_h
#define _FOX_sysvideo_h

#include "FOX_mouse.h"
#define FOX_PROTOTYPES_ONLY
#include "FOX_syswm.h"
#undef FOX_PROTOTYPES_ONLY

/* This file prototypes the video driver implementation.
   This is designed to be easily converted to C++ in the future.
 */

#include <windows.h>


/* The FOX video driver */
typedef struct FOX_VideoDevice FOX_VideoDevice;

/* Define the FOX video driver structure */
#define _THIS	FOX_VideoDevice *_this
#ifndef _STATUS
#define _STATUS	FOX_status *status
#endif
struct FOX_VideoDevice {
	/* * * */
	/* The name of this video driver */
	const char *name;

	/* * * */
	/* Initialization/Query functions */

	/* Initialize the native video subsystem, filling 'vformat' with the 
	   "best" display pixel format, returning 0 or -1 if there's an error.
	 */
	int (*VideoInit)(_THIS, FOX_PixelFormat *vformat);

	/* List the available video modes for the given pixel format, sorted
	   from largest to smallest.
	 */
	FOX_Rect **(*ListModes)(_THIS, FOX_PixelFormat *format, Uint32 flags);

	/* Set the requested video mode, returning a surface which will be
	   set to the FOX_VideoSurface.  The width and height will already
	   be verified by ListModes(), and the video subsystem is free to
	   set the mode to a supported bit depth different from the one
	   specified -- the desired bpp will be emulated with a shadow
	   surface if necessary.  If a new mode is returned, this function
	   should take care of cleaning up the current mode.
	 */
	FOX_Surface *(*SetVideoMode)(_THIS, FOX_Surface *current,
				int width, int height, int bpp, Uint32 flags);

	/* Toggle the fullscreen mode */
	int (*ToggleFullScreen)(_THIS, int on);

	/* This is called after the video mode has been set, to get the
	   initial mouse state.  It should queue events as necessary to
	   properly represent the current mouse focus and position.
	 */
	void (*UpdateMouse)(_THIS);

	/* Create a YUV video surface (possibly overlay) of the given
	   format.  The hardware should be able to perform at least 2x
	   scaling on display.
	 */
	FOX_Overlay *(*CreateYUVOverlay)(_THIS, int width, int height,
	                                 Uint32 format, FOX_Surface *display);

        /* Sets the color entries { firstcolor .. (firstcolor+ncolors-1) }
	   of the physical palette to those in 'colors'. If the device is
	   using a software palette (FOX_HWPALETTE not set), then the
	   changes are reflected in the logical palette of the screen
	   as well.
	   The return value is 1 if all entries could be set properly
	   or 0 otherwise.
	*/
	int (*SetColors)(_THIS, int firstcolor, int ncolors,
			 FOX_Color *colors);

	/* This pointer should exist in the native video subsystem and should
	   point to an appropriate update function for the current video mode
	 */
	void (*UpdateRects)(_THIS, int numrects, FOX_Rect *rects);

	/* Reverse the effects VideoInit() -- called if VideoInit() fails
	   or if the application is shutting down the video subsystem.
	*/
	void (*VideoQuit)(_THIS);

	/* * * */
	/* Hardware acceleration functions */

	/* Information about the video hardware */
	FOX_VideoInfo info;

	/* Allocates a surface in video memory */
	int (*AllocHWSurface)(_THIS, FOX_Surface *surface);

	/* Sets the hardware accelerated blit function, if any, based
	   on the current flags of the surface (colorkey, alpha, etc.)
	 */
	int (*CheckHWBlit)(_THIS, FOX_Surface *src, FOX_Surface *dst);

	/* Fills a surface rectangle with the given color */
	int (*FillHWRect)(_THIS, FOX_Surface *dst, FOX_Rect *rect, Uint32 color);

	/* Sets video mem colorkey and accelerated blit function */
	int (*SetHWColorKey)(_THIS, FOX_Surface *surface, Uint32 key);

	/* Sets per surface hardware alpha value */
	int (*SetHWAlpha)(_THIS, FOX_Surface *surface, Uint8 value);

	/* Returns a readable/writable surface */
	int (*LockHWSurface)(_THIS, FOX_Surface *surface);
	void (*UnlockHWSurface)(_THIS, FOX_Surface *surface);

	/* Performs hardware flipping */
	int (*FlipHWSurface)(_THIS, FOX_Surface *surface);

	/* Frees a previously allocated video surface */
	void (*FreeHWSurface)(_THIS, FOX_Surface *surface);

	/* * * */
	/* Gamma support */

	Uint16 *gamma;

	/* Set the gamma correction directly (emulated with gamma ramps) */
	int (*SetGamma)(_THIS, float red, float green, float blue);

	/* Get the gamma correction directly (emulated with gamma ramps) */
	int (*GetGamma)(_THIS, float *red, float *green, float *blue);

	/* Set the gamma ramp */
	int (*SetGammaRamp)(_THIS, Uint16 *ramp);

	/* Get the gamma ramp */
	int (*GetGammaRamp)(_THIS, Uint16 *ramp);


	int is_32bit;
 
	/* * * */
	/* Window manager functions */

	/* Set the title and icon text */
	void (*SetCaption)(_THIS, const char *title, const char *icon);

	/* Set the window icon image */
	void (*SetIcon)(_THIS, FOX_Surface *icon, Uint8 *mask);

	/* Iconify the window.
	   This function returns 1 if there is a window manager and the
	   window was actually iconified, it returns 0 otherwise.
	*/
	int (*IconifyWindow)(_THIS);

	/* Grab or ungrab keyboard and mouse input */
	FOX_GrabMode (*GrabInput)(_THIS, FOX_GrabMode mode);

	/* Get some platform dependent window information */
	int (*GetWMInfo)(_THIS, FOX_SysWMinfo *info);

	/* * * */
	/* Cursor manager functions */

	/* Free a window manager cursor
	   This function can be NULL if CreateWMCursor is also NULL.
	 */
	void (*FreeWMCursor)(_THIS, WMcursor *cursor);

	/* If not NULL, create a black/white window manager cursor */
	WMcursor *(*CreateWMCursor)(_THIS,
		Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y);

	/* Show the specified cursor, or hide if cursor is NULL */
	int (*ShowWMCursor)(_THIS, WMcursor *cursor);

	/* Warp the window manager cursor to (x,y)
	   If NULL, a mouse motion event is posted internally.
	 */
	void (*WarpWMCursor)(_THIS, Uint16 x, Uint16 y);

	/* If not NULL, this is called when a mouse motion event occurs */
	void (*MoveWMCursor)(_THIS, int x, int y);

	/* Determine whether the mouse should be in relative mode or not.
	   This function is called when the input grab state or cursor
	   visibility state changes.
	   If the cursor is not visible, and the input is grabbed, the
	   driver can place the mouse in relative mode, which may result
	   in higher accuracy sampling of the pointer motion.
	*/
	void (*CheckMouseMode)(_THIS);

	/* * * */
	/* Event manager functions */

	/* Initialize keyboard mapping for this driver */
	void (*InitOSKeymap)(_THIS);

	/* Handle any queued OS events */
	void (*PumpEvents)(_THIS);

	/* * * */
	/* Data common to all drivers */
	FOX_Surface *screen;
	FOX_Surface *shadow;
	FOX_Surface *visible;
        FOX_Palette *physpal;	/* physical palette, if != logical palette */
        FOX_Color *gammacols;	/* gamma-corrected colours, or NULL */
	char *wm_title;
	char *wm_icon;
	int offset_x;
	int offset_y;
	FOX_GrabMode input_grab;

	/* Driver information flags */
	int handles_any_size;	/* Driver handles any size video mode */


	/* * * */
	/* Data private to this driver */
	struct FOX_PrivateVideoData *hidden;
	struct FOX_PrivateGLData *gl_data;

	/* * * */
	/* The function used to dispose of this structure */
	void (*free)(_THIS);
};
#undef _THIS

typedef struct VideoBootStrap {
	const char *name;
	const char *desc;
	int (*available)(void);
	FOX_VideoDevice *(*create)(int devindex);
} VideoBootStrap;

//#ifdef ENABLE_X11
//extern VideoBootStrap X11_bootstrap;
//#endif
//#ifdef ENABLE_DGA
//extern VideoBootStrap DGA_bootstrap;
//#endif
//#ifdef ENABLE_NANOX
//extern VideoBootStrap NX_bootstrap;
//#endif
//#ifdef ENABLE_FBCON
//extern VideoBootStrap FBCON_bootstrap;
//#endif
//#ifdef ENABLE_DIRECTFB
//extern VideoBootStrap DirectFB_bootstrap;
//#endif
//#ifdef ENABLE_PS2GS
//extern VideoBootStrap PS2GS_bootstrap;
//#endif
//#ifdef ENABLE_GGI
//extern VideoBootStrap GGI_bootstrap;
//#endif
//#ifdef ENABLE_VGL
//extern VideoBootStrap VGL_bootstrap;
//#endif
//#ifdef ENABLE_SVGALIB
//extern VideoBootStrap SVGALIB_bootstrap;
//#endif
//#ifdef ENABLE_AALIB
//extern VideoBootStrap AALIB_bootstrap;
//#endif
//#ifdef ENABLE_WINDIB
//extern VideoBootStrap WINDIB_bootstrap;
//#endif
//#ifdef ENABLE_DIRECTX
extern VideoBootStrap DIRECTX_bootstrap;
//#endif
//#ifdef ENABLE_BWINDOW
//extern VideoBootStrap BWINDOW_bootstrap;
//#endif
/* MacOS X gets the proper defines from configure */
//#if defined(macintosh) && !defined(MACOSX)

//	#define ENABLE_TOOLBOX

//		#if !TARGET_API_MAC_CARBON
//		#define ENABLE_DRAWSPROCKET
//		#endif
//#endif

#ifdef ENABLE_TOOLBOX
extern VideoBootStrap TOOLBOX_bootstrap;
#endif
#ifdef ENABLE_DRAWSPROCKET
extern VideoBootStrap DSp_bootstrap;
#endif
#ifdef ENABLE_QUARTZ
extern VideoBootStrap QZ_bootstrap;
#endif
#ifdef ENABLE_CYBERGRAPHICS
extern VideoBootStrap CGX_bootstrap;
#endif
#ifdef ENABLE_PHOTON
extern VideoBootStrap ph_bootstrap;
#endif
#ifdef ENABLE_EPOC
extern VideoBootStrap EPOC_bootstrap;
#endif
#ifdef ENABLE_DUMMYVIDEO
extern VideoBootStrap DUMMY_bootstrap;
#endif
/* This is the current video device */
extern FOX_VideoDevice *current_video;

#define FOX_VideoSurface	(current_video->screen)
#define FOX_ShadowSurface	(current_video->shadow)
#define FOX_PublicSurface	(current_video->visible)

#endif /* _FOX_sysvideo_h */

// end
