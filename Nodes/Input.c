
#include <string.h> //TODO static instead of malloc'ed class name
#include <curses.h> //TODO separate from cluige core via 'server'

#include "../cluige.h"
#include "Input.h"

////////////////////////////////// _Input /////////

static void inp_delete_Input(Node* this_Node)
{
	struct _Input* this_Input = (struct _Input*)(this_Node->_sub_class);
	//tmp memorize function pointer before calling free(this)
	void (*delete_super)(Node*) = this_Input->_delete_super;

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
		struct _InputKey* k = (struct _InputKey*)(v.ptr);
		iCluige.iDeque.pre_delete_Deque(&(k->bound_actions));
		free(v.ptr);
	}
	iCluige.iDeque.pre_delete_Deque(&(this_Input->bound_keys));

	iCluige.iDeque.pre_delete_Deque(&(this_Input->just_pressed_actions));
	iCluige.iDeque.pre_delete_Deque(&(this_Input->just_released_actions));
	iCluige.iDeque.pre_delete_Deque(&(this_Input->just_pressed_keys));
	iCluige.iDeque.pre_delete_Deque(&(this_Input->just_released_keys));

	free(this_Input);
	this_Node->_sub_class = NULL;
	delete_super(this_Node);
}

static int inp__compare_InputKey_func(const Deque* this_Deque, Variant va, Variant vb)
{
	//va and vb are InputKey*
	struct _InputKey* ia = (struct _InputKey*)(va.ptr);
	struct _InputKey* ib = (struct _InputKey*)(vb.ptr);
	CLUIGE_ASSERT(ia != NULL, "Input::%s : arg 'va' points to NULL", __FUNCTION__);
	CLUIGE_ASSERT(ib != NULL, "Input::%s : arg 'vb' points to NULL", __FUNCTION__);
	return ia->unicode_char - ib->unicode_char;
}

//// private utility functions
//static int inp__find_key(const Deque* keys, int char_val)
//{
//	for(int i=0; i < iCluige.iDeque.size(keys); i++)
//	{
//		Variant v = iCluige.iDeque.at(keys, i);
//		struct _InputKey* vk = (struct _InputKey*)(v.ptr);
//		if(vk->unicode_char == char_val)
//		{
//			return i;
//		}
//	}
//	return -1;
//}
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

static void inp_process(Node* this_Node)
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
		struct _InputKey tmp_key;
		tmp_key.unicode_char = gotch;
		int bound_key_index = iCluige.iDeque.bsearch(&(iCluige.input->bound_keys), &tmp_key);
		if(bound_key_index != -1)
		{
			struct _InputKey* key = iCluige.iDeque.at(&(this_Input->bound_keys), bound_key_index).ptr;
//			if(!(key->pressed))
//			{
				for(int i=0; i < iCluige.iDeque.size(&(key->bound_actions)); i++)
				{
					int32_t action_id = iCluige.iDeque.at(&(key->bound_actions), i).i32;
					struct _InputAction* action = iCluige.iDeque.at(
							&(this_Input->available_actions), action_id).ptr;
					action->nb_pressed++;
//					action->just_pressed = true;
					iCluige.iDeque.push_back(
							&(this_Input->just_pressed_actions), (int32_t)action_id);
				}
				key->pressed = true;
//				key->just_pressed = true;
				iCluige.iDeque.push_back(
						&(this_Input->just_pressed_keys), key);
//			}
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
	new_node->_class_name = iCluige.iStringBuilder.string_alloc(&sb, 1 + strlen("NodeInput"));
	iCluige.iStringBuilder.append(&sb, "NodeInput");

	//virtual methods - private copies of mother class pointers
	new_input->_delete_super = new_node->delete_Node;

	new_node->delete_Node = inp_delete_Input;
	new_node->process = inp_process;

	new_node->process_priority = INT_MIN;

	iCluige.iDeque.deque_alloc(&(new_input->available_actions), VT_POINTER, 20);
	iCluige.iDeque.deque_alloc(&(new_input->bound_keys), VT_POINTER, 40);
	new_input->bound_keys._sorted = true;
	new_input->bound_keys._compare_func = inp__compare_InputKey_func;
	///// TODO     standard actions? //ui_accept ui_cancel ui_left ui_right ui_up ui_down

	iCluige.iDeque.deque_alloc(&(new_input->just_pressed_actions), VT_INT32, 9);
	iCluige.iDeque.deque_alloc(&(new_input->just_released_actions), VT_INT32, 9);
	iCluige.iDeque.deque_alloc(&(new_input->just_pressed_keys), VT_POINTER, 9);
	iCluige.iDeque.deque_alloc(&(new_input->just_released_keys), VT_POINTER, 9);

	return new_input;
}

