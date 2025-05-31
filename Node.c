//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h

//for print_tree_pretty()
#include <stdio.h>
#include <string.h>
#include "cluige.h"
//#include "Node.h" //already in cluige.h

//Deque to keep track of which malloc to free
Deque _queue_freed_nodes;
//const int _MAX_NAME_LENGTH = 150;//cf. iiNode / init()


////////////////////////////////// _Node /////////

// no : would encourage not freeable nodes on the stack
//static void nde_initZero(Node* node)
//{
//    ...
//}

static void nde_delete_Node(Node* node)
{
    if(node->children != NULL)
    {
        node->children->delete_Node(node->children);
        node->children = NULL;
    }
    if(node->next_sibling != NULL)
    {
        //TODO really delete siblings? (but see above : deleting chidren assumes sibling auto deleted
        node->next_sibling->delete_Node(node->next_sibling);
        node->next_sibling = NULL;
    }
     //beware in C, hardcoded strings like char* s = "bob" are allocated on stack
    if(node->name != NULL)
    {
        free(node->name);
    }
    free(node->_class_name);
    CLUIGE_ASSERT(node->_sub_class == NULL, "Node::delete_Node() : not null subclass found");

    if(node->script != NULL)
    {
        node->script->delete_Script(node->script);
    }

    free(node);
}

static void nde_enter_tree(Node* this_Node)
{
    if((this_Node->script != NULL) && (this_Node->script->enter_tree != NULL))
    {
        this_Node->script->enter_tree(this_Node->script);
    }
}

static void nde_ready(Node* this_Node)
{
    if((this_Node->script != NULL) && (this_Node->script->ready != NULL))
    {
        this_Node->script->ready(this_Node->script);
    }
}

static void nde_exit_tree(Node* this_Node)
{
    if((this_Node->script != NULL) && (this_Node->script->exit_tree != NULL))
    {
        this_Node->script->exit_tree(this_Node->script);
    }
    iCluige.iDeque.push_back(&(iCluige.iNode._just_removed_nodes), this_Node);
}

//on_loop_starting = NULL for Node

////better: maintain those data in all concerned methods instead of this systemtic copy each frame
//static void nde_erase(Node* this_Node)
//{
//    this_Node->_state_changes.parent = this_Node->parent;
//    this_Node->_state_changes.next_sibling = this_Node->next_sibling;
//    this_Node->_state_changes.children = this_Node->children;
//    this_Node->_state_changes.script = this_Node->script;
//    this_Node->_state_changes.name = this_Node->name;
////    this_Node->_state_changes.active = this_Node->active;
//    this_Node->_state_changes.process_priority = this_Node->process_priority;
//}

static void nde_process(Node* this_Node)
{
    if((this_Node->script != NULL) && (this_Node->script->process != NULL))
    {
        this_Node->script->process(this_Node->script, iCluige.clock->elapsed_seconds);
    }
}

//pre_draw = NULL for Node
//draw = NULL for Node


////////////////////////////////// iiNode /////////

static Node* nde_new_Node()
{
    Node* node = iCluige.checked_malloc(sizeof(Node));
    //nde_initZero(node);
    node->parent = NULL;
    node->next_sibling = NULL;
    node->children = NULL;
    node->script = NULL;
    node->name = NULL;
//    node->active = true;
    node->process_priority = 0;

    StringBuilder sb;
    node->_class_name = iCluige.iStringBuilder.string_alloc(&sb, 4);
    iCluige.iStringBuilder.append(&sb, "Node");

//    node->_state_changes.parent = node->parent;
//    node->_state_changes.next_sibling = node->next_sibling;
//    node->_state_changes.children = node->children;
//    node->_state_changes.script = node->script;
//    node->_state_changes.name = node->name;
////    node->_state_changes.active = node->active;
//    node->_state_changes.process_priority = node->process_priority;
//    node->_state_changes.already_entered_tree = false;
//    node->_state_changes.marked_for_queue_free = false;
    node->_already_entered_tree = false;
    node->_marked_for_queue_free = false;

    node->delete_Node = nde_delete_Node;
    node->enter_tree = nde_enter_tree;
    node->ready = nde_ready;
    node->exit_tree = nde_exit_tree;
//    node->on_loop_starting = NULL;
    node->erase = NULL;//nde_erase;
    node->process = nde_process;
    node->bake = NULL;
    node->draw = NULL;
    node->_sub_class = NULL;
    return node;
}

