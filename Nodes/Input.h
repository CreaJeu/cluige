#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "../Deque.h"

//"keys" may be:
// - characters like 'a' or ' '
// - other buttons represented by KEY_... constants from curses.h
//   (KEY_LEFT, KEY_RIGHT, KEY_ENTER, KEY_END...)
//Those are values returned by curses getch()

struct _InputAction
{
    char* name;//usefull apart from debug?
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
	Node* _this_Node;

	//virtual methods - private copies of mother class pointers
	void (*_delete_super)(Node*);

	//virtual methods in .c :
	//void (*delete_Input)(Node*);
	//void (*process)(Node*);
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiInput
{
	//Input* new_Input()
	struct _Input* (*new_Input)();
//	struct _InputAction* (*new_Action)(const char* name);
//	struct _InputKey* (*newKey)(int charVal);

    //action_name will be copied, can be freed afterward or stack allocated
	int (*add_action)(char* action_name);

	void (*bind_key)(int action_id, int keyCharVal);
//	bool (*is_action_pressed)(int action_id);
	bool (*is_action_just_pressed)(int action_id);
//	bool (*is_key_pressed)(int charVal);
//	bool (*is_key_just_pressed)(int charVal);
};
//iInput : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiInput_init();


#endif // INPUT_H_INCLUDED
