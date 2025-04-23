// Fox v0.5 
// by Jaroslaw Rozynski

// starter biblioteki

#ifndef _FOX_main_h
#define _FOX_main_h

// obsluga main i Winmain -> szczegoly w winmain.c

	#ifdef __cplusplus
		#define C_LINKAGE	"C"
	#else
		#define C_LINKAGE
	#endif 

#define main	FOX_main

// prototyp funkcji glownej

extern C_LINKAGE int FOX_main(int argc, char *argv[]);

// potrzebne 

	#include "FOX_types.h"
	
	#include "FOX_begin.h"	// poczatkowe smieci 

	// traktuj funkcje jako C

	#ifdef __cplusplus
		extern "C" {
	#endif

	// to ewentualnie wywolywac w WinMain()

	extern DECLSPEC void FOX_SetModuleHandle(void *hInst);

	// to tylko na wszelki wypadek, ale raczej jest niepotrzebne 

	extern DECLSPEC int FOX_RegisterApp(char *name, Uint32 style, void *hInst);

	// koniec funkcji jako C

	#ifdef __cplusplus
		}
	#endif

	// koncowe smieci

	#include "FOX_end.h"


#endif 

// end