static int nde_get_index(const Node* node)
{
    int res = 0;
    if(node->parent != NULL)
    {
        Node* sibling_i = node->parent->children;
        while(sibling_i != node)
        {
            sibling_i = sibling_i->next_sibling;
            CLUIGE_ASSERT(sibling_i != NULL, "Node::get_index() : internal bug? node not found as parent's child");
            res++;
        }
    }
    return res;
}

static int nde_get_depth(const Node* node)
{
    int res = 0;
    Node* parent = node->parent;
    while(parent != NULL)
    {
        parent = parent->parent;
        res++;
    }
    return res;
}

//returns child at i,
//negative values can also be used to start from the end of the list
//(-1 meaning last)
//or returns NULL if out of those bounds
static Node* nde_get_child(const Node* ths_node, int i)
{
    CLUIGE_ASSERT(ths_node != NULL, "Node::get_child(i) : calling object is null");
    CLUIGE_ASSERT(-(iCluige.iNode.get_child_count(ths_node)) < i && i < iCluige.iNode.get_child_count(ths_node),
				"Node::get_child(i) : i is out of bound (even if negative for reverse order)");
    int nb_steps = i;
    if(i < 0)//reverse order
    {
        nb_steps = iCluige.iNode.get_child_count(ths_node) + i;
    }
    Node* next_sib = ths_node->children;
    int curr_step = 0;
//    while(next_sib != NULL && curr_step != nb_steps)
//    {
//        if(i == -1 && next_sib->next_sibling == NULL)// case -1 inx returns last child
//        {
//            return next_sib;
//        }
    while(curr_step < nb_steps)
    {
        next_sib = next_sib->next_sibling;
        curr_step++;
    }
    return next_sib;
}

static int nde_get_child_count(const Node* ths_node)
{
    CLUIGE_ASSERT(ths_node != NULL, "Node::get_child_count() : calling object is null");
    int count = 0;
    Node* next_sib = ths_node->children;
    while(next_sib != NULL)
    {
        count++;
        next_sib = next_sib->next_sibling;
    }
    return count;
}

//private util
//returns a Node among next siblings (including this) having given name
//or NULL if none found
static Node* nde__on_level(Node* ths_node, const char* name)
{
    Node* next_sib = ths_node;
    while(next_sib != NULL)
    {
        if(strcmp(next_sib->name, name) == 0)
        {
            return next_sib;
        }
        next_sib = next_sib->next_sibling;
    }
    return NULL;
}

//private util
static Node* nde__get_node_rec(Node* ths_node, const char* node_path, bool absolute )//Not completed
{
    CLUIGE_ASSERT(node_path != NULL, "Node::get_node_rec() : node_path is null");
    int i =0;
    char element[iCluige.iNode._MAX_NAME_LENGTH];
    int size = strlen(node_path);
    Node* tmp;
    if(sscanf(node_path, "%[^/]/", element) == 1)// "test/main" or "main" format
    {
        i += strlen(element) + 1;

        //for  "." case
        if(strcmp(".",element) == 0)
        {
            return ths_node;
        }

        //for ".."
        if(strcmp("..",element) == 0)
        {
            if(i>=size)
            {
                return ths_node->parent;
            }
            else
            {
                return nde__get_node_rec(ths_node->parent,i + node_path,absolute);
            }
        }


        if(absolute)
        {
        	tmp = nde__on_level(ths_node,element);
        }
        else
        {
        	tmp = nde__on_level(ths_node->children,element);
        }


        if(tmp == NULL) //general cases like : "test" "root/test"
        {
            return NULL;
        }
        else if(i >= size)
        {
            return tmp;
        }
        else
        {
            if(absolute) return nde__get_node_rec(tmp->children,i + node_path,absolute);
            else         return nde__get_node_rec(tmp,i + node_path,absolute);
        }
    }
    else//HOW DO YOU GET HERE
    {
        return NULL;
    }
}

static Node* nde_get_node(Node* ths_node,const char* node_path )//Not completed
{
    CLUIGE_ASSERT(node_path != NULL, "Node::get_node() : node_path is null");
    bool absolute = node_path[0] == '/';
    Node* next_child;
    int ab = 0;

    if (absolute)
    {
        next_child = iCluige.public_root_2D;
        ab++;
    }
    else
    {
        next_child = ths_node;
    }
    return nde__get_node_rec(next_child,node_path + ab,absolute);
}

//evaluates if a node is the parent (or grandparent etc..) of current node
static bool nde_is_ancestor_of(Node* ths_node, Node* potential_ancestor)
{
    CLUIGE_ASSERT(ths_node != NULL, "Node::is_ancestor_of() : calling object is null");
    CLUIGE_ASSERT(potential_ancestor != NULL, "Node::is_ancestor_of() : potential_ancestor is null");
    if(ths_node == potential_ancestor)
    {
        return true;
    }
    else if (ths_node->parent == NULL)
    {
        return false;
    }
    else
    {
        return nde_is_ancestor_of(ths_node->parent,potential_ancestor);
    }
}

