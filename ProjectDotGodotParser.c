
// #include <string.h> // in cluige.h
#include <ctype.h> //for isblank()
#include "cluige.h"
#include "ProjectDotGodotParser.h"
#include "FileLineReader.h"
#include "PackedScene.h"

FileLineReader _file_reader;


////////////////////////////////// _ProjectDotGodotParser /////////

static bool pdgp__comment(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	FileLineReader* fr = &(_file_reader);
	int curr_line_i = this_ProjectDotGodotParser->_current_line_i;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	return curr_line[0] == ';';
}

static bool pdgp__section(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	FileLineReader* fr = &(_file_reader);
	int curr_line_i = this_ProjectDotGodotParser->_current_line_i;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	if(curr_line[0] == '[')
	{
//		sscanf(curr_line, "[%s]", this_ProjectDotGodotParser->_current_section);//no
		int section_len = min_int(159, strcspn(curr_line + 1, "]"));
		strncpy(this_ProjectDotGodotParser->_current_section,
				curr_line + 1, section_len);
		this_ProjectDotGodotParser->_current_section[section_len] = '\0';
		this_ProjectDotGodotParser->_current_section_len = section_len;
		this_ProjectDotGodotParser->_currently_in_input_section =
				str_equals(this_ProjectDotGodotParser->_current_section, "input");
		return true;
	}
	return false;
}

static bool pdgp__blank_line(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	FileLineReader* fr = &(_file_reader);
	int curr_line_i = this_ProjectDotGodotParser->_current_line_i;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	int len = strlen(curr_line);
	//maybe
	//  return str_equals(curr_line, "\n") ||  str_equals(curr_line, "")
	//  would be enough, but just in case
	if(len > 0 && curr_line[len - 1] == '\n')
	{
		len--;
	}
	for(int i=0; i < len; i++)
	{
		if(!isblank(curr_line[i]))
		{
			return false;
		}
	}
	return true;
}

static bool pdgp__ignored(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	return this_ProjectDotGodotParser->_comment(this_ProjectDotGodotParser) ||
			this_ProjectDotGodotParser->_section(this_ProjectDotGodotParser) ||
			this_ProjectDotGodotParser->_blank_line(this_ProjectDotGodotParser);
}

static bool pdgp__is_ending_quote(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	//maybe one day : factorize this function with TscnParser::is_ending_quote() (identical)
	FileLineReader* fr = &(_file_reader);
	int curr_line_i = this_ProjectDotGodotParser->_current_line_i;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	//reverse iterator
	int r = strlen(curr_line) - 1;
	if(r < 0)
	{
		return false;
	}
	else if(curr_line[r] == '\n')
	{
		r--;
	}

	if(r < 0 || curr_line[r] != '\"')
	{
		return false;
	}
	//else : line is non-empty and finished by '"'

	//handle cases like \", \\", \\\"...
	r--;
	int nb_antislashes = 0;
	while(r >= 0 && curr_line[r] == '\\')
	{
		nb_antislashes++;
		r--;
	}
	return (nb_antislashes % 2) == 0;
}

