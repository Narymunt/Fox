// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - czy dzialaja polskie czcionki ? 
// - zapalanie i zgaszanie diod ;) 

// obsluga zdarzen klawiatury

#ifndef _FOX_keyboard_h
#define _FOX_keyboard_h

#include "FOX_types.h"		// skroty typow
#include "FOX_keysym.h"		// tablice 

#include "FOX_begin.h"		// poczatkowe smieci

// traktuj funkcje jako C

#ifdef __cplusplus
extern "C" {
#endif

// kod klawisza jako scancode 

typedef struct {

	Uint8 scancode;		// zalezny od sprzetu scancode

	FOXKey sym;			// alias do klawisza

	FOXMod mod;			// modyfikator klawisza

	Uint16 unicode;		// przetlumaczony kod

} FOX_keysym;

// maska dla hotkeys

#define FOX_ALL_HOTKEYS		0xFFFFFFFF

// wlaczenie i wylaczenie konwersji 
// enable 1 - konwersja wlaczona
// enable 0 - konwersja wylaczona
// enable -1 - konwersja nie zmieniona (stan)

extern DECLSPEC int FOX_EnableUNICODE(int enable);

// wlaczenie i wylaczenie powtarzania klawisza, domyslnie jest wylaczone
// jezeli delay = 0 powtarzanie zablokowane

#define FOX_DEFAULT_REPEAT_DELAY	500
#define FOX_DEFAULT_REPEAT_INTERVAL	30

extern DECLSPEC int FOX_EnableKeyRepeat(int delay, int interval);

// podaj aktualny stan klawisza
// !!! lepiej korzystac z konwersji !!!
// 	Uint8 *keystate = FOX_GetKeyState(NULL);
//  if ( keystate[FOXK_RETURN] ) ... <RETURN> is pressed.

extern DECLSPEC Uint8 * FOX_GetKeyState(int *numkeys);

// podaj aktualny modyfikator klawisza

extern DECLSPEC FOXMod FOX_GetModState(void);

// ustaw modyfikator klawisza, nie zmienia stanu klawiatury, tylko same flagi

extern DECLSPEC void FOX_SetModState(FOXMod modstate);

// podaj nazwe keysym dla klawisza

extern DECLSPEC char * FOX_GetKeyName(FOXKey key);

// koniec traktowania funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"		

#endif 

// end