//returns true if given node is higher in hierarchy than current node
//also true if pther_node is just a higher son of the same parent node
//TODO : if used critically one day, check if exact same behavior than Godot
static bool nde_is_higher_than(Node* ths_node, Node* other_node)
{
    if(ths_node == other_node)// same nodes
    {
        return false;
    }
    int depth_ths_node = nde_get_depth(ths_node);
    int depth_other_node = nde_get_depth(other_node);
    if(depth_ths_node < depth_other_node)
    {
        return true;
    }
    else if (depth_ths_node == depth_other_node && ths_node->parent == other_node->parent)
    {
        while(other_node != NULL)
        {
            if(other_node->next_sibling == ths_node)
            {
                return true;
            }
            other_node = other_node->next_sibling;
        }
        return false;
    }
    else
    {
        return false;
    }
}

//private util
static int nde__path_char_length(const Node* ths_node)
{
    if(ths_node != iCluige.public_root_2D)
    {
        return strlen(ths_node->name) + nde__path_char_length(ths_node->parent) + 1;
    }
    else
    {
        return strlen(ths_node->name);
    }
}

//private util
static void nde__rec_get_path(const Node* ths_node, char* res, int remaining_path_size)
{
    if(ths_node == iCluige.public_root_2D)
    {
        strncpy(res,"/public_root_2D",strlen(ths_node->name));
    }
    else
    {
        int this_name_length = strlen(ths_node->name);
        char* write_here = res + remaining_path_size - this_name_length - 1;

        write_here[0] = '/';
        write_here++;

        strncpy(write_here,ths_node->name,this_name_length);

        nde__rec_get_path(ths_node->parent,res,remaining_path_size - this_name_length - 1);
    }
}

static char* nde_get_path_mallocing(const Node* ths_node)
{
    CLUIGE_ASSERT(ths_node != NULL, "Node::get_path_mallocing() : calling object is null");
    int max = nde__path_char_length(ths_node);
    char* res = iCluige.checked_malloc((max + 1) * sizeof(char));

    nde__rec_get_path(ths_node,res,max);
    res[max] = '\0';
    return res;
}

 void nde_print_tree_pretty(const Node* node)
{
    static int depth = 0;

    for(int i=0; i<depth; i++)
    {
        printf("   ");
    }
    printf("\\_%s (%s)\n", node->name, node->_class_name);

    if(node->children != NULL)
    {
        depth++;
        nde_print_tree_pretty(node->children);
        depth--;
    }
    if(node->next_sibling != NULL)
    {
        nde_print_tree_pretty(node->next_sibling);
    }
}



////////// write (iiNode) /////////


//private utility
//true if name is valid (contains none of : / " % @ : .) else false
static bool nde__valid_name(const char* new_name)
{
    const char *special_chars = "/: .@%";
    for(int i = 0; new_name[i] != '\0'; i++)
    {
        if(strchr(special_chars, new_name[i]) != NULL)
        {
            return false;
        }
    }
    return true;
}

static void nde_set_name(Node* n, const char* new_name)
{
    //allocate new name to prevent pointing to the stack
    int size = strlen(new_name);

    CLUIGE_ASSERT(nde__valid_name(new_name), "Node::set_name() : should contain none of : / \" % @ : .");
    char* next_name = iCluige.checked_malloc((size + 1) * sizeof(char));
    strcpy(next_name, new_name);
    //free old name
    if(n->name != NULL)
    {
        free(n->name);
    }
    n->name = next_name;
}

//private util
//make unique name
static void nde__auto_name(Node* node)
{
    int b2d = iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT;
    int p2d = iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_POINTER;
    size_t name_size_max = b2d + p2d + strlen(node->_class_name) + 3;
    if(node->parent != NULL && node->parent->name != NULL)
    {
        name_size_max += 1 + strlen(node->parent->name);
    }

    StringBuilder sb;
    node->name = iCluige.iStringBuilder.string_alloc(&sb, name_size_max);
    if(node->parent != NULL && node->parent->name != NULL)
    {
        iCluige.iStringBuilder.append(&sb, "%s>", node->parent->name);
    }
    iCluige.iStringBuilder.append(&sb, "%s@%p-%d", node->_class_name, node, nde_get_index(node));

}

