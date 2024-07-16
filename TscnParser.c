
//#include <stdio.h> // in TscnParser.h
#include <string.h>
#include <assert.h>
#include "cluige.h"
#include "TscnParser.h"


////////////////////////////////// _TscnParser /////////

static bool tsnp_read_line(TscnParser* this_TscnParser)
{
//A successful call to fgetpos stores a representation of the
//value of this mbstate_t object as part of the value of the fpos_t object. A later
//successful call to fsetpos using the same stored fpos_t value restores the value of
//the associated mbstate_t object as well as the position within the controlled stream.

	fpos_t backup_pos;
	int ok = fgetpos(this_TscnParser->_file, &backup_pos);
	if(ok != 0)
	{
		assert(00 == "failed to get stream position");
		return false;
	}

	int max_len = this_TscnParser->_current_line_capacity;//just shorter alias
	char* res = fgets(this_TscnParser->_current_line, max_len, this_TscnParser->_file);
	//final "\n" included (but no "\n" if last line of file is EOF without \n)
	if(res == NULL)
	{
		//assert(00 == "failed to read line");
		ok = fsetpos(this_TscnParser->_file, &backup_pos);
		assert(ok == 0 || 00 == "failed to set stream position");
		return false;
	}
	int nb_read = strlen(this_TscnParser->_current_line);
	int i = 0;
	while(nb_read >= max_len - 1) // _current_line not long enough
	{
		res = fgets(this_TscnParser->_current_line, max_len, this_TscnParser->_file);
		if(res == NULL)
		{
			//assert(00 == "failed to read line");
			ok = fsetpos(this_TscnParser->_file, &backup_pos);
			assert(ok == 0 || 00 == "failed to set stream position");
			return false;
		}
		nb_read = strlen(this_TscnParser->_current_line);
		i++;
	}
	if(i > 0)
	{
		// _current_line was not long enough, must allocate more
		max_len = (i + 1) * max_len;
		this_TscnParser->_current_line_capacity = max_len;
		free(this_TscnParser->_current_line);
		this_TscnParser->_current_line = iCluige.checked_malloc(max_len * sizeof(char));

		ok = fsetpos(this_TscnParser->_file, &backup_pos);
		if(ok != 0)
		{
			assert(00 == "failed to set stream position");
			return false;
		}
		res = fgets(this_TscnParser->_current_line, max_len, this_TscnParser->_file);
		if(res == NULL)
		{
			//assert(00 == "failed to read line");
			ok = fsetpos(this_TscnParser->_file, &backup_pos);
			assert(ok == 0 || 00 == "failed to set stream position");
			return false;
		}
	}
	return true;

//char *fgets(char * restrict s, int n, FILE * restrict stream);
//The fgets function reads at most one less than the number of characters specified by n
//from the stream pointed to by stream into the array pointed to by s. No additional
//characters are read after a new-line character (which is retained) or after end-of-file. A
//null character is written immediately after the last character read into the array.
//The fgets function returns s if successful.
//If end-of-file is encountered and no
//characters have been read into the array, the contents of the array remain unchanged and a
//null pointer is returned. If a read error occurs during the operation, the array contents are
//indeterminate and a null pointer is returned.
}

static bool tsnp_is_ending_quote(TscnParser* this_TscnParser)
{
	//reverse iterator
	int r = strlen(this_TscnParser->_current_line) - 1;
	if(r < 0)
	{
		return false;
	}
	else if(this_TscnParser->_current_line[r] == '\n')
	{
		r--;
	}

	if(r < 0 || this_TscnParser->_current_line[r] != '\"')
	{
		return false;
	}
	//else : line is non-empty and finished by '"'

	//handle cases like \", \\", \\\"...
	r--;
	int nb_antislashes = 0;
	while(r >= 0 && this_TscnParser->_current_line[r] == '\\')
	{
		nb_antislashes++;
		r--;
	}
	return (nb_antislashes % 2) == 0;
}

