// Fox v0.7.0
// by Jaroslaw Rozynski
//===
// *FILE*
//===
// TODO:
// - memread&write niepotrzebne ? 
// - memseek, potrzebne ? 
// - memclose, potrzebne ? 
// - wszystkie seterror - podzial na konsole i plik
// - FOX_RWops *FOX_RWFromFile(const char *file, const char *mode)
//	 bledy do konsoli i pliku
// - FOX_RWops *FOX_RWFromMem(void *mem, int size), potrzebne ? 
// - na konsole caly logger

// odczyt i zapis 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "FOX_error.h"
#include "FOX_rwops.h"

// dla pliku

static int stdio_seek(FOX_RWops *context, int offset, int whence)
{

	if ( fseek(context->hidden.stdio.fp, offset, whence) == 0 ) 
		return(ftell(context->hidden.stdio.fp));
	else 
	{
		FOX_Error(FOX_EFSEEK);
		return(-1);
	}
}

// odczyt przez fread

static int stdio_read(FOX_RWops *context, void *ptr, int size, int maxnum)
{
	size_t nread;

	nread = fread(ptr, size, maxnum, context->hidden.stdio.fp); 

	if ( nread == 0 && ferror(context->hidden.stdio.fp) ) 
		FOX_Error(FOX_EFREAD);
	
	return(nread);
}

// zapis przez fwrite

static int stdio_write(FOX_RWops *context, const void *ptr, int size, int num)
{
	size_t nwrote;

	nwrote = fwrite(ptr, size, num, context->hidden.stdio.fp);

	if ( nwrote == 0 && ferror(context->hidden.stdio.fp) ) 
	{
		FOX_Error(FOX_EFWRITE);
	}
	return(nwrote);
}

// zamknij 

static int stdio_close(FOX_RWops *context)
{
	if ( context ) 
	{
		// !!! sprawdzamy wartosc !!!
		if ( context->hidden.stdio.autoclose ) fclose(context->hidden.stdio.fp);			
		
		free(context);
	}
	return(0);
}

// odczyt i zapis na wskaznikach 

static int mem_seek(FOX_RWops *context, int offset, int whence)
{
	Uint8 *newpos;

	switch (whence) 
	{
		case SEEK_SET:
			newpos = context->hidden.mem.base+offset;
			break;
		case SEEK_CUR:
			newpos = context->hidden.mem.here+offset;
			break;
		case SEEK_END:
			newpos = context->hidden.mem.stop+offset;
			break;
		default:
			FOX_SetError("[FOX]: nieznana wartosc dla whence");
			return(-1);
	}
	
	if ( newpos < context->hidden.mem.base ) 
		newpos = context->hidden.mem.base;
	
	if ( newpos > context->hidden.mem.stop ) 
		newpos = context->hidden.mem.stop;
	
	context->hidden.mem.here = newpos;
	
	return(context->hidden.mem.here-context->hidden.mem.base);
}

// tak samo tylko, ze na pamieci 

static int mem_read(FOX_RWops *context, void *ptr, int size, int maxnum)
{
	int num;

	num = maxnum;
	
	if ( (context->hidden.mem.here + (num*size)) > context->hidden.mem.stop ) 
		num = (context->hidden.mem.stop-context->hidden.mem.here)/size;
	
	memcpy(ptr, context->hidden.mem.here, num*size);
	context->hidden.mem.here += num*size;
	return(num);
}

// tutaj zapis

static int mem_write(FOX_RWops *context, const void *ptr, int size, int num)
{

	if ( (context->hidden.mem.here + (num*size)) > context->hidden.mem.stop ) 
		num = (context->hidden.mem.stop-context->hidden.mem.here)/size;
	
	memcpy(context->hidden.mem.here, ptr, num*size);
	context->hidden.mem.here += num*size;
	return(num);
}

// zwolnij pamiec

static int mem_close(FOX_RWops *context)
{
	if ( context ) free(context);
	return(0);
}

// funkcje dla tworzenia rwops

static int in_FOX = 0;

// nie mozna wczytywac do dll ?

FOX_RWops *FOX_RWFromFile(const char *file, const char *mode)
{
	FILE *fp;
	FOX_RWops *rwops;

	rwops = NULL;

	// tutaj otwieramy plik 

	if ( (fp=fopen(file,mode)) == NULL ) FOX_SetError("[FOX]: nie mozna otworzyc pliku %s", file);
	else 
	{
		in_FOX = 1;
		rwops = FOX_RWFromFP(fp, 1);
		in_FOX = 0;
	}
	return(rwops);
}

// na pliku, odczyt  

FOX_RWops *FOX_RWFromFP(FILE *fp, int autoclose)
{
	FOX_RWops *rwops;

	if ( ! in_FOX ) {
		FOX_SetError("[FOX]: nie mozna przeniesc wskaznika pliku do dll (?)");
		/*return(NULL);*/
	}

	rwops = FOX_AllocRW();	// tutaj skok 

	if ( rwops != NULL ) 
	{
		rwops->seek = stdio_seek;
		rwops->read = stdio_read;
		rwops->write = stdio_write;
		rwops->close = stdio_close;
		rwops->hidden.stdio.fp = fp;
		rwops->hidden.stdio.autoclose = autoclose;
	}
	return(rwops);
}

// tak samo tylko z pamieci 

FOX_RWops *FOX_RWFromMem(void *mem, int size)
{
	FOX_RWops *rwops;

	rwops = FOX_AllocRW();
	
	if ( rwops != NULL ) 
	{
		rwops->seek = mem_seek;
		rwops->read = mem_read;
		rwops->write = mem_write;
		rwops->close = mem_close;
		rwops->hidden.mem.base = (Uint8 *)mem;
		rwops->hidden.mem.here = rwops->hidden.mem.base;
		rwops->hidden.mem.stop = rwops->hidden.mem.base+size;
	}
	return(rwops);
}

// przepraszam, czy mozna ? 

FOX_RWops *FOX_AllocRW(void)
{
	FOX_RWops *area;

	area = (FOX_RWops *)malloc(sizeof *area);
	
	if ( area == NULL ) FOX_OutOfMemory();	// tutaj zamienic na logger i konsole
	
	return(area);
}

// zwolnij to gowienko 

void FOX_FreeRW(FOX_RWops *area)
{
	free(area);
}

// end


