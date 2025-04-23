// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// do sprawdzania wersji itp.

#ifndef _FOX_version_h
#define _FOX_version_h

#include "FOX_types.h"

// poczatkowe smieci

#include "FOX_begin.h"

// funkcje traktuj jako c

#ifdef __cplusplus
extern "C" {
#endif

// format zapisany jest jako a,b,c czyli MAJOR, MINOR, PATCHLEVEL

#define FOX_MAJOR_VERSION	0
#define FOX_MINOR_VERSION	1
#define FOX_PATCHLEVEL		0

typedef struct {
	
	Uint8 major;
	Uint8 minor;
	Uint8 patch;

} FOX_version;

// macro do wypelniania info o wersji w bibliotece

#define FOX_VERSION(X)					\
{										\
	(X)->major = FOX_MAJOR_VERSION;		\
	(X)->minor = FOX_MINOR_VERSION;		\
	(X)->patch = FOX_PATCHLEVEL;		\
}

// macro zmienia numerki na wartosc numeryczna czyli 1,2,3 na 1203

#define FOX_VERSIONNUM(X, Y, Z)						\
	(X)*1000 + (Y)*100 + (Z)

// macro dla wersjonowania

#define FOX_COMPILEDVERSION \
	FOX_VERSIONNUM(FOX_MAJOR_VERSION, FOX_MINOR_VERSION, FOX_PATCHLEVEL)

// wersja co najmniej ... 

#define FOX_VERSION_ATLEAST(X, Y, Z) \
	(FOX_COMPILEDVERSION >= FOX_VERSIONNUM(X, Y, Z))

// pobierz wersje z dll

extern DECLSPEC const FOX_version * FOX_Linked_Version(void);

// koniec funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end
