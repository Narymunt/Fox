// Fox v0.5
// by Jaroslaw Rozynski
//===
// *THREAD*
//===
// TODO:


// obsluga watkow

#include "FOX_thread_c.h"

// tworzenie watku

extern int FOX_SYS_CreateThread(FOX_Thread *thread, void *args);

// ustawienia

extern void FOX_SYS_SetupThread(void);

// czekaj na watek 

extern void FOX_SYS_WaitThread(FOX_Thread *thread);

// zabij watek (niebezpieczne!)

extern void FOX_SYS_KillThread(FOX_Thread *thread);

// end