//'value' part of the ~grammar rule :
//key-value : key=bracketed_multi_line | key=quoted_multi_line | key=unquoted
static bool pdgp__value(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	//maybe one day : factorize this function with TscnParser::value()
	// (identical except "=" instead of " = ")
	FileLineReader* fr = &(_file_reader);
	int curr_line_i = this_ProjectDotGodotParser->_current_line_i;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);

	int len = strlen(curr_line);
	// "param" + "="
	int before_value_len = this_ProjectDotGodotParser->_current_param_len + 1;
	int token_len = len - before_value_len;
	if(token_len <= 0)
	{
		return false;
	}
	const char* current_token = curr_line + before_value_len;
	this_ProjectDotGodotParser->_current_value_line_i = curr_line_i;
	this_ProjectDotGodotParser->_current_value_first_i = before_value_len;

	//case with (potentially multiline) brackets { ... }
	//for example AnimationPlayer libraries
	if(current_token[0] == '{')
	{
		this_ProjectDotGodotParser->_current_value_len = 1;
		current_token++;
		int nb_opened = 1;
		int tmp_len;
		while(nb_opened > 0)
		{
			tmp_len = strlen(current_token);
			this_ProjectDotGodotParser->_current_value_len += tmp_len;
			//strpbrk
			while(current_token != NULL)
			{
				current_token = strpbrk(current_token, "{}");//1st occurrence of either
				if(current_token != NULL)
				{
					if(current_token[0] == '{')
					{
						nb_opened++;
						current_token++;
					}
					else if(current_token[0] == '}')
					{
						nb_opened--;
						current_token++;
					}
				}
			}
			curr_line_i++;
			this_ProjectDotGodotParser->_current_line_i = curr_line_i;
			if(iCluige.iFileLineReader.feof(fr, curr_line_i))
			{
				CLUIGE_ASSERT(false, "ProjectDotGodotParser::value() : file error : unclosed '{'");
				return false;
			}
			current_token = iCluige.iFileLineReader.get_line(fr, curr_line_i);
		}
		this_ProjectDotGodotParser->_current_value_len--;//exclude ending \n
		return true;
	}

	//case with (potentially multiline) quoted string
	else if(current_token[0] == '\"')
	{
		int line_len = strlen(curr_line);
		while(!(this_ProjectDotGodotParser->_is_ending_quote(this_ProjectDotGodotParser)))
		{
			this_ProjectDotGodotParser->_current_line_i++;
			curr_line_i = this_ProjectDotGodotParser->_current_line_i;
			curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
			line_len = strlen(curr_line);
			token_len += line_len;
		}

		if(curr_line[line_len - 1] == '\n')
		{
			token_len--;
		}
		this_ProjectDotGodotParser->_current_value_len = token_len;
		this_ProjectDotGodotParser->_current_line_i++;
		return true;
	}

	else //easy case : no delimiter, just read until end of line
	{
		if(curr_line[token_len + before_value_len - 1] == '\n')
		{
			token_len--;
		}
		this_ProjectDotGodotParser->_current_value_len = token_len;
		this_ProjectDotGodotParser->_current_line_i++;
		return true;
	}
	utils_breakpoint_trick(NULL, true);//why gone here?
	return false;
}

static bool pdgp__input_event_unsupported(ProjectDotGodotParser* this_ProjectDotGodotParser, char** input_str_ptr)
{
	char* delim = strstr(*input_str_ptr, "Object(");
	//"events": [Object(Input... \n
	//           ^
	//, Object(Input... \n
	//  ^
	if(delim == NULL)
	{
		return false;
	}
	delim = strstr(*input_str_ptr, "\n");
	//, Object(Input... \n
	//                   ^
	if(delim == NULL)
	{
		return false;
	}
	*input_str_ptr = delim + 1;
	return true;
}

static bool pdgp__input_event_unicode(ProjectDotGodotParser* this_ProjectDotGodotParser, char** input_str_ptr)
{
	char* delim = strstr(*input_str_ptr, "\"unicode\":");
	//"events": [Object(InputEv... ,"unicode":0, ..."script":null)\n,
	//                              ^
	if(delim == NULL)
	{
		return false;
	}
	delim += strlen("\"unicode\":");
	//"events": [Object(InputEv...,"unicode":0,..."script":null)\n,
	//                                       ^
	uint64_t uni = 0;
	sscanf(delim, "%lu%*s", &uni);
	if(uni != 0)
	{
		Deque* dq = this_ProjectDotGodotParser->_current_input_action_triggers;
		iCluige.iDeque.push_back(dq, uni);
		*input_str_ptr = strstr(delim, "\n") + 1;
		//"events": [Object(InputEv...,"unicode":0,..."script":null)\n,
		//                                                            ^
		return true;
	}
	return false;
}

