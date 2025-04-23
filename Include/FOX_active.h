// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:

// sprawdz czy przypadkiem tego juz nie dolaczylismy

#ifndef _FOX_active_h
#define _FOX_active_h

// dodatkowe wstawki

#include "FOX_begin.h"

// traktuj funkcje jako c nawet jesli sa c++

#ifdef __cplusplus
extern "C" {
#endif

// w jakim stanie jest nasza aplikacja?

#define FOX_APPMOUSEFOCUS	0x01		// po mouse focus
#define FOX_APPINPUTFOCUS	0x02		// po input focus
#define FOX_APPACTIVE		0x04		// aplikacja jest aktywna

// prototypy funkcji

// zwraca stan aplikacji, a wlasciwie kombinacje APP_MOUSEFOCUS, APPINPUTFOCUS,
// APPACTIVE (jesli jest aktywne, nie jesli jest iconified albo disabled)

extern DECLSPEC Uint8 FOX_GetAppState(void);

// koniec traktowania funkcji jako c 

#ifdef __cplusplus
}
#endif

// koniec dodatkowych deklaracji

#include "FOX_end.h"

#endif 

// end
