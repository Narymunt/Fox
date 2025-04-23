// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

//===  obsluga bledow

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

//=== tutaj nasze

#include "FOX_types.h"
#include "FOX_getenv.h"
#include "FOX_error.h"
#include "FOX_error_c.h"

//=== threads czyli watki nie zawsze musza byc uzywane

#ifndef DISABLE_THREADS
	#include "FOX_thread_c.h"
#endif

//=== domyslny, nie obsluguje bledow w watkach, nie przerywa z tego powodu aplikacji

#ifdef DISABLE_THREADS
	static FOX_error FOX_global_error;
	#define FOX_GetErrBuf()	(&FOX_global_error)
#endif 

//=== bufor na komunikaty, parachute itd. 

#define FOX_ERRBUFIZE	1024

//=== funkcje prywatne, nie uzywac poza FOX

static void FOX_LookupString(const Uint8 *key, Uint16 *buf, int buflen)
{
	
	// tego klawisza nie ma w tablicy
	
	while ( *key && (--buflen > 0) ) 
	{
		*buf++ = *key++;
	}

	*buf = 0;	// lancuch znakow konczy sie na 0 a nie 0x0d,0x0a
}

//=== publiczna, mozna wciskac do klass itd

void FOX_SetError (const char *fmt, ...)
{
	va_list ap;
	FOX_error *error;

	// skopiuj klawisz i maski

	error = FOX_GetErrBuf();
	error->error = 1;
	
	strncpy((char *)error->key, fmt, sizeof(error->key));
	
	error->key[sizeof(error->key)-1] = '\0';

	va_start(ap, fmt);
	
	error->argc = 0;
	
	while ( *fmt ) 
	{
		if ( *fmt++ == '%' ) 
		{
			switch (*fmt++) 
			{
			    case 0:  
				--fmt;
				break;
#if 0	/* co to za znak ? (UNICODE issues) */
			    
				case 'c':
				error->args[error->argc++].value_c =
						va_arg(ap, unsigned char);
				break;
#endif

				case 'd':
				error->args[error->argc++].value_i =
							va_arg(ap, int);
				break;

				case 'f':
				error->args[error->argc++].value_f =
							va_arg(ap, double);
				break;

				case 'p':
				error->args[error->argc++].value_ptr =
							va_arg(ap, void *);
				break;

				case 's':
				{
				  int index = error->argc;
				
				  strncpy((char *)error->args[index].buf,
						  va_arg(ap, char *), ERR_MAX_STRLEN);
				  error->args[index].buf[ERR_MAX_STRLEN-1] = 0;
				  error->argc++;
				}

				break;

			    default:
				break;
			}
			
			if ( error->argc >= ERR_MAX_ARGS ) 
			{
				break;
			}
		}
	}
	
	va_end(ap);

#ifndef DISABLE_STDIO
	
	// jezeli to jest debug mode, to stderr.txt
		
#ifdef DEBUG_ERROR

	fprintf(stderr, "FOX_SetError: %s\n", FOX_GetError());

#else

	if ( getenv("FOX_DEBUG") ) 
	{
		fprintf(stderr, "FOX_SetError: %s\n", FOX_GetError());
	}
#endif

#endif // diable _stdio
}

// wypisz do unicode buffer

static int PrintInt(Uint16 *str, unsigned int maxlen, int value)
{
	char tmp[128];
	int len, i;

	sprintf(tmp, "%d", value);
	
	len = 0;
	
	if ( strlen(tmp) < maxlen ) 
	{
		for ( i=0; tmp[i]; ++i ) 
		{
			*str++ = tmp[i];
			++len;
		}
	}
	
	return(len);
}

// wyrzuc jako double na unicode buffer


static int PrintDouble(Uint16 *str, unsigned int maxlen, double value)
{
	char tmp[128];
	int len, i;

	sprintf(tmp, "%f", value);

	len = 0;
	
	if ( strlen(tmp) < maxlen ) 
	{
		for ( i=0; tmp[i]; ++i ) 
		{
			*str++ = tmp[i];
			++len;
		}
	}
	return(len);
}

// wyrzuc jako pointer do unibuffer

static int PrintPointer(Uint16 *str, unsigned int maxlen, void *value)
{
	char tmp[128];
	int len, i;

	sprintf(tmp, "%p", value);

	len = 0;
	
	if ( strlen(tmp) < maxlen ) 
	{
		for ( i=0; tmp[i]; ++i ) 
		{
			*str++ = tmp[i];
			++len;
		}
	}
	return(len);
}

