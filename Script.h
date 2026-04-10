#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

//#include "Node.h"
//#include "SortedDictionary.h"

//typedef struct _Node Node;
typedef struct _Script Script;
typedef struct _ScriptFactory ScriptFactory;

struct _ScriptFactory
{
	Script* (*instantiate)(const SortedDictionary* parsed_params);
};

struct _Script
{
	Node* node;

	//for inheritance
	//char* _class_name;
	void* _sub_class;

	//virtual methods
	void (*delete_Script)(Script*);
	void (*enter_tree)(Script*);
	void (*ready)(Script*);
	void (*exit_tree)(Script*);
	void (*process)(Script* this_Script, float delta);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiScript
{
	//Script* new_Script()
	Script* (*new_Script)();

	//Dico<String, ScriptFactory*>
	// path -> factory*
	//path : absolute from res:// (excluding res://),
	// without extension like .c or .gd
	SortedDictionary script_factories;

	//path : absolute from res:// (excluding res://),
	// with extension like .c or .gd
	// "path/to/aa.gd"
	// "path/to/aa.c"
	// "path/to/aa.zzz"
	void (*register_ScriptFactory_with_ext)(const char* full_path, ScriptFactory* factory);

	//path : absolute from res:// (excluding res://),
	// but no extension (like .gd or .c)
	// "path/to/aa"
	void (*register_ScriptFactory_no_ext)(const char* full_path, ScriptFactory* factory);

	//script_file_path must exclude res:// and include extension (like .gd or .c) which is ignored
	// "path/to/aa.gd"
	Script* (*instantiate_from_factories_with_ext)(const char* script_file_path, const SortedDictionary* parsed_params);
	//script_file_path must exclude res:// but no extension (like .gd or .c)
	// "path/to/aa"
	Script* (*instantiate_from_factories_no_ext)(const char* script_file_path, const SortedDictionary* parsed_params);
};
//iScript : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiScript_init();


#endif // SCRIPT_H_INCLUDED
