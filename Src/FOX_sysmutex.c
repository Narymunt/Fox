// Fox v0.5
// by Jaroslaw Rozynski
//===
// *THREAD*
//===
// TODO:


// mutex dla windows przy wykorzystaniu api

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "FOX_error.h"
#include "FOX_mutex.h"

// struktura, w razie czego tutaj dodawac

struct FOX_mutex 
{
	HANDLE id;
};

// stworz mutex

FOX_mutex *FOX_CreateMutex(void)
{
	FOX_mutex *mutex;

	// pamiec 
	
	mutex = (FOX_mutex *)malloc(sizeof(*mutex));

	if ( mutex ) 
	{
		// tutaj id
		mutex->id = CreateMutex(NULL, FALSE, NULL);
		
		if ( ! mutex->id ) 
		{
			FOX_SetError("[FOX]: nie mozna stworzyc mutex");
			free(mutex);
			mutex = NULL;
		}
	} 
	else 
	{
		FOX_OutOfMemory();
	}
	return(mutex);
}

// zwolnij mutex

void FOX_DestroyMutex(FOX_mutex *mutex)
{
	if ( mutex ) 
	{
		if ( mutex->id ) 
		{
			CloseHandle(mutex->id);
			mutex->id = 0;
		}
		free(mutex);
	}
}

// zablokuj mutex

int FOX_mutexP(FOX_mutex *mutex)
{
	if ( mutex == NULL ) 
	{
		FOX_SetError("[FOX]: przejscie przez mutex NULL w FOX_sysmutex.c");
		return -1;
	}
	
	if ( WaitForSingleObject(mutex->id, INFINITE) == WAIT_FAILED ) 
	{
		FOX_SetError("[FOX]: nie mozna zaczekac na mutex [asynchroniczny?]");
		return -1;
	}
	return(0);
}

// odblokuj mutex

int FOX_mutexV(FOX_mutex *mutex)
{
	if ( mutex == NULL ) 
	{
		FOX_SetError("[FOX]: przejscie przez mutex NULL w FOX_sysmutex.c");
		return -1;
	}
	
	if ( ReleaseMutex(mutex->id) == FALSE ) 
	{
		FOX_SetError("[FOX]: nie mozna odblokowac i zwolnic mutex w FOX_sysmutex.c");
		return -1;
	}
	return(0);
}

// end
