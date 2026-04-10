

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

//	StringBuilder sb;
//	this_Script->_class_name = iCluige.iStringBuilder.string_alloc(&sb, 6);
//	iCluige.iStringBuilder.append(&sb, "Script");
	this_Script->_sub_class = NULL;

	this_Script->delete_Script = scpt_delete_Script;
	this_Script->enter_tree = NULL;
	this_Script->ready = NULL;
	this_Script->exit_tree = NULL;
	this_Script->process = NULL;
	return this_Script;
}

//path : absolute from res:// (excluding res://),
// with extension like .c or .gd
// "path/to/aa.gd"
// "path/to/aa.c"
// "path/to/aa.zzz"
static void scpt_register_ScriptFactory_with_ext(const char* full_path, ScriptFactory* factory)
{
	const char* last_dot = strrchr(full_path, '.');
	CLUIGE_ASSERT(last_dot,
		"Script::register_ScriptFactory() : expected extension in script file name (usually '.gd') not found (%s)",
		full_path);
	int key_len_term = last_dot - full_path;
	CLUIGE_ASSERT(key_len_term > 0,
		"Script::register_ScriptFactory() : script path '%s' too short", full_path);
	char* fcty_key = iCluige.checked_malloc((key_len_term + 1) * sizeof(char));
	strncpy(fcty_key, full_path, key_len_term);
	fcty_key[key_len_term] = '\0';
	iCluige.iScript.register_ScriptFactory_no_ext(fcty_key, factory);
}

//path : absolute from res:// (excluding res://),
// but no extension (like .gd or .c)
// "path/to/aa"
static void scpt_register_ScriptFactory_no_ext(const char* full_path, ScriptFactory* factory)
{
	CLUIGE_ASSERT(factory,
		"Script::register_ScriptFactory() : null factory");
	const char* last_dot = strrchr(full_path, '.');
	CLUIGE_ASSERT(last_dot == NULL,
		"Script::register_ScriptFactory() : unexpected extension in script file name (%s)",
		full_path);
	CLUIGE_ASSERT(!strstr(full_path, "res://"),
		"Script::register_ScriptFactory() : unexpected leading 'res://' in script file path (%s)",
		full_path);
	CLUIGE_ASSERT(strlen(full_path) > 0,
		"Script::register_ScriptFactory() : script path '%s' too short", full_path);
	SortedDictionary* fcties = &(iCluige.iScript.script_factories);
	Checked_Variant found = iCluige.iSortedDictionary.insert(fcties, full_path, factory);
	CLUIGE_ASSERT(!(found.valid),
		"Script::register_ScriptFactory() : trying to register a script with the already used key '%s'",
		full_path);
}


//script_file_path must exclude res:// and exclude extension (like .gd or .c) which is ignored
// "path/to/aa"
static Script* scpt_instantiate_from_factories_with_ext(const char* script_file_path, const SortedDictionary* parsed_params)
{
	const char* last_dot = strrchr(script_file_path, '.');
	CLUIGE_ASSERT(last_dot,
		"Script::instantiate_from_factories_with_ext() : expected extension in script file name (usually '.gd') not found (%s)",
		script_file_path);
	CLUIGE_ASSERT(!strstr(script_file_path, "res://"),
		"Script::instantiate_from_factories_with_ext() : unexpected leading 'res://' in script file path (%s)",
		script_file_path);
	int key_len_term = last_dot - script_file_path;
	CLUIGE_ASSERT(key_len_term > 6,
		"Script::instantiate_from_factories_with_ext() : script path '%s' too short", script_file_path);
	char fcty_key[key_len_term + 1];//444];
	strncpy(fcty_key, script_file_path, key_len_term);
	fcty_key[key_len_term] = '\0';
	return iCluige.iScript.instantiate_from_factories_no_ext(fcty_key, parsed_params);
}

//script_file_path must exclude res:// but no extension (like .gd or .c)
// "path/to/aa"
static Script* scpt_instantiate_from_factories_no_ext(const char* script_file_path, const SortedDictionary* parsed_params)
{
	SortedDictionary* fcties = &(iCluige.iScript.script_factories);
	Checked_Variant found = iCluige.iSortedDictionary.get(fcties, script_file_path);
	CLUIGE_ASSERT(found.valid,
		"Script::instantiate_from_factories_no_ext() : trying to instantiate unknown script '%s'",
		script_file_path);
	ScriptFactory* factory = (ScriptFactory*)(found.v.ptr);
	CLUIGE_ASSERT(factory,
		"Script::instantiate_from_factories_no_ext() : factory registered for script '%s' is NULL",
		script_file_path);
	return factory->instantiate(parsed_params);
}

/////////////////////////////////// iScript //////////

void iiScript_init()
{
	iCluige.iScript.new_Script = scpt_new_Script;

	SortedDictionary* fcties = &(iCluige.iScript.script_factories);
	iCluige.iSortedDictionary.sorted_dictionary_alloc(
		fcties, VT_POINTER, VT_POINTER, 20);
	iCluige.iSortedDictionary.set_compare_keys_func(
		fcties, iCluige.iDeque.default_compare_string_func);

	iCluige.iScript.register_ScriptFactory_with_ext = scpt_register_ScriptFactory_with_ext;
	iCluige.iScript.register_ScriptFactory_no_ext = scpt_register_ScriptFactory_no_ext;
	iCluige.iScript.instantiate_from_factories_with_ext = scpt_instantiate_from_factories_with_ext;
	iCluige.iScript.instantiate_from_factories_no_ext = scpt_instantiate_from_factories_no_ext;
}

