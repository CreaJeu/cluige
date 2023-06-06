#ifndef SPRITE_TEXT_H_INCLUDED
#define SPRITE_TEXT_H_INCLUDED

#include "../Vector2.h"

typedef struct _SpriteText SpriteText;

struct _SpriteText
{
    //first character of first line will be drawn at
    //Node2D.position - offset
    Vector2 offset;

    //char** text;//array of text lines, the ASCII art / unicode art
    char* text;//text with all \n replaced by \0, the ASCII art / unicode art
    int nb_lines;//TODO collection instead

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
struct iiSpriteText
{
	//SpriteText* new_SpriteText()
	SpriteText* (*new_SpriteText)();

	//void set_text(const char*)
	void (*set_text)(SpriteText*, const char*);
};
//iSpriteText : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiSpriteText_init();


#endif // SPRITE_TEXT_H_INCLUDED
