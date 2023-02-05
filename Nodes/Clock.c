//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "Clock.h"

////////////////////////////////// _Clock /////////

static void clk_deleteClock(Node* thisNode)
{
    struct _Clock* thisClock = (struct _Clock*)(thisNode->_subClass);
    free(thisClock);
    thisNode->_subClass = NULL;
}

static void clk_preProcessClock(Node* thisNode)
{
    struct _Clock* thisClock = (struct _Clock*)(thisNode->_subClass);
    clock_t newTick = clock();
    float dSec = (float)(newTick - thisClock->_lastTick) / CLOCKS_PER_SEC;
    thisClock->elapsedSeconds = thisClock->scale * dSec;
    thisClock->_lastTick = newTick;
}


////////////////////////////////// iiClock /////////

static struct _Clock* clk_newClock()
{
    Node* newNode = iCluige.iNode.newNode();
    struct _Clock* newClock = iCluige.checkedMalloc(sizeof(struct _Clock));

    newClock->scale = 1.;
    newClock->elapsedSeconds = 0.;
    newClock->_lastTick = clock();
    newClock->_thisNode = newNode;
    newClock->deleteNode = newNode->deleteNode;
    //newClock->deleteClock = clk_deleteClock;
    //newClock->preProcessClock = clk_preProcessClock;

    newNode->_subClass = newClock;

    free(newNode->_className); //TODO static value to avoid free
    StringBuilder sb;
    newNode->_className = iCluige.iStringBuilder.stringAlloc(&sb, strlen("NodeClock"));
    iCluige.iStringBuilder.append(&sb, "NodeClock");

    newNode->deleteNode = clk_deleteClock;
    newNode->preProcessNode = clk_preProcessClock;

    return newClock;
}

/////////////////////////////////// Node //////////

void iiClockInit()
{
    iCluige.iClock.newClock = clk_newClock;
}

