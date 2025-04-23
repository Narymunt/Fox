// Fox v0.5
// by Jaroslaw Rozynski
//===
// *EVENTS*
//===
// TODO:
// - bez podzialu na common i global, zamienic na zwykle h a nie _c.h

// standardowe ustawienia i zdarzenia

#include "FOX_events.h"

// uruchom i zatrzymaj petle obslugi zdarzen

extern int FOX_StartEventLoop(Uint32 flags);
extern void FOX_StopEventLoop(void);
extern void FOX_QuitInterrupt(void);

extern void FOX_Lock_EventThread();
extern void FOX_Unlock_EventThread();
extern Uint32 FOX_EventThreadID(void);

// inicjalizacja handlera 

extern int  FOX_AppActiveInit(void);
extern int  FOX_KeyboardInit(void);
extern int  FOX_MouseInit(void);
extern int  FOX_QuitInit(void);

// filtr dla zdarzen

extern FOX_EventFilter FOX_EventOK;

// tablica zdarzen 

extern Uint8 FOX_ProcessEvents[FOX_NUMEVENTS];

// wewnetrzne zdarzenia, dokladniej opisane w 
// FOX_active.c, FOX_mouse.c, FOX_keyboard.c, FOX_quit.c, FOX_events.c

extern int FOX_PrivateAppActive(Uint8 gain, Uint8 state);

extern int FOX_PrivateMouseMotion(Uint8 buttonstate, int relative,
						Sint16 x, Sint16 y);
extern int FOX_PrivateMouseButton(Uint8 state, Uint8 button,Sint16 x,Sint16 y);

extern int FOX_PrivateKeyboard(Uint8 state, FOX_keysym *key);

extern int FOX_PrivateResize(int w, int h);
extern int FOX_PrivateExpose(void);

extern int FOX_PrivateQuit(void);

extern int FOX_PrivateSysWMEvent(FOX_SysWMmsg *message);

// uzywane przy usuwaniu / zamianie mouse focus

extern void FOX_MouseFocus(int focus);

// tak samo tylko, ze dla klawiatury

extern void FOX_ResetKeyboard(void);

// powtarzania - kiedy klawisz jest wcisniety

extern void FOX_CheckKeyRepeat(void);

// konwersja nacisnietych znakow 

#ifndef DEFAULT_UNICODE_TRANSLATION
	#define DEFAULT_UNICODE_TRANSLATION 0	// domyslnie wylaczone
#endif

extern int FOX_TranslateUNICODE;	// jako tablica 

// end

