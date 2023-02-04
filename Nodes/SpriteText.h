#ifndef SPRITE_TEXT_H_INCLUDED
#define SPRITE_TEXT_H_INCLUDED

typedef struct _SpriteText SpriteText;

struct _SpriteText
{
    char* text;//the ASCII art / unicode art

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
struct iiSpriteText
{
	//SpriteText* newSpriteText()
	SpriteText* (*newSpriteText)();

	//void setText(const char*)
	void (*setText)(SpriteText*, const char*);
};
//iSpriteText : in iiCluige

//to be called only by cluigeInit() to set iNode functions pointers
void iiSpriteTextInit();


#endif // SPRITE_TEXT_H_INCLUDED
