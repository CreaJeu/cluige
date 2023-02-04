//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "Clock.h"

//TODO TMP
#include <curses.h>

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

    //bidon
    static float bidon = 0;
    static StringBuilder sb;
    static char* display = NULL;

    if(display == NULL) display = iCluige.iStringBuilder.stringAlloc(&sb, 4);

    bidon += thisClock->elapsedSeconds;
    iCluige.iStringBuilder.replace(&sb, "%2.1f", bidon);
    //printf("yo %s\n", display);
    mvaddstr(4, 7, display);
    if(bidon > 4)
    {
        iCluige.quitAsked = true;
    }
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
    newClock->deleteClock = clk_deleteClock;
    newClock->preProcessClock = clk_preProcessClock;

    newNode->_subClass = newClock;

    free(newNode->_className); //TODO static value to avoid free
    StringBuilder sb;
    newNode->_className = iCluige.iStringBuilder.stringAlloc(&sb, 5);
    iCluige.iStringBuilder.append(&sb, "Clock");

    newNode->deleteNode = newClock->deleteClock;
    newNode->preProcessNode = newClock->preProcessClock;

    return newClock;
}

/////////////////////////////////// Node //////////

void iiClockInit()
{
    iCluige.iClock.newClock = clk_newClock;
}

