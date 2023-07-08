//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
#include "SpriteSVG.h"

//#include <string.h>
#include <assert.h>
#include <curses.h>
//#include <math.h>

////////////////////////////////// _SpriteSVG /////////

static void ssvg_delete_SpriteSVG(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteSVG* this_SpriteSVG = (SpriteSVG*)(this_Node2D->_sub_class);
    void (*delete_Node2D)(Node*) = this_SpriteSVG->delete_Node2D;
    //delete my attributes
    //Deque<Path2D* > paths
    Deque* paths = &(this_SpriteSVG->paths);
    int nb_paths = iCluige.iDeque.size(paths);
    for(int i=0; i<nb_paths; i++)
    {
        Path2D* path_i = iCluige.iDeque.at(paths, i).ptr;
        iCluige.iPath2D.pre_delete_Path2D(path_i);
        free(path_i);
    }
    iCluige.iDeque.pre_delete_Deque(paths);
    //free(paths);

    assert(this_SpriteSVG->_sub_class == NULL);
    free(this_SpriteSVG);
    this_Node2D->_sub_class = NULL;
    delete_Node2D(this_Node);
}

//utility function
static void ssvg_draw_line(const float start_x, const float start_y, const float end_x, const float end_y, bool draw)
{
    Vector2 start_pos = {start_x, start_y};
    Vector2 end_pos = {end_x, end_y};

	LineDrawerThin d;
    iCluige.iLineDrawerThin.start(&d, &start_pos, &end_pos);
	while(!iCluige.iLineDrawerThin.has_finished(&d))
	{
	    if(draw)
        {
            const char* glyph = iCluige.iLineDrawerThin.glyph(&d);
            //mvaddch(d.current_position.y, d.current_position.x, shade(d.curr_dist));
            mvaddstr(d.current_position.y, d.current_position.x, glyph);
        }
        else
        {
            //erase
            mvaddch(d.current_position.y, d.current_position.x, ' ');
        }

		iCluige.iLineDrawerThin.next(&d);
	}
}

static void ssvg_pre_process_Node(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    if(!(this_Node2D->visible))
    {
        return;
    }
    SpriteSVG* this_SpriteSVG = (SpriteSVG*)(this_Node2D->_sub_class);
    //call super()
    this_SpriteSVG->pre_process_Node2D(this_Node);

    //clear old one (unless immobile? => no, because other masking things
    //could have moved and made this sprite visible again;
    //and curses already has characters cache)
    Vector2 orig;
    iCluige.iVector2.substract(
            &(this_Node2D->_tmp_global_position),
            &(this_SpriteSVG->offset),
            &orig);
    float sX = this_SpriteSVG->scale.x;
    float sY = this_SpriteSVG->scale.y;
    Deque* paths = &(this_SpriteSVG->paths);
    int nb_paths = iCluige.iDeque.size(paths);
    for(int i=0; i<nb_paths; i++)
    {
        Path2D* path_i = iCluige.iDeque.at(paths, i).ptr;
        int nb_points = iCluige.iPath2D.size(path_i);
        for(int j=0; j<(nb_points-1); j++)
        {
            Vector2* p1 = iCluige.iPath2D.at(path_i, j);
            Vector2* p2 = iCluige.iPath2D.at(path_i, j+1);
            ssvg_draw_line(
                orig.x + ((p1->x) * sX), orig.y + ((p1->y) * sY),
                orig.x + ((p2->x) * sX), orig.y + ((p2->y) * sY),
                false);
        }
    }
}

static void ssvg_post_process_Node(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteSVG* this_SpriteSVG = (SpriteSVG*)(this_Node2D->_sub_class);
    //call super()
    this_SpriteSVG->post_process_Node2D(this_Node);

    if(!(this_Node2D->visible))
    {
        return;
    }

    //draw new one
    Vector2 orig;
    iCluige.iVector2.substract(
            &(this_Node2D->_tmp_global_position),
            &(this_SpriteSVG->offset),
            &orig);
    float sX = this_SpriteSVG->scale.x;
    float sY = this_SpriteSVG->scale.y;
    Deque* paths = &(this_SpriteSVG->paths);
    int nb_paths = iCluige.iDeque.size(paths);
    for(int i=0; i<nb_paths; i++)
    {
        Path2D* path_i = iCluige.iDeque.at(paths, i).ptr;
        int nb_points = iCluige.iPath2D.size(path_i);
        for(int j=0; j<(nb_points-1); j++)
        {
            Vector2* p1 = iCluige.iPath2D.at(path_i, j);
            Vector2* p2 = iCluige.iPath2D.at(path_i, j+1);
            ssvg_draw_line(
                orig.x + ((p1->x) * sX), orig.y + ((p1->y) * sY),
                orig.x + ((p2->x) * sX), orig.y + ((p2->y) * sY),
                true);
        }
    }
}

////////////////////////////////// iiSpriteSVG /////////

static SpriteSVG* ssvg_new_SpriteSVG()
{
	Node2D* new_Node2D = iCluige.iNode2D.new_Node2D();
	Node* new_Node = new_Node2D->_this_Node;
    SpriteSVG* new_SpriteSVG = iCluige.checked_malloc(sizeof(SpriteSVG));

    new_SpriteSVG->offset = (Vector2) { 0., 0. };
    new_SpriteSVG->scale = (Vector2) { 1., 1. };
    iCluige.iDeque.deque_alloc(&(new_SpriteSVG->paths), VT_POINTER, 3);

	new_SpriteSVG->_this_Node2D = new_Node2D;
	new_SpriteSVG->_sub_class = NULL;
	new_SpriteSVG->delete_Node2D = new_Node->delete_Node;
	new_SpriteSVG->pre_process_Node2D = new_Node->pre_process_Node;
	new_SpriteSVG->post_process_Node2D = new_Node->post_process_Node;

    new_Node2D->_sub_class = new_SpriteSVG;

    free(new_Node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_Node->_class_name = iCluige.iStringBuilder.string_alloc(
            &sb, strlen("NodeNode2DSpriteSVG"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2DSpriteSVG");
    new_Node2D->_sub_class = new_SpriteSVG;

    new_Node->delete_Node = ssvg_delete_SpriteSVG;
    new_Node->pre_process_Node = ssvg_pre_process_Node;
    new_Node->post_process_Node = ssvg_post_process_Node;

    return new_SpriteSVG;
}

static void ssvg_add_path_from_array(SpriteSVG* this_SpriteSVG, Vector2* points, int nb_points)
{
    Path2D* new_path = iCluige.checked_malloc(sizeof(Path2D));
    iCluige.iPath2D.path2D_alloc(new_path, points, nb_points, true);
    iCluige.iDeque.push_back(&(this_SpriteSVG->paths), new_path);
}

static void ssvg_add_path_from_array_relative(SpriteSVG* this_SpriteSVG, Vector2* points, int nb_points)
{
    Path2D* new_path = iCluige.checked_malloc(sizeof(Path2D));
    iCluige.iPath2D.path2D_alloc(new_path, points, nb_points, false);
    iCluige.iDeque.push_back(&(this_SpriteSVG->paths), new_path);
}

/////////////////////////////////// Node //////////

void iiSpriteSVG_init()
{
    iCluige.iSpriteSVG.new_SpriteSVG = ssvg_new_SpriteSVG;
    iCluige.iSpriteSVG.add_path_from_array = ssvg_add_path_from_array;
    iCluige.iSpriteSVG.add_path_from_array_relative = ssvg_add_path_from_array_relative;
}

