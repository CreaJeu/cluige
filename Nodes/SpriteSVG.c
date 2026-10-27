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

	CLUIGE_ASSERT(this_SpriteSVG->_sub_class == NULL, "SpriteSVG::delete_SpriteSVG() : not null subclass found");
	free(this_SpriteSVG);
	this_Node2D->_sub_class = NULL;
	delete_super(this_Node);
}

static void ssvg_bake(Node* this_Node)
{
	Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
	SpriteSVG* this_SpriteSVG = (SpriteSVG*)(this_Node2D->_sub_class);

	this_SpriteSVG->_bake_super(this_Node);
	if(!(this_Node2D->_new_baked.visible))
	{
		return;
	}

	//backup old baked data
	memcpy(&(this_SpriteSVG->_old_baked), &(this_SpriteSVG->_new_baked),
		sizeof(this_SpriteSVG->_new_baked));
	//memcpy instead of =
	//so we can later use memcmp
	//whatever alignment bits might be inside

	this_SpriteSVG->_new_baked.cos_rot = cosf(this_SpriteSVG->rotation);
	this_SpriteSVG->_new_baked.sin_rot = sinf(this_SpriteSVG->rotation);

	//bake paths : not here (see draw()) because needs camera::bake()
	//which might be executed later, according to
	//relative position between camera node and this node
	//in scene tree
}

//utility function
static void ssvg_draw_line(const Vector2* start_pos, const Vector2* end_pos, bool draw)
{
	LineDrawerThin d;
	iCluige.iLineDrawerThin.start(&d, start_pos, end_pos);
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

	bool must_erase = this_Node2D->_old_baked.visible;//was visible last time
	must_erase = must_erase &&
		(
			this_Node2D->_position_changed ||
			(!(this_Node2D->_new_baked.visible)) ||
			iCluige.iCamera2D.state_changed ||
			this_Node->_marked_for_queue_free ||
			memcmp(&(this_SpriteSVG->_old_baked),
					&(this_SpriteSVG->_new_baked),
					sizeof (this_SpriteSVG->_old_baked)) != 0
		);
	if(!must_erase)
	{
//		this_SpriteSVG->_erase_super(this_Node);//NULL
		return;
	}

	Deque* paths = &(this_SpriteSVG->paths);
	int nb_paths = iCluige.iDeque.size(paths);
	for(int i=0; i<nb_paths; i++)
	{
		Path2D* path_i = iCluige.iDeque.at(paths, i).ptr;

		//trick instead of copying all elems one by one
		Deque tmp_old_baked_points_light_copy = path_i->_old_baked._baked_points;
		path_i->_old_baked._baked_points = path_i->_new_baked._baked_points;
		path_i->_new_baked._baked_points = tmp_old_baked_points_light_copy;

		if(path_i->_visible)
		{
			//erase old
			Deque* old_baked_points = &(path_i->_old_baked._baked_points);
			int nb_old_points = iCluige.iDeque.size(old_baked_points);
			for(int j=1; j<nb_old_points; j++)
			{
				Vector2* p1 = (Vector2*)(iCluige.iDeque.at(old_baked_points, j-1).ptr);
				Vector2* p2 = (Vector2*)(iCluige.iDeque.at(old_baked_points, j).ptr);
				ssvg_draw_line(p1, p2, false);
			}
		}
	}
}

