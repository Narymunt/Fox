// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO: 

// obsluga joysticka, przy FOX_Init() musi byc podana flaga FOX_INIT_JOYSTICK
// bo parametry przekazywane sa do directx

#ifndef _FOX_joystick_h
#define _FOX_joystick_h

#include "FOX_types.h"		// skroty i definicje

#include "FOX_begin.h"  // poczatkowe smieci

// traktuj jako funkcje C

#ifdef __cplusplus
extern "C" {
#endif

// struktura opisujaca joystick

struct _FOX_Joystick;
typedef struct _FOX_Joystick FOX_Joystick;

// ile mamy joystickow ?

extern DECLSPEC int FOX_NumJoysticks(void);

// pobierz nazwe joysticka, jak sie nie uda zwraca NULL

extern DECLSPEC const char *FOX_JoystickName(int device_index);

// zainicjalizuj joystick - dziala jak na karcie dzwiekowej
// zwraca identyfikator^handler, albo NULL jezeli wystapil blad

extern DECLSPEC FOX_Joystick *FOX_JoystickOpen(int device_index);

// jezeli port joysticka jest otwarty zwraca 1 w przeciwnym razie 0

extern DECLSPEC int FOX_JoystickOpened(int device_index);

// identyfikator otwartego portu joysticka

extern DECLSPEC int FOX_JoystickIndex(FOX_Joystick *joystick);

// pobierz ilosc kontrolek wychylenia joysticka

extern DECLSPEC int FOX_JoystickNumAxes(FOX_Joystick *joystick);

// pobierz ilosc trackbali na joysticku
// trackballa podaja tylko relatywne wspolrzedne

extern DECLSPEC int FOX_JoystickNumBalls(FOX_Joystick *joystick);

// podaj ilosc pov hats w joysticku

extern DECLSPEC int FOX_JoystickNumHats(FOX_Joystick *joystick);

// podaj ilosc przyciskow w joysticku

extern DECLSPEC int FOX_JoystickNumButtons(FOX_Joystick *joystick);

// aktualizuj stan joysticka, funkcja jest wywolywana automatycznie jezeli 
// wystapilo zdarzenie 

extern DECLSPEC void FOX_JoystickUpdate(void);

// zablokuj lub odblokuj spychanie zdarzen
// stan moze byc : FOX_QUERY, FOX_ENABLE, FOX_IGNORE
// jezeli zdarzenia joysticka sa zablokowane, trzeba wywola FOX_JoystickUpdate()
 
extern DECLSPEC int FOX_JoystickEventState(int state);

// pobierz aktualny stan wychylenia joysticka -32768 do 32768

extern DECLSPEC Sint16 FOX_JoystickGetAxis(FOX_Joystick *joystick, int axis);

// pobierz aktualny stan pov hat

#define FOX_HAT_CENTERED	0x00
#define FOX_HAT_UP			0x01
#define FOX_HAT_RIGHT		0x02
#define FOX_HAT_DOWN		0x04
#define FOX_HAT_LEFT		0x08
#define FOX_HAT_RIGHTUP		(FOX_HAT_RIGHT|FOX_HAT_UP)
#define FOX_HAT_RIGHTDOWN	(FOX_HAT_RIGHT|FOX_HAT_DOWN)
#define FOX_HAT_LEFTUP		(FOX_HAT_LEFT|FOX_HAT_UP)
#define FOX_HAT_LEFTDOWN	(FOX_HAT_LEFT|FOX_HAT_DOWN)

extern DECLSPEC Uint8 FOX_JoystickGetHat(FOX_Joystick *joystick, int hat);

// pobierz stan wychylenia od ostatniego zdarzenia
// jezeli zwrocil 0 lub -1 to parametry sa niewlasciwe

extern DECLSPEC int FOX_JoystickGetBall(FOX_Joystick *joystick, int ball, int *dx, int *dy);

// pobierz aktualny stan przyciskow joysticka

extern DECLSPEC Uint8 FOX_JoystickGetButton(FOX_Joystick *joystick, int button);

// zamknij port joysticka

extern DECLSPEC void FOX_JoystickClose(FOX_Joystick *joystick);

// koniec traktowania funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end


