

#include "cluige.h"
#include "Script.h"


////////////////////////////////// _Script /////////

static void scpt_delete_Script(Script* this_Script)
{
    free(this_Script);
}


////////////////////////////////// iiScript /////////

static Script* scpt_new_Script()
{
    Script* this_Script = iCluige.checked_malloc(sizeof(Script));
    this_Script->node = NULL;

//    StringBuilder sb;
//    this_Script->_class_name = iCluige.iStringBuilder.string_alloc(&sb, 6);
//    iCluige.iStringBuilder.append(&sb, "Script");
    this_Script->_sub_class = NULL;

    this_Script->delete_Script = scpt_delete_Script;
    this_Script->enter_tree = NULL;
    this_Script->ready = NULL;
    this_Script->exit_tree = NULL;
    this_Script->process = NULL;
    return this_Script;
}

static void scpt_attach(Script* this_Script, Node* n)
{
	this_Script->node = n;
	n->script = this_Script;
}

static void scpt_register_ScriptFactory(const char* full_path, ScriptFactory* factory)
{
    CLUIGE_ASSERT(factory,
		"Script::register_ScriptFactory() : null factory");
    SortedDictionary* fcties = &(iCluige.iScript.script_factories);
    const char* last_dot = strrchr(full_path, '.');
    CLUIGE_ASSERT(last_dot,
		"Script::register_ScriptFactory() : expected extension in script file name (usually '.gd') not found (%s)",
		full_path);
    CLUIGE_ASSERT(strstr(full_path, "res://"),
		"Script::register_ScriptFactory() : expected leading 'res://' in script file path not found (%s)",
		full_path);
    int key_len_term = last_dot - full_path;
    CLUIGE_ASSERT(key_len_term > 6,
		"Script::register_ScriptFactory() : script path '%s' too short", full_path);
    char* fcty_key = iCluige.checked_malloc((key_len_term + 1) * sizeof(char));
    strncpy(fcty_key, full_path, key_len_term);
    fcty_key[key_len_term] = '\0';
    Checked_Variant found = iCluige.iSortedDictionary.insert(fcties, fcty_key, factory);
    CLUIGE_ASSERT(!(found.valid),
		"Node::register_ScriptFactory() : trying to register a script with the already used key '%s'",
		fcty_key);
}

/////////////////////////////////// iScript //////////

void iiScript_init()
{
    iCluige.iScript.new_Script = scpt_new_Script;
    iCluige.iScript.attach = scpt_attach;

    SortedDictionary* fcties = &(iCluige.iScript.script_factories);
    iCluige.iSortedDictionary.sorted_dictionary_alloc(
		fcties, VT_POINTER, VT_POINTER, 20);
    iCluige.iSortedDictionary.set_compare_keys_func(
		fcties, iCluige.iDeque.default_compare_string_func);

    iCluige.iScript.register_ScriptFactory = scpt_register_ScriptFactory;
}