static struct _InputAction* inp__new_action(const char* name)
{
	struct _InputAction* new_action = iCluige.checked_malloc(sizeof(struct _InputAction));
	new_action->name = iCluige.iStringBuilder.clone(name);
	new_action->nb_pressed = 0;
//	new_action->just_pressed = false;
//	new_action->just_released = false;
	return new_action;
}

static struct _InputKey* inp__new_key(int char_val)
{
	struct _InputKey* new_key = iCluige.checked_malloc(sizeof(struct _InputKey));
	new_key->unicode_char = char_val;
	new_key->pressed = 0;
//	new_key->just_pressed = 0;
//	new_key->just_released = 0;
	iCluige.iDeque.deque_alloc(&(new_key->bound_actions), VT_INT32, 3);
	return new_key;
}

static int inp_add_action(const char* action_name)
{
	int res = iCluige.iDeque.size(&(iCluige.input->available_actions));
	struct _InputAction* new_action = inp__new_action(action_name);
	iCluige.iDeque.push_back(&(iCluige.input->available_actions), new_action);
	return res;
}

static int inp_action_id_from_name(const char* action_name, bool assert_exists)
{
	Deque* dq = &(iCluige.input->available_actions);
	int n = iCluige.iDeque.size(dq);
	for(int i=0; i<n; i++)
	{
		struct _InputAction* action_i = (struct _InputAction*)(iCluige.iDeque.at(dq, i).ptr);
		if(str_equals(action_i->name, action_name))
		{
			return i;
		}
	}
	CLUIGE_ASSERT(!assert_exists, "Input::%s : unknown action name '%s'",
			__FUNCTION__, action_name);
	return -1;
}

static void inp_bind_key(const char* action_name, int key_char_val)
{
	CLUIGE_ASSERT(action_name != NULL, "Input::%s : unexpected null action name",
			__FUNCTION__);
	struct _InputKey* key;
	int action_id = iCluige.iInput.action_id_from_name(action_name, true);//asserts exists
	struct _InputKey tmp_key;
	tmp_key.unicode_char = key_char_val;
	int key_index = iCluige.iDeque.bsearch(&(iCluige.input->bound_keys), &tmp_key);
	if(key_index == -1)
	{
		key = inp__new_key(key_char_val);
		iCluige.iDeque.insert_or_replace_sorted(
				&(iCluige.input->bound_keys), false, key);
	}
	else
	{
		key = iCluige.iDeque.at(&(iCluige.input->bound_keys), key_index).ptr;
	}
	CLUIGE_ASSERT(inp__find_action(&(key->bound_actions), action_id) == -1,
			"Input::bind_key() : action '%s' already bound to key '%c'",
			((struct _InputAction*)(iCluige.iDeque.at(&(iCluige.input->available_actions), action_id).ptr))->name,
			(char)key_char_val);
	iCluige.iDeque.push_back(&(key->bound_actions), (int32_t)action_id);
}

