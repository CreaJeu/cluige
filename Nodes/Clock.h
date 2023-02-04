#ifndef CLOCK_H_INCLUDED
#define CLOCK_H_INCLUDED

#include <time.h>

struct _Clock
{
	float scale;
	float elapsedSeconds;

	//private
	clock_t _lastTick;
	Node* _thisNode;

	//virtual methods
	void (*deleteNode)(Node*);
	void (*deleteClock)(Node*);

	void (*preProcessClock)(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiClock
{
	//Clock* newClock()
	struct _Clock* (*newClock)();
};
//iNode : in iiCluige

//to be called only by cluigeInit() to set iNode functions pointers
void iiClockInit();


#endif // CLOCK_H_INCLUDED
