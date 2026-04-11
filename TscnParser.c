
//#include <stdio.h> // in TscnParser.h <= FileLineReader.h
#include <string.h>
#include "cluige.h"
#include "TscnParser.h"


////////////////////////////////// _TscnParser /////////

//[gd_scene load_steps=7 format=3 uid="uid://b75aarxsf6n0o"]
static bool tsnp_header(TscnParser* this_TscnParser)
{
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	int curr_line_i = this_TscnParser->_current_line;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	if(curr_line == NULL)
	{
		return false;
	}
	char* delim = strstr(curr_line, " format=");
	if(delim == NULL)
	{
		return false;
	}
	//[gd_scene load_steps=7 format=3 uid="uid://b75aarxsf6n0o"]
	//                      ^
	delim += strlen(" format=");
	//[gd_scene load_steps=7 format=3 uid="uid://b75aarxsf6n0o"]
	//                              ^
	char tmp_str[200] = "";
	sscanf(delim, "%s", tmp_str);
	CLUIGE_ASSERT(strlen(tmp_str) < sizeof(this_TscnParser->_tscn_format),
			"TscnParser::%s : _tscn_format too little, fix by increasing its size in .h",
			__FUNCTION__);
	strcpy(this_TscnParser->_tscn_format, tmp_str);
	if(!str_equals(tmp_str, "3"))
	{
		return false;
	}

	delim = strstr(curr_line, " uid=\"");
	if(delim == NULL)
	{
		return false;
	}
	//[gd_scene load_steps=7 format=3 uid="uid://b75aarxsf6n0o"]
	//                               ^
	delim += strlen(" uid=\"");
	//[gd_scene load_steps=7 format=3 uid="uid://b75aarxsf6n0o"]
	//                                     ^
	tmp_str[0] = '\0';
	char* uid_end = strchr(delim, '\"');
	if(uid_end == NULL)
	{
		return false;
	}
	int uid_len = strlen(delim) - strlen(uid_end);
	//[gd_scene load_steps=7 format=3 uid="uid://b75aarxsf6n0o"]
	//                                     ^                  ^
	CLUIGE_ASSERT(uid_len < sizeof(tmp_str),
			"TscnParser::%s : tmp_str too little, fix by increasing its size in this function",
			__FUNCTION__);
	strncpy(tmp_str, delim, uid_len);
	tmp_str[uid_len] = '\0';
	this_TscnParser->_tmp_uid = iCluige.iStringBuilder.clone(tmp_str);
	return true;
}

static bool tsnp_is_ending_quote(TscnParser* this_TscnParser)
{
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	int curr_line_i = this_TscnParser->_current_line;
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

static bool tsnp_value(TscnParser* this_TscnParser)
{
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	int curr_line_i = this_TscnParser->_current_line;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);

	int len = strlen(curr_line);
	int before_value_len = this_TscnParser->_current_param_len + 3; // "param" + " = "
	int token_len = len - before_value_len;
	if(token_len <= 0)
	{
		return false;
	}
	const char* current_token = curr_line + before_value_len;
	this_TscnParser->_current_value_line_i = curr_line_i;
	this_TscnParser->_current_value_first_i = before_value_len;

	//case with (potentially multiline) brackets { ... }
	//for example AnimationPlayer libraries
	if(current_token[0] == '{')
	{
		this_TscnParser->_current_value_len = 1;
		current_token++;
		int nb_opened = 1;
		while(nb_opened > 0)
		{
			this_TscnParser->_current_value_len += strlen(current_token);
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
			this_TscnParser->_current_line = curr_line_i;
			if(iCluige.iFileLineReader.feof(fr, curr_line_i))
			{
				CLUIGE_ASSERT(false, "TscnParser::value() : tscn file error : unclosed '{'");
				return false;
			}
			current_token = iCluige.iFileLineReader.get_line(fr, curr_line_i);
		}
		return true;
	}

	//case with (potentially multiline) quoted string
	else if(current_token[0] == '\"')
	{
		int line_len = 0;
		bool more = true;
		while(more)
		{
			//despecialize \" or \\ (or other ?)
			char* tmp = strchr(curr_line, '\\');// find first \"
			while(tmp != NULL)
			{
				tmp += 2;
				line_len--;
				tmp = strchr(tmp, '\\');// find next \"
			}
			token_len += line_len;
			more = !(this_TscnParser->is_ending_quote(this_TscnParser));
			if(more)
			{
				this_TscnParser->_current_line++;
				curr_line = iCluige.iFileLineReader.get_line(
						fr, this_TscnParser->_current_line);
				line_len = strlen(curr_line);
			}
		}

		if(curr_line[strlen(curr_line) - 1] == '\n')
		{
			token_len--;
		}
		this_TscnParser->_current_value_len = token_len;
		this_TscnParser->_current_line++;
		return true;
	}

	else //easy case : no delimiter, just read until end of line
	{
		if(curr_line[token_len + before_value_len - 1] == '\n')
		{
			token_len--;
		}
		this_TscnParser->_current_value_len = token_len;
		this_TscnParser->_current_line++;
		return true;
	}
	CLUIGE_BREAKPOINT(NULL, true);//why gone here?
	return false;
}

