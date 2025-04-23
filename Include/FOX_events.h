// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:
// - zapis zdarzen do pliku (logowanie)


// wylapywanie zdarzen

#ifndef _FOX_events_h
#define _FOX_events_h

#include "FOX_types.h"
#include "FOX_active.h"
#include "FOX_keyboard.h"
#include "FOX_mouse.h"
#include "FOX_joystick.h"
#include "FOX_quit.h"

// ustawienia

#include "FOX_begin.h"

// traktuj funkcje jako C

#ifdef __cplusplus
extern "C" {
#endif

// lista wyjatkow^zdarzen

enum { FOX_NOEVENT = 0,			// nie uzywany 
       FOX_ACTIVEEVENT,			// aplikacja niewidoczna
       FOX_KEYDOWN,				// nacisnieto klawisz na klawiaturze
       FOX_KEYUP,				// zwolniono klawisz na klawiaturze
       FOX_MOUSEMOTION,			// poruszono myszka
       FOX_MOUSEBUTTONDOWN,		// klawisz myszy nacisniety
       FOX_MOUSEBUTTONUP,		// kalwisz myszy zwolniony
       FOX_JOYAXISMOTION,		// joystick poruszony
       FOX_JOYBALLMOTION,		// joystick trackball poruszony
       FOX_JOYHATMOTION,		// joystick hat poruszony
       FOX_JOYBUTTONDOWN,		// joystick button nacisniety
       FOX_JOYBUTTONUP,			// Joystick button zwolniony
       FOX_QUIT,				// wyjscie wywolane przez uzyszkodnika
       FOX_SYSWMEVENT,			// event systemowy
       FOX_EVENT_RESERVEDA,		// zarezerwowany
       FOX_EVENT_RESERVEDB,		// zarezerwowany
       FOX_VIDEORESIZE,			// zmiana trybu video 
       FOX_VIDEOEXPOSE,			// odswiezenie ekranu
       FOX_EVENT_RESERVED2,		// zarezerwowany
       FOX_EVENT_RESERVED3,		// zarezerwowany
       FOX_EVENT_RESERVED4,		// zarezerwowany
       FOX_EVENT_RESERVED5,		// zarezerwowany
       FOX_EVENT_RESERVED6,		// zarezerwowany
       FOX_EVENT_RESERVED7,		// zarezerwowany
       
	   // tutaj az do FOX_MAXEVENT-1 do wyboru... 
	   
       FOX_USEREVENT = 24,
       
	   // ostatni event jest tylko dla wypelnienia maski Uint32
	  	  	  
       FOX_NUMEVENTS = 32
};

// przedefiniowane maski dla zdarzen^wyjatkow

#define FOX_EVENTMASK(X)	(1<<(X))
enum {
	FOX_ACTIVEEVENTMASK		= FOX_EVENTMASK(FOX_ACTIVEEVENT),
	FOX_KEYDOWNMASK			= FOX_EVENTMASK(FOX_KEYDOWN),
	FOX_KEYUPMASK			= FOX_EVENTMASK(FOX_KEYUP),
	FOX_MOUSEMOTIONMASK		= FOX_EVENTMASK(FOX_MOUSEMOTION),
	FOX_MOUSEBUTTONDOWNMASK	= FOX_EVENTMASK(FOX_MOUSEBUTTONDOWN),
	FOX_MOUSEBUTTONUPMASK	= FOX_EVENTMASK(FOX_MOUSEBUTTONUP),
	FOX_MOUSEEVENTMASK		= FOX_EVENTMASK(FOX_MOUSEMOTION)|
	                          FOX_EVENTMASK(FOX_MOUSEBUTTONDOWN)|
	                          FOX_EVENTMASK(FOX_MOUSEBUTTONUP),
	FOX_JOYAXISMOTIONMASK	= FOX_EVENTMASK(FOX_JOYAXISMOTION),
	FOX_JOYBALLMOTIONMASK	= FOX_EVENTMASK(FOX_JOYBALLMOTION),
	FOX_JOYHATMOTIONMASK	= FOX_EVENTMASK(FOX_JOYHATMOTION),
	FOX_JOYBUTTONDOWNMASK	= FOX_EVENTMASK(FOX_JOYBUTTONDOWN),
	FOX_JOYBUTTONUPMASK		= FOX_EVENTMASK(FOX_JOYBUTTONUP),
	FOX_JOYEVENTMASK		= FOX_EVENTMASK(FOX_JOYAXISMOTION)|
	                          FOX_EVENTMASK(FOX_JOYBALLMOTION)|
	                          FOX_EVENTMASK(FOX_JOYHATMOTION)|
	                          FOX_EVENTMASK(FOX_JOYBUTTONDOWN)|
	                          FOX_EVENTMASK(FOX_JOYBUTTONUP),
	FOX_VIDEORESIZEMASK		= FOX_EVENTMASK(FOX_VIDEORESIZE),
	FOX_VIDEOEXPOSEMASK		= FOX_EVENTMASK(FOX_VIDEOEXPOSE),
	FOX_QUITMASK			= FOX_EVENTMASK(FOX_QUIT),
	FOX_SYSWMEVENTMASK		= FOX_EVENTMASK(FOX_SYSWMEVENT)
};

#define FOX_ALLEVENTS		0xFFFFFFFF

// struktura zdarzenie^wyjatek  obslugujaca stan aplikacji (widzialnosc)

typedef struct {

	Uint8 type;		// FOX_ACTIVEEVENT
	Uint8 gain;		// czy utrzymany czy stracony stan 1/0
	Uint8 state;	// maska dla focus state

} FOX_ActiveEvent;

// obsluga zdarzen klawiatury

typedef struct {

	Uint8 type;		// FOX_KEYDOWN lub FOX_KEYUP 
	Uint8 which;	// keyboard device index 
	Uint8 state;	// FOX_PRESSED lub FOX_RELEASED

	FOX_keysym keysym;

} FOX_KeyboardEvent;

// obsluga zdarzen ruchu myszy

typedef struct {

	Uint8 type;		// FOX_MOUSEMOTION 
	Uint8 which;	// device index dla myszy
	Uint8 state;	// aktualny stan przyciskow

	Uint16 x, y;	// wspolrzedne x i y 

	Sint16 xrel;	// relatywna wspolrzedna ruchu X
	Sint16 yrel;	// relatywna wspolrzedna ruchu Y

} FOX_MouseMotionEvent;

// obsluga zdarzen przyciskow myszy

typedef struct {

	Uint8 type;		// FOX_MOUSEBUTTONDOWN lub FOX_MOUSEBUTTONUP
	Uint8 which;	// device index 
	Uint8 button;	// mouse button index 
	
	Uint8 state;	// FOX_PRESSED lub FOX_RELEASED 

	Uint16 x, y;	// wspolrzedne x i y w momencie nacisniecia

} FOX_MouseButtonEvent;

// obsluga zdarzen ruchu osi joysticka 
// !!! nie testowane !!! 

typedef struct {
	
	Uint8 type;		// FOX_JOYAXISMOTION 
	Uint8 which;	// joystick device index 
	Uint8 axis;		// joystick axis index 

	Sint16 value;	// wspolrzedna wychylenia (axis) -32768 do 32767)

} FOX_JoyAxisEvent;


// obsluga zdarzen ruchu joystick trackball 

typedef struct {

	Uint8 type;		// FOX_JOYBALLMOTION
	Uint8 which;	// joystick device index 
	Uint8 ball;		// joystick trackball index 

	Sint16 xrel;	// relatywna wspolrzedna X dla wychylenia
	Sint16 yrel;	// relatywna wspolrzedna Y dla wychylenia

} FOX_JoyBallEvent;

// koordynacja wychylenia wspolrzednych 
//				8   1   2
//				7   0   3
//				6   5   4

typedef struct {
	
	Uint8 type;		// FOX_JOYHATMOTION 
	Uint8 which;	// joystick device index 
	Uint8 hat;		// joystick hat index 
	Uint8 value;	// hat position value:

} FOX_JoyHatEvent;

// obsluga zdarzen przycisku joysticka

typedef struct {

	Uint8 type;		// FOX_JOYBUTTONDOWN lub FOX_JOYBUTTONUP 
	Uint8 which;	// joystick device index 

	Uint8 button;	// joystick button index 

	Uint8 state;	// FOX_PRESSED lub FOX_RELEASED 

} FOX_JoyButtonEvent;

// zdarzenie zmiany rozmiaru okna, albo przestawienia rozdzielczosci
// gdy takie zdarzenie sie pojawi, trzeba od nowa ustawic tryb graficzny, 
// przeliczyc wspolrzedne itd. 

typedef struct {

	Uint8 type;	// FOX_VIDEORESIZE 

	int w;		// szerokosc
	int h;		// wysokosc

} FOX_ResizeEvent;

// odswiezenie ekranu, to nie jest event gdzie rysujemy cos na ekranie !!!

typedef struct {
	
	Uint8 type;	// FOX_VIDEOEXPOSE 

} FOX_ExposeEvent;

// quit request event - natychmiastowe zadanie zakonczenia

typedef struct {

	Uint8 type;	// FOX_QUIT 

} FOX_QuitEvent;

// wlasne, predefiniowane zdarzenie

typedef struct {

	Uint8 type;		// FOX_USEREVENT az do FOX_NUMEVENTS-1 

	int code;		// kod definiujacy obsluge zdarzenia

	void *data1;	// wskaznik na dane
	void *data2;	// wskaznik na dane

} FOX_UserEvent;

// zdarzenie generowane przez system, jezeli ma byc uzywane to musi byc
// dolaczony FOX_syswm.h

struct FOX_SysWMmsg;

typedef struct FOX_SysWMmsg FOX_SysWMmsg;

typedef struct {
	Uint8 type;
	FOX_SysWMmsg *msg;
} FOX_SysWMEvent;

// glowna unia zdarzenia, pamietamy tylko jedno zdarzenie

typedef union {

	Uint8 type;

	FOX_ActiveEvent active;

	FOX_KeyboardEvent key;

	FOX_MouseMotionEvent motion;
	FOX_MouseButtonEvent button;

	FOX_JoyAxisEvent jaxis;
	FOX_JoyBallEvent jball;
	FOX_JoyHatEvent jhat;
	FOX_JoyButtonEvent jbutton;

	FOX_ResizeEvent resize;
	FOX_ExposeEvent expose;

	FOX_QuitEvent quit;

	FOX_UserEvent user;

	FOX_SysWMEvent syswm;

} FOX_Event;

// funkcja obslugujaca zdarzenie dla petli, do odswiezania informacji o events
// powinna byc wywolywana tylko w watku, ktory ustawia tryb graficzny

extern DECLSPEC void FOX_PumpEvents(void);

// sprawdz zdarzenia, i ewentualnie przeslij o nich informacje
// zwraca ilosc aktualnie oczekujaych zdarzen, lub -1 jezeli wystapil blad
// funkcja jest thread - safe 

typedef enum {
	FOX_ADDEVENT,
	FOX_PEEKEVENT,
	FOX_GETEVENT
} FOX_eventaction;

extern DECLSPEC int FOX_PeepEvents(FOX_Event *events, int numevents,
				FOX_eventaction action, Uint32 mask);

// zepchnij aktualnie zdarzenie, zwraca 1 jezeli sa jeszcze jakies oczekujace
// 0 jezeli nie ma, jezeli event jest rozny od NULL - nastepne zdarzenie jest
// usuwane i wciskane tutaj

extern DECLSPEC int FOX_PollEvent(FOX_Event *event);

// czekaj na konkretne zdarzenie, zwraca 1, albo 0 jezeli wystapil blad
// w czasie czekania na zdarzenie. jezeli zdarzenie nie jest NULL, nastepne
// jest przesuwane i wciskane tutaj

extern DECLSPEC int FOX_WaitEvent(FOX_Event *event);

// dodanie zdarzenia do listy, funkcja zwraca 0, albo -1 jezeli zdarzenie
// nie moze byc wcisniete
// !!! wypieprza sie przy przepelnieniu !!!

extern DECLSPEC int FOX_PushEvent(FOX_Event *event);

// ustawia filtr dla zdarzen

typedef int (*FOX_EventFilter)(const FOX_Event *event);

extern DECLSPEC void FOX_SetEventFilter(FOX_EventFilter filter);

// zwraca aktualny filtr dla zdarzenia

extern DECLSPEC FOX_EventFilter FOX_GetEventFilter(void);

// ustawianie statusu zdarzenia

#define FOX_QUERY	-1
#define FOX_IGNORE	 0
#define FOX_DISABLE	 0
#define FOX_ENABLE	 1

extern DECLSPEC Uint8 FOX_EventState(Uint8 type, int state);


// koniec traktowania jako funkcji C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end

