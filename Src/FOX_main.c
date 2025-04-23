// Fox v0.5
// by Jaroslaw Rozynski
//===
// *MAIN*
//===
// TODO:
// - bledy przez message box
// - bez przekierowania pliku
// 

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <windows.h>
#include <malloc.h>		// _alloca() 

// definicja 

#include "FOX.h"
#include "FOX_main.h"

// bledy do pliku 

#define STDOUT_FILE	TEXT("stdout.txt")
#define STDERR_FILE	TEXT("stderr.txt")


// command line do bufora 

static int ParseCommandLine(char *cmdline, char **argv)
{
	char *bufp;
	int argc;

	argc = 0;
	for ( bufp = cmdline; *bufp; ) 
	{
		
		while ( isspace(*bufp) ) 
		{
			++bufp;
		}
		
		if ( *bufp == '"' ) 
		{
			++bufp;
			if ( *bufp ) 
			{
				if ( argv ) 
				{
					argv[argc] = bufp;
				}
				++argc;
			}
			
			while ( *bufp && (*bufp != '"') ) 
			{
				++bufp;
			}
		} 
		else 
		{
			if ( *bufp ) 
			{
				if ( argv ) 
				{
					argv[argc] = bufp;
				}
				++argc;
			}
			
			while ( *bufp && ! isspace(*bufp) ) 
			{
				++bufp;
			}
		}
		if ( *bufp ) 
		{
			if ( argv ) 
			{
				*bufp = '\0';
			}
			++bufp;
		}
	}
	if ( argv ) 
	{
		argv[argc] = NULL;
	}
	return(argc);
}

// pokaz blad 

static void ShowError(const char *title, const char *message)
{

// trzeba dodac user32.lib	
	
//#ifdef USE_MESSAGEBOX
	MessageBox(NULL, message, title, MB_ICONEXCLAMATION|MB_OK);
//#else
//	fprintf(stderr, "[FOX]: %s: %s\n", title, message);
//#endif
}

// bledow 

static BOOL OutOfMemory(void)
{
	ShowError("[FOX]: blad", " za malo pamieci");
	return FALSE;
}

// usun zapisane dane 

static void __cdecl cleanup_output(void)
{
#ifndef NO_STDIO_REDIRECT
	FILE *file;
	int empty;
#endif
	
	fclose(stdout);
	fclose(stderr);

#ifndef NO_STDIO_REDIRECT
	
	file = fopen(STDOUT_FILE, "rb");
	
	if ( file ) 
	{
		empty = (fgetc(file) == EOF) ? 1 : 0;
		fclose(file);
	
		if ( empty ) 
		{
			remove(STDOUT_FILE);
		}
	}
	file = fopen(STDERR_FILE, "rb");
	if ( file ) 
	{
		empty = (fgetc(file) == EOF) ? 1 : 0;
		fclose(file);
	
		if ( empty ) 
		{
			remove(STDERR_FILE);
		}
	}
#endif
}

#if defined(_MSC_VER) && !defined(_WIN32_WCE)
/* The VC++ compiler needs main defined */
	#define console_main main
#endif

// tutaj syf sie zaczyna 

int console_main(int argc, char *argv[])
{
	int n;
	char *bufp, *appname;

	appname = argv[0];
	if ( (bufp=strrchr(argv[0], '\\')) != NULL ) 
	{
		appname = bufp+1;
	}
	else
	if ( (bufp=strrchr(argv[0], '/')) != NULL ) 
	{
		appname = bufp+1;
	}

	if ( (bufp=strrchr(appname, '.')) == NULL )
		n = strlen(appname);
	else
		n = (bufp-appname);

	bufp = (char *)alloca(n+1);
	
	if ( bufp == NULL ) 
	{
		return OutOfMemory();
	}
	strncpy(bufp, appname, n);
	bufp[n] = '\0';
	appname = bufp;

	// zaladuj dll
	
	if ( FOX_Init(FOX_INIT_NOPARACHUTE) < 0 ) 
	{
		ShowError("[FOX]: WinMain() error", FOX_GetError());
		return(FALSE);
	}
	atexit(cleanup_output);
	atexit(FOX_Quit);

#ifndef DISABLE_VIDEO
#if 0

#else
	FOX_SetModuleHandle(GetModuleHandle(NULL));
#endif //0 
#endif 

	// uruchom main() 
	
	FOX_main(argc, argv);

	// wyjscie 

	exit(0);
}

// tutaj sie zaczyna 

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
	HINSTANCE handle;
	char **argv;
	int argc;
	char *cmdline;
	char *bufp;

#ifndef NO_STDIO_REDIRECT
	FILE *newfp;
#endif

	handle = LoadLibrary(TEXT("DDRAW.DLL"));

	if ( handle != NULL ) 
	{
		FreeLibrary(handle);
	}

#ifndef NO_STDIO_REDIRECT
	/* Redirect standard input and standard output */
	newfp = freopen(STDOUT_FILE, "w", stdout);
	if ( newfp == NULL ) 
	{	
#if !defined(stdout)
		stdout = fopen(STDOUT_FILE, "w");
#else
		newfp = fopen(STDOUT_FILE, "w");
		
		if ( newfp ) 
		{
			*stdout = *newfp;
		}
#endif
	}
	newfp = freopen(STDERR_FILE, "w", stderr);
	if ( newfp == NULL ) 
	{	// NT
#if !defined(stderr)
		stderr = fopen(STDERR_FILE, "w");
#else
		newfp = fopen(STDERR_FILE, "w");
	
		if ( newfp ) 
		{
			*stderr = *newfp;
		}
#endif
	}
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);	// bufor
	setbuf(stderr, NULL);			// bez bufora 
#endif 

	// command line 
	
	bufp = GetCommandLine();
	cmdline = (char *)alloca(strlen(bufp)+1);
	
	if ( cmdline == NULL ) 
	{
		return OutOfMemory();
	}
	strcpy(cmdline, bufp);
	
	// przenies gowno 
	
	argc = ParseCommandLine(cmdline, NULL);
	argv = (char **)alloca((argc+1)*(sizeof *argv));
	
	if ( argv == NULL ) 
	{
		return OutOfMemory();
	}
	ParseCommandLine(cmdline, argv);

	// powrot do glownego 
	
	return(console_main(argc, argv));
}

// end
