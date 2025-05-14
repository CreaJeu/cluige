//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "Node2D.h"

#include <string.h>

////////////////////////////////// _Node2D /////////

static void n2d_delete_Node2D(Node* this_Node)
{
    struct _Node2D* this_Node2D = (struct _Node2D*)(this_Node->_sub_class);
    //tmp memorize function pointer before calling free(this)
    void (*delete_super)(Node*) = this_Node2D->_delete_super;
    CLUIGE_ASSERT(this_Node2D->_sub_class == NULL, "Node2D::delete_Node2D() : not null subclass found");
    free(this_Node2D);
    this_Node->_sub_class = NULL;
    delete_super(this_Node);
}

static void n2d_erase(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    this_Node2D->_state_changes.visible = this_Node2D->visible;
    this_Node2D->_state_changes.position_changed = false;
    this_Node2D->_erase_super(this_Node);
}

static void n2d_pre_draw(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    Node* n = this_Node;
    bool must_update_global_pos = false;
    while((n->parent != NULL) && !must_update_global_pos)//TODO benefit from DFS, see cluige::process_tree()
    {//first Node2D parent was already updated
        void* n_subclass = n->_sub_class;
        if(n_subclass != NULL)
        {
            char* found = strstr(n->_class_name, "NodeNode2D");
            if(found == n->_class_name) //n is a Node2D
            {
                Node2D* n2d = (Node2D*)(n->_sub_class);
                must_update_global_pos = n2d->_state_changes.position_changed;
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
        this_Node2D->_state_changes.position_changed = true;
    }
}

//static void n2d_enter_tree_Node2D(Node* this_Node)
//{
//    struct _Node2D* this_Node2D = (struct _Node2D*)(this_Node->_sub_class);
//    this_Node2D->enter_tree_Node(this_Node);//calls script.enter_tree()
//}

static void n2d_on_loop_starting_Node2D(Node* this_Node)
{
    struct _Node2D* this_Node2D = (struct _Node2D*)(this_Node->_sub_class);
    this_Node2D->_state_changes.position_changed = true;
    n2d_pre_draw(this_Node);//compute _tmp_global_position
}


////////////////////////////////// iiNode2D /////////

static Node2D* n2d_new_Node2D_from_Node(Node* new_node)
{
    struct _Node2D* new_node2D = iCluige.checked_malloc(sizeof(Node2D));

    new_node2D->visible = true;
    new_node2D->position = (Vector2){0., 0.};
    new_node2D->_tmp_global_position = (Vector2){0., 0.};

    new_node2D->_state_changes.visible = new_node2D->visible;
    new_node2D->_state_changes.position_changed = true;

    new_node2D->_this_Node = new_node;
    new_node2D->_sub_class = NULL;

	//virtual methods - private copies of mother class pointers
    new_node2D->_delete_super = new_node->delete_Node;
    new_node2D->_erase_super = new_node->erase;
//    new_node2D->_post_process_super = new_node->post_process;
//    new_node2D->delete_Node2D = n2d_delete_Node2D;
//    new_node2D->enter_tree_Node = new_node->enter_tree;
//    new_node->enter_tree = n2d_enter_tree_Node2D;

    new_node->_sub_class = new_node2D;

    free(new_node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_node->_class_name = iCluige.iStringBuilder.string_alloc(&sb, strlen("NodeNode2D"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2D");
    new_node->_sub_class = new_node2D;

    new_node->delete_Node = n2d_delete_Node2D;
    new_node->erase = n2d_erase;
    new_node->pre_draw = n2d_pre_draw;
    new_node->on_loop_starting = n2d_on_loop_starting_Node2D;

    return new_node2D;
}

static struct _Node2D* n2d_new_Node2D()
{
    Node* new_node = iCluige.iNode.new_Node();
    return n2d_new_Node2D_from_Node(new_node);
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
    this_Node2D->_state_changes.position_changed = true;
}

static void n2d_set_local_position(Node2D* this_Node2D, Vector2 new_pos)
{
    this_Node2D->position = new_pos;
    this_Node2D->_state_changes.position_changed = true;
}

static Node* n2d_instanciate(const SortedDictionary* params)
{
    //mother class
    Node* res_node = iCluige.iNode._Node_factory.instanciate(params);
    Node2D* res_node2D = n2d_new_Node2D_from_Node(res_node);

    utils_bool_from_parsed(&(res_node2D->visible), params, "visible");
    utils_vector2_from_parsed(&(res_node2D->position), params, "position");
    // TODO scale : utils_vector2_from_parsed(&(this_Node2D->scale), params, "scale");
    //maybe one day : rotation = 1.11177
    return res_node;
}

/////////////////////////////////// Node //////////

void iiNode2D_init()
{
    iCluige.iNode2D.new_Node2D = n2d_new_Node2D;
    iCluige.iNode2D.new_Node2D_from_Node = n2d_new_Node2D_from_Node;
    iCluige.iNode2D.show = n2d_show;
    iCluige.iNode2D.hide = n2d_hide;
    iCluige.iNode2D.move_local = n2d_move_local;
    iCluige.iNode2D.set_local_position = n2d_set_local_position;

    SortedDictionary* fcties = &(iCluige.iNode.node_factories);
    NodeFactory* fcty = &(iCluige.iNode2D._Node2D_factory);
    fcty->instanciate = n2d_instanciate;

    char* fcty_key = iCluige.checked_malloc(7 * sizeof(char));
    strncpy(fcty_key, "Node2D", 7);
    iCluige.iSortedDictionary.insert(fcties, fcty_key, fcty);
}

