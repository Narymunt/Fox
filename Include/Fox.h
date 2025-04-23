// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - 2d primitives
// - 3d parse
// - swf
// - dodac FOX_tables.h
// - tablice offsetow dla Y
// - tablice offsetow dla dzielen


// glowny plik z deklaracjami funkcji, struktur itd. 
// w pliku .cpp piszemy tylko include fox.h

// sprawdz czy ten plik juz nie byl wczesniej dolaczony

#ifndef _FOX_H
#define _FOX_H

#include "FOX_main.h"		// glowne funkcje
#include "FOX_types.h"		// definicje i skroty 
#include "FOX_getenv.h"		// zmienne srodowiskowe
#include "FOX_error.h"		// obsluga bledow
#include "FOX_rwops.h"		// operacje zapisu i odczytu na strumieniach
#include "FOX_timer.h"		// obsluga zegara + directX
#include "FOX_audio.h"		// obsluga dzwieku + directX
#include "FOX_cdrom.h"		// obsluga dzwieku jako sciezki CD
#include "FOX_joystick.h"	// obsluga joysticka
#include "FOX_events.h"		// obsluga zdarzen + directX
#include "FOX_video.h"		// obsluga ekranu
#include "FOX_byteorder.h"	// sposob zapisywania danych 16bpp, 32bpp itd
#include "FOX_version.h"	// kontrola wersji
#include "FOX_filesys.h"	// obsluga spakowanego systemu plikow
//#include "FOX_matrix.h"		// obsluga macierzy
#include "FOX_draw2d.h"		// rysowanie 2d prymitywow
#include "FOX_begin.h"		// wstawki pack itd. 



// przestaw definicje funkcji na typy c nawet jesli uzywasz c++

#ifdef __cplusplus
extern "C" {
#endif

// biblioteka jest ladowana dynamicznie

// flagi dla funkcji FOX, uzywamy praktycznnie tylko przy FOX_Init()
// np FOX_Init(FOX_INIT_TIMER|FOX_INIT_VIDEO)

#define	FOX_INIT_TIMER		  0x00000001		// czy uzywamy timer'ow ? 
#define FOX_INIT_AUDIO		  0x00000010		// czy uzywamy karty dzwiekowej ?
#define FOX_INIT_VIDEO		  0x00000020		// czy uzywamy karty graficznej ? :)))
#define FOX_INIT_CDROM		  0x00000100		// czy bedziemy odtwarzac sciezke audio z CD ?
#define FOX_INIT_JOYSTICK	  0x00000200		// czy bedziemy uzywac joysticka ? 
#define FOX_INIT_NOPARACHUTE  0x00100000		// czy bedziemy przechwytywac wyjatki (exceptions)	
#define FOX_INIT_EVENTTHREAD  0x01000000		// czy w thread maja byc generowane event'y ? 
#define FOX_INIT_EVERYTHING	  0x0000FFFF		// czy wszystko inicjalizaowac ?

// ta funkcja laduje fox jako dll i inicjalizuje wszystkie potrzebne rzeczy,
// ktore sobie wybralismmy. zwraca uwage przede wszystkim na FOX_INIT_NOPARACHUTE

extern DECLSPEC int FOX_Init(Uint32 flags);

// inicjalizacja podsystemu fox'a

extern DECLSPEC int FOX_InitSubSystem(Uint32 flags);

// deinicjalizacja podsystemu fox'a

extern DECLSPEC void FOX_QuitSubSystem(Uint32 flags);

// funkcja do zwrotu maski, ktory z podsystemu zostal zainicjalizowany
// jezeli flags jest ustawiona na 0 to funkcja zwraca maske dla wszystkich
// zainicjalizowany podsystemow

extern DECLSPEC Uint32 FOX_WasInit(Uint32 flags);

// czyszczenie i wyjscie, oprocz tego usuwa wszystkie podsystemy i 
// zwalnia pamiec po wszystkich dll

extern DECLSPEC void FOX_Quit(void);

// koniec przestawiania metody definicji funkcji

#ifdef __cplusplus
}
#endif

#include "FOX_end.h"

#endif 

// end
