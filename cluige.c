
#include <stdio.h>
#include <curses.h>
#include "cluige.h"
#include "Nodes/Clock.h"

////////////////////////////////// iiCluige /////////

static void* clg_checkedMalloc(size_t s)
{
    void* res = malloc(s);
    if(res == NULL)
    {
        printf("\n\n\n    fatal memory alloc error !\n\n\n");
        exit(EXIT_FAILURE);
    }
    return res;
}


/////////////////////////////////// Cluige //////////

//redeclaration because of extern (like static members in c++)
struct iiCluige iCluige;

void cluigeInit()
{
    iCluige.checkedMalloc = clg_checkedMalloc;
    iCluige.wantedFrameSeconds = .0666;//15 fps by default
    iCluige.quitAsked = false;

    iiStringBuilderInit();
    iiNodeInit();
    iCluige.privateRoot2D = iCluige.iNode.newNode();
    iCluige.iNode.setName(iCluige.privateRoot2D, "privateRoot2D");

    iiScriptInit();

    iiClockInit();
    iCluige.clock = iCluige.iClock.newClock();
    iCluige.iNode.addChild(iCluige.privateRoot2D, iCluige.clock->_thisNode);

    iCluige.publicRoot2D = iCluige.iNode.newNode();
    iCluige.iNode.setName(iCluige.publicRoot2D, "publicRoot2D");
    iCluige.iNode.addChild(iCluige.privateRoot2D, iCluige.publicRoot2D);

    //...
    //...

    //...
    //...

    //curses
    initscr();
    //nl();//?
    noecho();
    curs_set(0);
    timeout(-1);// for getch()
    keypad(stdscr, TRUE);
}

enum ProcessPass
{
    PRE_PROCESS_PASS,
    PROCESS_PASS,
    POST_PROCESS_PASS
};

static void processTree(Node* root, enum ProcessPass pass)
{
    //recursion mode : DFS
    if(root->children != NULL)
    {
        processTree(root->children, pass);
    }

    switch(pass)
    {
    case PRE_PROCESS_PASS:
        if(root->preProcessNode != NULL)
        {
            root->preProcessNode(root);
        }
        break;
    case PROCESS_PASS:
        if(root->processNode != NULL)
        {
            root->processNode(root);
        }
        break;
    case POST_PROCESS_PASS:
        if(root->postProcessNode != NULL)
        {
            root->postProcessNode(root);
        }
        break;
    }

    if(root->nextSibling != NULL)
    {
        processTree(root->nextSibling, pass);
    }
}

void cluigeRun()
{
    //game loop
    while(!(iCluige.quitAsked))
    {
        processTree(iCluige.privateRoot2D, PRE_PROCESS_PASS);
        processTree(iCluige.privateRoot2D, PROCESS_PASS);
        processTree(iCluige.privateRoot2D, POST_PROCESS_PASS);

        //curses
        int sleepFrameMilliseconds = 1;
        if(iCluige.clock->elapsedSeconds < iCluige.wantedFrameSeconds)
        {
            float sleepFrameSeconds = iCluige.wantedFrameSeconds - iCluige.clock->elapsedSeconds;
            sleepFrameMilliseconds = (int)(sleepFrameSeconds * 1000);
        }
        refresh();
        napms(sleepFrameMilliseconds);//sleep to avoid 100% CPU
    }
}

int cluigeFinish()
{
    //iCluige.iNode.deleteNode(iCluige.privateRoot2D);
    iCluige.privateRoot2D->deleteNode(iCluige.privateRoot2D);
    //close files
    //free tmp locks
    //...


	curs_set(1);
	endwin();
    return EXIT_SUCCESS;
}

