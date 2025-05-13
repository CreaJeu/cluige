//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
#include "SpriteSVG.h"
#include "../SVGParser.h"

//#include <string.h>
#include <curses.h>
#include <math.h>



////////////////////////////////// _SpriteSVG /////////

// ~private static
struct _SVGParser _svg_parser;

static void ssvg_delete_SpriteSVG(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteSVG* this_SpriteSVG = (SpriteSVG*)(this_Node2D->_sub_class);
    //tmp memorize function pointer before calling free(this)
    void (*delete_super)(Node*) = this_SpriteSVG->_delete_super;

    //clear display
	if(this_Node2D->visible && !(iCluige.quit_asked))
	{
		this_Node->erase(this_Node);
	}
    //delete my attributes
    //Deque<Path2D* > paths
    Deque* paths = &(this_SpriteSVG->paths);
    int nb_paths = iCluige.iDeque.size(paths);
    for(int i=0; i<nb_paths; i++)
    {
        Path2D* path_i = iCluige.iDeque.at(paths, i).ptr;
        iCluige.iPath2D.pre_delete_Path2D(path_i);
        free(path_i);
    }
    iCluige.iDeque.pre_delete_Deque(paths);
    //free(paths);

    CLUIGE_ASSERT(this_SpriteSVG->_sub_class == NULL, "SpriteSVG::delete_SpriteSVG() : not null subclass found");
    free(this_SpriteSVG);
    this_Node2D->_sub_class = NULL;
    delete_super(this_Node);
}

//utility function
static void ssvg_draw_line(const float start_x, const float start_y, const float end_x, const float end_y, bool draw)
{
    Vector2 start_pos = {start_x, start_y};
    Vector2 end_pos = {end_x, end_y};

	LineDrawerThin d;
    iCluige.iLineDrawerThin.start(&d, &start_pos, &end_pos);
	while(!iCluige.iLineDrawerThin.has_finished(&d))
	{
	    if(draw)
        {
            const char* glyph = iCluige.iLineDrawerThin.glyph(&d);
            //mvaddch(d.current_position.y, d.current_position.x, shade(d.curr_dist));
            mvaddstr(d.current_position.y, d.current_position.x, glyph);
        }
        else
        {
            //erase
            mvaddch(d.current_position.y, d.current_position.x, ' ');
        }

		iCluige.iLineDrawerThin.next(&d);
	}
}

