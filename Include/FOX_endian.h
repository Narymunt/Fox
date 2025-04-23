// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - lepsza obsluga 64bitowych zmiennych

// funkcje do odczytu i konwersji bitowych
// poza tym funkcje sa zalezne od RWops 
// RWops sa uzywane zamiast wskaznikow FILE

#ifndef _FOX_endian_h
#define _FOX_endian_h

#include <stdio.h>

#include "FOX_types.h"			// typy danych
#include "FOX_rwops.h"			// read write ops
#include "FOX_byteorder.h"		// 1234 czy 4321 ? 

// dodatkowe deklaracje

#include "FOX_begin.h"

// traktuj funkcje jako C

#ifdef __cplusplus
extern "C" {
#endif

// funkcje inline dzialaja o wiele szybciej

// zamiana bajtow w slowie

#ifndef FOX_Swap16
static __inline__ Uint16 FOX_Swap16(Uint16 D) {
	return((D<<8)|(D>>8));
}
#endif

// zamiana slow w podwojnym slowie 1234 -> 3412

#ifndef FOX_Swap32
static __inline__ Uint32 FOX_Swap32(Uint32 D) {
	return((D<<24)|((D<<8)&0x00FF0000)|((D>>8)&0x0000FF00)|(D>>24));
}
#endif

// ciezka sprawa... !!!nie testowane!!!

#ifdef FOX_HAS_64BIT_TYPE
#ifndef FOX_Swap64
static __inline__ Uint64 FOX_Swap64(Uint64 val) {
	Uint32 hi, lo;

	// rozdziel na dwa slowa i wtedy zamien
	
	lo = (Uint32)(val&0xFFFFFFFF);
	val >>= 32;
	hi = (Uint32)(val&0xFFFFFFFF);
	val = FOX_Swap32(lo);
	val <<= 32;
	val |= FOX_Swap32(hi);
	return(val);
}
#endif
#else
#ifndef FOX_Swap64

// fakeowany tryb int64

#define FOX_Swap64(X)	(X)
#endif
#endif 

// swap z innego ne na domyslny ne

#if FOX_BYTEORDER == FOX_LIL_ENDIAN
	#define FOX_SwapLE16(X)	(X)
	#define FOX_SwapLE32(X)	(X)
	#define FOX_SwapLE64(X)	(X)
	#define FOX_SwapBE16(X)	FOX_Swap16(X)
	#define FOX_SwapBE32(X)	FOX_Swap32(X)
	#define FOX_SwapBE64(X)	FOX_Swap64(X)
#else
	#define FOX_SwapLE16(X)	FOX_Swap16(X)
	#define FOX_SwapLE32(X)	FOX_Swap32(X)
	#define FOX_SwapLE64(X)	FOX_Swap64(X)
	#define FOX_SwapBE16(X)	(X)
	#define FOX_SwapBE32(X)	(X)
	#define FOX_SwapBE64(X)	(X)
#endif

// odczytaj zmienna i zwroc ja we wlasciwym formacie

extern DECLSPEC Uint16 FOX_ReadLE16(FOX_RWops *src);
extern DECLSPEC Uint16 FOX_ReadBE16(FOX_RWops *src);
extern DECLSPEC Uint32 FOX_ReadLE32(FOX_RWops *src);
extern DECLSPEC Uint32 FOX_ReadBE32(FOX_RWops *src);
extern DECLSPEC Uint64 FOX_ReadLE64(FOX_RWops *src);
extern DECLSPEC Uint64 FOX_ReadBE64(FOX_RWops *src);

// zapisz zmienna w domyslnym formacie

extern DECLSPEC int FOX_WriteLE16(FOX_RWops *dst, Uint16 value);
extern DECLSPEC int FOX_WriteBE16(FOX_RWops *dst, Uint16 value);
extern DECLSPEC int FOX_WriteLE32(FOX_RWops *dst, Uint32 value);
extern DECLSPEC int FOX_WriteBE32(FOX_RWops *dst, Uint32 value);
extern DECLSPEC int FOX_WriteLE64(FOX_RWops *dst, Uint64 value);
extern DECLSPEC int FOX_WriteBE64(FOX_RWops *dst, Uint64 value);

// koniec uzywania funkcji C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end


