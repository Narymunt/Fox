// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:

// w razie gdyby putenv() i getenv() nie zwracalay 

// w razie testowania

#include "FOX_getenv.h"

#ifdef NEED_FOX_GETENV

#include <stdlib.h>
#include <string.h>

static char **FOX_env = (char **)0;

// ustaw name=value, ciezko sprawdzic jak traktuje to windows, czasem wycina

int FOX_putenv(const char *variable)
{
	const char *name, *value;
	int added;
	int len, i;
	char **new_env;
	char *new_variable;

	// na wszelki wypadek
	
	if ( ! variable ) 
	{
		return(-1);
	}

	name = variable;
	
	for ( value=variable; *value && (*value != '='); ++value ) 
	{
		// tutaj szukanie 
		;
	}
	
	if ( *value ) 
	{
		++value;
	} 
	else 
	{
		return(-1);
	}

	// zaalokuj pamiec na zmienna
	
	new_variable = (char *)malloc(strlen(variable)+1);
	
	if ( ! new_variable ) 
	{
		return(-1);
	}
	
	strcpy(new_variable, variable);

	// aktualnie w srodowisku
	
	added = 0;
	
	i = 0;
	
	if ( FOX_env ) 
	{
		// zobacz czy juz nie ma 
		
		len = (value - name);
		
		for ( ; FOX_env[i]; ++i ) 
		{
			if ( strncmp(FOX_env[i], name, len) == 0 ) 
			{
				break;
			}
		}
		
		// proba zamiany 

		if ( FOX_env[i] ) 
		{
			free(FOX_env[i]);
			FOX_env[i] = new_variable;
			added = 1;
		}
	}

	// nie ma takiej jeszcze - dodaj i rozszerz
	
	if ( ! added ) 
	{
		new_env = realloc(FOX_env, (i+2)*sizeof(char *));
		
		if ( new_env ) 
		{
			FOX_env = new_env;
			FOX_env[i++] = new_variable;
			FOX_env[i++] = (char *)0;
			added = 1;
		} 
		else 
		{
			free(new_variable);
		}
	}
	return (added ? 0 : -1);
}

// pobierz wartosc zmiennej ze srodowiska

char *FOX_getenv(const char *name)
{
	int len, i;
	char *value;

	value = (char *)0;
	
	if ( FOX_env ) 
	{
		len = strlen(name);
		for ( i=0; FOX_env[i] && !value; ++i ) 
		{
			if ( (strncmp(FOX_env[i], name, len) == 0) &&
			     (FOX_env[i][len] == '=') ) 
			{
				value = &FOX_env[i][len+1];
			}
		}
	}
	return value;
}

#endif 

// end

