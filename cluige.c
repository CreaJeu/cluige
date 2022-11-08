
#include<stdio.h>
#include "cluige.h"

////////////////////////////////// iiCluige /////////

static void* clg_checkedMalloc(size_t s)
{
    void* res = malloc(s);
    if(res == NULL)
    {
        wprintf(L"\n\n\n    fatal memory alloc error !\n\n\n");
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
    //...
    //...

    iiStringBuilderInit();
    iiNodeInit();
    iCluige.privateRoot2D = iCluige.iNode.newNode();
    iCluige.privateRoot2D->name = L"privateRoot2D";
    iCluige.publicRoot2D = iCluige.iNode.newNode();
    iCluige.publicRoot2D->name = L"publicRoot2D";
    iCluige.iNode.addChild(iCluige.privateRoot2D, iCluige.publicRoot2D);
    //...
    //...

    //...
    //...
}

void cluigeRun()
{
    //game loop ...
}

int cluigeFinish()
{
    iCluige.iNode.deleteNode(iCluige.privateRoot2D);
    //close files
    //free tmp locks
    //...
    return EXIT_SUCCESS;
}

