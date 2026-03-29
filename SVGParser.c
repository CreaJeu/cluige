
#include <stdio.h>
#include <string.h>

#include "cluige.h"
#include "SVGParser.h"

//for a better parser example, see TscnParser


////////////////////////////////// _SVGParser /////////

// ~private static
static Vector2 svg_cursor;
static char last_command;
static bool absolute;
static FILE* file;

static bool svp__parse_point(struct _SVGParser* this_SVGParser, FILE* file, char* buffer)
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
//        n = fscanf(file, "%c", &read_command_char);
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
//        first_float ignored
        fscanf(file, ",%f %f,%f %f,%f",
               &ignored,&ignored,&ignored,
               &(parsed_point.x), &(parsed_point.y));
        break;
    case 'S':
    case 's':
    case 'Q':
    case 'q':
//        first_float ignored
        fscanf(file, ",%f %f,%f",
               &ignored,
               &(parsed_point.x), &(parsed_point.y));//TO TEST
        break;
    case 'A':
    case 'a':
//        first_float ignored
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
        CLUIGE_ASSERT(false, "SVGParser::parse_point() : wrong svg path command in file");
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

//    int nbCoord = iCluige.iDeque.size(&(this_SVGParser->coordinates_sequence));
//    char* dbg = "                                                                                                                                                                                                                                                                                                                                                            ";
//    StringBuilder sb;
//    iCluige.iStringBuilder.connect_existing_string(&sb, dbg);
//    for(int i=0; i < nbCoord; i++)
//    {
//        iCluige.iStringBuilder.append(&sb,"%f ", iCluige.iDeque.at(&(this_SVGParser->coordinates_sequence), i).f);
//        if((i % 2) == 0)
//            iCluige.iStringBuilder.append(&sb,"  ");;
//    }

    return true;
}

void svp__parse_path(struct _SVGParser* this_SVGParser, FILE* file, char* buffer)
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
            //printf("    %s\n", buffer);
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
    while(svp__parse_point(this_SVGParser, file, buffer)) //sequence of points and commands
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
	int res_path_len = strlen(iCluige.resource_path);
	char* full_file_path = file_path;
//	iCluige.resource_path has no final / or \ thanks to set_resrouce_path()
	if(res_path_len != 0)
	{
		int new_path_len = res_path_len + 1 + strlen(file_path);
		full_file_path = iCluige.iStringBuilder.clone_formatted(
			new_path_len, "%s/%s", iCluige.resource_path, file_path);
			// " cluige.res_path / file_path "
	}

	file = fopen(full_file_path, "r");
    if(file == 00)
    {
//		printf("\n\n  ERROR : cannot read file %s\n\n", full_file_path);
		CLUIGE_ASSERT(file != 00, "SVGParser::prepare_parsing() : cannot read file %s", full_file_path);
		if(res_path_len != 0)
			free(full_file_path);
        return false;
    }
	if(res_path_len != 0)
	{
		free(full_file_path);
	}

//    fscanf : all whitespaces are ignored even if not specified in the format param
    svg_cursor = (Vector2){ 0.f, 0.f};
    last_command = 'M';
    //this_SVGParser->file_finished = false;
    return true;
}

static float svp__scale_by_unit(const char* unit, float x)
{
	if(str_equals(unit, "mm"))
	{
		return x * 3.78;
	}
	else if(str_equals(unit, "cm"))
	{
		return x * 37.8;
	}
	else if(str_equals(unit, "in"))
	{
		return x * 96;
	}
	else if(str_equals(unit, "pt"))
	{
		return x * 1.33333;
	}
	else if(str_equals(unit, "pc"))
	{
		return x * 16;
	}
	else //assume px => 1<=>1
	{
		return x;
	}
}

static bool svp__parse_tag_attributes(struct _SVGParser* this_SVGParser)
{
	char buffer_tag[99];
	char buffer_unit[9];
	int nb_found = 0;
	while(nb_found < 2 && !feof(file))
	{
		fscanf(file, "%*98[ \t\n]%[^<=]s", buffer_tag);
		if(str_equals("height", buffer_tag))
		{
			float h = 0.;
			fscanf(file, "%98s", buffer_tag);
			int nbc = sscanf(buffer_tag, "=\"%f%8[^\"]s", &h, buffer_unit);
			if(nbc < 1)
			{
				utils_breakpoint_trick("can't read height", true);
				return false;
			}
			this_SVGParser->height = svp__scale_by_unit(buffer_unit, h);
			nb_found++;
		}
		else if(str_equals("width", buffer_tag))
		{
			float w = 0.;
			fscanf(file, "%98s", buffer_tag);
			int nbc = sscanf(buffer_tag, "=\"%f%8[^\"]s", &w, buffer_unit);
			if(nbc < 1)
			{
				utils_breakpoint_trick("can't read width", true);
				return false;
			}
			this_SVGParser->width = svp__scale_by_unit(buffer_unit, w);
			nb_found++;
		}
		else
		{
			fscanf(file, "%s", buffer_tag);
		}
	}
	return !feof(file);
}

static bool svp_parse_svg_tag(struct _SVGParser* this_SVGParser)
{
	char buffer_tag[19];
	//for unknown reason, "<" cannot be used with fscanf()
	while(!feof(file))
	{
		fscanf(file, "%1s", buffer_tag);
		if(feof(file))
		{
			return false;
		}
		while(buffer_tag[0] != '<')
		{
			fscanf(file, "%1s", buffer_tag);
			if(feof(file))
			{
				return false;
			}
		}
		//tag start
		fscanf(file, "%18s", buffer_tag);
		if(feof(file))
		{
			return false;
		}
		if(str_equals(buffer_tag, "svg"))
		{
			bool ok = svp__parse_tag_attributes(this_SVGParser);
			if(!ok)
			{
				utils_breakpoint_trick("VGParser::_svp_parse_svg_tag() incorrect <svg> attributes", true);
				return false;
			}
			else
			{
				return true;
			}
		}
		else if(buffer_tag[0] != '?' && buffer_tag[0] != '!')
		{
			//other than <?xml or <!--
			return false;
		}
	}
	return false;
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
            svp__parse_path(this_SVGParser, file, buffer_tag);
        }
    }
    return !feof(file);
    //this_SVGParser->file_finished = feof(file);
}
static void svp_end_parsing(struct _SVGParser* this_SVGParser)
{
    fclose(file);
}


/////////////////////////////////// iSVGParser //////////

void iiSVGParser_init(struct iiSVGParser* this_iiSVGParser)
{
	this_iiSVGParser->SVGParser_alloc = svp_SVGParser_alloc;
	//this_iiSVGParser.pre_delete_SVGParser = svp_pre_delete_SVGParser;
	this_iiSVGParser->prepare_parsing = svp_prepare_parsing;
	this_iiSVGParser->parse_svg_tag = svp_parse_svg_tag;
    this_iiSVGParser->parse_path = svp_parse_path;
    this_iiSVGParser->end_parsing = svp_end_parsing;
}

