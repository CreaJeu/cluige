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

    Deque paths;//all paths of the svg Deque<Path2D* >

	//private
	Node2D* _this_Node2D;
	void* _sub_class;

	//virtual methods
	void (*delete_Node2D)(Node*);
	void (*pre_process_Node2D)(Node*);
	void (*post_process_Node2D)(Node*);
	//in .c : void pre_process_Node(Node*);
	//in .c : void post_process_Node(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiSpriteSVG
{
    //too specific to be in iCluige
    struct iiSVGParser iSVGParser;

	//SpriteSVG* new_SpriteSVG()
	SpriteSVG* (*new_SpriteSVG)();

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