/* example
ui_up={
"deadzone": 0.5,
"events": [Object(InputEventKey,"resource_local_to_scene":false,"resource_name":"","device":0,"window_id":0,"alt_pressed":false,"shift_pressed":false,"ctrl_pressed":false,"meta_pressed":false,"pressed":false,"keycode":4194320,"physical_keycode":0,"key_label":0,"unicode":0,"location":0,"echo":false,"script":null)
, Object(InputEventJoypadButton,"resource_local_to_scene":false,"resource_name":"","device":-1,"button_index":11,"pressure":0.0,"pressed":false,"script":null)
, Object(InputEventJoypadMotion,"resource_local_to_scene":false,"resource_name":"","device":-1,"axis":1,"axis_value":-1.0,"script":null)
, Object(InputEventKey,"resource_local_to_scene":false,"resource_name":"","device":-1,"window_id":0,"alt_pressed":false,"shift_pressed":false,"ctrl_pressed":false,"meta_pressed":false,"pressed":false,"keycode":0,"physical_keycode":87,"key_label":0,"unicode":122,"location":0,"echo":false,"script":null)
]
}
*/
static bool pdgp__input(ProjectDotGodotParser* this_ProjectDotGodotParser, const char* action_name, const char* input_str)
{
	char* delim = strstr(input_str, "{\n");
	if(delim != input_str)
	{
		return false;
	}
	//bra{ck}eted={\n
	//      delim:^
	//"my thing"={\n
	//     delim:^
	//easy={\n
	//     ^
	delim += 2;
	//"events": [Object(InputEv... ,"unicode":0, ..."script":null)\n,
	//^

	delim = strstr(delim, "\"deadzone\":");
	//"deadzone": 0.5,\n
	//^
	if(delim == NULL)
	{
		return false;
	}
	delim = strstr(delim, ",\n");
	//"deadzone": 0.5,\n
	//           delim:^
	if(delim == NULL)
	{
		return false;
	}
	delim += 2;
	//"events": [Object(InputEv...,"unicode":0,..."script":null)\n,
	//^
	Deque* dq_action_triggers = iCluige.checked_malloc(sizeof(Deque));
	iCluige.iDeque.deque_alloc(dq_action_triggers, VT_UINT64, 4);
	int action_name_len = strlen(action_name);
	char* keeped_action_name = iCluige.checked_malloc((action_name_len + 1) * sizeof(char));
	strncpy(keeped_action_name, action_name, action_name_len);
	keeped_action_name[action_name_len] = '\0';
	iCluige.iSortedDictionary.insert(
			&(this_ProjectDotGodotParser->_dico_inputs),
			keeped_action_name, dq_action_triggers);
	this_ProjectDotGodotParser->_current_input_action_triggers = dq_action_triggers;
	char* tmp_delim = delim;
	while(
		this_ProjectDotGodotParser->_input_event_unicode(this_ProjectDotGodotParser, &tmp_delim)
		// TODO : || event_physical() || event_mouse() || ...
		|| this_ProjectDotGodotParser->_input_event_unsupported(this_ProjectDotGodotParser, &tmp_delim)
		)
	{
		delim = tmp_delim;
	}
	return true;
}

