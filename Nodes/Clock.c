//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "Clock.h"

#include <string.h> //TODO static instead of malloc'ed class name

////////////////////////////////// _Clock /////////

static void clk_delete_Clock(Node* this_Node)
{
    struct _Clock* this_Clock = (struct _Clock*)(this_Node->_sub_class);
    void (*delete_Node)(Node*) = this_Clock->delete_Node;
    free(this_Clock);
    this_Node->_sub_class = NULL;
    delete_Node(this_Node);
}

static void clk_pre_process_Clock(Node* this_Node)
{
    struct _Clock* this_Clock = (struct _Clock*)(this_Node->_sub_class);
    clock_t new_tick = clock();
    float d_sec = (float)(new_tick - this_Clock->_last_tick) / CLOCKS_PER_SEC;
    this_Clock->elapsed_seconds = this_Clock->scale * d_sec;
    this_Clock->_last_tick = new_tick;
}


////////////////////////////////// iiClock /////////

static struct _Clock* clk_new_Clock()
{
    Node* new_node = iCluige.iNode.new_Node();
    struct _Clock* new_clock = iCluige.checked_malloc(sizeof(struct _Clock));

    new_clock->scale = 1.;
    new_clock->elapsed_seconds = 0.;
    new_clock->_last_tick = clock();
    new_clock->_this_Node = new_node;
    new_clock->delete_Node = new_node->delete_Node;
    //new_clock->delete_Clock = clk_delete_Clock;
    //new_clock->pre_process_Clock = clk_pre_process_Clock;

    new_node->_sub_class = new_clock;

    free(new_node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_node->_class_name = iCluige.iStringBuilder.string_alloc(&sb, strlen("NodeClock"));
    iCluige.iStringBuilder.append(&sb, "NodeClock");

    new_node->delete_Node = clk_delete_Clock;
    new_node->pre_process_Node = clk_pre_process_Clock;

    return new_clock;
}

/////////////////////////////////// Node //////////

void iiClock_init()
{
    iCluige.iClock.new_Clock = clk_new_Clock;
}

