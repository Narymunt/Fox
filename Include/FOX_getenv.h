// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:


// ustawianie i pobieranie zmiennych srodowiskowych
// kompletna bzdura ale czasem potrzebne
// w razie czego wystarczy usunac ta linie

// #define NEED_FOX_GETENV

#ifdef NEED_FOX_GETENV

#include "FOX_begin.h"	// poczatkowe ustawienia

// traktuj funkcje jako C

#ifdef __cplusplus
	extern "C" {
#endif

// ustaw zmienna na form "name=value" w srodowisku 
// !!! nie testowane !!!

extern DECLSPEC int FOX_putenv(const char *variable);
#define putenv(X)   FOX_putenv(X)

// pobierz zmienna o nazwie name ze srodowiska
// !!! nie testowane !!!

extern DECLSPEC char *FOX_getenv(const char *name);
#define getenv(X)     FOX_getenv(X)

// koniec traktowania funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end


