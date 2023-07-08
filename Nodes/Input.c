
#include "../cluige.h"
#include "Input.h"

#include <string.h> //TODO static instead of malloc'ed class name
#include <curses.h> //TODO separate from cluige core via 'server'

////////////////////////////////// _Input /////////

static void inp_delete_Input(Node* this_Node)
{
    struct _Input* this_Input = (struct _Input*)(this_Node->_sub_class);
    void (*delete_Node)(Node*) = this_Input->delete_Node;

    for(int i=0; i < iCluige.iDeque.size(&(this_Input->available_actions)); i++)
    {
        Variant v = iCluige.iDeque.at(&(this_Input->available_actions), i);
        struct _InputAction* va = (struct _InputAction*)(v.ptr);
        free(va->name);
        free(v.ptr);
    }
    iCluige.iDeque.pre_delete_Deque(&(this_Input->available_actions));

    for(int i=0; i < iCluige.iDeque.size(&(this_Input->bound_keys)); i++)
    {
        Variant v = iCluige.iDeque.at(&(this_Input->bound_keys), i);
        free(v.ptr);
    }
    iCluige.iDeque.pre_delete_Deque(&(this_Input->bound_keys));

    iCluige.iDeque.pre_delete_Deque(&(this_Input->just_pressed_actions));
    iCluige.iDeque.pre_delete_Deque(&(this_Input->just_released_actions));
    iCluige.iDeque.pre_delete_Deque(&(this_Input->just_pressed_keys));
    iCluige.iDeque.pre_delete_Deque(&(this_Input->just_released_keys));

    free(this_Input);
    this_Node->_sub_class = NULL;
    delete_Node(this_Node);
}

// private utility functions
static int inp__find_key(const Deque* keys, int char_val)
{
    for(int i=0; i < iCluige.iDeque.size(keys); i++)
    {
        Variant v = iCluige.iDeque.at(keys, i);
        struct _InputKey* vk = (struct _InputKey*)(v.ptr);
        if(vk->unicode_char == char_val)
        {
            return i;
        }
    }
    return -1;
}
static int inp__find_action(const Deque* action_ids, int32_t id_action)
{
    for(int i=0; i < iCluige.iDeque.size(action_ids); i++)
    {
        int32_t candidate = iCluige.iDeque.at(action_ids, i).i32;
        if(candidate == id_action)
        {
            return i;
        }
    }
    return -1;
}

static void inp_pre_process_Input(Node* this_Node)
{
    struct _Input* this_Input = (struct _Input*)(this_Node->_sub_class);
    //reinit all "just_XXX"
    iCluige.iDeque.clear(&(this_Input->just_pressed_actions));
    iCluige.iDeque.clear(&(this_Input->just_released_actions));
    iCluige.iDeque.clear(&(this_Input->just_pressed_actions));
    iCluige.iDeque.clear(&(this_Input->just_released_keys));

    int gotch = getch();
    while(gotch != ERR)
    {
        int bound_key_index = inp__find_key(&(this_Input->bound_keys), gotch);
        if(bound_key_index != -1)
        {
            struct _InputKey* key = iCluige.iDeque.at(&(this_Input->bound_keys), bound_key_index).ptr;
//            if(!(key->pressed))
//            {
                for(int i=0; i < iCluige.iDeque.size(&(key->bound_actions)); i++)
                {
                    int32_t action_id = iCluige.iDeque.at(&(key->bound_actions), i).i32;
                    struct _InputAction* action = iCluige.iDeque.at(
                            &(this_Input->available_actions), i).ptr;
                    action->nb_pressed++;
//                    action->just_pressed = true;
                    iCluige.iDeque.push_back(
                            &(this_Input->just_pressed_actions), (int32_t)action_id);
                }
                key->pressed = true;
//                key->just_pressed = true;
                iCluige.iDeque.push_back(
                        &(this_Input->just_pressed_keys), key);
//            }
        }
        //TODO non-bound keys, for is_key_pressed()
        //TODO release events !
        gotch = getch();
    }
}


////////////////////////////////// iiInput /////////

