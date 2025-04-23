// Fox v0.5
// by Jaroslaw Rozynski

// obsluga bledow 
// TODO:
// - sporo ;)
// - logowanie wszystkich akcji do pliku

#ifndef _FOX_error_h
#define _FOX_error_h

// poczatkowe ustawienia

#include "FOX_begin.h"

// traktuj funkcje jako w C

#ifdef __cplusplus
extern "C" {
#endif

// domyslne rozszerzenia 

extern DECLSPEC void FOX_SetError(const char *fmt, ...);
extern DECLSPEC char * FOX_GetError(void);
extern DECLSPEC void FOX_ClearError(void);

// szczegolowe info

#define FOX_OutOfMemory()	FOX_Error(FOX_ENOMEM)
typedef enum {
	FOX_ENOMEM,
	FOX_EFREAD,
	FOX_EFWRITE,
	FOX_EFSEEK,
	FOX_LASTERROR
} FOX_errorcode;
extern void FOX_Error(FOX_errorcode code);
 
// koniec traktowania funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end
