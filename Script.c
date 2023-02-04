

#include "cluige.h"
#include "Script.h"


////////////////////////////////// _Script /////////

static void nde_deleteScript(Script* thisScript)
{
    free(thisScript);
}


////////////////////////////////// iiScript /////////

static Script* nde_newScript(Node* node)
{
    Script* thisScript = iCluige.checkedMalloc(sizeof(Script));
    thisScript->node = node;

//    StringBuilder sb;
//    thisScript->_className = iCluige.iStringBuilder.stringAlloc(&sb, 6);
//    iCluige.iStringBuilder.append(&sb, "Script");
    thisScript->_subClass = NULL;

    thisScript->deleteScript = nde_deleteScript;
    thisScript->ready = NULL;
    thisScript->process = NULL;
    return thisScript;
}

/////////////////////////////////// iScript //////////

void iiScriptInit()
{
    iCluige.iScript.newScript = nde_newScript;
}

