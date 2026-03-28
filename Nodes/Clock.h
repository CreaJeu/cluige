#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include "clock_non_portable.h"
typedef struct _Node Node;//forward declaration

struct _Clock
{
	float scale;
	float elapsed_seconds;

	//private
	struct CluigeTick _last_tick;

	Node* _this_Node;

	//virtual methods - private copies of mother class pointers
	void (*_delete_super)(Node*);

	//virtual methods in .c :
	//void (*process)(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiClock
{
	//Clock* new_Clock()
	struct _Clock* (*new_Clock)();

	//private
	//in clock_non_portable.c/h
	void (*_init_tick)(struct _Clock* this_Clock);
	//in clock_non_portable.c/h
	float (*_precise_unscaled_elapsed_seconds)(struct _Clock*, bool memorize);
};
//iClock : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiClock_init();


#endif // CLOCK_H_INCLUDED