static bool tsnp_param(TscnParser* this_TscnParser)
{
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	int curr_line_i = this_TscnParser->_current_line;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	if(curr_line == NULL)
	{
		return false;
	}
	int param_len = 0;
	int ok = sscanf(curr_line, "%*s%n = ", &param_len);
	if(param_len <= 0 || param_len >= (strlen(curr_line) - 1) )
	{
		return false;
	}
	ok = strncmp(curr_line, "[node", 5);
	if(ok == 0)//not a param
	{
		return false;
	}
	this_TscnParser->_current_param_line_i = curr_line_i;
	this_TscnParser->_current_param_len = param_len;

//int fscanf(FILE * restrict stream, const char * restrict format, ...)
//returns the number of input items assigned (or EOF if zero items ok)
//The fscanf function reads input from the stream pointed to by stream, under control
//of the string pointed to by format that specifies the admissible input sequences and how
//they are to be converted for assignment, using subsequent arguments as pointers to the
//objects to receive the converted input.
//The first character, if any, after the input item remains unread. Trailing white space
// (including new-line characters) is left unread unless matched by a directive.

	bool parse_ok = this_TscnParser->value(this_TscnParser);
	if(!parse_ok)
	{
		return false;
	}
	return true;
}


//header examples :
//[node name="UneRacine" type="Node2D"]
//[node name="UnLabel" type="Label" parent="." node_paths=PackedStringArray("_une_racine")]
//[node name="UnAnimationPlayer" type="AnimationPlayer" parent="UnLabel"]
//[node name="UnSprite2D" type="Sprite2D" parent="."]
//[node name="UnSousSprite2D" type="Sprite2D" parent="UnSprite2D"]
//[node name="UnNode" type="Node" parent="UnSprite2D/UnSousSprite2D"]
//[node name="UnNode2D" type="Node2D" parent="UnSprite2D/UnSousSprite2D"]
//[node name="UnAutreLabel" type="Label" parent="."]
//[node name="unLabelFourbe" type="Label" parent="."]
//
//[node name="XXX" type="YYY" parent="ZZZ" WWW]
//[node name="cello" parent="cellist" instance=ExtResource("1_2rkvl")]
static bool tsnp_node(TscnParser* this_TscnParser)
{
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	int curr_line_i = this_TscnParser->_current_line;
//	int backup_curr_line_i = curr_line_i;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	//[node name="XXX" type="YYY" parent="ZZZ" WWW]
	const char* from = curr_line;//beware : pointer can become invalid if FileLineReader realloc buffer

	int tmp_len = strlen("[node name=\"");
	int ok = strncmp(from, "[node name=\"", tmp_len);
	if(ok != 0)
	{
		return false;
	}
	from += tmp_len;//XXX" type="YYY" parent="ZZZ" WWW]
	tmp_len = strcspn(from, "\"");
	char* tmp_name = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
	strncpy(tmp_name, from, tmp_len);
	tmp_name[tmp_len] = '\0';

	this_TscnParser->_current_packed_scene = iCluige.iPackedScene.new_PackedScene();
	PackedScene* ps = this_TscnParser->_current_packed_scene;
	ps->name = tmp_name;
	SortedDictionary* ps_dico = &(ps->dico_node);
	char* name_name = iCluige.checked_malloc(6 * sizeof(char));//because will be deleted via dico.pre_delete()
	strncpy(name_name, "name", 6);
	int tmp_name_len = strlen(tmp_name);
	char* quoted_name = iCluige.checked_malloc((3 + tmp_name_len) * sizeof(char));
	strcpy(quoted_name, "\"");
	strncpy(quoted_name + 1, tmp_name, tmp_name_len);
	strncpy(quoted_name + 1 + tmp_name_len, "\"", 2);
	iCluige.iSortedDictionary.insert(ps_dico, name_name, quoted_name);

	int type_xor_inst = 0;

	tmp_len = strlen("\" type=\"");
	from = strstr(curr_line, "\" type=\"");//" type="YYY" parent="ZZZ" WWW]
	if(from != NULL)
	{
		from += tmp_len;//YYY" parent="ZZZ" WWW]
		tmp_len = strcspn(from, "\"");
		char* tmp_type = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
		strncpy(tmp_type, from, tmp_len);
		tmp_type[tmp_len] = '\0';
		ps->type = tmp_type;
		type_xor_inst++;
	}

	tmp_len = strlen("instance=ExtResource(\"");
	from = strstr(curr_line, "instance=ExtResource(\"");//instance=ExtResource("1_hl8su")
	if(from != NULL)
	{
		from += tmp_len;//1_hl8su")
		tmp_len = strcspn(from, "\"");
		char tmp_id_str[60] = "";
		CLUIGE_ASSERT(tmp_len < 60, "TscnParser::node() : cluige bug, tmp_id_str is too small");
		strncpy(tmp_id_str, from, tmp_len);
		tmp_id_str[tmp_len] = '\0';
		SortedDictionary* dic = &(this_TscnParser->_dico_id_to_path);
		Checked_Variant cv = iCluige.iSortedDictionary.get(dic, tmp_id_str);
		CLUIGE_ASSERT(cv.valid, "TscnParser::node() : ExtResource id was not registered in _dico_id_to_path");
		char* path = (char*)(cv.v.ptr);
		tmp_len = strlen(path);
		char* tmp_inst = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
		strcpy(tmp_inst, path);
		ps->instance_path = tmp_inst;
		type_xor_inst++;
	}

	if(type_xor_inst != 1)
	{
		free(tmp_name);
		CLUIGE_ASSERT(false,
			"TscnParser::node() : in scene file reading : node with incorrect type or instance resource");
		return false;
	}

	from = strstr(curr_line, "\" parent=\"");//" parent="ZZZ" WWW]
	tmp_len = strlen("\" parent=\"");
	if(from == NULL)//ok != 0)
	{
		CLUIGE_ASSERT(this_TscnParser->scene_root == NULL, "TscnParser::node() : scene_root is not null");
		ps->parent = NULL;
		this_TscnParser->scene_root = ps;
	}
	else
	{
		from += tmp_len;//ZZZ" WWW]
		tmp_len = strcspn(from, "\"");
		char* tmp_parent = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
		strncpy(tmp_parent, from, tmp_len);
		tmp_parent[tmp_len] = '\0';
		ps->parent = tmp_parent;
		Deque* siblings = NULL;
		if(tmp_parent[0] == '.')
		{
			siblings = &(this_TscnParser->scene_root->children);
		}
		else
		{
			PackedScene* parent_ps = iCluige.iPackedScene.get_packed_node(
					this_TscnParser->scene_root, tmp_parent);
			siblings = &(parent_ps->children);
		}
		iCluige.iDeque.push_back(siblings, ps);
	}

	SortedDictionary* dic = &(ps->dico_node);
	Checked_Variant cv;
	this_TscnParser->_current_line++;

	bool parse_ok = this_TscnParser->param(this_TscnParser);
	char* keeped_key, *keeped_val;//, *dbg_dic;
	while(parse_ok)
	{
		tmp_len = this_TscnParser->_current_param_len;
		keeped_key = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
//		strncpy(keeped_key, this_TscnParser->_current_param, tmp_len);
		const char* param_line = iCluige.iFileLineReader.get_line(fr,
				this_TscnParser->_current_param_line_i);
		strncpy(keeped_key, param_line,
				this_TscnParser->_current_param_len);
		keeped_key[tmp_len] = '\0';

		//strcpy() value, but can be multiline and file_reader buffer contains '\0' between lines
		tmp_len = this_TscnParser->_current_value_len;
		keeped_val = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
		int nb_copied = 0;
//		const char* tmp_value_line = this_TscnParser->_current_value;
		const char* tmp_value_line =
				iCluige.iFileLineReader.get_line(fr,
						this_TscnParser->_current_value_line_i)
				+ this_TscnParser->_current_value_first_i;
		char* tmp_append_here = keeped_val;
		while(nb_copied < tmp_len)
		{
			int sub_len = strlen(tmp_value_line);
//			strncpy(tmp_append_here, tmp_value_line, sub_len);
			//despecialize \" or \\ (or other ?)
			char* special = strchr(tmp_value_line, '\\');
			while(special != NULL)
			{
				int sub_sub_len = (int) (special - tmp_value_line);
				strncpy(tmp_append_here, tmp_value_line, sub_sub_len);
				tmp_append_here[sub_sub_len] = tmp_value_line[sub_sub_len + 1];
				sub_sub_len++;
				tmp_append_here += sub_sub_len;
				tmp_value_line += sub_sub_len + 1;
				nb_copied += sub_sub_len;
				special = strchr(tmp_value_line, '\\');
				sub_len -= sub_sub_len + 1;
			}
			strncpy(tmp_append_here, tmp_value_line, sub_len);
			tmp_append_here += sub_len;
			tmp_value_line += sub_len + 1;
			nb_copied += sub_len;
		}
		keeped_val[tmp_len] = '\0';

		if(str_equals(keeped_key, "texture") && str_equals(ps->type, "Sprite2D"))
		{
			//keeped_val is like 'ExtResource("2_efpur")\0'
			//we must extract the id, then find filepath from it
			char* svg_id;
			bool ok = utils_id_str_from_ExtResource_parsed(&svg_id, keeped_val);
			CLUIGE_ASSERT(ok, "TscnParser::node() : texture id could not be read");

			SortedDictionary* dico_ids = &(this_TscnParser->_dico_id_to_path);
			cv = iCluige.iSortedDictionary.get(dico_ids, svg_id);
			//CLUIGE_ASSERT(cv.valid || 0=="id of texture not found in tscn_parser dico", "TscnParser::() : ");
			if(cv.valid)
			{
				keeped_key = realloc(keeped_key, strlen("svg_file_path!") * sizeof(char));
				//TODO utils::checked_realloc()
				strcpy(keeped_key, "svg_file_path");
				char* file_path = (char*)(cv.v.ptr);
				free(keeped_val);
				keeped_val = file_path;
				//char* dbg = iCluige.iSortedDictionary.debug_str_str(dic);
				//CLUIGE_BREAKPOINT(dbg, true);
			}
			free(svg_id);
		}
		else if(str_equals(keeped_key, "script"))
		{
			//keeped_val is like 'ExtResource("2_efpur")\0'
			//we must extract the id, then find filepath from it
			char* script_id;
			bool ok = utils_id_str_from_ExtResource_parsed(&script_id, keeped_val);
			CLUIGE_ASSERT(ok, "TscnParser::node() : script id could not be read");

			SortedDictionary* dico_ids = &(this_TscnParser->_dico_id_to_path);
			cv = iCluige.iSortedDictionary.get(dico_ids, script_id);
			CLUIGE_ASSERT(cv.valid, "TscnParser::node() : id of script '%s' not found in '%s' tscn_parser dico", keeped_val, ps->name);
			if(cv.valid)
			{
				char* file_path_ext = (char*)(cv.v.ptr);//"res://path/to/gg.gd"
				const char* last_dot = strrchr(file_path_ext, '.');
				int no_ext_len = last_dot - file_path_ext;
//				StringBuilder sb;
//				iCluige.iStringBuilder.string_alloc(&sb, no_ext_len + 2);
//				char* file_path_no_ext = sb.built_string;
//				iCluige.iStringBuilder.append(&sb, "\"");
//				iCluige.iStringBuilder.nappend(&sb, no_ext_len, file_path_ext);
//				iCluige.iStringBuilder.append(&sb, "\"");
				char* file_path_no_ext = iCluige.checked_malloc(no_ext_len + 1);
				snprintf(file_path_no_ext, no_ext_len + 1, file_path_ext);
				file_path_no_ext[no_ext_len] = '\0';//"res://path/to/gg"
				free(keeped_val);//'ExtResource("2_efpur")\0'
				keeped_val = file_path_no_ext;
				//char* dbg = iCluige.iSortedDictionary.debug_str_str(dic);
				//CLUIGE_BREAKPOINT(dbg, true);
			}
			free(script_id);
		}

		cv = iCluige.iSortedDictionary.insert(dic,
				keeped_key,
				keeped_val);
		//dbg_dic = iCluige.iSortedDictionary.debug_str_str(dic);
		bool overwritten = (cv.valid);
		CLUIGE_ASSERT(!overwritten,
			"TscnParser::node() : parsed param '%s' would overwrite another param-value", keeped_key);

		parse_ok = this_TscnParser->param(this_TscnParser);
	}
	return true;
}

