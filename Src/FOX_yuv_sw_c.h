// Fox v0.5a
// by Jaroslaw Rozynski
//===
// TODO:


#include "FOX_video.h"
#include "FOX_sysvideo.h"

// standardy dla yuv

extern FOX_Overlay *FOX_CreateYUV_SW(_THIS, int width, int height, Uint32 format, FOX_Surface *display);

extern int FOX_LockYUV_SW(_THIS, FOX_Overlay *overlay);

extern void FOX_UnlockYUV_SW(_THIS, FOX_Overlay *overlay);

extern int FOX_DisplayYUV_SW(_THIS, FOX_Overlay *overlay, FOX_Rect *dstrect);

extern void FOX_FreeYUV_SW(_THIS, FOX_Overlay *overlay);

//[=] end
