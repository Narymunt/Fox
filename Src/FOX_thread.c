// Fox v0.5
// by Jaroslaw Rozynski
//===
// *THREAD*
//===
// TODO:


// obsluga watkow 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FOX_error.h"
#include "FOX_mutex.h"
#include "FOX_thread.h"
#include "FOX_thread_c.h"
#include "FOX_systhread.h"

// tablica aktualnych watkow

#define ARRAY_CHUNKSIZE	32

static int FOX_maxthreads = 0;
static int FOX_numthreads = 0;

static FOX_Thread **FOX_Threads = NULL;
static FOX_mutex *thread_lock = NULL;
int _creating_thread_lock = 0;

// inicjalizacja 

int FOX_ThreadsInit(void)
{
	int retval;

	retval = 0;

	_creating_thread_lock = 1;

	thread_lock = FOX_CreateMutex();
	
	_creating_thread_lock = 0;
	
	if ( thread_lock == NULL ) 
	{
		retval = -1;
	}
	return(retval);
}

// wyjscie 

void FOX_ThreadsQuit()
{
	FOX_mutex *mutex;

	mutex = thread_lock;
	thread_lock = NULL;

	if ( mutex != NULL ) 
	{
		FOX_DestroyMutex(mutex);
	}
}

// zmiany w liscie watkow

static void FOX_AddThread(FOX_Thread *thread)
{
	FOX_Thread **threads;

	if ( thread_lock == NULL ) 
	{
		if ( FOX_ThreadsInit() < 0 ) 
		{
			return;
		}
	}
	FOX_mutexP(thread_lock);

	// rozszerz liste jezeli potrzebne 
	
#ifdef DEBUG_THREADS
	printf("[FOX]: dodaje watek (aktualnie %d, maksymalnie %d)\n",
			FOX_numthreads, FOX_maxthreads);
#endif
	if ( FOX_numthreads == FOX_maxthreads ) 
	{
		threads=(FOX_Thread **)malloc((FOX_maxthreads+ARRAY_CHUNKSIZE)*
		                              (sizeof *threads));
		if ( threads == NULL ) 
		{
			FOX_OutOfMemory();
			goto done;
		}
		
		memcpy(threads, FOX_Threads, FOX_numthreads*(sizeof *threads));
		FOX_maxthreads += ARRAY_CHUNKSIZE;
		
		if ( FOX_Threads ) 
		{
			free(FOX_Threads);
		}
		FOX_Threads = threads;
	}
	FOX_Threads[FOX_numthreads++] = thread;
done:
	FOX_mutexV(thread_lock);
}

// usun watek 

static void FOX_DelThread(FOX_Thread *thread)
{
	int i;

	if ( thread_lock ) 
	{
		FOX_mutexP(thread_lock);
	
		for ( i=0; i<FOX_numthreads; ++i ) 
		{
			if ( thread == FOX_Threads[i] ) 
			{
				break;
			}
		}
		
		if ( i < FOX_numthreads ) 
		{
			--FOX_numthreads;
			while ( i < FOX_numthreads ) 
			{
				FOX_Threads[i] = FOX_Threads[i+1];
				++i;
			}
#ifdef DEBUG_THREADS
			printf("[FOX]: usuwam watek (aktualnie %d, maksymalnie %d)\n",
					FOX_numthreads, FOX_maxthreads);
#endif
		}
		FOX_mutexV(thread_lock);
	}
}

// blad ? 

static FOX_error FOX_global_error;

// jaki numer ? 

FOX_error *FOX_GetErrBuf(void)
{
	FOX_error *errbuf;

	errbuf = &FOX_global_error;
	
	if ( FOX_Threads ) 
	{
		int i;
		Uint32 this_thread;

		this_thread = FOX_ThreadID();
		FOX_mutexP(thread_lock);
	
		for ( i=0; i<FOX_numthreads; ++i ) 
		{
			if ( this_thread == FOX_Threads[i]->threadid ) 
			{
				errbuf = &FOX_Threads[i]->errbuf;
				break;
			}
		}
		FOX_mutexV(thread_lock);
	}
	return(errbuf);
}

// callback

typedef struct 
{

	int (*func)(void *);
	
	void *data;
	
	FOX_Thread *info;
	FOX_sem *wait;

} thread_args;

// uruchom 

void FOX_RunThread(void *data)
{
	thread_args *args;
	
	int (*userfunc)(void *);
	
	void *userdata;
	
	int *statusloc;

	FOX_SYS_SetupThread();
	
	// id watku 
	
	args = (thread_args *)data;
	args->info->threadid = FOX_ThreadID();

	// ktora funkcja ?

	userfunc = args->func;
	userdata = args->data;
	statusloc = &args->info->status;

	// obudz parent thread 
	
	FOX_SemPost(args->wait);
	
	// uruchom funkcje
	
	*statusloc = userfunc(userdata);
}

// stworz watek 

FOX_Thread *FOX_CreateThread(int (*fn)(void *), void *data)
{
	FOX_Thread *thread;
	thread_args *args;
	int ret;

	// pamiec na strukture 
	
	thread = (FOX_Thread *)malloc(sizeof(*thread));

	if ( thread == NULL ) 
	{
		FOX_OutOfMemory();
		return(NULL);
	}
	
	memset(thread, 0, (sizeof *thread));
	thread->status = -1;
	
	args = (thread_args *)malloc(sizeof(*args));

	if ( args == NULL ) 
	{
		FOX_OutOfMemory();
		free(thread);
		return(NULL);
	}
	
	args->func = fn;
	args->data = data;
	args->info = thread;
	args->wait = FOX_CreateSemaphore(0);
	
	if ( args->wait == NULL ) 
	{
		free(thread);
		free(args);
		return(NULL);
	}

	// dodaj 
	
	FOX_AddThread(thread);

	ret = FOX_SYS_CreateThread(thread, args);
	
	if ( ret >= 0 ) 
	{
		FOX_SemWait(args->wait);
	} 
	else 
	{
		// nie udalo sie, zwolnij wszystko 
		FOX_DelThread(thread);
		free(thread);
		thread = NULL;
	}

	FOX_DestroySemaphore(args->wait);
	free(args);

	// teraz juz dziala 

	return(thread);
}

// czekaj na watek 

void FOX_WaitThread(FOX_Thread *thread, int *status)
{
	if ( thread ) 
	{
		FOX_SYS_WaitThread(thread);
		if ( status ) 
		{
			*status = thread->status;
		}
		FOX_DelThread(thread);
		free(thread);
	}
}

// pobierz id 

Uint32 FOX_GetThreadID(FOX_Thread *thread)
{
	Uint32 id;

	if ( thread ) 
	{
		id = thread->threadid;
	} 
	else 
	{
		id = FOX_ThreadID();
	}
	return(id);
}

// zabij watek 

void FOX_KillThread(FOX_Thread *thread)
{
	if ( thread ) 
	{
		FOX_SYS_KillThread(thread);
		FOX_WaitThread(thread, NULL);
	}
}

// end