static void ssvg_erase(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteSVG* this_SpriteSVG = (SpriteSVG*)(this_Node2D->_sub_class);
    //call super()
    this_SpriteSVG->_erase_super(this_Node);
    if(!(this_Node2D->visible))
    {
        return;
    }

    //clear old one (unless immobile? => no, because other masking things
    //could have moved and made this sprite visible again;
    //and curses already has characters cache)
    Vector2 orig;
    iCluige.iVector2.add(
            &(this_Node2D->_tmp_global_position),
            &(this_SpriteSVG->offset),
            &orig);

    Camera2D* current_camera = iCluige.iCamera2D.current_camera;
    CLUIGE_ASSERT(current_camera != NULL, "SpriteSVG::erase() : current_camera is null");

    float x_camera = current_camera->_tmp_limited_offseted_global_position.x;
    float y_camera = current_camera->_tmp_limited_offseted_global_position.y;
    float res_x_1 ;
    float res_y_1 ;
    float res_x_2 ;
    float res_y_2 ;
    float res_zoom_x_1;
    float res_zoom_y_1;
    float res_zoom_x_2;
    float res_zoom_y_2;
    Vector2 zoom = current_camera->zoom;

    float sX = this_SpriteSVG->scale.x;
    float sY = this_SpriteSVG->scale.y;
    Deque* paths = &(this_SpriteSVG->paths);
    int nb_paths = iCluige.iDeque.size(paths);
    for(int i=0; i<nb_paths; i++)
    {
        Path2D* path_i = iCluige.iDeque.at(paths, i).ptr;
        if(path_i->_visible)
        {
            int nb_points = iCluige.iPath2D.size(path_i);
            for(int j=0; j<(nb_points-1); j++)
            {
                Vector2* p1 = iCluige.iPath2D.at(path_i, j);
                Vector2* p2 = iCluige.iPath2D.at(path_i, j+1);

                //Apply zoom + center on camera
                res_zoom_x_1 = (((orig.x + ((p1->x) * sX)) - x_camera)*zoom.x);
                res_zoom_y_1 = (((orig.y + ((p1->y) * sY)) - y_camera)*zoom.y);
                res_zoom_x_2 = (((orig.x + ((p2->x) * sX)) - x_camera)*zoom.x);
                res_zoom_y_2 = (((orig.y + ((p2->y) * sY)) - y_camera)*zoom.y);

                if(!iCluige.iCamera2D.current_camera->ignore_rotation)
                {
                    //float rotation_angle = -iCluige.iCamera2D.current_camera->rotation;
                    float cf =  current_camera->global_tmp_cos_rotation;
                    float sf = current_camera->global_tmp_sin_rotation;

                    if(current_camera->anchor_mode == ANCHOR_MODE_DRAG_CENTER)//rotation around center of screen (camera in center)
                    {
                        float drag_center_offset_x = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_X;
                        float drag_center_offset_y = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_Y;

                        res_x_1 = (res_zoom_x_1 - drag_center_offset_x)  * cf - (res_zoom_y_1 - drag_center_offset_y)  * sf;
                        res_y_1 = (res_zoom_x_1 - drag_center_offset_x)  * sf + (res_zoom_y_1 - drag_center_offset_y)  * cf;

                        res_x_2 = (res_zoom_x_2 - drag_center_offset_x)  * cf - (res_zoom_y_2 - drag_center_offset_y)  * sf;
                        res_y_2 = (res_zoom_x_2 - drag_center_offset_x)  * sf + (res_zoom_y_2 - drag_center_offset_y)  * cf;

                        res_x_1 += drag_center_offset_x;
                        res_y_1 += drag_center_offset_y;
                        res_x_2 += drag_center_offset_x;
                        res_y_2 += drag_center_offset_y;
                    }
                    else
                    {
                        res_x_1 = res_zoom_x_1  * cf - res_zoom_y_1  * sf;
                        res_y_1 = res_zoom_x_1  * sf + res_zoom_y_1  * cf;
                        res_x_2 = res_zoom_x_2  * cf - res_zoom_y_2  * sf;
                        res_y_2 = res_zoom_x_2  * sf + res_zoom_y_2  * cf;
                    }
                }
                else
                {
                    res_x_1 = res_zoom_x_1;
                    res_y_1 = res_zoom_y_1;
                    res_x_2 = res_zoom_x_2;
                    res_y_2 = res_zoom_y_2;
                }

                ssvg_draw_line(
                    res_x_1,
                    res_y_1,
                    res_x_2,
                    res_y_2,
                false);
            }
        }
    }
}

