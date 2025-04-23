// Fox v0.5
// by Jaroslaw Rozynski
//===
// *VIDEO DIRECTX*
//===
// TODO:


#ifndef _FOX_lowvideo_h
#define _FOX_lowvideo_h

#include <windows.h>

#include "FOX_sysvideo.h"

// ukryty wskaznik *this

#define _THIS	FOX_VideoDevice *this

#define DDRAW_FULLSCREEN() 						\
(									\
	((FOX_VideoSurface->flags & FOX_FULLSCREEN) == FOX_FULLSCREEN) && \
	((FOX_VideoSurface->flags & FOX_OPENGL    ) != FOX_OPENGL    ) && \
	(strcmp(this->name, "directx") == 0)				\
)

#define DINPUT_FULLSCREEN() 						\
(									\
	((FOX_VideoSurface->flags & FOX_FULLSCREEN) == FOX_FULLSCREEN) && \
	(strcmp(this->name, "directx") == 0)				\
)

// glowne okno i funkcje do kontroli jego ustawien

extern const char *FOX_Appname;
extern HINSTANCE FOX_Instance;
extern HWND FOX_Window;
extern const char *FOX_windowid;

// eksportowane

extern void (*WIN_RealizePalette)(_THIS);
extern void (*WIN_PaletteChanged)(_THIS, HWND window);
extern void (*WIN_SwapGamma)(_THIS);
extern void (*WIN_WinPAINT)(_THIS, HDC hdc);
extern LONG (*HandleMessage)(_THIS, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// kursor myszy sysmouse.c

extern HCURSOR FOX_hcursor;

// clipping

extern RECT FOX_bounds;

extern int FOX_resizing;

extern int mouse_relative;

// dx8_audio

extern void DX8_SoundFocus(HWND window);

LONG CALLBACK WinMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif 

// end

