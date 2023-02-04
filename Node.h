#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <stdbool.h>

typedef struct _Node Node;
typedef struct _Script Script;

struct _Node
{
	// ... position/transform
	Node* parent;
	Node* nextSibling;
	Node* children;
	Script* script;
	char* name;//must not point to the stack, see setName()
	bool active;

	//TODO : get child by name, get node by path, _draw, queue_free

	//for inheritance
	char* _className;
	void* _subClass;

	//virtual methods
	void (*deleteNode)(Node*);
	void (*preProcessNode)(Node*);
	void (*processNode)(Node*);
	void (*postProcessNode)(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiNode
{
    //void initZero(Node* node)
//	void (*initZero)(Node*);// no : would encourage not freeable nodes on the stack

	//Node* newNode()
	Node* (*newNode)();

	//void deleteNode(Node*)
	//void (*deleteNode)(Node*); //see struct _Node (virtual method)

	//int getIndex(Node*)
	int (*getIndex)(const Node*);

	//int getDepth(Node*)
	int (*getDepth)(const Node*);

	//use setName(n, "bob") instead of n->name = "bob"
	//previous name is automatically freed
	//void setName(Node*, char* newName)
	void (*setName)(Node*, const char* newName);

	//asserts that wanted child doesn't have already a parent
	//void addChild(Node* p, Node* c)
	void (*addChild)(Node* p, Node* c);

	//void printTreePretty(Node*)
	void (*printTreePretty)(const Node*);

	//TODO removeChild()
	//TODO queueFree()
};
//iNode : in iiCluige

//to be called only by cluigeInit() to set iNode functions pointers
void iiNodeInit();


#endif // NODE_H_INCLUDED
