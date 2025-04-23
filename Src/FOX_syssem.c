// Fox v0.5
// by Jaroslaw Rozynski
//===
// *THREAD*
//===
// TODO:


// dodatkowe dla watkow

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "FOX_error.h"
#include "FOX_thread.h"

struct FOX_semaphore 
{
	HANDLE id;
	Uint32 volatile count;
};

// nowy semafor 


FOX_sem *FOX_CreateSemaphore(Uint32 initial_value)
{
	FOX_sem *sem;

	// pamiec na semafor 
	sem = (FOX_sem *)malloc(sizeof(*sem));
	
	if ( sem ) 
	{
		// max 32k

		sem->id = CreateSemaphore(NULL, initial_value, 32*1024, NULL);
		sem->count = initial_value;
		
		if ( ! sem->id ) 
		{
			FOX_SetError("[FOX]: nie mozna stworzyc semafora");
			free(sem);
			sem = NULL;
		}
	} 
	else 
	{
		FOX_OutOfMemory();
	}
	return(sem);
}

// zwolnij semafor 

void FOX_DestroySemaphore(FOX_sem *sem)
{
	if ( sem ) 
	{
		if ( sem->id ) 
		{
			CloseHandle(sem->id);
			sem->id = 0;
		}
		free(sem);
	}
}

// zaczekaj na semafor 

int FOX_SemWaitTimeout(FOX_sem *sem, Uint32 timeout)
{
	int retval;
	DWORD dwMilliseconds;

	if ( ! sem ) 
	{
		FOX_SetError("[FOX]: przejscie przez NULL semafor w FOX_syssem.c");
		return -1;
	}

	if ( timeout == FOX_MUTEX_MAXWAIT ) 
	{
		dwMilliseconds = INFINITE;
	} 
	else 
	{
		dwMilliseconds = (DWORD)timeout;
	}

	switch (WaitForSingleObject(sem->id, dwMilliseconds)) 
	{

	    case WAIT_OBJECT_0:
		--sem->count;
		retval = 0;
		break;
	
		case WAIT_TIMEOUT:
		retval = FOX_MUTEX_TIMEDOUT;
		break;
	    
		default:
		FOX_SetError("[FOX]: WaitForSingleObject() failed");
		retval = -1;
		break;
	}
	return retval;
}

// czekaj na semafor 

int FOX_SemTryWait(FOX_sem *sem)
{
	return FOX_SemWaitTimeout(sem, 0);
}

// tu rowniez

int FOX_SemWait(FOX_sem *sem)
{
	return FOX_SemWaitTimeout(sem, FOX_MUTEX_MAXWAIT);
}

// zwroc aktualny licznik dla semaforu

Uint32 FOX_SemValue(FOX_sem *sem)
{
	if ( ! sem ) 
	{
		FOX_SetError("[FOX]: przejscie przez NULL semafor");
		return 0;
	}
	return sem->count;
}

// podrzuc semafor 

int FOX_SemPost(FOX_sem *sem)
{
	if ( ! sem ) 
	{
		FOX_SetError("[FOX]: przejscie przez NULL semafor");
		return -1;
	}

	++sem->count;

	if ( ReleaseSemaphore(sem->id, 1, NULL) == FALSE ) 
	{
		--sem->count;	// powrot 
		FOX_SetError("[FOX]: nie mozna zwolnic pamieci po semaforze");
		return -1;
	}
	return 0;
}

// end
