#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

//#include "Node.h"
//#include "SortedDictionary.h"

//typedef struct _Node Node;
typedef struct _Script Script;
typedef struct _ScriptFactory ScriptFactory;

struct _ScriptFactory
{
    Script* (*instanciate)(const SortedDictionary* parsed_params);
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

//	void attach(Node*)
	void (*attach)(Script* this_Script, Node* n);

	//Dico<String, ScriptFactory*>
	// path -> factory*
	//path : absolute from res:// (including res://),
	// without extension like .c or .gd
	SortedDictionary script_factories;

	//path : absolute from res:// (including res://),
	// with extension like .c or .gd
	void (*register_ScriptFactory)(const char* full_path, ScriptFactory* factory);
};
//iScript : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiScript_init();


#endif // SCRIPT_H_INCLUDED
