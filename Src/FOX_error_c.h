// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// struktura danych dla obslugi bledow

#ifndef _FOX_error_c_h
#define _FOX_error_c_h

// tylko dla tego modulu

#define ERR_MAX_STRLEN	128
#define ERR_MAX_ARGS	5

typedef struct {
	
	int error;	// numer bledu
   
	unsigned char key[ERR_MAX_STRLEN]; // klucz dla string

	int argc;	// tak jak w main, tylko ze dla errhandler
	
	union {
		void *value_ptr;
#if 0	// dla unicode
		unsigned char value_c;
#endif
		int value_i;
		double value_f;
		unsigned char buf[ERR_MAX_STRLEN];
	} args[ERR_MAX_ARGS];
} FOX_error;

#endif 

// end

