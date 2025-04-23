// Fox v0.5
// by Jaroslaw Rozynski
//===
// *THREAD*
//===
// TODO:
 
// zarzadzanie watkami

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "FOX_error.h"
#include "FOX_thread.h"
#include "FOX_systhread.h"

// uruchom 

static DWORD WINAPI RunThread(LPVOID data)
{
	FOX_RunThread(data);
	return(0);
}

// stworz 

int FOX_SYS_CreateThread(FOX_Thread *thread, void *args)
{
	DWORD  threadnum;

	thread->handle = CreateThread(NULL, 0, RunThread, args, 0, &threadnum);
	
	if (thread->handle == NULL) 
	{
		FOX_SetError("[FOX]: za malo zasobow systemowych aby stworzyc watek");
		return(-1);
	}
	return(0);
}

// ustaw watek 

void FOX_SYS_SetupThread(void)
{
	return;
}

// identyfikator watku

Uint32 FOX_ThreadID(void)
{
	return((Uint32)GetCurrentThreadId());
}

// czekaj na watek 

void FOX_SYS_WaitThread(FOX_Thread *thread)
{
	WaitForSingleObject(thread->handle, INFINITE);
	CloseHandle(thread->handle);
}

// bardzo niebezpieczne !!!


void FOX_SYS_KillThread(FOX_Thread *thread)
{
	TerminateThread(thread->handle, FALSE);
}

// end

