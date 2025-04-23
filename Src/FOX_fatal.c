// Fox v0.5
// by Jaroslaw Rozynski
//===
// TODO:


// obsluga fatal signal (przekroczenie segmentu etc)

#ifdef NO_SIGNAL_H

// zrzucanie parachute

void FOX_InstallParachute(void)
{
	return;
}



void FOX_UninstallParachute(void)
{
	return;
}

#else

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "FOX.h"
#include "FOX_fatal.h"

// zwykly printf

static void print_msg(const char *text)
{
#ifndef DISABLE_STDIO
	fprintf(stderr, "[FOX]: %s", text);
#endif
}

// zrzuc parachute

static void FOX_Parachute(int sig)
{
	signal(sig, SIG_DFL);
	print_msg("[FOX]: fatal signal: ");
	switch (sig) {
		case SIGSEGV:
			print_msg("[FOX]: segmentation fault");
			break;
#ifdef SIGBUS
#if SIGBUS != SIGSEGV
		case SIGBUS:
			print_msg("[FOX]: blad bus");
			break;
#endif
#endif 
#ifdef SIGFPE
		case SIGFPE:
			print_msg("[FOX]: wyjatek floating point");
			break;
#endif /* SIGFPE */
#ifdef SIGQUIT
		case SIGQUIT:
			print_msg("[FOX]: wyjscie na zadanie klawiatury");
			break;
#endif /* SIGQUIT */
#ifdef SIGPIPE
		case SIGPIPE:
			print_msg("[FOX]: broken pipe");
			break;
#endif /* SIGPIPE */
		default:
#ifndef DISABLE_STDIO
			fprintf(stderr, "[FOX]: # %d", sig);
#endif
			break;
	}
	print_msg(" [FOX]: dziennik zrzucony\n");
	FOX_Quit();
	exit(-sig);
}

static int FOX_fatal_signals[] = {
	SIGSEGV,
#ifdef SIGBUS
	SIGBUS,
#endif
#ifdef SIGFPE
	SIGFPE,
#endif
#ifdef SIGQUIT
	SIGQUIT,
#endif
#ifdef SIGPIPE
	SIGPIPE,
#endif
	0
};

void FOX_InstallParachute(void)
{
	int i;
	
	void (*ohandler)(int);

	// ustaw handler 
	
	for ( i=0; FOX_fatal_signals[i]; ++i ) 
	{
		ohandler = signal(FOX_fatal_signals[i], FOX_Parachute);
		if ( ohandler != SIG_DFL ) 
		{
			signal(FOX_fatal_signals[i], ohandler);
		}
	}
#ifdef SIGALRM
	// ignoruj sigalrm 
	
	{
		struct sigaction action, oaction;

		// set sig_on

		memset(&action, 0, (sizeof action));
		action.sa_handler = SIG_IGN;
		sigaction(SIGALRM, &action, &oaction);

		//	wyzeruj
		
		if ( oaction.sa_handler != SIG_DFL ) 
		{
			sigaction(SIGALRM, &oaction, NULL);
		}
	}
#endif
	return;
}

//=== wyrzuc handler 

void FOX_UninstallParachute(void)
{
	int i;

	void (*ohandler)(int);

	// usun ze wszystkich fatali
	
	for ( i=0; FOX_fatal_signals[i]; ++i ) 
	{
		ohandler = signal(FOX_fatal_signals[i], SIG_DFL);
		
		if ( ohandler != FOX_Parachute ) 
		{
			signal(FOX_fatal_signals[i], ohandler);
		}
	}
}

#endif 

// end



