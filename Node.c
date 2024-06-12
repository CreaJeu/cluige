//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include <stdio.h> //for print_tree_pretty()
#include <assert.h>
#include <string.h>
#include "cluige.h"
//#include "Node.h" //already in cluige.h


////////////////////////////////// _Node /////////

// no : would encourage not freeable nodes on the stack
//static void nde_initZero(Node* node)
//{
//    ...
//}


//Deque to keep track of which malloc to free
Deque _queue_freed_nodes;
const int _MAX_NAME_LENGTH = 150;



static void nde_delete_Node(Node* node)
{
    if(node->children != NULL)
    {
        node->children->delete_Node(node->children);
        node->children = NULL;
    }
    if(node->next_sibling != NULL)
    {
        node->next_sibling->delete_Node(node->next_sibling);
        node->next_sibling = NULL;
    }
     //beware in C, hardcoded strings like char* s = "bob" are allocated on stack
    if(node->name != NULL)
    {
        free(node->name);
    }
    free(node->_class_name);
    assert(node->_sub_class == NULL);

    if(node->script != NULL)
    {
        node->script->delete_Script(node->script);
    }

    free(node);
}

static void nde_process_Node(Node* this_Node)
{
    if((this_Node->script != NULL) && (this_Node->script->process != NULL))
    {
        this_Node->script->process(this_Node->script, iCluige.clock->elapsed_seconds);
    }
}

static void nde_enter_tree(Node* this_Node)
{
    //TODO enter_tree in Script?
//    if((this_Node->script != NULL) && (this_Node->script->enter_tree != NULL))
//    {
//        this_Node->script->enter_tree(this_Node->script);
//    }
}


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
    node->active = true;

    StringBuilder sb;
    node->_class_name = iCluige.iStringBuilder.string_alloc(&sb, 4);
    iCluige.iStringBuilder.append(&sb, "Node");

    node->delete_Node = nde_delete_Node;
    node->enter_tree = nde_enter_tree;
    node->on_loop_starting = NULL;
    node->pre_process_Node = NULL;
    node->process_Node = nde_process_Node;
    node->post_process_Node = NULL;
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

//true if name is valid (do not contain of : / " % @ : .) else false
static bool valid_name(const char* new_name)
{
    const char *special_chars = "/: .@%";
    for (int i = 0; new_name[i] != '\0'; i++) {
        if (strchr(special_chars, new_name[i]) != NULL) {
            return false;
        }
    }

    return true;
}

static void nde_set_name(Node* n, const char* new_name)
{
    //allocate new name to prevent pointing to the stack
    int size = strlen(new_name);

    assert(valid_name(new_name));
    char* next_name = iCluige.checked_malloc((size + 1) * sizeof(char));
    strcpy(next_name, new_name);
    //free old name
    if(n->name != NULL)
    {
        free(n->name);
    }
    n->name = next_name;
}






//make unique name
static void nde_auto_name(Node* node)
{
    int b2d = iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT;
    int p2d = iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_POINTER;
    size_t name_size_max = b2d + p2d + 1;
    if(node->parent != NULL)
    {
        name_size_max += 1 + strlen(node->parent->name);
    }

    StringBuilder sb;
    node->name = iCluige.iStringBuilder.string_alloc(&sb, name_size_max);
    iCluige.iStringBuilder.append(&sb, "%p-%d", node, nde_get_index(node));

    if(node->parent != NULL)
    {
        iCluige.iStringBuilder.append(&sb, "<%s", node->parent->name);
    }
}

//returns child at idx in current node, if index is -1 returns last child else NULL
static Node* nde_get_child(const Node* ths_node, int idx)
{
    assert(ths_node != NULL);
    assert(idx >= -1);

    Node* next_sib = ths_node->children;
    int count = 0;
    while(next_sib != NULL && count != idx  )
    {
        if(idx == -1 && next_sib->next_sibling == NULL)// case -1 inx returns last child
        {
            return next_sib;
        }
        next_sib = next_sib->next_sibling;
        count++;
    }
    return next_sib;
}

