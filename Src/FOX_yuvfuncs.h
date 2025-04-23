// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// definicja yuv 

#include "FOX_video.h"
#include "FOX_sysvideo.h"

#ifndef _THIS
	#define _THIS	FOX_VideoDevice *_this
#endif

// tutaj opis 

struct private_yuvhwfuncs 
{
	int (*Lock)(_THIS, FOX_Overlay *overlay);
	
	void (*Unlock)(_THIS, FOX_Overlay *overlay);
	
	int (*Display)(_THIS, FOX_Overlay *overlay, FOX_Rect *dstrect);
	
	void (*FreeHW)(_THIS, FOX_Overlay *overlay);
};

// end

