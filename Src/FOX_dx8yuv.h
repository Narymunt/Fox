// Fox v0.5
// by Jaroslaw Rozynski
//===
// *DIRECTX*
//===
// TODO: 

// implementacja nakladek yuv w directdraw 

#include "FOX_video.h"
#include "FOX_lowvideo.h"
#include "FOX_dx8video.h"

extern FOX_Overlay *DX8_CreateYUVOverlay(_THIS, int width, int height, Uint32 format, FOX_Surface *display);

extern int DX8_LockYUVOverlay(_THIS, FOX_Overlay *overlay);

extern void DX8_UnlockYUVOverlay(_THIS, FOX_Overlay *overlay);

extern int DX8_DisplayYUVOverlay(_THIS, FOX_Overlay *overlay, FOX_Rect *dstrect);

extern void DX8_FreeYUVOverlay(_THIS, FOX_Overlay *overlay);

// end
