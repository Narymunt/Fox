// Fox v0.5
// by Jaroslaw Rozynski
//===
// *DIRECTX*
//===
// TODO:

#ifndef _directx_h_
#define _directx_h_

// naglowki dla directx

#include <windows.h>
#include <mmsystem.h>

#ifndef WIN32
	#define WIN32
#endif
#undef  WINNT

// dalekie wskazniki nie istnieja w 32

#ifndef FAR
	#define FAR
#endif

// kody bledow nie dolaczamy

#ifndef MAKE_HRESULT
#define MAKE_HRESULT(sev,fac,code) \
	((HRESULT)(((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))))
#endif

// operacja poprawna

#ifndef S_OK
	#define S_OK			(HRESULT)0x00000000L
#endif

// poprawnie

#ifndef SUCCEEDED
	#define SUCCEEDED(x)	((HRESULT)(x) >= 0)
#endif

// z bledem

#ifndef FAILED
	#define FAILED(x)	((HRESULT)(x)<0)
#endif

#ifndef E_FAIL
	#define E_FAIL		(HRESULT)0x80000008L
#endif

#ifndef E_NOINTERFACE
	#define E_NOINTERFACE	(HRESULT)0x80004002L
#endif

#ifndef E_OUTOFMEMORY
	#define E_OUTOFMEMORY	(HRESULT)0x8007000EL
#endif

#ifndef E_INVALIDARG
	#define E_INVALIDARG	(HRESULT)0x80070057L
#endif

#ifndef E_NOTIMPL
	#define E_NOTIMPL	(HRESULT)0x80004001L
#endif

#ifndef REGDB_E_CLASSNOTREG
	#define REGDB_E_CLASSNOTREG	(HRESULT)0x80040154L
#endif

// kody

#ifndef SEVERITY_ERROR
	#define SEVERITY_ERROR	1
#endif

// kody bledow 

#ifndef FACILITY_WIN32
	#define FACILITY_WIN32	7
#endif

// offsety dla pol

#ifndef FIELD_OFFSET
	#define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))
#endif

// naglowki dla directx - fix ? 

#define DIRECTDRAW_VERSION  0x0700
#define DIRECTSOUND_VERSION 0x0500
#define DIRECTINPUT_VERSION 0x0500

// tutaj naglowki dla directa

#include <ddraw.h>
#include <dsound.h>
#include <dinput.h>

#endif 

// end