static void ssvg_post_process(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    if(!(this_Node2D->visible))
    {
        return;
    }
    SpriteSVG* this_SpriteSVG = (SpriteSVG*)(this_Node2D->_sub_class);
    //call super()
    this_SpriteSVG->_post_process_super(this_Node);

    //clear old one (unless immobile? => no, because other masking things
    //could have moved and made this sprite visible again;
    //and curses already has characters cache)
    Vector2 orig;
    iCluige.iVector2.add(
            &(this_Node2D->_tmp_global_position),
            &(this_SpriteSVG->offset),
            &orig);

    Camera2D* current_camera = iCluige.iCamera2D.current_camera;
    CLUIGE_ASSERT(current_camera != NULL, "SpriteSVG::post_process_Node() : current_camera is null");

    float x_camera = current_camera->_tmp_limited_offseted_global_position.x;
    float y_camera = current_camera->_tmp_limited_offseted_global_position.y;
    float res_x_1 ;
    float res_y_1 ;
    float res_x_2 ;
    float res_y_2 ;
    float res_zoom_x_1;
    float res_zoom_y_1;
    float res_zoom_x_2;
    float res_zoom_y_2;
    Vector2 zoom = current_camera->zoom;

    float sX = this_SpriteSVG->scale.x;
    float sY = this_SpriteSVG->scale.y;
    Deque* paths = &(this_SpriteSVG->paths);
    int nb_paths = iCluige.iDeque.size(paths);
    for(int i=0; i<nb_paths; i++)
    {
        Path2D* path_i = iCluige.iDeque.at(paths, i).ptr;
        if(path_i->_visible)
        {
            int nb_points = iCluige.iPath2D.size(path_i);
            for(int j=0; j<(nb_points-1); j++)
            {
                Vector2* p1 = iCluige.iPath2D.at(path_i, j);
                Vector2* p2 = iCluige.iPath2D.at(path_i, j+1);

                //Apply zoom + center on camera
                res_zoom_x_1 = (((orig.x + ((p1->x) * sX)) - x_camera)*zoom.x);
                res_zoom_y_1 = (((orig.y + ((p1->y) * sY)) - y_camera)*zoom.y);
                res_zoom_x_2 = (((orig.x + ((p2->x) * sX)) - x_camera)*zoom.x);
                res_zoom_y_2 = (((orig.y + ((p2->y) * sY)) - y_camera)*zoom.y);

                if(!current_camera->ignore_rotation)
                {
                    //float rotation_angle = -iCluige.iCamera2D.current_camera->rotation;
                    float cf =  current_camera->global_tmp_cos_rotation;
                    float sf = current_camera->global_tmp_sin_rotation;

                    if(current_camera->anchor_mode == ANCHOR_MODE_DRAG_CENTER)//rotation around center of screen (camera in center)
                    {
                        float drag_center_offset_x = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_X;
                        float drag_center_offset_y = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_Y;

                        res_x_1 = (res_zoom_x_1 - drag_center_offset_x)  * cf - (res_zoom_y_1 - drag_center_offset_y)  * sf;
                        res_y_1 = (res_zoom_x_1 - drag_center_offset_x)  * sf + (res_zoom_y_1 - drag_center_offset_y)  * cf;

                        res_x_2 = (res_zoom_x_2 - drag_center_offset_x)  * cf - (res_zoom_y_2 - drag_center_offset_y)  * sf;
                        res_y_2 = (res_zoom_x_2 - drag_center_offset_x)  * sf + (res_zoom_y_2 - drag_center_offset_y)  * cf;

                        res_x_1 += drag_center_offset_x;
                        res_y_1 += drag_center_offset_y;
                        res_x_2 += drag_center_offset_x;
                        res_y_2 += drag_center_offset_y;
                    }
                    else
                    {
                        res_x_1 = res_zoom_x_1  * cf - res_zoom_y_1  * sf;
                        res_y_1 = res_zoom_x_1  * sf + res_zoom_y_1  * cf;
                        res_x_2 = res_zoom_x_2  * cf - res_zoom_y_2  * sf;
                        res_y_2 = res_zoom_x_2  * sf + res_zoom_y_2  * cf;
                    }
                }
                else
                {
                    res_x_1 = res_zoom_x_1;
                    res_y_1 = res_zoom_y_1;
                    res_x_2 = res_zoom_x_2;
                    res_y_2 = res_zoom_y_2;
                }

                ssvg_draw_line(
                    res_x_1,
                    res_y_1,
                    res_x_2,
                    res_y_2,
                true);
            }
        }
    }
}

////////////////////////////////// iiSpriteSVG /////////