static void ssvg_draw(Node* this_Node)
{
	Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
	SpriteSVG* this_SpriteSVG = (SpriteSVG*)(this_Node2D->_sub_class);
	this_SpriteSVG->_draw_super(this_Node);
	if(!(this_Node2D->_new_baked.visible))
	{
		return;
	}

	Vector2 orig;
	iCluige.iVector2.add(
			&(this_Node2D->_new_baked.tmp_global_position),
			&(this_SpriteSVG->offset),
			&orig);
	iCluige.iVector2.add(
			&orig,
			&(this_SpriteSVG->_centered_offest),
			&orig);

	//Camera2D* current_camera = iCluige.iCamera2D.current_camera;
	//CLUIGE_ASSERT(current_camera != NULL, "SpriteSVG::draw() : current_camera is null");

	float x_camera = iCluige.iCamera2D._new_baked._tmp_limited_offseted_global_position.x;
	float y_camera = iCluige.iCamera2D._new_baked._tmp_limited_offseted_global_position.y;
	float res_zoom_x;
	float res_zoom_y;
	Vector2 zoom = iCluige.iCamera2D._new_baked.zoom;

	float c_r = this_SpriteSVG->_new_baked.cos_rot;
	float s_r = this_SpriteSVG->_new_baked.sin_rot;
	Vector2 p2_copy_rotated;

	float sX = this_SpriteSVG->scale.x;
	float sY = this_SpriteSVG->scale.y;
	float half_width = this_SpriteSVG->size_svg.x / 2;
	float half_height = this_SpriteSVG->size_svg.y / 2;

	Deque* paths = &(this_SpriteSVG->paths);
	int nb_paths = iCluige.iDeque.size(paths);
	for(int i=0; i<nb_paths; i++)
	{
		Path2D* path_i = iCluige.iDeque.at(paths, i).ptr;
		if(path_i->_visible)
		{
			//bake new (done here instead of bake() because
			//needs camera::bake()
			//which might be executed later, according to
			//relative position between camera node and this node
			//in scene tree)
			int nb_points = iCluige.iPath2D.size(path_i);
			Deque* new_baked_points = &(path_i->_new_baked._baked_points);
			float res_x = -1;
			float res_y = -1;
			for(int j=0; j<nb_points; j++)
			{
				Vector2 p2_copy = *(iCluige.iPath2D.at(path_i, j));
				//change of reference point because
				// godot sprite "center" is not at (0,0) in .svg
				p2_copy.x -= half_width;
				p2_copy.y -= half_height;
				p2_copy_rotated = p2_copy;//copy of the copy

				p2_copy_rotated.x = p2_copy.x * c_r - p2_copy.y * s_r;
				p2_copy_rotated.y = p2_copy.x * s_r + p2_copy.y * c_r;

				//Apply zoom + center on camera
				res_zoom_x = (((orig.x + ((p2_copy_rotated.x) * sX)) + half_width - x_camera)*zoom.x);
				res_zoom_y = (((orig.y + ((p2_copy_rotated.y) * sY)) + half_height - y_camera)*zoom.y);

				if(!iCluige.iCamera2D._new_baked.ignore_rotation)
				{
					//float rotation_angle = -iCluige.iCamera2D.current_camera->rotation;
					float cf =  iCluige.iCamera2D._new_baked.global_tmp_cos_rotation;
					float sf = iCluige.iCamera2D._new_baked.global_tmp_sin_rotation;

					if(iCluige.iCamera2D._new_baked.anchor_mode == ANCHOR_MODE_DRAG_CENTER)//rotation around center of screen (camera in center)
					{
						float drag_center_offset_x = iCluige.iCamera2D._new_baked._screen_anchor_center_x;
						float drag_center_offset_y = iCluige.iCamera2D._new_baked._screen_anchor_center_y;

						res_x = (res_zoom_x - drag_center_offset_x)  * cf - (res_zoom_y - drag_center_offset_y)  * sf;
						res_y = (res_zoom_x - drag_center_offset_x)  * sf + (res_zoom_y - drag_center_offset_y)  * cf;

						res_x += drag_center_offset_x;
						res_y += drag_center_offset_y;
					}
					else
					{
						res_x = res_zoom_x  * cf - res_zoom_y  * sf;
						res_y = res_zoom_x  * sf + res_zoom_y  * cf;
					}
				}
				else
				{
					res_x = res_zoom_x;
					res_y = res_zoom_y;
				}

				Vector2* baked_point = (Vector2*)(iCluige.iDeque.at(new_baked_points, j).ptr);
				baked_point->x = res_x;
				baked_point->y = res_y;
			}

//			int nb_points = iCluige.iPath2D.size(path_i);
//			Deque* baked_points = &(path_i->_new_baked._baked_points);
			for(int j=1; j<nb_points; j++)
			{
				Vector2* p1 = (Vector2*)(iCluige.iDeque.at(new_baked_points, j-1).ptr);
				Vector2* p2 = (Vector2*)(iCluige.iDeque.at(new_baked_points, j).ptr);
				ssvg_draw_line(p1, p2, true);
			}
		}
	}
}

