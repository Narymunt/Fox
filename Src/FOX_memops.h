// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - czy to potrzebne zamiast memcpy

#ifndef _FOX_memops_h
#define _FOX_memops_h

#include <string.h>

// w razie czego dopisac w asm

#ifndef FOX_memcpy
	#define FOX_memcpy(dst, src, len)	memcpy(dst, src, len)
#endif

#ifndef FOX_memcpy4
	#define FOX_memcpy4(dst, src, len)	memcpy(dst, src, (len) << 2)
#endif

#ifndef FOX_revcpy
	#define FOX_revcpy(dst, src, len)	memmove(dst, src, len)
#endif

#ifndef FOX_memset4
#define FOX_memset4(dst, val, len)		\
do {						\
	unsigned _count = (len);		\
	unsigned _n = (_count + 3) / 4;		\
	Uint32 *_p = (Uint32 *)(dst);		\
	Uint32 _val = (val);			\
        switch (_count % 4) {			\
        case 0: do {    *_p++ = _val;		\
        case 3:         *_p++ = _val;		\
        case 2:         *_p++ = _val;		\
        case 1:         *_p++ = _val;		\
		} while ( --_n );		\
	}					\
} while(0)
#endif

#endif 

// end
