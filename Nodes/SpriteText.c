//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "SpriteText.h"

#include <string.h>
#include <assert.h>
#include <curses.h>
#include <math.h>

////////////////////////////////// _SpriteText /////////

static void sprtx_delete_SpriteText(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteText* this_SpriteText = (SpriteText*)(this_Node2D->_sub_class);
    void (*delete_Node2D)(Node*) = this_SpriteText->delete_Node2D;
//    for(int i=0; i<this_SpriteText->nb_lines; i++)
//    {
//        free(this_SpriteText->text[i]);
//    }
	//clear display
	if(this_Node2D->visible && !(iCluige.quit_asked))
	{
		this_Node->pre_process_Node(this_Node);
	}
    free(this_SpriteText->text);
    assert(this_SpriteText->_sub_class == NULL);
    free(this_SpriteText);
    this_Node2D->_sub_class = NULL;
    delete_Node2D(this_Node);
}

static void sprtx_pre_process_Node(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteText* this_SpriteText = (SpriteText*)(this_Node2D->_sub_class);
    //call super()
    this_SpriteText->pre_process_Node2D(this_Node);

    if(!(this_Node2D->visible))
    {
        return;
    }

    //clear old one (unless immobile? => no, because other masking things
    //could have moved and made this sprite visible again;
    //and curses already has characters cache)
    Vector2 orig;
    iCluige.iVector2.add(
            &(this_Node2D->_tmp_global_position),
            &(this_SpriteText->offset),
            &orig);
    int flat_i = 0;
    Camera2D* current_camera = iCluige.iCamera2D.current_camera;
    assert(current_camera != NULL);

    float x_camera = current_camera->_tmp_limited_offseted_global_position.x;
    float y_camera = current_camera->_tmp_limited_offseted_global_position.y;
    float res_x;
    float res_y;
    float res_zoom_x;
    float res_zoom_y;
    Vector2 zoom = current_camera->zoom;

    for(int line = 0; line < this_SpriteText->nb_lines; line++)
    {
        int col = 0;
        //char* lineString = (char*)(this_SpriteText->text[line]);
        //char curr_char = lineString[col];
        char curr_char = this_SpriteText->text[flat_i];

        while(curr_char != 0)
        {
            if(curr_char != ' ')
            {
                res_zoom_x = ((lrintf(orig.x) + col) - x_camera) * zoom.x;
                res_zoom_y = ((lrintf(orig.y) + line) - y_camera) *zoom.y ;
                if(!current_camera->ignore_rotation)
                {
                    //float rotation_angle = -current_camera->rotation;
                    float cf =  current_camera->global_tmp_cos_rotation;
                    float sf = current_camera->global_tmp_sin_rotation;

                    if(current_camera->anchor_mode == ANCHOR_MODE_DRAG_CENTER)//rotation around center of screen (camera in center)
                    {
                        float drag_center_offset_x = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_X;
                        float drag_center_offset_y = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_Y;

                        //substract the point you want to turn around (here the camera offset )

                        res_x = (res_zoom_x - drag_center_offset_x)  * cf - (res_zoom_y - drag_center_offset_y)  * sf;
                        res_y = (res_zoom_y - drag_center_offset_x)  * sf + (res_zoom_y - drag_center_offset_y)  * cf;

                        res_x = res_x + drag_center_offset_x;
                        res_y = res_y + drag_center_offset_y;
                    }
                    else//rotation around corner left
                    {
                        res_x = res_zoom_x  * cf - res_zoom_y  * sf;
                        res_y = res_zoom_x  * sf + res_zoom_y  * cf;
                    }
                }
                else
                {
                    res_x = res_zoom_x;
                    res_y = res_zoom_y;
                }
                mvaddch(res_y,res_x , ' ');
            }

            col++;
            //curr_char = lineString[col];
            flat_i++;
            curr_char = this_SpriteText->text[flat_i];
        }
        flat_i++;
    }
}

static void sprtx_post_process_Node(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteText* this_SpriteText = (SpriteText*)(this_Node2D->_sub_class);
    //call super()
    this_SpriteText->post_process_Node2D(this_Node);

    if(!(this_Node2D->visible))
    {
        return;
    }

    //clear old one (unless immobile? => no, because other masking things
    //could have moved and made this sprite visible again;
    //and curses already has characters cache)
    Vector2 orig;
    iCluige.iVector2.add(
            &(this_Node2D->_tmp_global_position),
            &(this_SpriteText->offset),
            &orig);
    int flat_i = 0;
    Camera2D* current_camera = iCluige.iCamera2D.current_camera;
    assert(current_camera != NULL);

    float x_camera = current_camera->_tmp_limited_offseted_global_position.x;
    float y_camera = current_camera->_tmp_limited_offseted_global_position.y;
    float res_x;
    float res_y;
    float res_zoom_x;
    float res_zoom_y;
    Vector2 zoom = current_camera->zoom;

    for(int line = 0; line < this_SpriteText->nb_lines; line++)
    {
        int col = 0;
        //char* lineString = (char*)(this_SpriteText->text[line]);
        //char curr_char = lineString[col];
        char curr_char = this_SpriteText->text[flat_i];

        while(curr_char != 0)
        {
            if(curr_char != ' ')
            {
                res_zoom_x = ((lrintf(orig.x) + col) - x_camera) * zoom.x;
                res_zoom_y = ((lrintf(orig.y) + line) - y_camera) *zoom.y ;
                if(!current_camera->ignore_rotation)
                {
                    //float rotation_angle = -current_camera->rotation;
                    float cf =  current_camera->global_tmp_cos_rotation;
                    float sf = current_camera->global_tmp_sin_rotation;

                    if(current_camera->anchor_mode == ANCHOR_MODE_DRAG_CENTER)//rotation around center of screen (camera in center)
                    {
                        float drag_center_offset_x = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_X;
                        float drag_center_offset_y = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_Y;

                        //substract the point you want to turn around (here the camera offset )

                        res_x = (res_zoom_x - drag_center_offset_x)  * cf - (res_zoom_y - drag_center_offset_y)  * sf;
                        res_y = (res_zoom_y - drag_center_offset_x)  * sf + (res_zoom_y - drag_center_offset_y)  * cf;

                        res_x = res_x + drag_center_offset_x;
                        res_y = res_y + drag_center_offset_y;
                    }
                    else//rotation around corner left
                    {
                        res_x = res_zoom_x  * cf - res_zoom_y  * sf;
                        res_y = res_zoom_x  * sf + res_zoom_y  * cf;
                    }
                }
                else
                {
                    res_x = res_zoom_x;
                    res_y = res_zoom_y;
                }
                mvaddch(res_y,res_x , curr_char);
            }

            col++;
            //curr_char = lineString[col];
            flat_i++;
            curr_char = this_SpriteText->text[flat_i];
        }
        flat_i++;
    }
}