////////////////////////////////// iiSpriteSVG /////////

static SpriteSVG* ssvg_new_SpriteSVG_from_Node2D(Node2D* new_Node2D)
{
	Node* new_Node = new_Node2D->_this_Node;
	SpriteSVG* new_SpriteSVG = iCluige.checked_malloc(sizeof(SpriteSVG));

	new_SpriteSVG->centered = true;
	new_SpriteSVG->_centered_offest = (Vector2) { 0., 0. };
	new_SpriteSVG->offset = (Vector2) { 0., 0. };
	new_SpriteSVG->scale = (Vector2) { 1., 1. };
	new_SpriteSVG->rotation = 0;
	new_SpriteSVG->_old_baked.cos_rot = 1;
	new_SpriteSVG->_old_baked.sin_rot = 0;
	new_SpriteSVG->_new_baked.cos_rot = 1;
	new_SpriteSVG->_new_baked.sin_rot = 0;
	iCluige.iDeque.deque_alloc(&(new_SpriteSVG->paths), VT_POINTER, 3);

	new_SpriteSVG->_this_Node2D = new_Node2D;
	new_SpriteSVG->_sub_class = NULL;

	//virtual methods - private copies of mother class pointers
	//	constructors of all ancestors in inheritance hierarchy
	//	have already been called, so :
	//	new_Node virtual methods pointers are already pointing to
	//	overriding methods (if any)
	new_SpriteSVG->_delete_super = new_Node->delete_Node;
	new_SpriteSVG->_bake_super = new_Node->bake;
	new_SpriteSVG->_draw_super = new_Node->draw;
//	new_SpriteSVG->_erase_super = new_Node->erase;
//	new_SpriteSVG->_post_process_super = new_Node->post_process;

	new_Node2D->_sub_class = new_SpriteSVG;

	free(new_Node->_class_name); //TODO static value to avoid free
	StringBuilder sb;
	new_Node->_class_name = iCluige.iStringBuilder.string_alloc(
			&sb, 1 + strlen("NodeNode2DSpriteSVG"));
	iCluige.iStringBuilder.append(&sb, "NodeNode2DSpriteSVG");
	new_Node2D->_sub_class = new_SpriteSVG;

	new_Node->delete_Node = ssvg_delete_SpriteSVG;
	new_Node->erase = ssvg_erase;
	new_Node->bake = ssvg_bake;
	new_Node->draw = ssvg_draw;

	return new_SpriteSVG;
}

static SpriteSVG* ssvg_new_SpriteSVG()
{
	Node2D* new_Node2D = iCluige.iNode2D.new_Node2D();
	return ssvg_new_SpriteSVG_from_Node2D(new_Node2D);
}

static void ssvg_rotate(SpriteSVG* this_SpriteSVG, float radians)
{
	this_SpriteSVG->rotation += radians;
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
	bool ok = iCluige.iSpriteSVG.iSVGParser.parse_svg_tag(&_svg_parser);
	if(!ok)
	{
		CLUIGE_BREAKPOINT("SpriteSVG::parse_file() incorrect <svg> attributes", true);
	}
	this_SpriteSVG->size_svg.x = _svg_parser.width;
	this_SpriteSVG->size_svg.y = _svg_parser.height;

	while(iCluige.iSpriteSVG.iSVGParser.parse_path(&_svg_parser))
	{
		iCluige.iSpriteSVG.add_path_from_parsed_deque(this_SpriteSVG, &(_svg_parser.coordinates_sequence));
	}

	iCluige.iSpriteSVG.iSVGParser.end_parsing(&_svg_parser);

	if(this_SpriteSVG->centered)
	{
		iCluige.iVector2.k_mul(&(this_SpriteSVG->size_svg), -.5, &(this_SpriteSVG->_centered_offest));
	}
}