//examples:
//[gd_scene load_steps=7 format=3 uid="uid://b75aarxsf6n0o"]
//[ext_resource type="Script" path="res://Label.gd" id="1_4fevc"]
//[sub_resource type="Animation" id="Animation_1oibt"]
//length = 0.001
//(or empty line)
//no advance in file stream from this function
static bool tsnp_is_starting_node(TscnParser* this_TscnParser)
{
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	int curr_line_i = this_TscnParser->_current_line;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	int tmp_len = strlen("[node name=\"");
	int cmp = strncmp(curr_line, "[node name=\"", tmp_len);
	return cmp == 0;
}

//examples:
//[ext_resource type="Texture2D" uid="uid://8j50qrs1q30l" path="res://loupe.svg" id="1_kwa0e"]
//[ext_resource type="Script" path="res://player.gd" id="2_pseyi"]
//[ext_resource type="PackedScene" uid="uid://dqk05t62v3vgm" path="res://cello.tscn" id="1_2rkvl"]
//(or empty line)
//advances in file stream if returns true
static bool tsnp_ext_res(TscnParser* this_TscnParser)
{
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	int curr_line_i = this_TscnParser->_current_line;
	const char* curr_line = iCluige.iFileLineReader.get_line(fr, curr_line_i);
	if(strncmp(curr_line, "\n", 1) == 0)
	{
		this_TscnParser->_current_line++;
		return true;
	}
	int tmp_len = strlen("[ext_resource type=\"");
	int cmp = strncmp(curr_line, "[ext_resource type=\"", tmp_len);
	if(cmp != 0)
	{
		return false;
	}
	const char* from = curr_line + tmp_len;//Texture2D" uid="uid://8j50qrs1q30l" path="res://loupe.svg" id="1_kwa0e"]
	tmp_len = strcspn(from, "\"");
	//SVG, Script, PackedScene : same parsing
	if(
		(strncmp(from, "Texture2D\"", tmp_len+1) == 0) ||
		(strncmp(from, "Script\"", tmp_len+1) == 0) ||
		(strncmp(from, "PackedScene\"", tmp_len+1) == 0)
	)
	{
		from = strstr(curr_line, "\" id=\"");
		CLUIGE_ASSERT(from != NULL,
			"TscnParser::ext_res() : error in cluige or in tscn : 'id' not found for texture");
		tmp_len = strlen("\" id=\"");
		from += tmp_len;//1_kwa0e"]
		tmp_len = strcspn(from, "\"");
		char* keeped_key = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
		strncpy(keeped_key, from, tmp_len);
		keeped_key[tmp_len] = '\0';

		from = strstr(curr_line, "\" path=\"res://");
		CLUIGE_ASSERT(from != NULL,
			"TscnParser::ext_res() : error in cluige or in tscn : 'path' not found for texture");
		tmp_len = strlen("\" path=\"res://");
		from += tmp_len;//loupe.svg" id="1_kwa0e"]
		tmp_len = strcspn(from, "\"");
		char* keeped_val = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
		strncpy(keeped_val, from, tmp_len);//loupe.svg
		keeped_val[tmp_len] = '\0';

//		bool is_svg = (keeped_val[tmp_len - 4] == '.');
//		is_svg = is_svg && ( (keeped_val[tmp_len - 3] == 's') || (keeped_val[tmp_len - 3] == 'S') );
//		is_svg = is_svg && ( (keeped_val[tmp_len - 2] == 'v') || (keeped_val[tmp_len - 2] == 'V') );
//		is_svg = is_svg && ( (keeped_val[tmp_len - 1] == 'g') || (keeped_val[tmp_len - 1] == 'G') );
//		if(is_svg)
//		{
			SortedDictionary* dico = &(this_TscnParser->_dico_id_to_path);
			Checked_Variant cv = iCluige.iSortedDictionary.insert(dico, keeped_key, keeped_val);
			CLUIGE_BREAKPOINT(dico, cv.valid);//key/val overwriten
//		}
//		else
//		{
//			free(keeped_key);
//			free(keeped_val);
//		}
	}
	this_TscnParser->_current_line++;
	return true;
}