////////////////////////////////// iiSpriteText /////////

static SpriteText* sprtx_new_SpriteText_from_Node2D(Node2D* new_Node2D)
{
	Node* new_Node = new_Node2D->_this_Node;
    SpriteText* new_SpriteText = iCluige.checked_malloc(sizeof(SpriteText));

    new_SpriteText->offset = (Vector2) { 0., 0. };
    new_SpriteText->text = NULL;//the ASCII art / unicode art
    new_SpriteText->nb_lines = 0;
	new_SpriteText->_this_Node2D = new_Node2D;
	new_SpriteText->_sub_class = NULL;
	new_SpriteText->delete_Node2D = new_Node->delete_Node;
	new_SpriteText->pre_process_Node2D = new_Node->pre_process_Node;
	new_SpriteText->post_process_Node2D = new_Node->post_process_Node;

    new_Node2D->_sub_class = new_SpriteText;

    free(new_Node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_Node->_class_name = iCluige.iStringBuilder.string_alloc(
            &sb, strlen("NodeNode2DSpriteText"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2DSpriteText");
    new_Node2D->_sub_class = new_SpriteText;

    new_Node->delete_Node = sprtx_delete_SpriteText;
    new_Node->pre_process_Node = sprtx_pre_process_Node;
    new_Node->post_process_Node = sprtx_post_process_Node;

    return new_SpriteText;
}

static SpriteText* sprtx_new_SpriteText()
{
	Node2D* new_Node2D = iCluige.iNode2D.new_Node2D();
	return sprtx_new_SpriteText_from_Node2D(new_Node2D);
}

//private util
static void sprtx__bake_text(SpriteText* this_SpriteText)
{
    int total_length = strlen(this_SpriteText->text);
    this_SpriteText->nb_lines = 0;
    for(int i=0; i<total_length; i++)
    {
        if(this_SpriteText->text[i] == '\n')
        {
            this_SpriteText->text[i] = '\0';
            this_SpriteText->nb_lines++;
        }
    }
    if(total_length > 0)
    {
        this_SpriteText->nb_lines++;
    }
}

static void sprtx_set_text(SpriteText* this_SpriteText, const char* new_text)
{
    if(this_SpriteText->text != NULL)
    {
//        for(int i=0; i<this_SpriteText->nb_lines; i++)
//        {
//            free(this_SpriteText->text[i]);
//        }
        free(this_SpriteText->text);
    }
//    this_SpriteText->text =
//        iCluige.iStringBuilder.split(new_text, "\n", &(this_SpriteText->nb_lines));
    int total_length = strlen(new_text);
    StringBuilder sb;
    this_SpriteText->text = iCluige.iStringBuilder.string_alloc(&sb, total_length);
    iCluige.iStringBuilder.append(&sb, new_text);
    sprtx__bake_text(this_SpriteText);
}

static Node* sprtx_instanciate(const SortedDictionary* params)
{
    //mother class
    Node* res_node = iCluige.iNode2D._Node2D_factory.instanciate(params);
    Node2D* res_node2D = (Node2D*)(res_node->_sub_class);
    SpriteText* res_SpriteText = sprtx_new_SpriteText_from_Node2D(res_node2D);

    //offset = Vector2(2.265, -3.2)
    //text = "un autre label
    //sans param modifié
    //à part texte et pos"
    utils_vector2_from_parsed(&(res_SpriteText->offset), params, "offset");
    assert(res_SpriteText->text == NULL);
    utils_str_from_parsed(&(res_SpriteText->text), params, "text");
    assert(res_SpriteText->text != NULL);
    sprtx__bake_text(res_SpriteText);
    return res_node;
}

/////////////////////////////////// global //////////

void iiSpriteText_init()
{
    iCluige.iSpriteText.new_SpriteText = sprtx_new_SpriteText;
    iCluige.iSpriteText.new_SpriteText_from_Node2D = sprtx_new_SpriteText_from_Node2D;
    iCluige.iSpriteText.set_text = sprtx_set_text;

    SortedDictionary* fcties = &(iCluige.iNode.node_factories);
    NodeFactory* fcty = &(iCluige.iSpriteText._SpriteText_factory);
    fcty->instanciate = sprtx_instanciate;

    char* fcty_key = iCluige.checked_malloc(6 * sizeof(char));
    strncpy(fcty_key, "Label", 6);
    iCluige.iSortedDictionary.insert(fcties, fcty_key, fcty);
}