static int nde_get_child_count(const Node* ths_node)
{
    assert(ths_node != NULL);

    int count = 0;
    Node* next_sib = ths_node->children;

    while(next_sib != NULL)
    {
        count++;
        next_sib = next_sib->next_sibling;
    }
    return count;
}

//auxiliary functions that returns the node if there is one on this level comparing with names
static Node* nde_on_level(Node* ths_node,const char* last_word)
{
    Node* next_sib = ths_node;
    while(next_sib != NULL)
    {
        if(strcmp(next_sib->name,last_word) == 0)
        {
            return next_sib;
        }
        next_sib = next_sib->next_sibling;
    }
    return NULL;
}

static Node* nde_get_node_rec(Node* ths_node,const char* node_path,bool absolute )//Not completed
{
    assert(node_path != NULL);
    int i =0;
    char element[_MAX_NAME_LENGTH];
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
                return nde_get_node_rec(ths_node->parent,i + node_path,absolute);
            }
        }


        if(absolute)
        {
        	tmp = nde_on_level(ths_node,element);
        }
        else
        {
        	tmp = nde_on_level(ths_node->children,element);
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
            if(absolute) return nde_get_node_rec(tmp->children,i + node_path,absolute);
            else         return nde_get_node_rec(tmp,i + node_path,absolute);
        }
    }
    else//HOW DO YOU GET HERE
    {
        return NULL;
    }
}


static Node* nde_get_node(Node* ths_node,const char* node_path )//Not completed
{
    assert(node_path != NULL);
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
    return nde_get_node_rec(next_child,node_path + ab,absolute);

}


static void nde_remove_child( Node* ths_node, Node* child)
{
    assert(ths_node != NULL);
    assert(child != NULL);
    assert(child->parent == ths_node);//verify that the child given is indeed a child of ths_node
    int pos = nde_get_index(child);

    //link between parent and child
    if(ths_node->children == child) //if there is more than one children to ths_node
    {
        ths_node->children = child->next_sibling;
    }

    //link between siblings
    if(pos > 0 )
    {
        Node* node_to_mod = ths_node->children;  //take the node before the one we want to remove
        Node* node_to_point = child->next_sibling;           //take the node after we the one we want to remove
        node_to_mod->next_sibling = node_to_point;          //link both

    }
    child->next_sibling = NULL;                         //remove the link between the child and the rest
    child->parent = NULL;
}


static int nde__tree_max_char(Node* ths_node)
{
    if(ths_node != iCluige.public_root_2D)
    {
        return strlen(ths_node->name) + nde__tree_max_char(ths_node->parent) + 1;
    }
    else
    {
        return strlen(ths_node->name);
    }
}

static void nde__rec_get_path_mallocing(Node* ths_node, char* res, int remaining_path_size)
{
    if(ths_node == iCluige.public_root_2D)
    {
        strncpy(res,"/public_root_2D",strlen(ths_node->name));
    }
    else //cas general
    {
        int this_name_length = strlen(ths_node->name);
        char* write_here = res + remaining_path_size - this_name_length - 1;

        write_here[0] = '/';
        write_here++;

        strncpy(write_here,ths_node->name,this_name_length);

        nde__rec_get_path_mallocing(ths_node->parent,res,remaining_path_size - this_name_length - 1);
    }
}
static char* nde_get_path_mallocing(Node* ths_node)
{
    assert(ths_node != NULL);
    int max = nde__tree_max_char(ths_node);
    char* res = iCluige.checked_malloc((max + 1) * sizeof(char));

    nde__rec_get_path_mallocing(ths_node,res,max);
    res[strlen(res)] = '\0';
    return res;
}