static Node* ssvg_instantiate(const SortedDictionary* params)
{
	//mother class
	Node* res_node = iCluige.iNode2D._Node2D_factory.instantiate(params);
	Node2D* res_node2D = (Node2D*)(res_node->_sub_class);
	SpriteSVG* res_SpriteSVG = ssvg_new_SpriteSVG_from_Node2D(res_node2D);

	//offset = Vector2(2.265, -3.2)
	//scale = Vector2(2.265, -3.2)
	//svg_file_path = "../some/where"
	utils_bool_from_parsed(&(res_SpriteSVG->centered), params, "centered");
	utils_vector2_from_parsed(&(res_SpriteSVG->offset), params, "offset");
	utils_vector2_from_parsed(&(res_SpriteSVG->scale), params, "scale");//TODO scale in Node2D instead
	utils_float_from_parsed(&(res_SpriteSVG->rotation), params, "rotation");//TODO tmp, Node2D instead
	//res_SpriteSVG->rotation *= iCluige.PI / 180;//already in radians in .tscn
	CLUIGE_ASSERT(iCluige.iDeque.empty(&(res_SpriteSVG->paths)), "SpriteSVG::instantiate() : trying to instantiate() into non empty object");
	char* svg_file_path;
	//'texture'=>'path/to/file.svg' see TscnParser::node()
	bool ok = utils_nonquoted_str_from_parsed(&svg_file_path, params, "svg_file_path");
//	char* dbg = iCluige.iSortedDictionary.debug_str_str(params);
//	CLUIGE_BREAKPOINT(dbg, !ok);
	CLUIGE_ASSERT(ok, "SpriteSVG::instantiate() : missing 'texture' field");
	ssvg_parse_file(res_SpriteSVG, svg_file_path);
	//free(svg_file_path);no need with utils_nonquoted_str_from_parsed()
	return res_node;
}

static SpriteSVG* ssvg_cast_from_Node(Node* this_Node)
{
	Node2D* n2D = iCluige.iNode2D.cast_from_Node(this_Node);
	CLUIGE_ASSERT(strncmp(this_Node->_class_name, "NodeNode2DSpriteSVG", strlen("NodeNode2DSpriteSVG")) == 0,
			"SpriteSVG::cast_from_Node() : node '%s' is not an instance SpriteSVG",
			this_Node->name);
	return (SpriteSVG*)(n2D->_sub_class);
}

/////////////////////////////////// Node //////////

void iiSpriteSVG_init()
{
	iCluige.iSpriteSVG.new_SpriteSVG = ssvg_new_SpriteSVG;
	iCluige.iSpriteSVG.new_SpriteSVG_from_Node2D = ssvg_new_SpriteSVG_from_Node2D;
	iCluige.iSpriteSVG.rotate = ssvg_rotate;
	iCluige.iSpriteSVG.add_path_from_array = ssvg_add_path_from_array;
	iCluige.iSpriteSVG.add_path_from_array_relative = ssvg_add_path_from_array_relative;
	iCluige.iSpriteSVG.add_path_from_parsed_deque = ssvg_add_path_from_parsed_deque;
	iCluige.iSpriteSVG.parse_file = ssvg_parse_file;
	iCluige.iSpriteSVG.cast_from_Node = ssvg_cast_from_Node;

	iCluige.iSpriteSVG._SpriteSVG_factory.instantiate = ssvg_instantiate;
	iCluige.iNode.register_NodeFactory("Sprite2D", &(iCluige.iSpriteSVG._SpriteSVG_factory));

	iiSVGParser_init(&(iCluige.iSpriteSVG.iSVGParser));
	iCluige.iSpriteSVG.iSVGParser.SVGParser_alloc(&_svg_parser);
}

