// Fox v0.5
// by Jaroslaw Rozynski
//===
// *TIMERS*
//===
// TODO:


// standardowe ustawienia

#include "FOX_timer.h"

#define ROUND_RESOLUTION(X)	\
	(((X+TIMER_RESOLUTION-1)/TIMER_RESOLUTION)*TIMER_RESOLUTION)

// kiedy start i ile dziala 

extern int FOX_timer_started;
extern int FOX_timer_running;

// dla jednego timera

extern Uint32 FOX_alarm_interval;
extern FOX_TimerCallback FOX_alarm_callback;

// czy timer jest w watku 

extern int FOX_SetTimerThreaded(int value);

// init i quit 

extern int FOX_TimerInit(void);
extern void FOX_TimerQuit(void);

// zdarzenie w watku 

extern void FOX_ThreadedTimerCheck(void);

// end
