

#include "cluige.h"
#include "Script.h"


////////////////////////////////// _Script /////////

static void scpt_delete_Script(Script* this_Script)
{
    free(this_Script);
}


////////////////////////////////// iiScript /////////

static Script* scpt_new_Script(Node* node)
{
    Script* this_Script = iCluige.checked_malloc(sizeof(Script));
    this_Script->node = node;

//    StringBuilder sb;
//    this_Script->_class_name = iCluige.iStringBuilder.string_alloc(&sb, 6);
//    iCluige.iStringBuilder.append(&sb, "Script");
    this_Script->_sub_class = NULL;

    this_Script->delete_Script = scpt_delete_Script;
    this_Script->ready = NULL;
    this_Script->process = NULL;
    return this_Script;
}

/////////////////////////////////// iScript //////////

void iiScript_init()
{
    iCluige.iScript.new_Script = scpt_new_Script;
}

