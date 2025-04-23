// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO: 

// definicja typow, skrotow itd. 

#ifndef _FOX_types_h
#define _FOX_types_h

// numer elementow tablicy

#define FOX_TABLESIZE(table)	(sizeof(table)/sizeof(table[0]))

// tak na wszelki wypadek

typedef enum {
	FOX_FALSE = 0,
	FOX_TRUE  = 1
} FOX_bool;

// skroty

typedef unsigned char	Uint8;
typedef signed char		Sint8;
typedef unsigned short	Uint16;
typedef signed short	Sint16;
typedef unsigned int	Uint32;
typedef signed int		Sint32;

// dziala ? 

#define FOX_HAS_64BIT_TYPE	__int64
//#define FOX_HAS_64BIT_TYPE	long long

// w razie gdyby nie bylo long long

#ifdef FOX_HAS_64BIT_TYPE
typedef unsigned FOX_HAS_64BIT_TYPE Uint64;
typedef FOX_HAS_64BIT_TYPE Sint64;
#else
typedef struct {
	Uint32 hi;
	Uint32 lo;
} Uint64, Sint64;
#endif

// to dla pewnosci, ciezko stwierdzic jak to dziala... 

#define FOX_COMPILE_TIME_ASSERT(name, x)               \
       typedef int FOX_dummy_ ## name[(x) * 2 - 1]

FOX_COMPILE_TIME_ASSERT(uint8, sizeof(Uint8) == 1);
FOX_COMPILE_TIME_ASSERT(sint8, sizeof(Sint8) == 1);
FOX_COMPILE_TIME_ASSERT(uint16, sizeof(Uint16) == 2);
FOX_COMPILE_TIME_ASSERT(sint16, sizeof(Sint16) == 2);
FOX_COMPILE_TIME_ASSERT(uint32, sizeof(Uint32) == 4);
FOX_COMPILE_TIME_ASSERT(sint32, sizeof(Sint32) == 4);
FOX_COMPILE_TIME_ASSERT(uint64, sizeof(Uint64) == 8);
FOX_COMPILE_TIME_ASSERT(sint64, sizeof(Sint64) == 8);

#undef FOX_COMPILE_TIME_ASSERT

// to tak w gwoli scislosci

enum { FOX_PRESSED = 0x01, FOX_RELEASED = 0x00 };

#endif

// end
