// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// obsluga windows 

#ifndef _FOX_syswm_h
#define _FOX_syswm_h

#include "FOX_version.h"	// tak dla pewnosci

#include "FOX_begin.h"

// funkcje traktuj jako C

#ifdef __cplusplus
extern "C" {
#endif

// przechwytywanie zdarzenia FOX_SYSWMEVENT

#ifdef FOX_PROTOTYPES_ONLY
	struct FOX_SysWMinfo;
	typedef struct FOX_SysWMinfo FOX_SysWMinfo;
#else

#include <windows.h>

// szczegolowy opis zdarzenia

struct FOX_SysWMmsg {
	
	FOX_version version;
	
	HWND hwnd;				// okno na wiadomosc
	
	UINT msg;				// typ wiadomosci
	
	WPARAM wParam;			// parametr typu word
	LPARAM lParam;			// parametr long

};

// szczegoly 

typedef struct {
	FOX_version version;
	HWND window;			// Win32 display window
} FOX_SysWMinfo;


#endif 

// hook do info, wypelnia strukture info

extern DECLSPEC int FOX_GetWMInfo(FOX_SysWMinfo *info);

// koniec funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci 

#include "FOX_end.h"

#endif 

// end