static SpriteSVG* ssvg_new_SpriteSVG_from_Node2D(Node2D* new_Node2D)
{
	Node* new_Node = new_Node2D->_this_Node;
    SpriteSVG* new_SpriteSVG = iCluige.checked_malloc(sizeof(SpriteSVG));

    new_SpriteSVG->offset = (Vector2) { 0., 0. };
    new_SpriteSVG->scale = (Vector2) { 1., 1. };
    iCluige.iDeque.deque_alloc(&(new_SpriteSVG->paths), VT_POINTER, 3);

	new_SpriteSVG->_this_Node2D = new_Node2D;
	new_SpriteSVG->_sub_class = NULL;

	//virtual methods - private copies of mother class pointers
	//	constructors of all ancestors in inheritance hierarchy
	//	have already been called, so :
	//	new_Node virtual methods pointers are already pointing to
	//	overriding methods (if any)
	new_SpriteSVG->_delete_super = new_Node->delete_Node;
	new_SpriteSVG->_erase_super = new_Node->erase;
	new_SpriteSVG->_post_process_super = new_Node->post_process;

    new_Node2D->_sub_class = new_SpriteSVG;

    free(new_Node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_Node->_class_name = iCluige.iStringBuilder.string_alloc(
            &sb, strlen("NodeNode2DSpriteSVG"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2DSpriteSVG");
    new_Node2D->_sub_class = new_SpriteSVG;

    new_Node->delete_Node = ssvg_delete_SpriteSVG;
    new_Node->erase = ssvg_erase;
    new_Node->post_process = ssvg_post_process;

    return new_SpriteSVG;
}

static SpriteSVG* ssvg_new_SpriteSVG()
{
	Node2D* new_Node2D = iCluige.iNode2D.new_Node2D();
	return ssvg_new_SpriteSVG_from_Node2D(new_Node2D);
}

static void ssvg_add_path_from_array(SpriteSVG* this_SpriteSVG, Vector2* points, int nb_points)
{
    Path2D* new_path = iCluige.checked_malloc(sizeof(Path2D));
    iCluige.iPath2D.path2D_alloc(new_path, points, nb_points, true);
    iCluige.iDeque.push_back(&(this_SpriteSVG->paths), new_path);
}

static void ssvg_add_path_from_array_relative(SpriteSVG* this_SpriteSVG, Vector2* points, int nb_points)
{
    Path2D* new_path = iCluige.checked_malloc(sizeof(Path2D));
    iCluige.iPath2D.path2D_alloc(new_path, points, nb_points, false);
    iCluige.iDeque.push_back(&(this_SpriteSVG->paths), new_path);
}

static void ssvg_add_path_from_parsed_deque(SpriteSVG* this_SpriteSVG, Deque* coordinates_sequence)
{
    if(iCluige.iDeque.size(coordinates_sequence) > 0)
    {
        Path2D* new_path = iCluige.checked_malloc(sizeof(Path2D));
        iCluige.iPath2D.path2D_alloc_from_parsed(new_path, coordinates_sequence);
        iCluige.iDeque.push_back(&(this_SpriteSVG->paths), new_path);
    }
}

static void ssvg_parse_file(SpriteSVG* this_SpriteSVG, char* file_path)
{
    iCluige.iSpriteSVG.iSVGParser.prepare_parsing(
            &_svg_parser, file_path);

    while(iCluige.iSpriteSVG.iSVGParser.parse_path(&_svg_parser))
    {
        iCluige.iSpriteSVG.add_path_from_parsed_deque(this_SpriteSVG, &(_svg_parser.coordinates_sequence));
    }

    iCluige.iSpriteSVG.iSVGParser.end_parsing(&_svg_parser);
}

static Node* ssvg_instanciate(const SortedDictionary* params)
{
    //mother class
    Node* res_node = iCluige.iNode2D._Node2D_factory.instanciate(params);
    Node2D* res_node2D = (Node2D*)(res_node->_sub_class);
    SpriteSVG* res_SpriteSVG = ssvg_new_SpriteSVG_from_Node2D(res_node2D);

    //offset = Vector2(2.265, -3.2)
    //scale = Vector2(2.265, -3.2)
    //svg_file_path = "../some/where"
    utils_vector2_from_parsed(&(res_SpriteSVG->offset), params, "offset");
    utils_vector2_from_parsed(&(res_SpriteSVG->scale), params, "scale");
    CLUIGE_ASSERT(iCluige.iDeque.empty(&(res_SpriteSVG->paths)), "SpriteSVG::instanciate() : trying to instanciate() into non empty object");
    char* svg_file_path;
    //'texture'=>'path/to/file.svg' see TscnParser::node()
    bool ok = utils_nonquoted_str_from_parsed(&svg_file_path, params, "svg_file_path");
//	char* dbg = iCluige.iSortedDictionary.debug_str_str(params);
//	utils_breakpoint_trick(dbg, !ok);
    utils_breakpoint_trick(&ok, !ok);
    CLUIGE_ASSERT(ok, "SpriteSVG::instanciate() : missing 'texture' field");
    ssvg_parse_file(res_SpriteSVG, svg_file_path);
    //free(svg_file_path);no need with utils_nonquoted_str_from_parsed()
    return res_node;
}

/////////////////////////////////// Node //////////

void iiSpriteSVG_init()
{
    iCluige.iSpriteSVG.new_SpriteSVG = ssvg_new_SpriteSVG;
    iCluige.iSpriteSVG.new_SpriteSVG_from_Node2D = ssvg_new_SpriteSVG_from_Node2D;
    iCluige.iSpriteSVG.add_path_from_array = ssvg_add_path_from_array;
    iCluige.iSpriteSVG.add_path_from_array_relative = ssvg_add_path_from_array_relative;
    iCluige.iSpriteSVG.add_path_from_parsed_deque = ssvg_add_path_from_parsed_deque;
    iCluige.iSpriteSVG.parse_file = ssvg_parse_file;
    SortedDictionary* fcties = &(iCluige.iNode.node_factories);
    NodeFactory* fcty = &(iCluige.iSpriteSVG._SpriteSVG_factory);
    fcty->instanciate = ssvg_instanciate;
    char* fcty_key = iCluige.checked_malloc(9 * sizeof(char));
    strncpy(fcty_key, "Sprite2D", 9);
    iCluige.iSortedDictionary.insert(fcties, fcty_key, fcty);

    iiSVGParser_init(&(iCluige.iSpriteSVG.iSVGParser));
    iCluige.iSpriteSVG.iSVGParser.SVGParser_alloc(&_svg_parser);
}