static void nde__branch_enter_tree(Node* root)
{
	//godot order precisely specified at
	//https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-private-method-ready
	//https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-private-method-enter-tree
	//first enter_tree in DFS order on all branch, even if already added-removed-readded
	//then ready in reverse DFS but only once per node
	root->enter_tree(root);
	Node* child = root->children;
	while(child != NULL)
	{
		nde__branch_enter_tree(child);
		child = child->next_sibling;
	}
	if(!(root->_already_entered_tree))
	{
		root->_already_entered_tree = true;
		root->ready(root);
	}
}

static void nde__branch_exit_tree(Node* root)
{
	//https://docs.godotengine.org/en/stable/classes/class_node.html#class-node-private-method-exit-tree
	//first exit_tree in reverse DFS order (on whole branch before starting remove_child logic?)
	//then do remove_child logic
	//then (but not implemented in cluige) tree_exited signal
	Node* child = root->children;
	while(child != NULL)
	{
		nde__branch_exit_tree(child);
		child = child->next_sibling;
	}
	root->exit_tree(root);
}

//asserts that wanted child doesn't have already a parent
static void nde_add_child(Node* parent, Node* child)
{
    CLUIGE_ASSERT(child->parent == NULL, "Node::add_child() : child must have no parent");
    CLUIGE_ASSERT(child != parent, "Node::add_child() : same Node is asked for both child and parent");
	CLUIGE_ASSERT(
			child->name == NULL || NULL == iCluige.iNode.get_node(parent, child->name),
			"Node::add_child() : a Node with the same name already exists among future siblings");
    if(parent->children == NULL)
    {
        parent->children = child;
    }
    else
    {
        Node* last_child = parent->children;
        while(last_child->next_sibling != NULL)
        {
            last_child = last_child->next_sibling;
        }
        last_child->next_sibling = child;
    }
    child->parent = parent;

    if(child->name == NULL)
    {
        nde__auto_name(child);
    }
    if(iCluige.iNode.is_ancestor_of(parent, iCluige._private_root_2D))
	{
		nde__branch_enter_tree(child);
	}
}

static void nde_remove_child( Node* ths_node, Node* child)
{
    CLUIGE_ASSERT(ths_node != NULL, "Node::remove_child() : calling object is null ");
    CLUIGE_ASSERT(child != NULL, "Node::remove_child() : asked child is null");
    CLUIGE_ASSERT(child->parent == ths_node, "Node::remove_child() : child->parent != ths_node");
    if(iCluige.iNode.is_ancestor_of(ths_node, iCluige._private_root_2D))
	{
		nde__branch_exit_tree(child);
	}
    int pos = nde_get_index(child);

    if(pos == 0 )
    {
    	ths_node->children = child->next_sibling;
    }
    else
	{
		Node* node_to_mod = iCluige.iNode.get_child(ths_node, pos-1);
		node_to_mod->next_sibling = child->next_sibling;
	}
    child->next_sibling = NULL;//remove the link between the child and the rest
    child->parent = NULL;
}

static void nde__mark_branch_for_queue_free(Node* root)
{
	root->_marked_for_queue_free = true;
    if(root->children != NULL)
    {
        nde__mark_branch_for_queue_free(root->children);
    }

    if(root->next_sibling != NULL)
    {
        nde__mark_branch_for_queue_free(root->next_sibling);
    }
}

static void nde_queue_free(Node* node)
{
    if(!node->_marked_for_queue_free)
    {
        iCluige.iDeque.append(&_queue_freed_nodes, node);
        node->_marked_for_queue_free = true;
        if(node->children != NULL)
		{
			nde__mark_branch_for_queue_free(node->children);
		}
    }
}

//void nde__debug_dq()
//{
//    int size = iCluige.iDeque.size(&_queue_freed_nodes);
//    printf("dq_free: ");
//    for(int i = 0; i < size; i++)
//    {
//        Node* node = iCluige.iDeque.at(&_queue_freed_nodes,i).ptr;
//
//        printf(" %s |",node->name);
//    }
//    printf("\n");
//}

static void nde__do_all_queue_free_early_step()
{
//	nde__debug_dq();
    int size = iCluige.iDeque.size(&_queue_freed_nodes);
    for(int i = 0; i < size; i++)
    {
        Node* node = iCluige.iDeque.at(&_queue_freed_nodes, i).ptr;
        nde_remove_child(node->parent, node);
    }
}

static void nde__do_all_queue_free_late_step()
{
//	nde__debug_dq();
    int size = iCluige.iDeque.size(&_queue_freed_nodes);
    for(int i = 0; i < size; i++)
    {
        Node* node = iCluige.iDeque.at(&_queue_freed_nodes, i).ptr;
        node->delete_Node(node);
    }
    iCluige.iDeque.clear(&_queue_freed_nodes);
}

