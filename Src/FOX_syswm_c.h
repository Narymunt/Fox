// Fox v0.5
// by Jaroslaw Rozynski
//===
// *VIDEO DIRECTX*
//===
// TODO:

// pierdoly dla ikonek

#include "FOX_lowvideo.h"

// dane musza byc zwolnione w FOX_SYS_VideoQuit()

extern HICON screen_icn;

// exportowane funkcje

extern void WIN_SetWMIcon(_THIS, FOX_Surface *icon, Uint8 *mask);
extern void WIN_SetWMCaption(_THIS, const char *title, const char *icon);
extern int WIN_IconifyWindow(_THIS);
extern FOX_GrabMode WIN_GrabInput(_THIS, FOX_GrabMode mode);
extern int WIN_GetWMInfo(_THIS, FOX_SysWMinfo *info);

// end