// thread safe error

Uint16 *FOX_GetErrorMsgUNICODE(Uint16 *errstr, unsigned int maxlen)
{
	FOX_error *error;

	// wyczysc bufor dla komunikatu bledow
	
	*errstr = 0; --maxlen;

	// wypisz info jaki blad
	
	error = FOX_GetErrBuf();
	
	if ( error->error ) 
	{
		Uint16 translated[ERR_MAX_STRLEN], *fmt, *msg;
		int len;
		int argi;

		// wypisz blad
		FOX_LookupString(error->key, translated, sizeof(translated));
		
		msg = errstr;
		argi = 0;
		
		for ( fmt=translated; *fmt && (maxlen > 0); ) 
		{
			if ( *fmt == '%' ) 
			{
				switch (fmt[1]) 
				{
				    case 'S':	/* SKIP */
					argi += (fmt[2] - '0');
					++fmt;
					break;
				    
					case '%':
					*msg++ = '%';
					maxlen -= 1;
					break;
#if 0	
				    case 'c':
                                        *msg++ = (unsigned char)
					         error->args[argi++].value_c;
					maxlen -= 1;
					break;
#endif
				    case 'd':
					len = PrintInt(msg, maxlen,
						error->args[argi++].value_i);
					msg += len;
					maxlen -= len;
					break;

					case 'f':
					len = PrintDouble(msg, maxlen,
						error->args[argi++].value_f);
					msg += len;
					maxlen -= len;
					break;
				    
					case 'p':
					len = PrintPointer(msg, maxlen,
						error->args[argi++].value_ptr);
					msg += len;
					maxlen -= len;
					break;
				    
					case 's': /* UNICODE string */
					{ Uint16 buf[ERR_MAX_STRLEN], *str;
					  FOX_LookupString(error->args[argi++].buf, buf, sizeof(buf));
					  str = buf;
					  while ( *str && (maxlen > 0) ) 
					  {
						*msg++ = *str++;
						maxlen -= 1;
					  }
					}
					break;
				}
				fmt += 2;
			} else 
			{
				*msg++ = *fmt++;
				maxlen -= 1;
			}
		}
		*msg = 0;	// lancuch konczy 0, a nie 0x0d 0x0a
	}
	return(errstr);
}

// zwroc error msg

Uint8 *FOX_GetErrorMsg(Uint8 *errstr, unsigned int maxlen)
{
	Uint16 *errstr16;
	unsigned int i;

	// alokacja bufora unicode

	errstr16 = (Uint16 *)malloc(maxlen * (sizeof *errstr16));
	
	if ( ! errstr16 ) 
	{
		strncpy((char *)errstr, "Out of memory", maxlen);
		errstr[maxlen-1] = '\0';
		return(errstr);
	}

	// podaj kod bledu
	
	FOX_GetErrorMsgUNICODE(errstr16, maxlen);

	// konwersja na latin1 
	
	for ( i=0; i<maxlen; ++i ) 
	{
		errstr[i] = (Uint8)errstr16[i];
	}

	// zwolnij bufor
	
	free(errstr16);

	return(errstr);
}

// na wszelki wypadek 

char *FOX_GetError (void)
{
	static char errmsg[FOX_ERRBUFIZE];

	return((char *)FOX_GetErrorMsg((unsigned char *)errmsg, FOX_ERRBUFIZE));
}

// wyczysc liste bledow

void FOX_ClearError(void)
{
	FOX_error *error;

	error = FOX_GetErrBuf();
	error->error = 0;
}

//=== typowe bledy 

void FOX_Error(FOX_errorcode code)
{
	switch (code) 
	{
		case FOX_ENOMEM:
			FOX_SetError("[FOX]: za malo pamieci");
			break;
		
		case FOX_EFREAD:
			FOX_SetError("[FOX]: blad w odczycie ze strumienia");
			break;
		
		case FOX_EFWRITE:
			FOX_SetError("[FOX]: blad w zapisie do strumienia");
			break;

		case FOX_EFSEEK:
			FOX_SetError("[FOX]: blad w otwieraniu strumienia (seek)");
			break;

		default:
			FOX_SetError("[FOX]: nieznany blad");
			break;
	}
}

// end