static bool tsnp_value(TscnParser* this_TscnParser)
{
	int len = 0;
	fpos_t backup_pos;
	int ok = fgetpos(this_TscnParser->_file, &backup_pos);
	if(ok != 0)
	{
		assert(00 == "failed to get stream position");
		return false;
	}
	if(!(this_TscnParser->read_line(this_TscnParser)))
	{
		return false;
	}
	int nb_read = strlen(this_TscnParser->_current_line);
	if(nb_read == 0)
	{
		return false;
	}

	//later : add case with brackets { ... }

	//case with (potentially multiline) quoted string
	if(this_TscnParser->_current_line[0] == '\"')
	{
		while(!(this_TscnParser->is_ending_quote(this_TscnParser)))
		{
			if(!(this_TscnParser->read_line(this_TscnParser)))
			{
				return false;
			}
			nb_read += strlen(this_TscnParser->_current_line);
		}
		ok = fsetpos(this_TscnParser->_file, &backup_pos);
		if(ok != 0)
		{
			assert(00 == "failed to set stream position");
			return false;
		}
		if(this_TscnParser->_current_value_capacity < nb_read)
		{
			free(this_TscnParser->_current_value);
			int new_cap = (nb_read * 11) / 10;
			this_TscnParser->_current_value_capacity = new_cap;
			this_TscnParser->_current_value = iCluige.checked_malloc(new_cap * sizeof(char));
		}
		char* append_here = this_TscnParser->_current_value;
		if(!(this_TscnParser->read_line(this_TscnParser)))
		{
			return false;
		}
		strcpy(append_here, this_TscnParser->_current_line);
		nb_read = strlen(append_here);
		append_here += nb_read;
		while(!(this_TscnParser->is_ending_quote(this_TscnParser)))
		{
			if(!(this_TscnParser->read_line(this_TscnParser)))
			{
				return false;
			}
			strcpy(append_here, this_TscnParser->_current_line);
			nb_read = strlen(append_here);
			append_here += nb_read;
		}
		//trim final \n
		len = strlen(this_TscnParser->_current_value);
		if(len > 0 && this_TscnParser->_current_value[len - 1] == '\n')
		{
			this_TscnParser->_current_value[len - 1] = '\0';
		}
		return true;
	}
	else //easy case : no delimiter, just read until end of line
	{
		if(this_TscnParser->_current_value_capacity < nb_read)
		{
			free(this_TscnParser->_current_value);
			int new_cap = (nb_read * 11) / 10;
			this_TscnParser->_current_value_capacity = new_cap;
			this_TscnParser->_current_value = iCluige.checked_malloc(new_cap * sizeof(char));
		}
		strcpy(this_TscnParser->_current_value, this_TscnParser->_current_line);
		len = strlen(this_TscnParser->_current_value);
		if(len > 0 && this_TscnParser->_current_value[len - 1] == '\n')
		{
			this_TscnParser->_current_value[len - 1] = '\0';
		}
		return true;
	}
	utils_breakpoint_trick(NULL, true);//why gone here?
	return false;
}

static bool tsnp_param(TscnParser* this_TscnParser)
{
	fpos_t backup_pos;
	int ok = fgetpos(this_TscnParser->_file, &backup_pos);
	if(ok != 0)
	{
		assert(00 == "failed to get stream position");
		return false;
	}

	ok = fscanf(this_TscnParser->_file, "%s = ", this_TscnParser->_current_param);
	if(ok == 0 || ok == EOF)
	{
		return false;
	}
	ok = strncmp(this_TscnParser->_current_param, "[node", 5);
	if(ok == 0)//not a param
	{
		ok = fsetpos(this_TscnParser->_file, &backup_pos);
		assert(ok == 0 || 00 == "failed to set stream position");
		return false;
	}

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
		ok = fsetpos(this_TscnParser->_file, &backup_pos);
		assert(ok == 0 || 00 == "failed to set stream position");
		return false;
	}
	return true;
}