static struct _Input* inp_new_Input()
{
    Node* new_node = iCluige.iNode.new_Node();
    struct _Input* new_input = iCluige.checked_malloc(sizeof(struct _Input));
    //...

    new_node->_sub_class = new_input;
    new_input->_this_Node = new_node;

    free(new_node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_node->_class_name = iCluige.iStringBuilder.string_alloc(&sb, strlen("NodeInput"));
    iCluige.iStringBuilder.append(&sb, "NodeInput");

    new_input->delete_Node = new_node->delete_Node;
    new_node->delete_Node = inp_delete_Input;
    new_node->pre_process_Node = inp_pre_process_Input;

    iCluige.iDeque.deque_alloc(&(new_input->available_actions), VT_POINTER, 20);
    iCluige.iDeque.deque_alloc(&(new_input->bound_keys), VT_POINTER, 40);
    ///// TODO     standard actions? //ui_accept ui_cancel ui_left ui_right ui_up ui_down

    iCluige.iDeque.deque_alloc(&(new_input->just_pressed_actions), VT_INT32, 9);
    iCluige.iDeque.deque_alloc(&(new_input->just_released_actions), VT_INT32, 9);
    iCluige.iDeque.deque_alloc(&(new_input->just_pressed_keys), VT_POINTER, 9);
    iCluige.iDeque.deque_alloc(&(new_input->just_released_keys), VT_POINTER, 9);

    return new_input;
}

static struct _InputAction* inp__new_action(char* name)
{
    struct _InputAction* new_action = iCluige.checked_malloc(sizeof(struct _InputAction));
    new_action->name = iCluige.iStringBuilder.clone(name);
    new_action->nb_pressed = 0;
//    new_action->just_pressed = false;
//    new_action->just_released = false;
    return new_action;
}

static struct _InputKey* inp__new_key(int char_val)
{
    struct _InputKey* new_key = iCluige.checked_malloc(sizeof(struct _InputKey));
    new_key->unicode_char = char_val;
    new_key->pressed = 0;
//    new_key->just_pressed = 0;
//    new_key->just_released = 0;
    iCluige.iDeque.deque_alloc(&(new_key->bound_actions), VT_INT32, 3);
    return new_key;
}

static int inp_add_action(char* action_name)
{
    int res = iCluige.iDeque.size(&(iCluige.input->available_actions));
    struct _InputAction* new_action = inp__new_action(action_name);
    iCluige.iDeque.push_back(&(iCluige.input->available_actions), new_action);
    return res;
}

static void inp_bind_key(int action_id, int key_char_val)
{
    //TODO check action_id exists in available_actions
    struct _InputKey* key;
    int key_index = inp__find_key(&(iCluige.input->bound_keys), key_char_val);
    if(key_index == -1)
    {
        key = inp__new_key(key_char_val);
        key_index = iCluige.iDeque.size(&(iCluige.input->bound_keys));
        iCluige.iDeque.push_back(&(iCluige.input->bound_keys), key);
    }
    else
    {
        key = iCluige.iDeque.at(&(iCluige.input->bound_keys), key_index).ptr;
    }
    iCluige.iDeque.push_back(&(key->bound_actions), (int32_t)action_id);
}

static bool inp_is_action_just_pressed(int action_id)
{
    int found_at = inp__find_action(&(iCluige.input->just_pressed_actions), action_id);
    return (found_at != -1);
}

/////////////////////////////////// Node //////////

void iiInput_init()
{
    iCluige.iInput.new_Input = inp_new_Input;
//    iCluige.iInput.new_action = inp_new_action;
//    iCluige.iInput.new_key = inp_new_key;
    iCluige.iInput.add_action = inp_add_action;
    iCluige.iInput.bind_key = inp_bind_key;

//    iCluige.iInput.is_action_pressed = inp_is_action_pressed;
    iCluige.iInput.is_action_just_pressed = inp_is_action_just_pressed;
//    iCluige.iInput.is_key_pressed = inp_is_key_pressed;
//    iCluige.iInput.is_key_just_pressed = inp_is_key_just_pressed;

}