//one whole line of the ~grammar rule :
//key-value : key=bracketed_multi_line | key=quoted_multi_line | key=unquoted
static bool pdgp__param(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	//maybe one day : factorize this function with TscnParser::value()
	// (identical except "=" instead of " = ", and '[node' cannot be found)
	FileLineReader* fr = &(_file_reader);
	int curr_line_i = this_ProjectDotGodotParser->_current_line_i;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	if(curr_line == NULL)
	{
		return false;
	}
	int param_len = strcspn(curr_line, "=");//	sscanf(curr_line, "%*s%n=", &param_len);
	if(param_len <= 0 || param_len >= (strlen(curr_line) - 1) )
	{
		return false;
	}
	this_ProjectDotGodotParser->_current_param_line_i = curr_line_i;
	this_ProjectDotGodotParser->_current_param_len = param_len;

	bool parse_ok = this_ProjectDotGodotParser->_value(this_ProjectDotGodotParser);
	if(parse_ok)
	{
		int sect_len = this_ProjectDotGodotParser->_current_section_len;
		int keeped_key_len =
				this_ProjectDotGodotParser->_current_param_len
				+ sect_len
				+ 2;
		char* keeped_key = iCluige.checked_malloc(keeped_key_len * sizeof(char));
		const char* param_line = iCluige.iFileLineReader.get_line(fr,
				this_ProjectDotGodotParser->_current_param_line_i);
		strncpy(keeped_key, this_ProjectDotGodotParser->_current_section,
				sect_len);
		keeped_key[sect_len] = '/';
		strncpy(keeped_key + sect_len + 1, param_line,
				this_ProjectDotGodotParser->_current_param_len);
		keeped_key[this_ProjectDotGodotParser->_current_param_len
				+ sect_len + 1] = '\0';

		//strcpy() value, but can be multiline and file_reader buffer contains '\0' between lines
		int tmp_len = this_ProjectDotGodotParser->_current_value_len;
		char* keeped_val = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
		int nb_copied = 0;
		const char* tmp_value_line =
				iCluige.iFileLineReader.get_line(fr,
						this_ProjectDotGodotParser->_current_value_line_i)
				+ this_ProjectDotGodotParser->_current_value_first_i;
		char* tmp_append_here = keeped_val;
		int sub_line_i = 0;
		while(nb_copied < tmp_len)
		{
			int sub_len = strlen(tmp_value_line);
			strncpy(tmp_append_here, tmp_value_line, sub_len);
			tmp_append_here += sub_len;
			sub_line_i++;
			tmp_value_line = iCluige.iFileLineReader.get_line(fr,
					this_ProjectDotGodotParser->_current_value_line_i + sub_line_i);
			nb_copied += sub_len;
		}
		keeped_val[tmp_len] = '\0';

		SortedDictionary* dic = &(this_ProjectDotGodotParser->_dico_params);
		Checked_Variant cv = iCluige.iSortedDictionary.insert(dic,
				keeped_key,//key_copy,//
				keeped_val);
//		CLUIGE_ASSERT(str_equals(keeped_key, key_copy), "changed!");
		bool overwritten = (cv.valid);
		CLUIGE_ASSERT(!overwritten,
			"ProjectDotGodotParser::param() : parsed param '%s' would overwrite another param-value", keeped_key);

		if(this_ProjectDotGodotParser->_currently_in_input_section)
		{
			if(this_ProjectDotGodotParser->_input(
					this_ProjectDotGodotParser,
					keeped_key + strlen("input/"),
					keeped_val))
			{
				//all done inside _input()
			}
		}

		return true;
	}
	return false;
}

static bool pdgp__parse_whole(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	int nb_params = 0;
	FileLineReader* fr = &(_file_reader);
	while(!iCluige.iFileLineReader.feof(fr, this_ProjectDotGodotParser->_current_line_i))
	{
		while( this_ProjectDotGodotParser->_ignored(this_ProjectDotGodotParser)
			&& !iCluige.iFileLineReader.feof(fr, this_ProjectDotGodotParser->_current_line_i))
		{
			//consume empty lines or non-param lines
			this_ProjectDotGodotParser->_current_line_i++;
		}
		if(fr->_nb_chars > 200)//arbitrary number to lighten buffer but not too often
		{
			iCluige.iFileLineReader.forget_lines_before(fr, this_ProjectDotGodotParser->_current_line_i);
		}
		if(this_ProjectDotGodotParser->_param(this_ProjectDotGodotParser))
		{
			nb_params++;
		}
		else
		{
			CLUIGE_ASSERT(
				iCluige.iFileLineReader.feof(fr, this_ProjectDotGodotParser->_current_line_i),
				"ProjectDotGodotParser::%s : unable to parse line '%s'", __FUNCTION__,
				iCluige.iFileLineReader.get_line(
						&_file_reader,
						this_ProjectDotGodotParser->_current_line_i));
		}
	}
	return nb_params > 0;
}