/*
header examples :
[node name="UneRacine" type="Node2D"]
[node name="UnLabel" type="Label" parent="." node_paths=PackedStringArray("_une_racine")]
[node name="UnAnimationPlayer" type="AnimationPlayer" parent="UnLabel"]
[node name="UnSprite2D" type="Sprite2D" parent="."]
[node name="UnSousSprite2D" type="Sprite2D" parent="UnSprite2D"]
[node name="UnNode" type="Node" parent="UnSprite2D/UnSousSprite2D"]
[node name="UnNode2D" type="Node2D" parent="UnSprite2D/UnSousSprite2D"]
[node name="UnAutreLabel" type="Label" parent="."]
[node name="unLabelFourbe" type="Label" parent="."]

[node name="XXX" type="YYY" parent="ZZZ" WWW]
*/
static bool tsnp_node(TscnParser* this_TscnParser)
{
	fpos_t backup_pos;
	int ok = fgetpos(this_TscnParser->_file, &backup_pos);
	if(ok != 0)
	{
		assert(00 == "failed to get stream position");
		return false;
	}
	bool parse_ok = false;
	int line_len = 0;
	while(line_len == 0)
	{
		parse_ok = this_TscnParser->read_line(this_TscnParser);
		if(!parse_ok)
		{
			return false;
		}
		line_len = strlen(this_TscnParser->_current_line);
		if(line_len > 0 && this_TscnParser->_current_line[0] == '\n')
		{
			line_len--;
		}
	}
	//sscanf not enough, %s expands till next blank
//	ok = sscanf(this_TscnParser->_current_line, "[node name=\"%s\" type=\"%s\"", tmp_name, tmp_str);
//	if(ok ...)
	char* from = this_TscnParser->_current_line;//[node name="XXX" type="YYY" parent="ZZZ" WWW]
	int tmp_len = strlen("[node name=\"");
	ok = strncmp(from, "[node name=\"", tmp_len);
	if(ok != 0)
	{
		ok = fsetpos(this_TscnParser->_file, &backup_pos);
		assert(ok == 0 || 00 == "failed to set stream position");
		return false;
	}
	from += tmp_len;//XXX" type="YYY" parent="ZZZ" WWW]
	tmp_len = strcspn(from, "\"");
	char* tmp_name = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
	strncpy(tmp_name, from, tmp_len);
	tmp_name[tmp_len] = '\0';

	from += tmp_len;//" type="YYY" parent="ZZZ" WWW]
	tmp_len = strlen("\" type=\"");
	ok = strncmp(from, "\" type=\"", tmp_len);
	if(ok != 0)
	{
		ok = fsetpos(this_TscnParser->_file, &backup_pos);
		assert(ok == 0 || 00 == "failed to set stream position");
		return false;
	}
	from += tmp_len;//YYY" parent="ZZZ" WWW]
	tmp_len = strcspn(from, "\"");
	char* tmp_type = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
	strncpy(tmp_type, from, tmp_len);
	tmp_type[tmp_len] = '\0';

	this_TscnParser->_current_packed_scene = iCluige.iPackedScene.new_PackedScene();
	PackedScene* ps = this_TscnParser->_current_packed_scene;
	ps->name = tmp_name;//iCluige.checked_malloc(strlen(tmp_name) * sizeof(char));
	ps->type = tmp_type;//iCluige.checked_malloc(strlen(tmp_str) * sizeof(char));

	from += tmp_len;//" parent="ZZZ" WWW]
	tmp_len = strlen("\" parent=\"");
	ok = strncmp(from, "\" parent=\"", tmp_len);
	if(ok != 0)
	{
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
	parse_ok = this_TscnParser->param(this_TscnParser);
	char* keeped_key, *keeped_val;//, *dbg_dic;
	while(parse_ok)
	{
		keeped_key = iCluige.checked_malloc((sizeof(char)) * (1 + strlen(this_TscnParser->_current_param)));
		strcpy(keeped_key, this_TscnParser->_current_param);
		keeped_val = iCluige.checked_malloc((sizeof(char)) * (1 + strlen(this_TscnParser->_current_value)));
		strcpy(keeped_val, this_TscnParser->_current_value);
		cv = iCluige.iSortedDictionary.insert(dic,
				keeped_key,
				keeped_val);
		//dbg_dic = iCluige.iSortedDictionary.debug_str_str(dic);
		bool inserted_ok = !(cv.valid);
		assert(inserted_ok || 00 == "could not insert parsed param-value");
		parse_ok = this_TscnParser->param(this_TscnParser);
	}
	return true;
}

static bool tsnp_ignore(TscnParser* this_TscnParser)
{
	return false;
}

static bool tsnp_parse_scene(TscnParser* this_TscnParser)
{
	return false;
}

//later : script


////////////////////////////////// iiTscnParser /////////

static void tsnp_tscn_parser_alloc(struct _TscnParser* this_TscnParser, const char* file_path)
{
	//deep copy to prevent pointing to the stack
	assert(file_path != NULL);
	int len = strlen(file_path);
	this_TscnParser->file_path = iCluige.checked_malloc((len + 1) * sizeof(char));
	strcpy(this_TscnParser->file_path, file_path);

	this_TscnParser->scene_root = NULL;
	this_TscnParser->_current_packed_scene = NULL;
	this_TscnParser->_current_line_capacity = 200;//grows if needed, see tsnp_read_line()
	this_TscnParser->_current_line = iCluige.checked_malloc(this_TscnParser->_current_line_capacity * sizeof(char));
	this_TscnParser->_current_param = iCluige.checked_malloc(200 * sizeof(char));//never grows, enough?
	this_TscnParser->_current_value = iCluige.checked_malloc(200 * sizeof(char));//grows if needed, see tsnp_value()
	this_TscnParser->_current_value_capacity = 200;

	this_TscnParser->_file = fopen(file_path, "r");
	if(this_TscnParser->_file == 00)
	{
		printf("\n\n  ERROR : cannot read file %s\n\n", file_path);
		assert((this_TscnParser->_file != 00) && "cannot read file");// && file_path);
	}

	this_TscnParser->parse_scene = tsnp_parse_scene;
	this_TscnParser->ignore = tsnp_ignore;
	this_TscnParser->node = tsnp_node;
	this_TscnParser->param = tsnp_param;
	this_TscnParser->value = tsnp_value;
	this_TscnParser->read_line = tsnp_read_line;
	this_TscnParser->is_ending_quote = tsnp_is_ending_quote;
}

static void tsnp_pre_delete_TscnParser(TscnParser* this_TscnParser)
{
	fclose(this_TscnParser->_file);
	free(this_TscnParser->_current_line);
	free(this_TscnParser->_current_param);
	free(this_TscnParser->_current_value);
	// free file_path ? used in result packed scene ?
}


/////////////////////////////////// iTscnParser //////////

void iiTscnParser_init()
{
	iCluige.iTscnParser.tscn_parser_alloc = tsnp_tscn_parser_alloc;
	iCluige.iTscnParser.pre_delete_TscnParser = tsnp_pre_delete_TscnParser;
}




/*
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// fd = fileno(f); //if you have a stream (e.g. from fopen), not a file descriptor.
struct stat buf;
fstat(fd, &buf);
off_t size = buf.st_size;

https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
stat gives you both numbers. st_size is the real length, with byte granularity. st_blocks is the number of 512-byte disk blocks used by the file (including extra blocks for metadata, attributes, and even block-lists or extent-lists for large files where the list of blocks or extents doesn't fit in the inode itself.) Whether the FS actually allocates in 512B blocks or not, that's the unit stat uses. (man7.org/linux/man-pages/man2/lstat.2.html). For most filesystems, st_size is accurate, but not on Linux /proc and /sys –
Peter Cordes
 CommentedNov 25, 2021
*/


/*

static bool _svp_parse_point(struct _SVGParser* this_SVGParser, FILE* file, char* buffer)
{
	//commands
	// m x y : move to x y
	// L x y line to x y
	// h dx horiz line
	// V y vertic line
	// C x1 y1, x2 y2, x y bezier (cubic) to x y
	// c dx1 dy1, dx2 dy2, dx dy
	// S x2 y2, x y bezier cubic with symetric slopes
	// Q x1 y1, x y bezier quadratic
	// T x y bezier quadratic infered from previous segment
	// A rx ry x-axis-rot large-arc-flag sweep-flag x y : arc to x y
	// z|Z close path

	char read_command_char;
	//char read_command_cZZZZ;read_command_cZZZZ='y';read_command_cZZZZ++;
	char command;
	float first_float;
	Vector2 parsed_point;
	float ignored;

	int n;
	do
	{
		//n = fscanf(file, "%1s", command);
//		n = fscanf(file, "%c", &read_command_char);
		n = fscanf(file, "%c", &read_command_char);
	}while((n == 0) ||
		   (read_command_char == ' ') ||
		   (read_command_char == '\n') ||
		   (read_command_char == '\t'));

	if(strchr("MLHVCSQTAZ", read_command_char) != 00)
	{
		absolute = true;
		command = read_command_char;
		last_command = read_command_char;
		fscanf(file, "%f", &first_float);
	}
	else if(strchr("mlhvcsqtaz", read_command_char) != 00)
	{
		absolute = false;
		command = read_command_char;
		last_command = read_command_char;
		fscanf(file, "%f", &first_float);
	}
	else if(read_command_char == '\"')
	{
		absolute = true;
		//printf("\n");
		return false;
	}
	else
	{
		command = last_command;
		// ~ put back first digit into buffer
		char first_float_str[19];
		first_float_str[0] = read_command_char;
		first_float_str[1] = (char)0; //special case, "%17[^,]," with "0,..."
		fscanf(file, "%17[^,\" ]", first_float_str+1);
		sscanf(first_float_str, "%f", &first_float);
	}

	switch(command)
	{
	case 'M':
	case 'm':
	case 'L':
	case 'l':
	case 'T':
	case 't':
		// L/l : TO TEST
		parsed_point.x = first_float;
		fscanf(file, ",%f", &(parsed_point.y));
		break;
	case 'H':
		parsed_point.x = first_float;
		parsed_point.y = svg_cursor.y; //TO TEST
		break;
	case 'h':
		parsed_point.x = first_float;
		parsed_point.y = 0; //TO TEST
		break;
	case 'V':
		parsed_point.x = svg_cursor.x; //TO TEST
		parsed_point.y = first_float;
		break;
	case 'v':
		parsed_point.x = 0; //TO TEST
		parsed_point.y = first_float;
		break;
	case 'C':
	case 'c':
//		first_float ignored
		fscanf(file, ",%f %f,%f %f,%f",
			   &ignored,&ignored,&ignored,
			   &(parsed_point.x), &(parsed_point.y));
		break;
	case 'S':
	case 's':
	case 'Q':
	case 'q':
//		first_float ignored
		fscanf(file, ",%f %f,%f",
			   &ignored,
			   &(parsed_point.x), &(parsed_point.y));//TO TEST
		break;
	case 'A':
	case 'a':
//		first_float ignored
		fscanf(file, ",%f %f,%f %f %f,%f",
			   &ignored,&ignored,&ignored,&ignored,
			   &(parsed_point.x), &(parsed_point.y));//TO TEST
		break;
	case 'Z':
	case 'z':
		absolute = true; // easier for z
		parsed_point.x = iCluige.iDeque.at(&(this_SVGParser->coordinates_sequence), 0).f;
		parsed_point.y = iCluige.iDeque.at(&(this_SVGParser->coordinates_sequence), 1).f;
		break;
	default:
		assert(false && "wrong svg path command in file");
	}

	if(absolute ||
	   //special case : first point is never relative
	   iCluige.iDeque.size(&(this_SVGParser->coordinates_sequence)) == 0)
	{
		svg_cursor = parsed_point;
	}
	else
	{
		iCluige.iVector2.add(&svg_cursor, &parsed_point, &svg_cursor);
	}
	//printf("%f,%f ", parsed_point.x, parsed_point.y);
	iCluige.iDeque.push_back(&(this_SVGParser->coordinates_sequence), svg_cursor.x);
	iCluige.iDeque.push_back(&(this_SVGParser->coordinates_sequence), svg_cursor.y);

//	int nbCoord = iCluige.iDeque.size(&(this_SVGParser->coordinates_sequence));
//	char* dbg = "																																																																																							";
//	StringBuilder sb;
//	iCluige.iStringBuilder.connect_existing_string(&sb, dbg);
//	for(int i=0; i < nbCoord; i++)
//	{
//		iCluige.iStringBuilder.append(&sb,"%f ", iCluige.iDeque.at(&(this_SVGParser->coordinates_sequence), i).f);
//		if((i % 2) == 0)
//			iCluige.iStringBuilder.append(&sb,"  ");;
//	}

	return true;
}

void _svp_parse_path(struct _SVGParser* this_SVGParser, FILE* file, char* buffer)
{
	//  <path ¤ ... d="..."
	iCluige.iDeque.clear(&(this_SVGParser->coordinates_sequence));
	last_command = 'M';
	bool finished = false;
	while(!finished)
	{
		fscanf(file, "%2s", buffer);
		if(strcmp(buffer, "/>") == 0)
		{
			//no point in path
			return;
		}
		else if(strcmp(buffer, "d=") == 0)
		{
			//printf("	%s\n", buffer);
			finished = true;
		}
	}
	//  <path ... d= ¤ "[M|m] -5.7154991,-26.450343 ..."
	fscanf(file, "%1s", buffer);
	if(strcmp(buffer, "\"") != 0)
	{
		//no point in path
		return;
	}

	//  <path ... d=" ¤ [M|m] -5.7154991,-26.450343 ..."
	while(_svp_parse_point(this_SVGParser, file, buffer)) //sequence of points and commands
		;
}

////////////////////////////////// iistruct _SVGParser /////////

static void svp_SVGParser_alloc(struct _SVGParser* this_SVGParser)
{
	iCluige.iDeque.deque_alloc(
		&(this_SVGParser->coordinates_sequence), VT_FLOAT, 2 * 50);
}

static bool svp_prepare_parsing(struct _SVGParser* this_SVGParser, char* file_path)
{
	file = fopen(file_path, "r");
	if(file == 00)
	{
		printf("\n\n  ERROR : cannot read file %s\n\n", file_path);
		assert((file != 00) && "cannot read file");// && file_path);
		return false;
	}

//	fscanf : all whitespaces are ignored even if not specified in the format param
	svg_cursor = (Vector2){ 0.f, 0.f};
	last_command = 'M';
	//this_SVGParser->file_finished = false;
	return true;
}

static bool svp_parse_path(struct _SVGParser* this_SVGParser)
{
	iCluige.iDeque.clear(&(this_SVGParser->coordinates_sequence));

	char buffer_tag[19];
	fscanf(file, "%1s", buffer_tag);
	//for unknown reason, "<" cannot be used with fscanf()
	if(buffer_tag[0] == '<')
	{
		fscanf(file, "%18s", buffer_tag);
		if(strcmp(buffer_tag, "path") == 0)
		{
			//printf("%s\n", buffer_tag);
			_svp_parse_path(this_SVGParser, file, buffer_tag);
		}
	}
	return !feof(file);
	//this_SVGParser->file_finished = feof(file);
}
static void svp_end_parsing(struct _SVGParser* this_SVGParser)
{
	fclose(file);
}



*/