static void inp_un_bind_key_one_action(int action_id, int key_char_val)
{
	struct _InputKey tmp_key;
	tmp_key.unicode_char = key_char_val;
	int key_index = iCluige.iDeque.bsearch(&(iCluige.input->bound_keys), &tmp_key);
	CLUIGE_ASSERT(key_index != -1,
			"Input::un_bind_key_one_action() : key '%c' not found in input->bound_keys",
			(char)key_char_val);
	struct _InputKey* key = iCluige.iDeque.at(&(iCluige.input->bound_keys), key_index).ptr;
	Deque* dq_act = &(key->bound_actions);
	int dq_act_n = iCluige.iDeque.size(dq_act);
	for(int i = dq_act_n-1; i>=0; i--)
	{
		int32_t candidate = iCluige.iDeque.at(dq_act, i).i32;
		if(candidate == action_id)
		{
			iCluige.iDeque.remove(dq_act, i);
			CLUIGE_ASSERT(inp__find_action(dq_act, action_id) == -1,
					"Input::un_bind_key_one_action() : action '%s' found bound twice to key '%c'",
					((struct _InputAction*)(iCluige.iDeque.at(&(iCluige.input->available_actions), action_id).ptr))->name,
					(char)key_char_val);
			return;
		}
	}
	utils_breakpoint_trick("Input::un_bind_key_one_action() : action_id not found", true);
}

//also removes key from bound_keys
static void inp_un_bind_key_all_actions(int key_char_val)
{
	struct _InputKey tmp_key;
	tmp_key.unicode_char = key_char_val;
	int key_index = iCluige.iDeque.bsearch(&(iCluige.input->bound_keys), &tmp_key);
	CLUIGE_ASSERT(key_index != -1,
			"Input::un_bind_key_one_action() : key '%c' not found in input->bound_keys",
			(char)key_char_val);
	struct _InputKey* key = iCluige.iDeque.at(&(iCluige.input->bound_keys), key_index).ptr;
	Deque* dq_act = &(key->bound_actions);
	iCluige.iDeque.clear(dq_act);
	//also remove key from bound_keys
	iCluige.iDeque.remove(&(iCluige.input->bound_keys), key_index);
}

//whether exists at least 1 key bound to it
static int inp__is_action_orphan(int action_id)
{
	Deque* dq_keys = &(iCluige.input->bound_keys);
	int dq_keys_n = iCluige.iDeque.size(dq_keys);
	for(int i=0; i<dq_keys_n; i++)
	{
		struct _InputKey* key = (struct _InputKey*)iCluige.iDeque.at(dq_keys, i).ptr;
		int found = inp__find_action(&(key->bound_actions), action_id);
		if(found != -1)
		{
			return key->unicode_char;
		}
	}
	return -1;
}

//asserts that !_is_action_orphan()
static void inp_remove_last_available_action()
{
	Deque* dq_act = &(iCluige.input->available_actions);
	int last_i = iCluige.iDeque.size(dq_act) - 1;
	CLUIGE_ASSERT(last_i >= 0, "Input::%s input->available_actions is already empty",
			__FUNCTION__);
	CLUIGE_ASSERT(inp__is_action_orphan(last_i) == -1, "Input::%s action %s is still bound to at least key '%c'",
			__FUNCTION__,
			((struct _InputAction*)(iCluige.iDeque.at(dq_act, last_i).ptr))->name,
			(char)inp__is_action_orphan(last_i));;
	iCluige.iDeque.pop_back(dq_act);
}

static bool inp_is_action_just_pressed(int action_id)
{
	//TODO : with just_released implementation, use action->just_pressed instead
	int found_at = inp__find_action(&(iCluige.input->just_pressed_actions), action_id);
	return (found_at != -1);
}

/////////////////////////////////// Node //////////

void iiInput_init()
{
	iCluige.iInput.new_Input = inp_new_Input;
//	iCluige.iInput.new_action = inp_new_action;
//	iCluige.iInput.new_key = inp_new_key;
	iCluige.iInput.add_action = inp_add_action;
	iCluige.iInput.action_id_from_name = inp_action_id_from_name;
	iCluige.iInput.bind_key = inp_bind_key;
	iCluige.iInput.un_bind_key_one_action = inp_un_bind_key_one_action;
	iCluige.iInput.un_bind_key_all_actions = inp_un_bind_key_all_actions;
	iCluige.iInput._is_action_orphan = inp__is_action_orphan;
	iCluige.iInput.remove_last_available_action = inp_remove_last_available_action;

//	iCluige.iInput.is_action_pressed = inp_is_action_pressed;
	iCluige.iInput.is_action_just_pressed = inp_is_action_just_pressed;
//	iCluige.iInput.is_key_pressed = inp_is_key_pressed;
//	iCluige.iInput.is_key_just_pressed = inp_is_key_just_pressed;

}

