// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// implementacja YUV


#include <stdlib.h>
#include <string.h>

#include "FOX_getenv.h"
#include "FOX_video.h"
#include "FOX_sysvideo.h"
#include "FOX_yuvfuncs.h"
#include "FOX_yuv_sw_c.h"


FOX_Overlay *FOX_CreateYUVOverlay(int w, int h, Uint32 format,
                                  FOX_Surface *display)
{
	FOX_VideoDevice *video = current_video;
	FOX_VideoDevice *this  = current_video;
	const char *yuv_hwaccel;
	FOX_Overlay *overlay;

	overlay = NULL;
	
	if ( getenv("FOX_VIDEO_YUV_DIRECT") ) 
	{
		if ( (display == FOX_PublicSurface) &&
		     ((FOX_VideoSurface->format->BytesPerPixel == 2) ||
		      (FOX_VideoSurface->format->BytesPerPixel == 4)) ) 
		{
			display = FOX_VideoSurface;
		}
	}
        yuv_hwaccel = getenv("FOX_VIDEO_YUV_HWACCEL");
	
	if ( ((display == FOX_VideoSurface) && video->CreateYUVOverlay) &&
	     (!yuv_hwaccel || (atoi(yuv_hwaccel) > 0)) ) 
	{
		overlay = video->CreateYUVOverlay(this, w, h, format, display);
	}
	
	if ( overlay == NULL ) 
	{
		overlay = FOX_CreateYUV_SW(this, w, h, format, display);
	}
	return overlay;
}

int FOX_LockYUVOverlay(FOX_Overlay *overlay)
{
	return overlay->hwfuncs->Lock(current_video, overlay);
}

void FOX_UnlockYUVOverlay(FOX_Overlay *overlay)
{
	overlay->hwfuncs->Unlock(current_video, overlay);
}

int FOX_DisplayYUVOverlay(FOX_Overlay *overlay, FOX_Rect *dstrect)
{
	return overlay->hwfuncs->Display(current_video, overlay, dstrect);
}

void FOX_FreeYUVOverlay(FOX_Overlay *overlay)
{
	if ( overlay ) {
		if ( overlay->hwfuncs ) {
			overlay->hwfuncs->FreeHW(current_video, overlay);
		}
		free(overlay);
	}
}

// end

