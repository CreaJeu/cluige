#ifndef SPRITE_SVG_H_INCLUDED
#define SPRITE_SVG_H_INCLUDED

#include "../Vector2.h"
#include "../Deque.h"

typedef struct _SpriteSVG SpriteSVG;

struct _SpriteSVG
{
    //top left corner of svg will be drawn at
    //Node2D.position - offset
    Vector2 offset;

    Deque paths;//all paths of the svg Deque<Deque<Vector2*>* >

	//private
	Node2D* _thisNode2D;
	void* _subClass;

	//virtual methods
	void (*deleteNode2D)(Node*);
	void (*preProcessNode2D)(Node*);
	void (*postProcessNode2D)(Node*);
	//in .c : void preProcessNode(Node*);
	//in .c : void postProcessNode(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiSpriteSVG
{
	//SpriteSVG* newSpriteSVG()
	SpriteSVG* (*newSpriteSVG)();

	//copies the points from the array
	void (*add_path_from_array)(SpriteSVG*, Vector2*, int nbPoints);
};
//iSpriteSVG : in iiCluige

//to be called only by cluigeInit() to set iNode functions pointers
void iiSpriteSVGInit();


#endif // SPRITE_SVG_H_INCLUDED