static bool tsnp_parse_scene(TscnParser* this_TscnParser)
{
	int nb_nodes = 0;
	bool more_ignore = true;
	bool more_ext_res = true;
	bool more_node = true;
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	this_TscnParser->_current_line = 0;
	if(!(this_TscnParser->header(this_TscnParser)))
	{
		return false;
	}
	this_TscnParser->_current_line++;
	while(!iCluige.iFileLineReader.feof(fr, this_TscnParser->_current_line))
	{
		more_ext_res = this_TscnParser->ext_res(this_TscnParser);//auto-consumes empty lines
		while(more_ext_res && !iCluige.iFileLineReader.feof(fr, this_TscnParser->_current_line))
		{
			//this_TscnParser->_current_line++;//auto-consumed
			more_ext_res = (this_TscnParser->ext_res(this_TscnParser));
			if(fr->_nb_chars > 400)//arbitrary number to lighten buffer but not too often
			{
				iCluige.iFileLineReader.forget_lines_before(fr, this_TscnParser->_current_line);
			}
		}
		more_ignore = !(this_TscnParser->is_starting_node(this_TscnParser));
		while(more_ignore && !iCluige.iFileLineReader.feof(fr, this_TscnParser->_current_line))
		{
			//consume empty lines or non-node lines
			this_TscnParser->_current_line++;
			more_ignore = !(this_TscnParser->is_starting_node(this_TscnParser));
			if(fr->_nb_chars > 400)//arbitrary number to lighten buffer but not too often
			{
				iCluige.iFileLineReader.forget_lines_before(fr, this_TscnParser->_current_line);
			}
		}
		more_node = this_TscnParser->node(this_TscnParser);
//		PackedScene* ps = this_TscnParser->_current_packed_scene;//dbg
//		CLUIGE_BREAKPOINT(ps, false);
		if(more_node)
		{
			iCluige.iFileLineReader.forget_lines_before(fr, this_TscnParser->_current_line);
			nb_nodes++;
		}
	}
	SortedDictionary* path_to_ps = &(iCluige.iPackedScene.dico_path_to_packed);
//	int k_len = strlen(fr->_file_path);
//	char* k = iCluige.checked_malloc((1 + k_len) * sizeof(char));
//	strcpy(k, fr->_file_path);
	char* k = NULL;
	int res_path_len = strlen(iCluige.resource_path);
	if(res_path_len == 0)
	{
		k = iCluige.iStringBuilder.clone(fr->_file_path);
	}
	else
	{
		k = iCluige.iStringBuilder.clone(fr->_file_path + res_path_len + 1);
	}
	PackedScene* v = this_TscnParser->scene_root;
	Checked_Variant cv = iCluige.iSortedDictionary.insert(path_to_ps, k, v);
	CLUIGE_ASSERT(!(cv.valid),
			"TscnParser::parse_scene() : another parsed scene was already registered at this path");
	this_TscnParser->scene_root->uid = this_TscnParser->_tmp_uid;
	return nb_nodes > 0;
}


