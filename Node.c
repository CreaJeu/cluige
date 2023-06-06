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

static void nde_set_name(Node* n, const char* new_name)
{
    //allocate new name to prevent pointing to the stack
    int size = strlen(new_name);
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
        Node* lastChild = parent->children;
        while(lastChild->next_sibling != NULL)
        {
            lastChild = lastChild->next_sibling;
        }
        lastChild->next_sibling = child;
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

static void nde_print_tree_pretty(const Node* node)
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
    //iCluige.iNode.initZero = nde_initZero;
    iCluige.iNode.new_Node = nde_new_Node;
    //iCluige.iNode.delete_Node = nde_delete_Node;
    iCluige.iNode.get_index = nde_get_index;
    iCluige.iNode.get_depth = nde_get_depth;
    iCluige.iNode.set_name = nde_set_name;
    iCluige.iNode.add_child = nde_add_child;
    iCluige.iNode.print_tree_pretty = nde_print_tree_pretty;
}