static bool pdgp_run(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	bool ok = this_ProjectDotGodotParser->_parse_whole(this_ProjectDotGodotParser);
	if(!ok)
	{
		CLUIGE_ASSERT(false, "ProjectDotGodotParser::pdgp_run() : parsing failed");
		return false;
	}

	//apply results to cluige
	SortedDictionary* params = &(this_ProjectDotGodotParser->_dico_params);

	int tmp_int =-1;
	if(utils_int_from_parsed(&tmp_int, params, "application/run/max_fps"))
	{
		iCluige.wanted_frame_seconds = 1 / (float)tmp_int;
	}
	utils_int_from_parsed(&(iCluige.window_initial_size_cols), params, "display/window/size/viewport_width");
	utils_int_from_parsed(&(iCluige.window_initial_size_lines), params, "display/window/size/viewport_height");
	utils_str_from_parsed(&(iCluige.window_title), params, "application/config/name");

	//instantiate main scene
	char* tmp_uid = NULL;
	ok = utils_str_from_parsed(&tmp_uid, params, "application/run/main_scene");
	if(!ok)
	{
		CLUIGE_ASSERT(false, "ProjectDotGodotParser::run() : 'main_scene' field not found in project.godot");
		return false;
	}
	PackedScene* root_ps = iCluige.iPackedScene.packed_scene_from_uid(tmp_uid);
	free(tmp_uid);
	if(!root_ps)
	{
		CLUIGE_ASSERT(false, "ProjectDotGodotParser::run() : main_scene not found from known UIDs (scenes have to be parsed beforehand)");
		return false;
	}
	Node* root_node = iCluige.iPackedScene.instantiate(root_ps);
	if(!root_node)
	{
		CLUIGE_ASSERT(false, "ProjectDotGodotParser::run() : main_scene not found from known UIDs (scenes have to be parsed beforehand)");
		return false;
	}
	iCluige.iNode.add_child(iCluige.public_root_2D, root_node);

	//inputs
	SortedDictionary* dic_in = &(this_ProjectDotGodotParser->_dico_inputs);
	int d_n = iCluige.iSortedDictionary.size(dic_in);
	for(int i=0; i<d_n; i++)
	{
		Pair pair_i = iCluige.iSortedDictionary.at(dic_in, i);
		const char* action_name = (const char*)(pair_i.first.ptr);
		Deque* dq = (Deque*)(pair_i.second.ptr);
		int m = iCluige.iDeque.size(dq);
		if(m > 0)
		{
			iCluige.iInput.add_action(action_name);
			for(int j=0; j<m; j++)
			{
				uint64_t uni = iCluige.iDeque.at(dq, j).ui64;
				iCluige.iInput.bind_key(action_name, uni);
			}
		}
	}
	return true;
}

static const char* pdgp__dbg_curr_line(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	return iCluige.iFileLineReader.get_line(&_file_reader, this_ProjectDotGodotParser->_current_line_i);
}


////////////////////////////////// iiProjectDotGodotParser /////////

