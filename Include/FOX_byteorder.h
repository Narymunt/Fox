// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:
// - autodetekcja zapisu danych rgb w 16bpp


// przy konwersjach z 16bpp na 32bpp itp

#ifndef _FOX_byteorder_h
#define _FOX_byteorder_h

// dwa sposoby zapisu

#define FOX_LIL_ENDIAN	1234
#define FOX_BIG_ENDIAN	4321

// jaki jest wlasciwy ? 

#define FOX_BYTEORDER	FOX_LIL_ENDIAN

// w razie gdyby to nie chcialo dzialac to zamienic na drugi

// #define FOX_BYTEORDER	FOX_BIG_ENDIAN

#endif 

// end


