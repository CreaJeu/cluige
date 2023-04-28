
#include "../cluige.h"
#include "Input.h"

#include <string.h> //TODO static instead of malloc'ed class name
#include <curses.h> //TODO separate from cluige core via 'server'

////////////////////////////////// _Input /////////

static void inp_deleteInput(Node* thisNode)
{
    struct _Input* thisInput = (struct _Input*)(thisNode->_subClass);
    void (*deleteNode)(Node*) = thisInput->deleteNode;

    for(int i=0; i < iCluige.iDeque.size(&(thisInput->available_actions)); i++)
    {
        Variant v = iCluige.iDeque.at(&(thisInput->available_actions), i);
        struct _InputAction* va = (struct _InputAction*)(v.ptr);
        free(va->name);
        free(v.ptr);
    }
    iCluige.iDeque.deleteDeque(&(thisInput->available_actions));

    for(int i=0; i < iCluige.iDeque.size(&(thisInput->bound_keys)); i++)
    {
        Variant v = iCluige.iDeque.at(&(thisInput->bound_keys), i);
        free(v.ptr);
    }
    iCluige.iDeque.deleteDeque(&(thisInput->bound_keys));

    iCluige.iDeque.deleteDeque(&(thisInput->just_pressed_actions));
    iCluige.iDeque.deleteDeque(&(thisInput->just_released_actions));
    iCluige.iDeque.deleteDeque(&(thisInput->just_pressed_keys));
    iCluige.iDeque.deleteDeque(&(thisInput->just_released_keys));

    free(thisInput);
    thisNode->_subClass = NULL;
    deleteNode(thisNode);
}

// private utility functions
static int inp__findKey(const Deque* keys, int charVal)
{
    for(int i=0; i < iCluige.iDeque.size(keys); i++)
    {
        Variant v = iCluige.iDeque.at(keys, i);
        struct _InputKey* vk = (struct _InputKey*)(v.ptr);
        if(vk->unicode_char == charVal)
        {
            return i;
        }
    }
    return -1;
}
static int inp__findAction(const Deque* action_ids, int32_t id_action)
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

static void inp_preProcessInput(Node* thisNode)
{
    struct _Input* thisInput = (struct _Input*)(thisNode->_subClass);
    //reinit all "just_XXX"
    iCluige.iDeque.clear(&(thisInput->just_pressed_actions));
    iCluige.iDeque.clear(&(thisInput->just_released_actions));
    iCluige.iDeque.clear(&(thisInput->just_pressed_actions));
    iCluige.iDeque.clear(&(thisInput->just_released_keys));

    int gotch = getch();
    while(gotch != ERR)
    {
        int boundKeyIndex = inp__findKey(&(thisInput->bound_keys), gotch);
        if(boundKeyIndex != -1)
        {
            struct _InputKey* key = iCluige.iDeque.at(&(thisInput->bound_keys), boundKeyIndex).ptr;
//            if(!(key->pressed))
//            {
                for(int i=0; i < iCluige.iDeque.size(&(key->bound_actions)); i++)
                {
                    int32_t actionId = iCluige.iDeque.at(&(key->bound_actions), i).i32;
                    struct _InputAction* action = iCluige.iDeque.at(
                            &(thisInput->available_actions), i).ptr;
                    action->nb_pressed++;
//                    action->just_pressed = true;
                    iCluige.iDeque.push_back(
                            &(thisInput->just_pressed_actions), (int32_t)actionId);
                }
                key->pressed = true;
//                key->just_pressed = true;
                iCluige.iDeque.push_back(
                        &(thisInput->just_pressed_keys), key);
//            }
        }
        //TODO non-bound keys, for is_key_pressed()
        //TODO release events !
        gotch = getch();
    }
}


////////////////////////////////// iiInput /////////

static struct _Input* inp_newInput()
{
    Node* newNode = iCluige.iNode.newNode();
    struct _Input* newInput = iCluige.checkedMalloc(sizeof(struct _Input));
    //...

    newNode->_subClass = newInput;
    newInput->_thisNode = newNode;

    free(newNode->_className); //TODO static value to avoid free
    StringBuilder sb;
    newNode->_className = iCluige.iStringBuilder.stringAlloc(&sb, strlen("NodeInput"));
    iCluige.iStringBuilder.append(&sb, "NodeInput");

    newInput->deleteNode = newNode->deleteNode;
    newNode->deleteNode = inp_deleteInput;
    newNode->preProcessNode = inp_preProcessInput;

    iCluige.iDeque.dequeAlloc(&(newInput->available_actions), VT_POINTER, 20);
    iCluige.iDeque.dequeAlloc(&(newInput->bound_keys), VT_POINTER, 40);
    ///// TODO     standard actions? //ui_accept ui_cancel ui_left ui_right ui_up ui_down

    iCluige.iDeque.dequeAlloc(&(newInput->just_pressed_actions), VT_INT32, 9);
    iCluige.iDeque.dequeAlloc(&(newInput->just_released_actions), VT_INT32, 9);
    iCluige.iDeque.dequeAlloc(&(newInput->just_pressed_keys), VT_POINTER, 9);
    iCluige.iDeque.dequeAlloc(&(newInput->just_released_keys), VT_POINTER, 9);

    return newInput;
}

static struct _InputAction* inp__newAction(char* name)
{
    struct _InputAction* newAction = iCluige.checkedMalloc(sizeof(struct _InputAction));
    newAction->name = iCluige.iStringBuilder.clone(name);
    newAction->nb_pressed = 0;
//    newAction->just_pressed = false;
//    newAction->just_released = false;
    return newAction;
}

static struct _InputKey* inp__newKey(int charVal)
{
    struct _InputKey* newKey = iCluige.checkedMalloc(sizeof(struct _InputKey));
    newKey->unicode_char = charVal;
    newKey->pressed = 0;
//    newKey->just_pressed = 0;
//    newKey->just_released = 0;
    iCluige.iDeque.dequeAlloc(&(newKey->bound_actions), VT_INT32, 3);
    return newKey;
}

static int inp_add_action(char* action_name)
{
    int res = iCluige.iDeque.size(&(iCluige.input->available_actions));
    struct _InputAction* newAction = inp__newAction(action_name);
    iCluige.iDeque.push_back(&(iCluige.input->available_actions), newAction);
    return res;
}

static void inp_bind_key(int action_id, int keyCharVal)
{
    //TODO check action_id exists in available_actions
    struct _InputKey* key;
    int key_index = inp__findKey(&(iCluige.input->bound_keys), keyCharVal);
    if(key_index == -1)
    {
        key = inp__newKey(keyCharVal);
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
    int found_at = inp__findAction(&(iCluige.input->just_pressed_actions), action_id);
    return (found_at != -1);
}

/////////////////////////////////// Node //////////

void iiInputInit()
{
    iCluige.iInput.newInput = inp_newInput;
//    iCluige.iInput.newAction = inp_newAction;
//    iCluige.iInput.newKey = inp_newKey;
    iCluige.iInput.add_action = inp_add_action;
    iCluige.iInput.bind_key = inp_bind_key;

//    iCluige.iInput.is_action_pressed = inp_is_action_pressed;
    iCluige.iInput.is_action_just_pressed = inp_is_action_just_pressed;
//    iCluige.iInput.is_key_pressed = inp_is_key_pressed;
//    iCluige.iInput.is_key_just_pressed = inp_is_key_just_pressed;

}