//evaluates if a node is the parent (or grandparent etc..) of current node
static bool nde_is_ancestor_of(Node* ths_node, Node* potential_ancestor)
{
    assert(ths_node != NULL);
    assert(potential_ancestor != NULL);
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
static bool nde_is_greater_than(Node* ths_node, Node* other_node)
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

static void nde_queue_free(Node* node)
{
    assert(node != NULL);
    int size = iCluige.iDeque.size(&_queue_freed_nodes);
    bool will_be_deleted = false;
    for(int i = 0; i < size; i++)
    {
        Node* node_in_deque = iCluige.iDeque.at(&_queue_freed_nodes,i).ptr;

        will_be_deleted = nde_is_ancestor_of(node_in_deque,node);
        if (will_be_deleted) break;
    }
    if(!will_be_deleted)
    {
        iCluige.iDeque.append(&_queue_freed_nodes,node);
    }
}

void nde__debug_dq()
{
    int size = iCluige.iDeque.size(&_queue_freed_nodes);
    printf("dq_free: ");
    for(int i = 0; i < size; i++)
    {
        Node* node = iCluige.iDeque.at(&_queue_freed_nodes,i).ptr;

        printf(" %s |",node->name);
    }
    printf("\n");
}

static void nde__do_all_queue_free()
{
    int size = iCluige.iDeque.size(&_queue_freed_nodes);
    //nde__debug_dq();
    for(int i = 0; i < size; size--)
    {

        Node* node = iCluige.iDeque.at(&_queue_freed_nodes,i).ptr;

        nde_remove_child(node->parent,node);

        nde_delete_Node(node);
        iCluige.iDeque.remove(&_queue_freed_nodes,i);
        nde__debug_dq();
    }
}





//asserts that wanted child doesn't have already a parent
static void nde_add_child(Node* parent, Node* child)
{
    assert(child->parent == NULL);
    assert(child != parent);
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
        nde_auto_name(child);
    }

    //if script, call ready (not yet enter_tree())
    if(!child->_already_entered_tree)
    {
        child->_already_entered_tree = true;
        if((child->script != NULL) && (child->script->ready != NULL))
        {
            child->script->ready(child->script);
        }
    }
    if(child->enter_tree != NULL)
    {
        child->enter_tree(child);
    }
}
//TODO tester la fonction
//not used in Cluige and not tested
//Search in each children and grandchildren of the current node if there is a node with the type specified in parameters
static Node*  check_children_type(Node* parent, char* class_name)
{
     // Check if the current node has children
    if (parent->children!= NULL) {
        // Recursively search through the children
        Node* foundChild = check_children_type(parent->children, class_name);
        if (foundChild!= NULL && strcmp(foundChild->_class_name,class_name) == 0) {
            return foundChild; // Found a matching child, return it
        }
    }
    else
    {
        if (parent != NULL && strcmp(parent->_class_name,class_name) == 0) {
            return parent; // Found a matching child, return it
        }
    }

    // Check if the current node has a next sibling
    if (parent->next_sibling!= NULL) {
        // Recursively search through the siblings
        Node* foundSibling = check_children_type(parent->next_sibling, class_name);
        if (foundSibling!= NULL && strcmp(foundSibling->_class_name,class_name) == 0) {
            return foundSibling; // Found a matching sibling, return it
        }
    }
    else
    {
        if (strcmp(parent->_class_name,class_name) == 0) {
            return parent; // Found a matching child, return it
        }
    }

    // No matching child or sibling found
    return NULL;
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

/////////////////////////////////// Node //////////

void iiNode_init()
{
    iCluige.iDeque.deque_alloc(&_queue_freed_nodes, VT_POINTER, 100);


    //iCluige.iNode.initZero = nde_initZero;
    iCluige.iNode.new_Node = nde_new_Node;
    //iCluige.iNode.delete_Node = nde_delete_Node;
    iCluige.iNode.get_index = nde_get_index;
    iCluige.iNode.get_depth = nde_get_depth;
    iCluige.iNode.set_name = nde_set_name;
    iCluige.iNode.add_child = nde_add_child;
    iCluige.iNode.print_tree_pretty = nde_print_tree_pretty;
    iCluige.iNode.get_child = nde_get_child;
    iCluige.iNode.get_child_count = nde_get_child_count;
    iCluige.iNode.get_node = nde_get_node;
    iCluige.iNode.remove_child = nde_remove_child;
    iCluige.iNode.get_path_mallocing = nde_get_path_mallocing;
    iCluige.iNode.queue_free = nde_queue_free;
    iCluige.iNode._do_all_queue_free = nde__do_all_queue_free;
    iCluige.iNode.is_ancestor_of = nde_is_ancestor_of;
    iCluige.iNode.is_greater_than = nde_is_greater_than;

}
