#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include <time.h>

struct _Clock
{
	float scale;
	float elapsed_seconds;

	//private
	clock_t _last_tick;
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
};
//iClock : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiClock_init();


#endif // CLOCK_H_INCLUDED
