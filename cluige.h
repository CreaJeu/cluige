#ifndef CLUIGE_H_INCLUDED
#define CLUIGE_H_INCLUDED


#include <stdbool.h> //for bool, true, false (standard since C99)
#include <stddef.h> //for NULL
#include <stdlib.h> //for EXIT_SUCCESS, malloc, etc.

#include "Node.h"
#include "StringBuilder.h"

#include "Nodes/Clock.h"
#include "Script.h"
#include "Vector2.h"
#include "Nodes/Node2D.h"
#include "Nodes/SpriteText.h"

//just a kind of namespace to prevent name collisions
struct iiCluige
{
	struct iiStringBuilder iStringBuilder;
	struct iiNode iNode;
	struct iiClock iClock;
	struct _Clock* clock;
	struct iiScript iScript;
	struct iiVector2 iVector2;
	struct iiNode2D iNode2D;
	struct iiSpriteText iSpriteText;

	//for target FPS, 15 fps by default
	float wantedFrameSeconds;

	//to quit a game
	bool quitAsked;

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
