#ifndef CLUIGE_H_INCLUDED
#define CLUIGE_H_INCLUDED


#include <stdbool.h> //for bool, true, false (standard since C99)
#include <stddef.h> //for NULL
#include <stdlib.h> //for EXIT_SUCCESS, malloc, etc.

#include "Node.h"
#include "StringBuilder.h"

//just a kind of namespace to prevent name collisions
struct iiCluige
{
	struct iiStringBuilder iStringBuilder;
	struct iiNode iNode;

	//for cluige nodes like clock
	Node* privateRoot2D;

	//for user nodes
	Node* publicRoot2D;

	//void* checkedMalloc(size_t)
	void* (*checkedMalloc)(size_t);
};
extern struct iiCluige iCluige;

void cluigeInit();

void cluigeRun();

int cluigeFinish();

#endif // CLUIGE_H_INCLUDED