////////////////////////////////// iiTscnParser /////////

static void tsnp_tscn_parser_alloc(struct _TscnParser* this_TscnParser, const char* file_path, int buffer_capacity)
{
	CLUIGE_ASSERT(file_path != NULL, "TscnParser::tscn_parser_alloc() : file_path is null");
	//public
	this_TscnParser->scene_root = NULL;
	this_TscnParser->_tscn_format[0] = '\0';
	this_TscnParser->_tmp_uid = NULL;

	//private
	this_TscnParser->_current_packed_scene = NULL;
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	iCluige.iFileLineReader.fileLineReader_alloc(fr, buffer_capacity);
	bool ok = iCluige.iFileLineReader.open_file_start_reader(fr, file_path);
	if(!ok)
	{
		printf("\n  ERROR : cannot read scene file %s\n\n", file_path);
		CLUIGE_ASSERT(false, "TscnParser::tscn_parser_alloc() : cannot read file");
	}
	this_TscnParser->_current_line = -1;//first line of file is # 0

	this_TscnParser->parse_scene = tsnp_parse_scene;
	this_TscnParser->header = tsnp_header;
	this_TscnParser->is_starting_node = tsnp_is_starting_node;
	this_TscnParser->ext_res = tsnp_ext_res;
	this_TscnParser->node = tsnp_node;
	this_TscnParser->param = tsnp_param;
	this_TscnParser->value = tsnp_value;
//	this_TscnParser->read_line = tsnp_read_line;
	this_TscnParser->is_ending_quote = tsnp_is_ending_quote;

	SortedDictionary* dico = &(this_TscnParser->_dico_id_to_path);
	iCluige.iSortedDictionary.sorted_dictionary_alloc(dico, VT_POINTER, VT_POINTER, 7);
	iCluige.iSortedDictionary.set_compare_keys_func(dico, iCluige.iDeque.default_compare_string_func);
}

static void tsnp_pre_delete_TscnParser(TscnParser* this_TscnParser)
{
	//fclose(this_TscnParser->_file);
	FileLineReader* fr = &(this_TscnParser->_file_reader);
	iCluige.iFileLineReader.close_file(fr);
	iCluige.iFileLineReader.pre_delete_FileLineReader(fr);

	SortedDictionary* dico = &(this_TscnParser->_dico_id_to_path);
	iCluige.iSortedDictionary.pre_delete_SortedDictionary(dico);
	// ? scene_root - including _current_packed_scene

//	free(this_TscnParser->_current_line);
//	free(this_TscnParser->_current_param);
//	free(this_TscnParser->_current_value);
	// free file_path ? used in result packed scene ? => freed in iFileLineReader.pre_delete_FileLineReader()
}


/////////////////////////////////// iTscnParser //////////

void iiTscnParser_init()
{
	iCluige.iTscnParser.tscn_parser_alloc = tsnp_tscn_parser_alloc;
	iCluige.iTscnParser.pre_delete_TscnParser = tsnp_pre_delete_TscnParser;
}

