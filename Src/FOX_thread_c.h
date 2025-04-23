// Fox v0.5
// by Jaroslaw Rozynski
//===
// *THREAD*
//===
// TODO:


// standardowe ustawienia

#ifndef _FOX_thread_c_h
#define _FOX_thread_c_h

#include "FOX_error_c.h"
#include "FOX_systhread_c.h"

// struktura 

struct FOX_Thread 
{

	Uint32 threadid;
	
	SYS_ThreadHandle handle;
	
	int status;
	
	FOX_error errbuf;
	
	void *data;

};

// funkcja uruchamia watek 

extern void FOX_RunThread(void *data);

// obsluga bledow 

extern FOX_error *FOX_GetErrBuf(void);

#endif 

// end

