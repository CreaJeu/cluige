//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "Node2D.h"

#include <string.h>
#include <assert.h>

////////////////////////////////// _Node2D /////////

static void n2d_delete_Node2D(Node* this_Node)
{
    struct _Node2D* this_Node2D = (struct _Node2D*)(this_Node->_sub_class);
    void (*delete_Node)(Node*) = this_Node2D->delete_Node;
    assert(this_Node2D->_sub_class == NULL);
    free(this_Node2D);
    this_Node->_sub_class = NULL;
    delete_Node(this_Node);
}

static void n2d_pre_process_Node(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    this_Node2D->_local_position_changed = false;
}

static void n2d_post_process_Node(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    Node* n = this_Node;
    bool must_update_global_pos = false;
    while((n->parent != NULL) && !must_update_global_pos)
    {
        void* n_subclass = n->_sub_class;
        if(n_subclass != NULL)
        {
            char* found = strstr(n->_class_name, "NodeNode2D");
            if(found == n->_class_name) //n is a Node2D
            {
                Node2D* n2d = (Node2D*)(n->_sub_class);
                must_update_global_pos = n2d->_local_position_changed;
            }
        }
        n = n->parent;
    }
    if(must_update_global_pos)
    {
        n = this_Node;
        this_Node2D->_tmp_global_position = (Vector2){0, 0};
        while(n->parent != NULL)
        {
            void* n_subclass = n->_sub_class;
            if(n_subclass != NULL)
            {
                char* found = strstr(n->_class_name, "NodeNode2D");
                if(found == n->_class_name) //n is a Node2D
                {
                    Node2D* n2d = (Node2D*)(n->_sub_class);
                    iCluige.iVector2.add(
                        &(this_Node2D->_tmp_global_position),
                        &(n2d->position),
                        &(this_Node2D->_tmp_global_position)
                        );
                }
            }
            n = n->parent;
        }
    }
}

static void n2d_enter_tree_Node2D(Node* this_Node)
{
    struct _Node2D* this_Node2D = (struct _Node2D*)(this_Node->_sub_class);
    this_Node2D->enter_tree_Node(this_Node);//calls script.enter_tree()
}

static void n2d_on_loop_starting_Node2D(Node* this_Node)
{
    struct _Node2D* this_Node2D = (struct _Node2D*)(this_Node->_sub_class);
    this_Node2D->_local_position_changed = true;
    n2d_post_process_Node(this_Node);//compute _tmp_global_position
}


////////////////////////////////// iiNode2D /////////

static struct _Node2D* n2d_new_Node2D()
{
    Node* new_node = iCluige.iNode.new_Node();
    struct _Node2D* new_node2D = iCluige.checked_malloc(sizeof(Node2D));

    new_node2D->visible = true;
    new_node2D->position = (Vector2){0., 0.};
    new_node2D->_tmp_global_position = (Vector2){0., 0.};
    new_node2D->_local_position_changed = false;

    new_node2D->_this_Node = new_node;
    new_node2D->_sub_class = NULL;
    new_node2D->delete_Node = new_node->delete_Node;
    new_node2D->delete_Node2D = n2d_delete_Node2D;
    new_node2D->enter_tree_Node = new_node->enter_tree;
    new_node->enter_tree = n2d_enter_tree_Node2D;

    new_node->_sub_class = new_node2D;

    free(new_node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_node->_class_name = iCluige.iStringBuilder.string_alloc(&sb, strlen("NodeNode2D"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2D");
    new_node->_sub_class = new_node2D;

    new_node->delete_Node = n2d_delete_Node2D;
    new_node->pre_process_Node = n2d_pre_process_Node;
    new_node->post_process_Node = n2d_post_process_Node;
    new_node->on_loop_starting = n2d_on_loop_starting_Node2D;

    return new_node2D;
}

static void n2d_show(Node2D* this_Node2D)
{
    this_Node2D->visible = true;
}

static void n2d_hide(Node2D* this_Node2D)
{
    this_Node2D->visible = false;
}

static void n2d_move_local(Node2D* this_Node2D, Vector2 depl)
{
    iCluige.iVector2.add(
        &(this_Node2D->position),
        &(depl),
        &(this_Node2D->position)
        );
    this_Node2D->_local_position_changed = true;
}

static void n2d_set_local_position(Node2D* this_Node2D, Vector2 new_pos)
{
    this_Node2D->position = new_pos;
    this_Node2D->_local_position_changed = true;
}

static void n2d_deserialize_dico(Node2D* this_Node2D, const SortedDictionary* params)
{
    //mother class
    iCluige.iNode.deserialize_dico(this_Node2D->_this_Node, params);

    utils_bool_from_parsed(&(this_Node2D->visible), params, "visible");
    utils_vector2_from_parsed(&(this_Node2D->position), params, "position");
    // TODO scale : utils_vector2_from_parsed(&(this_Node2D->scale), params, "scale");
    //maybe one day : rotation = 1.11177
}

/////////////////////////////////// Node //////////

void iiNode2D_init()
{
    iCluige.iNode2D.new_Node2D = n2d_new_Node2D;
    iCluige.iNode2D.show = n2d_show;
    iCluige.iNode2D.hide = n2d_hide;
    iCluige.iNode2D.move_local = n2d_move_local;
    iCluige.iNode2D.set_local_position = n2d_set_local_position;
    iCluige.iNode2D.deserialize_dico = n2d_deserialize_dico;
}

