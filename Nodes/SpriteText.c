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
    iCluige.iVector2.substract(
            &(this_Node2D->_tmp_global_position),
            &(this_SpriteText->offset),
            &orig);
    int flat_i = 0;
    Camera2D* current_camera = iCluige.iCamera2D.current_camera;
    assert(current_camera != NULL);

    int x_camera = current_camera->_tmp_limited_offseted_global_position.x;
    int y_camera = current_camera->_tmp_limited_offseted_global_position.y;

    int res_x;
    int res_y;

    Vector2 zoom = current_camera->zoom;

    for(int line = 0; line < this_SpriteText->nb_lines; line++)
    {
        int col = 0;
        //char* lineString = (char*)(this_SpriteText->text[line]);
        //char curr_char = lineString[col];
        char curr_char = this_SpriteText->text[flat_i];

        res_y =lrintf(orig.y) + line;
        while(curr_char != 0)
        {

            if(curr_char != ' ')
            {

                res_x =lrintf(orig.x) + col;
                mvaddch((res_y - y_camera) *zoom.y , (res_x - x_camera) * zoom.x, ' ');
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

    //draw new one
    Vector2 orig;
    iCluige.iVector2.substract(
            &(this_Node2D->_tmp_global_position),
            &(this_SpriteText->offset),
            &orig);
    int flat_i = 0;
    Camera2D* current_camera = iCluige.iCamera2D.current_camera;
    assert(current_camera != NULL);

    int x_camera = current_camera->_tmp_limited_offseted_global_position.x;
    int y_camera = current_camera->_tmp_limited_offseted_global_position.y;

    int res_x;
    int res_y;

    Vector2 zoom = current_camera->zoom;

    for(int line = 0; line < this_SpriteText->nb_lines; line++)
    {
        int col = 0;
        //char* lineString = (char*)(this_SpriteText->text[line]);
        //char curr_char = lineString[col];
        char curr_char = this_SpriteText->text[flat_i];

        res_y =lrintf(orig.y) + line;
        while(curr_char != 0)
        {

            if(curr_char != ' ')
            {

                res_x =lrintf(orig.x) + col;
                mvaddch((res_y - y_camera) *zoom.y , (res_x - x_camera) * zoom.x, curr_char);
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

static SpriteText* sprtx_new_SpriteText()
{
	Node2D* new_Node2D = iCluige.iNode2D.new_Node2D();
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
    this_SpriteText->nb_lines = 0;
    StringBuilder sb;
    this_SpriteText->text = iCluige.iStringBuilder.string_alloc(&sb, total_length);
    iCluige.iStringBuilder.append(&sb, new_text);
    for(int i=0; i<total_length; i++)
    {
        if(new_text[i] == '\n')
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

/////////////////////////////////// Node //////////

void iiSpriteText_init()
{
    iCluige.iSpriteText.new_SpriteText = sprtx_new_SpriteText;
    iCluige.iSpriteText.set_text = sprtx_set_text;
}