static Node* nde_instanciate(const SortedDictionary* parsed_params)
{
    Node* res = nde_new_Node();
    bool ok = utils_str_from_parsed(&(res->name), parsed_params, "name");
    CLUIGE_ASSERT(ok, "Node::instanciate() : missing 'name' field");
    //optional
    utils_int_from_parsed(&(res->process_priority), parsed_params, "process_priority");
    // TODO ? from godot process mode? // utils_bool_from_parsed(&(res->active), params, "active");
    return res;
}

static void nde_register_NodeFactory(const char* key, NodeFactory* factory)
{
    SortedDictionary* fcties = &(iCluige.iNode.node_factories);
    int key_len_term = strlen(key) + 1;
    char* fcty_key = iCluige.checked_malloc(key_len_term * sizeof(char));
    strncpy(fcty_key, key, key_len_term);
    Checked_Variant found = iCluige.iSortedDictionary.insert(fcties, fcty_key, factory);
    CLUIGE_ASSERT(!(found.valid), "Node::register_NodeFactory() : trying to register a factory with an already used key");
}

/////////////////////////////////// Node //////////

void iiNode_init()
{
	iCluige.iNode._MAX_NAME_LENGTH = 150;

    iCluige.iDeque.deque_alloc(&_queue_freed_nodes, VT_POINTER, 20);
    iCluige.iDeque.deque_alloc(&(iCluige.iNode._just_removed_nodes), VT_POINTER, 20);

    iCluige.iNode.new_Node = nde_new_Node;
    //iCluige.iNode.delete_Node = nde_delete_Node; // virtual

    iCluige.iNode.get_index = nde_get_index;
    iCluige.iNode.get_depth = nde_get_depth;
    iCluige.iNode.get_child = nde_get_child;
    iCluige.iNode.get_child_count = nde_get_child_count;
    iCluige.iNode.get_node = nde_get_node;
    iCluige.iNode.is_ancestor_of = nde_is_ancestor_of;
    iCluige.iNode.is_higher_than = nde_is_higher_than;
    iCluige.iNode.get_path_mallocing = nde_get_path_mallocing;
    iCluige.iNode.print_tree_pretty = nde_print_tree_pretty;

    iCluige.iNode.set_name = nde_set_name;
    iCluige.iNode.add_child = nde_add_child;
    iCluige.iNode.remove_child = nde_remove_child;
    iCluige.iNode.queue_free = nde_queue_free;
    iCluige.iNode._do_all_queue_free_early_step = nde__do_all_queue_free_early_step;
    iCluige.iNode._do_all_queue_free_late_step = nde__do_all_queue_free_late_step;

    SortedDictionary* fcties = &(iCluige.iNode.node_factories);
    iCluige.iSortedDictionary.sorted_dictionary_alloc(fcties, VT_POINTER, VT_POINTER, 20);
    iCluige.iSortedDictionary.set_compare_keys_func(fcties, iCluige.iDeque.default_compare_string_func);
    iCluige.iNode.register_NodeFactory = nde_register_NodeFactory;

    iCluige.iNode._Node_factory.instanciate = nde_instanciate;
    iCluige.iNode.register_NodeFactory("Node", &(iCluige.iNode._Node_factory));
}


////not used in Cluige and not tested
////Search in each children and grandchildren of the current node if there is a node with the type specified in parameters
//static Node*  check_children_type(Node* parent, char* class_name)
//{
//     // Check if the current node has children
//    if (parent->children!= NULL) {
//        // Recursively search through the children
//        Node* foundChild = check_children_type(parent->children, class_name);
//        if (foundChild!= NULL && strcmp(foundChild->_class_name,class_name) == 0) {
//            return foundChild; // Found a matching child, return it
//        }
//    }
//    else
//    {
//        if (parent != NULL && strcmp(parent->_class_name,class_name) == 0) {
//            return parent; // Found a matching child, return it
//        }
//    }
//
//    // Check if the current node has a next sibling
//    if (parent->next_sibling!= NULL) {
//        // Recursively search through the siblings
//        Node* foundSibling = check_children_type(parent->next_sibling, class_name);
//        if (foundSibling!= NULL && strcmp(foundSibling->_class_name,class_name) == 0) {
//            return foundSibling; // Found a matching sibling, return it
//        }
//    }
//    else
//    {
//        if (strcmp(parent->_class_name,class_name) == 0) {
//            return parent; // Found a matching child, return it
//        }
//    }
//
//    // No matching child or sibling found
//    return NULL;
//}
