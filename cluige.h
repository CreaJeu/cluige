#ifndef CLUIGE_H_INCLUDED
#define CLUIGE_H_INCLUDED


#include <stdbool.h> //for bool, true, false (standard since C99)
#include <stddef.h> //for NULL
#include <stdlib.h> //for EXIT_SUCCESS, malloc, etc.
#include <string.h>

#include "Utils.h"
#include "Node.h"
#include "StringBuilder.h"

#include "Variant.h"
#include "Deque.h"
#include "SortedDictionary.h"
#include "Nodes/Clock.h"
#include "Script.h"
#include "Vector2.h"
#include "Nodes/Node2D.h"
#include "Nodes/SpriteText.h"
#include "Nodes/Input.h"
#include "LineDrawerThin.h"
#include "Path2D.h"
#include "Nodes/SpriteSVG.h"
#include "Nodes/Camera2D.h"

//just a kind of namespace to prevent name collisions
struct iiCluige
{
    struct iiVariant iVariant;
    struct iiDeque iDeque;
    struct iiSortedDictionary iSortedDictionary;
	struct iiStringBuilder iStringBuilder;
	struct iiNode iNode;
	struct iiClock iClock;
	struct _Clock* clock;
	struct iiInput iInput;
	struct _Input* input;
	struct iiScript iScript;
	struct iiVector2 iVector2;
	struct iiNode2D iNode2D;
	struct iiSpriteText iSpriteText;
    struct iiLineDrawerThin iLineDrawerThin;
	struct iiPath2D iPath2D;
	struct iiSpriteSVG iSpriteSVG;
	struct iiCamera2D iCamera2D;



	//for target FPS, 15 fps by default
	float wanted_frame_seconds;//TODO rename wanted_seconds_per_frame

	//to quit a game
	bool quit_asked;

	//for cluige nodes like clock
	Node* private_root_2D;

	//for user nodes
	Node* public_root_2D;

	// if epsilon = 5
	// 0.000001 will be considered zero
	// 0.0001 will not be considered zero
	float EPSILON;

	//void* checked_malloc(size_t)
	void* (*checked_malloc)(size_t);

//	Vector2 get_screen_size();
	Vector2 (*get_screen_size)();
};
extern struct iiCluige iCluige;

void cluige_init();

void cluige_run();

int cluige_finish();

#endif // CLUIGE_H_INCLUDED
