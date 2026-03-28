
#ifdef CLUIGE_X11

#include "clock_non_portable.h"
#include "../cluige.h"

void clk__init_tick(struct _Clock* this_Clock)
{
	//	the POSIX function for wall clock
	clock_gettime(CLOCK_REALTIME, &(this_Clock->_last_tick.t));
}

float clk__precise_unscaled_elapsed_seconds(struct _Clock* this_Clock, bool memorize)
{
	float res;

	struct timespec now;
	//the POSIX function for wall clock
	clock_gettime(CLOCK_REALTIME, &now);
	__syscall_slong_t delta_nsec = now.tv_nsec - this_Clock->_last_tick.t.tv_nsec;
	__time_t delta_sec = now.tv_sec - this_Clock->_last_tick.t.tv_sec;
	if (delta_sec > 0 && delta_nsec < 0)
	{
		delta_nsec += 1000000000;
		delta_sec--;
	}
	else if (delta_sec < 0 && delta_nsec > 0)
	{
		utils_breakpoint_trick("why delta_sec < 0 ?", true);
		delta_nsec -= 1000000000;
		delta_sec++;
	}
	res = delta_sec + (delta_nsec * .000000001);
	if(memorize)
	{
		this_Clock->_last_tick.t = now;
	}
	return res;
}



#else
// windows version

#include "../cluige.h"
#include "clock_non_portable.h"

void clk__init_tick(struct _Clock* this_Clock)
{
	this_Clock->_last_tick.t = clock();
}

float clk__precise_unscaled_elapsed_seconds(struct _Clock* this_Clock, bool memorize)
{
	float res;

	clock_t new_tick = clock();
	res =  ((float)(new_tick - this_Clock->_last_tick.t)) / CLOCKS_PER_SEC;
	//handle clock() wrap around or errors
	if(res < 0)
	{
		//arbitrary value to provoque a short sleep in main loop
		utils_breakpoint_trick("clock() wrap around", true);
		res = iCluige.wanted_frame_seconds * .9;
	}
	if(memorize)
	{
		this_Clock->_last_tick.t = new_tick;
	}

	return res;
}

#endif // CLUIGE_X11
