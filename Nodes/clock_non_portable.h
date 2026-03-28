#ifndef CLOCK_NON_PORTABLE_H_INCLUDED
#define CLOCK_NON_PORTABLE_H_INCLUDED

//for non-portable code, resolved in .c with #ifdef...

#include <stdbool.h>

struct _Clock;//forward declaration


#ifdef CLUIGE_X11

//minimum _POSIX_C_SOURCE for clock_gettime()
#define _POSIX_C_SOURCE 199309L

#include <time.h>

struct CluigeTick
{
	struct timespec t;
};


#else
//windows version

#include <time.h>

struct CluigeTick
{
	clock_t t;
};

#endif // CLUIGE_X11


void clk__init_tick(struct _Clock* this_Clock);
float clk__precise_unscaled_elapsed_seconds(struct _Clock* this_Clock, bool memorize);

#endif // _H_INCLUDED
