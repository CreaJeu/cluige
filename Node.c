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
    Deque dq_free;




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
    return  (strrchr(new_name,'/') == NULL) && (strrchr(new_name,'"') == NULL) && (strrchr(new_name,'%') == NULL) &&
    (strrchr(new_name,'@') == NULL) && (strrchr(new_name,':') == NULL) && (strrchr(new_name,'.') != NULL);
}

static void nde_set_name(Node* n, const char* new_name)
{
    //allocate new name to prevent pointing to the stack
    int size = strlen(new_name);

    assert(!valid_name(new_name));
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

//return child at idx in current child else NULL
static Node* nde_get_child(const Node* ths_node, int idx)
{
    if (ths_node == NULL ||ths_node->children == NULL)
    {
        return NULL;
    }
    Node* next_sib = ths_node->children;
    while(next_sib != NULL && nde_get_index(next_sib) != idx  )
    {
        next_sib = next_sib->next_sibling;
    }
    return next_sib;
}

static int nde_get_child_count(const Node* ths_node)
{
    int count = 0;
    if (ths_node == NULL ||ths_node->children == NULL)
    {
        return count;
    }
    Node* next_sib = ths_node->children;

    while(next_sib != NULL){
        count++;
        next_sib = next_sib->next_sibling;
    }
    return count;
}

//auxiliary functions that return the node if there is on this level comparing with names
static Node* nde_on_level(const Node* ths_node, char* last_word)
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

static Node* nde_get_node_rec(Node* ths_node, char* node_path )//Not completed
{
    assert(node_path != NULL);
    int i =0;
    char element[50];
    int size = strlen(node_path);
    if(sscanf(node_path, "%[^/]/", element) == 1)// "test/main" or "main" format
    {
        i += strlen(element) + 1;
        Node* tmp = nde_on_level(ths_node,element);
        if(tmp == NULL)
        {
            return NULL;
        }
        else if(i >= size)
        {
            return tmp;
        }
        else
        {
            return nde_get_node_rec(tmp->children,i + node_path);
        }
    }
    else//HOW DO YOU GET HERE
    {
        return NULL;
    }
}


static Node* nde_get_node(const Node* ths_node, char* node_path )//Not completed
{
    assert(node_path != NULL);
    bool absolute = node_path[0] == '/';
    Node* next_child;

    if (absolute)
    {
        next_child = iCluige.public_root_2D;
        node_path++;
    }
    else
    {
        if(ths_node->children == NULL) return NULL;
        next_child = ths_node->children;
    }
    return nde_get_node_rec(next_child,node_path);

}


static void nde_remove_child( Node* ths_node, Node* child)
{
    assert(ths_node != NULL);
    assert(child != NULL);
    assert(nde_on_level(ths_node->children,child->name) != NULL);//verify that the child given is indeed a child of ths_node
    int pos = nde_get_index(child);

    //link between parent and child
    if(ths_node->children == child && child->next_sibling != NULL) //if there is more than one children to ths_node
    {
        ths_node->children = child->next_sibling;
    }
    else if((ths_node->children == child)&& child->next_sibling == NULL)
    {
        ths_node->children = NULL;
    }

    //link between children
    if(pos > 0 )
    {
        Node* node_to_mod = ths_node->children;  //take the node before the one we want to remove
        Node* node_to_point = child->next_sibling;           //take the node after we the one we want to remove
        node_to_mod->next_sibling = node_to_point;          //link both

    }
    child->next_sibling = NULL;                         //remove the link between the child and the rest
    child->parent = NULL;
}


//auxilliary function for get_path
void insert_string(char *str1, const char *str2) {//first string is the container and second the string you want to add before str1
    int len1 = strlen(str1);
    int len2 = strlen(str2);

    // Shift the characters of str1 to the right to make space for str2
    for (int i = len1; i >= 0; i--) {
        str1[i + len2] = str1[i];
    }

    // Copy str2 into the beginning of str1
    for (int i = 0; i < len2; i++) {
        str1[i] = str2[i];
    }
}

static char* nde_get_path_mallocing(Node* ths_node)
{
    StringBuilder sb;
    int max_depth = nde_get_depth(ths_node);
    char* pth = iCluige.iStringBuilder.string_alloc(&sb,max_depth + 100);

    assert(ths_node != NULL);

    int index = 1;
    char bar[5] = "/";
    Node* nxt_parent = ths_node;


    while(nxt_parent != iCluige.public_root_2D)
    {
        insert_string(pth,nxt_parent->name);
        index += strlen(nxt_parent->name); // Update index

        insert_string(pth,&bar);
        index += strlen(bar);

        nxt_parent = nxt_parent->parent;
    }
    // Null-terminate the final string
    pth[index] = '\0';
    return pth;
}


static void nde_queue_free(Node* node)
{
    iCluige.iDeque.append(&dq_free,node);
}

void debug_dq()
{
    int size = iCluige.iDeque.size(&dq_free);
    printf("dq_free: ");
    for(int i = 0; i < size; i++)
    {
        Node* node = iCluige.iDeque.at(&dq_free,i).ptr;

        printf(" %s |",node->name);
    }
    printf("\n");
}

static void nde_empty_dq_free()
{
    int size = iCluige.iDeque.size(&dq_free);
    debug_dq();
    for(int i = 0; i < size; size--)
    {

        Node* node = iCluige.iDeque.at(&dq_free,i).ptr;

        if(node->next_sibling !=NULL)//removes the link between sibilings else delete_node will delete all siblings
        {
            node->parent->children = node->next_sibling;
            node->next_sibling = NULL;
        }
        else
        {
            node->parent->children = NULL;
        }

        nde_delete_Node(node);
        iCluige.iDeque.remove(&dq_free,i);
        debug_dq();
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
    iCluige.iDeque.deque_alloc(&dq_free, VT_POINTER, 100);


    //iCluige.iNode.initZero = nde_initZero;
    iCluige.iNode.new_Node = nde_new_Node;
    //iCluige.iNode.delete_Node = nde_delete_Node;
    iCluige.iNode.get_index = nde_get_index;
    iCluige.iNode.get_depth = nde_get_depth;
    iCluige.iNode.set_name = nde_set_name;
    iCluige.iNode.add_child = nde_add_child;
    iCluige.iNode.print_tree_pretty = nde_print_tree_pretty;
    iCluige.iNode.get_child = nde_get_child;
    iCluige.iNode.get_node = nde_get_node;
    iCluige.iNode.remove_child = nde_remove_child;
    iCluige.iNode.get_path_mallocing = nde_get_path_mallocing;
    iCluige.iNode.queue_free = nde_queue_free;
    iCluige.iNode.empty_dq_free = nde_empty_dq_free;

}
