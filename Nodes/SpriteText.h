#ifndef SPRITE_TEXT_H_INCLUDED
#define SPRITE_TEXT_H_INCLUDED

#include "../Vector2.h"

typedef struct _SpriteText SpriteText;

struct _SpriteText
{
	//private
	bool _text_changed;

	struct
	{
		//first character of first line will be drawn at
		//Node2D.position - offset
		Vector2 offset;

		char* text;//the ASCII art / unicode art

		int nb_chars;
		int allocated_text_length;
	} _new_baked, _old_baked;

	//private
	Node2D* _this_Node2D;
	void* _sub_class;

	//virtual methods - private copies of mother class pointers
	void (*_delete_super)(Node*);
//	void (*_enter_tree_super)(Node*);
//	void (*_erase_super)(Node*);
//	void (*_post_process_super)(Node*);

	//virtual methods in .c :
	//void erase(Node*);
	//void draw(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiSpriteText
{
	// ~private static
	NodeFactory _SpriteText_factory;

	//SpriteText* new_SpriteText()
	SpriteText* (*new_SpriteText)();
	SpriteText* (*new_SpriteText_from_Node2D)(Node2D* new_Node2D);

	//void set_text(const char*)
	void (*set_text)(SpriteText*, const char*);
};
//iSpriteText : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiSpriteText_init();


#endif // SPRITE_TEXT_H_INCLUDED
