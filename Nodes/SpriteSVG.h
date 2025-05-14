#ifndef SPRITE_SVG_H_INCLUDED
#define SPRITE_SVG_H_INCLUDED

#include "../Vector2.h"
#include "../Deque.h"
#include "../SVGParser.h"

typedef struct _SpriteSVG SpriteSVG;

struct _SpriteSVG
{
    //top left corner of svg will be drawn at
    //Node2D.position - offset
    Vector2 offset;

    Vector2 scale;//tmp before general Node2D scale

    Deque paths;//Deque<Path2D* > all paths of the svg

	//private
	struct
	{
		bool state_changed;
	} _state_changes;

	//private
	Node2D* _this_Node2D;
	void* _sub_class;

	//virtual methods - private copies of mother class pointers
	void (*_delete_super)(Node*);
	void (*_erase_super)(Node*);
	void (*_post_process_super)(Node*);

	//virtual methods in .c :
	//void erase_Node(Node*);
	//void post_process_Node(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiSpriteSVG
{
	// ~private static
	NodeFactory _SpriteSVG_factory;

    //too specific to be in iCluige
    struct iiSVGParser iSVGParser;

	//SpriteSVG* new_SpriteSVG()
	SpriteSVG* (*new_SpriteSVG)();
	SpriteSVG* (*new_SpriteSVG_from_Node2D)();

	//copies the points from the array
	void (*add_path_from_array)(SpriteSVG*, Vector2*, int nb_points);
	//copies the points from the array
	void (*add_path_from_array_relative)(SpriteSVG*, Vector2*, int nb_points);
	//copies the points from the deque [x y x y x y...]
	void (*add_path_from_parsed_deque)(SpriteSVG*, Deque*);

	void (*parse_file)(SpriteSVG*, char* file_path);
};
//iSpriteSVG : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiSpriteSVG_init();


#endif // SPRITE_SVG_H_INCLUDED
