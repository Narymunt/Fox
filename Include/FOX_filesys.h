// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// osbluga systemu plikow
#ifndef _fox__filesys_h_
#define _fox__filesys_h_

#include "FOX_begin.h"  // poczatkowe smieci

// traktuj jako funkcje C

#ifdef __cplusplus
extern "C" {
#endif

extern DECLSPEC void FOX_filesys_archive(char name[]);
extern DECLSPEC long FOX_filesys_search(char name[]);
extern DECLSPEC void FOX_filesys_load_in(unsigned char *tbuffer);
extern DECLSPEC int FOX_filesys_load(unsigned char *buffer,char filename[]);

// koniec traktowania funkcji jako C

#ifdef __cplusplus
}
#endif

// koncowe smieci

#include "FOX_end.h"

#endif

// end

