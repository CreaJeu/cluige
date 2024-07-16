#ifndef TSCN_PARSER_H_INCLUDED
#define TSCN_PARSER_H_INCLUDED

//for FILE*
#include <stdio.h>

//by default this "mother class" is for Godot 4
//(for future versions, "sub-classes" will have to be provided)

typedef struct _TscnParser TscnParser;
typedef struct _PackedScene PackedScene;

struct _TscnParser //"mother class", Godot 4 by default
{
	//public
	char* file_path;
	PackedScene* scene_root;

	//private
	FILE* _file;

	PackedScene* _current_packed_scene;
	char* _current_line;
	int _current_line_capacity;
	char* _current_value;
	int _current_value_capacity;
	char* _current_param;

	bool (*parse_scene)(TscnParser* this_TscnParser);
	bool (*ignore)(TscnParser* this_TscnParser);
	bool (*node)(TscnParser* this_TscnParser);
	bool (*param)(TscnParser* this_TscnParser);
	bool (*value)(TscnParser* this_TscnParser);

	bool (*read_line)(TscnParser* this_TscnParser);
	bool (*is_ending_quote)(TscnParser* this_TscnParser);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiTscnParser
{
	//deep-copies file_path
	void (*tscn_parser_alloc)(TscnParser* this_TscnParser, const char* file_path);

	void (*pre_delete_TscnParser)(TscnParser* this_TscnParser);

	//not here, outside in calling code
	//bool (*prepare_parsing)(struct _TscnParser* this_TscnParser, char* file_path);
	//void (*end_parsing)(struct _TscnParser* this_TscnParser);
};
//iTscnParser : in iiSpriteGodotScene

//to be called only by SpriteGodotScene_init() to set functions pointers
void iiTscnParser_init();





//  ~grammar godot 4 ("format=3" in .tscn)
/*
scene : ignored_top format=3 ignored_more_top node* ignored_bottom

node : node_header param* whole_script?
node_header : \[node name=quoted_string type=quoted_string [parent=quoted_string] [ignored] \]
whole_script : script = ExtResource("1_4fevc") \n param*
param : identifier = value
value : int | real | quoted_multi_line_string
	| ExtResource(quoted_string) | NodePath(quoted_string)
	| ...other? other_ignored?


parsing of "ignored_top format=3" is already done outside this "class"
quoted_string // included " are escaped as \", unicode special chars are "as is"
/!\ /parent\ is the /name\ of a previously parsed node, or "." as alias to root node

*/


#endif // TSCN_PARSER_H_INCLUDED
