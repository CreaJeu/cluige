#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "../Deque.h"

struct _InputAction
{
    char* name;
    int nb_pressed;
//    bool just_pressed;//tmp lists instead
//    bool just_released;
};

struct _InputKey
{
    int unicode_char;
    bool pressed;
//    bool just_pressed;//tmp lists instead
//    bool just_released;
    Deque bound_actions;//Deque<int32_t>
};

struct _Input
{
    Deque available_actions;//Deque<InputAction>
    Deque bound_keys;//Deque<InputKey>

    Deque just_pressed_actions;//Deque<int32_t>
    Deque just_released_actions;//Deque<int32_t>
    Deque just_pressed_keys;//Deque<InputKey>
    Deque just_released_keys;//Deque<InputKey>

	//private
	Node* _thisNode;

	//virtual methods
	void (*deleteNode)(Node*);
	//in .c : void (*deleteInput)(Node*);
	//in .c : void (*preProcessInput)(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiInput
{
	//Input* newInput()
	struct _Input* (*newInput)();
//	struct _InputAction* (*newAction)(const char* name);
//	struct _InputKey* (*newKey)(int charVal);
	int (*add_action)(struct _Input* thisInput, char* action_name);
	void (*bind_key)(struct _Input* thisInput, int action_id, int keyCharVal);
//	bool (*is_action_pressed)(const struct _Input* thisInput, int action_id);
	bool (*is_action_just_pressed)(const struct _Input* thisInput, int action_id);
//	bool (*is_key_pressed)(const struct _Input* thisInput, int charVal);
//	bool (*is_key_just_pressed)(const struct _Input* thisInput, int charVal);
};
//iInput : in iiCluige

//to be called only by cluigeInit() to set iNode functions pointers
void iiInputInit();


#endif // INPUT_H_INCLUDED
