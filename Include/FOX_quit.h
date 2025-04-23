// Fox v0.5 
// by Jaroslaw Rozynski
//===
// TODO:

// przechwytywanie zdarzenia quit
// jest wywolywane tylko gdy system, albo luser wywola quit

#ifndef _FOX_quit_h
#define _FOX_quit_h

#define FOX_QuitRequested() \
        (FOX_PumpEvents(), FOX_PeepEvents(NULL,0,FOX_PEEKEVENT,FOX_QUITMASK))

#endif 

// end
