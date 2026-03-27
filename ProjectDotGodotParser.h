#ifndef PROJECTDOTGODOT_PARSER_H_INCLUDED
#define PROJECTDOTGODOT_PARSER_H_INCLUDED


//#include "FileLineReader.h"

//by default this "mother class" is for Godot 4
//(for future versions, "sub-classes" will have to be provided)

typedef struct _ProjectDotGodotParser ProjectDotGodotParser;
//typedef struct _FileLineReader FileLineReader;

struct _ProjectDotGodotParser //"mother class", Godot 4 by default
{
	//private
	SortedDictionary _dico_params;
	FileLineReader* _file_line_reader;
	int _current_line_i;//first line of file is # 0
	int _current_param_line_i;
	int _current_param_len;
	int _current_value_line_i;
	int _current_value_first_i;
	int _current_value_len;
	bool _currently_in_input_section;
	char* _current_section;
	int _current_section_len;
	Deque* _current_input_action_triggers;
	SortedDictionary _dico_inputs;

	//public
	bool (*run)(ProjectDotGodotParser* this_ProjectDotGodotParser);

	//private
	bool (*_parse_whole)(ProjectDotGodotParser* this_ProjectDotGodotParser);
	bool (*_comment)(ProjectDotGodotParser* this_ProjectDotGodotParser);
	bool (*_section)(ProjectDotGodotParser* this_ProjectDotGodotParser);
	bool (*_blank_line)(ProjectDotGodotParser* this_ProjectDotGodotParser);
	bool (*_ignored)(ProjectDotGodotParser* this_ProjectDotGodotParser);
	bool (*_is_ending_quote)(ProjectDotGodotParser* this_ProjectDotGodotParser);
	bool (*_param)(ProjectDotGodotParser* this_ProjectDotGodotParser);
	bool (*_value)(ProjectDotGodotParser* this_ProjectDotGodotParser);
	bool (*_input)(ProjectDotGodotParser* this_ProjectDotGodotParser, const char* action_name, const char* input_str);
	bool (*_input_event_unicode)(ProjectDotGodotParser* this_ProjectDotGodotParser, char** input_str_ptr);
	bool (*_input_event_unsupported)(ProjectDotGodotParser* this_ProjectDotGodotParser, char** input_str_ptr);

	const char* (*_dbg_curr_line)(ProjectDotGodotParser* this_ProjectDotGodotParser);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiProjectDotGodotParser
{
	void (*project_dot_godot_parser_alloc)(ProjectDotGodotParser* this_ProjectDotGodotParser, const char* file_path);

	void (*pre_delete_ProjectDotGodotParser)(ProjectDotGodotParser* this_ProjectDotGodotParser);
};
//iProjectDotGodotParser : in iiCluige

//to be called only by cluige_init() to set functions pointers
void iiProjectDotGodotParser_init();


//  ~grammar godot 4 (config_version=5 in project.godot)
/*
whole_project.godot : line*

line : ignored | key-val

ignored : comment | section | blank-line
comment : ;.*\n
section : [.*\n
blank-line : blank*\n

key-val : key=bracketed_multi_line | key=quoted_multi_line | key=unquoted


(Notes)
quoted_string // included " are escaped as \", unicode special chars are "as is"
project.godot is UF-8

*/


#endif // PROJECTDOTGODOT_PARSER_H_INCLUDED
