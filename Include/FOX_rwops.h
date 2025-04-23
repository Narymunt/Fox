// Fox v0.5
// by Jaroslaw Rozynski

// wczytywanie i zapisywanie

#ifndef _FOX_RWops_h
#define _FOX_RWops_h

#include <stdio.h>

#include "FOX_types.h"

// poczatkowe smieci

#include "FOX_begin.h"

// funkcje traktuj jako C

#ifdef __cplusplus
extern "C" {
#endif

// podstawowa struktura do rw

typedef struct FOX_RWops {

	int (*seek)(struct FOX_RWops *context, int offset, int whence);
	int (*read)(struct FOX_RWops *context, void *ptr, int size, int maxnum);
	int (*write)(struct FOX_RWops *context, const void *ptr, int size, int num);
	int (*close)(struct FOX_RWops *context);  // zamknij strukture

	Uint32 type;
	union {
	    struct {
		int autoclose;
	 	FILE *fp;
	    } stdio;

	    struct {
		Uint8 *base;
	 	Uint8 *here;
		Uint8 *stop;
	    } mem;

	    struct {
		void *data1;
	    } unknown;

	} hidden;

} FOX_RWops;

// funkcja do obslugi struktury RW

extern DECLSPEC FOX_RWops * FOX_RWFromFile(const char *file, const char *mode);

extern DECLSPEC FOX_RWops * FOX_RWFromFP(FILE *fp, int autoclose);

extern DECLSPEC FOX_RWops * FOX_RWFromMem(void *mem, int size);

extern DECLSPEC FOX_RWops * FOX_AllocRW(void);

extern DECLSPEC void FOX_FreeRW(FOX_RWops *area);

// makra ulatwiajace operowanie na strukturze

#define FOX_RWseek(ctx, offset, whence)	(ctx)->seek(ctx, offset, whence)
#define FOX_RWtell(ctx)			(ctx)->seek(ctx, 0, SEEK_CUR)
#define FOX_RWread(ctx, ptr, size, n)	(ctx)->read(ctx, ptr, size, n)
#define FOX_RWwrite(ctx, ptr, size, n)	(ctx)->write(ctx, ptr, size, n)
#define FOX_RWclose(ctx)		(ctx)->close(ctx)

// koniec funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif 

// end