static void pdgp_project_dot_godot_parser_alloc(struct _ProjectDotGodotParser* this_ProjectDotGodotParser, const char* file_path)
{
	CLUIGE_ASSERT(file_path != NULL, "ProjectDotGodotParser::pdgp_project_dot_godot_parser_alloc() : file_path is null");
	iCluige.iFileLineReader.fileLineReader_alloc(&(_file_reader), 6000);
	this_ProjectDotGodotParser->_file_line_reader = &(_file_reader);
	bool ok = iCluige.iFileLineReader.open_file_start_reader(&(_file_reader), file_path);
	if(!ok)
	{
		printf("\n  ERROR : cannot read file %s\n\n", file_path);
		CLUIGE_ASSERT(false, "ProjectDotGodotParser::pdgp_project_dot_godot_parser_alloc() : cannot read file");
	}
	this_ProjectDotGodotParser->_current_line_i = 0;//first line of file is # 0
	this_ProjectDotGodotParser->_currently_in_input_section = false;
	this_ProjectDotGodotParser->_current_section = iCluige.checked_malloc(160 * sizeof(char));
	this_ProjectDotGodotParser->_current_section[0] = '\0';
	this_ProjectDotGodotParser->_current_section_len = 0;
	this_ProjectDotGodotParser->_current_input_action_triggers = NULL;

	SortedDictionary* dico = &(this_ProjectDotGodotParser->_dico_params);
	iCluige.iSortedDictionary.sorted_dictionary_alloc(dico, VT_POINTER, VT_POINTER, 40);
	iCluige.iSortedDictionary.set_compare_keys_func(dico, iCluige.iDeque.default_compare_string_func);

	SortedDictionary* dic_in = &(this_ProjectDotGodotParser->_dico_inputs);
	iCluige.iSortedDictionary.sorted_dictionary_alloc(dic_in, VT_POINTER, VT_POINTER, 20);
	iCluige.iSortedDictionary.set_compare_keys_func(dic_in, iCluige.iDeque.default_compare_string_func);

	this_ProjectDotGodotParser->run = pdgp_run;
	this_ProjectDotGodotParser->_parse_whole = pdgp__parse_whole;
	this_ProjectDotGodotParser->_comment = pdgp__comment;
	this_ProjectDotGodotParser->_section = pdgp__section;
	this_ProjectDotGodotParser->_blank_line = pdgp__blank_line;
	this_ProjectDotGodotParser->_ignored = pdgp__ignored;
	this_ProjectDotGodotParser->_param = pdgp__param;
	this_ProjectDotGodotParser->_value = pdgp__value;
	this_ProjectDotGodotParser->_input = pdgp__input;
	this_ProjectDotGodotParser->_input_event_unicode = pdgp__input_event_unicode;
	this_ProjectDotGodotParser->_input_event_unsupported = pdgp__input_event_unsupported;
	this_ProjectDotGodotParser->_is_ending_quote = pdgp__is_ending_quote;

	this_ProjectDotGodotParser->_dbg_curr_line = pdgp__dbg_curr_line;
}

static void pdgp_pre_delete_ProjectDotGodotParser(ProjectDotGodotParser* this_ProjectDotGodotParser)
{
	free(this_ProjectDotGodotParser->_current_section);

	FileLineReader* fr = &(_file_reader);
	iCluige.iFileLineReader.close_file(fr);
	iCluige.iFileLineReader.pre_delete_FileLineReader(fr);

	SortedDictionary* dico = &(this_ProjectDotGodotParser->_dico_params);
	for(int i=0; i < iCluige.iSortedDictionary.size(dico); i++)
	{
		Pair p = iCluige.iSortedDictionary.at(dico, i);
		free( (char*)(p.first.ptr) );
		free( (char*)(p.second.ptr) );
	}
	iCluige.iSortedDictionary.pre_delete_SortedDictionary(dico);

	SortedDictionary* dic_in = &(this_ProjectDotGodotParser->_dico_inputs);
	for(int i=0; i < iCluige.iSortedDictionary.size(dic_in); i++)
	{
		Pair p = iCluige.iSortedDictionary.at(dic_in, i);
		free( (char*)(p.first.ptr) );
		Deque* dq = (Deque*)(p.second.ptr);
		iCluige.iDeque.pre_delete_Deque(dq);
		free(dq);
	}
	iCluige.iSortedDictionary.pre_delete_SortedDictionary(dic_in);
}


/////////////////////////////////// iProjectDotGodotParser //////////

void iiProjectDotGodotParser_init()
{
	iCluige.iProjectDotGodotParser.project_dot_godot_parser_alloc = pdgp_project_dot_godot_parser_alloc;
	iCluige.iProjectDotGodotParser.pre_delete_ProjectDotGodotParser = pdgp_pre_delete_ProjectDotGodotParser;
}
