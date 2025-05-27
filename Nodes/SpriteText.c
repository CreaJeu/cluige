//#include <stdlib.h> //already in cluige.h
//#include <stddef.h> //already in cluige.h
#include "../cluige.h"
//#include "Node.h" //already in cluige.h
#include "SpriteText.h"

#include <string.h>
#include <curses.h>
#include <math.h>

////////////////////////////////// _SpriteText /////////

static void sprtx_delete_SpriteText(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteText* this_SpriteText = (SpriteText*)(this_Node2D->_sub_class);
    //tmp memorize function pointer before calling free(this)
    void (*delete_super)(Node*) = this_SpriteText->_delete_super;
//    for(int i=0; i<this_SpriteText->nb_lines; i++)
//    {
//        free(this_SpriteText->text[i]);
//    }
    free(this_SpriteText->text);
    CLUIGE_ASSERT(this_SpriteText->_sub_class == NULL, "SpriteText::delete_SpriteText() : not null subclass found");
    free(this_SpriteText);
    this_Node2D->_sub_class = NULL;
    delete_super(this_Node);
}

//static void sprtx_enter_tree_SpriteText(Node* this_Node)
//{
//    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
//    SpriteText* this_SpriteText = (SpriteText*)(this_Node2D->_sub_class);
//	CLUIGE_ASSERT(
//		this_SpriteText->_allocated_text_length >= 0,
//		"SpriteText::enter_tree() : text must be given before entering tree");
//	this_SpriteText->enter_tree_Node2D(this_Node);//super()
//}

static void sprtx_erase(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteText* this_SpriteText = (SpriteText*)(this_Node2D->_sub_class);

    bool must_erase = this_Node2D->_old_baked.visible;//was visible last time
    must_erase = must_erase &&
        (
            this_Node2D->_position_changed ||
            iCluige.iCamera2D._old_baked.state_changed ||
            (!(this_Node2D->visible)) ||
            this_SpriteText->_text_changed ||
            this_Node->_marked_for_queue_free
        );
    if(!must_erase)
    {
//        this_SpriteText->_erase_super(this_Node);//NULL
        return;
    }

    //clear old one
    Vector2 orig;
    iCluige.iVector2.add(
            &(this_Node2D->_old_baked.tmp_global_position),
            &(this_SpriteText->offset),
            &orig);
    int flat_i = 0;

    float x_camera = iCluige.iCamera2D._old_baked._tmp_limited_offseted_global_position.x;
    float y_camera = iCluige.iCamera2D._old_baked._tmp_limited_offseted_global_position.y;
    float res_x;
    float res_y;
    float res_zoom_x;
    float res_zoom_y;
    Vector2 zoom = iCluige.iCamera2D._old_baked.zoom;

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
                if(!iCluige.iCamera2D._old_baked.ignore_rotation)
                {
                    //float rotation_angle = -iCluige.iCamera2D._state_changes.rotation;
                    float cf =  iCluige.iCamera2D._old_baked.global_tmp_cos_rotation;
                    float sf = iCluige.iCamera2D._old_baked.global_tmp_sin_rotation;

                    if(iCluige.iCamera2D._old_baked.anchor_mode == ANCHOR_MODE_DRAG_CENTER)//rotation around center of screen (camera in center)
                    {
                        float drag_center_offset_x = iCluige.iCamera2D._old_baked._screen_anchor_center_x;
                        float drag_center_offset_y = iCluige.iCamera2D._old_baked._screen_anchor_center_y;

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

    //call super()
//    this_SpriteText->_erase_super(this_Node);//NULL
    this_SpriteText->_text_changed = false;
}

static void sprtx_draw(Node* this_Node)
{
    Node2D* this_Node2D = (Node2D*)(this_Node->_sub_class);
    SpriteText* this_SpriteText = (SpriteText*)(this_Node2D->_sub_class);
    //call super()
//    this_SpriteText->_post_process_super(this_Node);//no, just pre_draw in Node2D, not overridden here

    if(!(this_Node2D->visible))
    {
        return;
    }

    //clear old one (unless immobile? => no, because other masking things
    //could have moved and made this sprite visible again;
    //and curses already has characters cache)
    Vector2 orig;
    iCluige.iVector2.add(
            &(this_Node2D->_new_baked.tmp_global_position),
            &(this_SpriteText->offset),
            &orig);
    int flat_i = 0;
    Camera2D* current_camera = iCluige.iCamera2D.current_camera;
    CLUIGE_ASSERT(current_camera != NULL, "SpriteText::draw() : current_camera is null");

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
                        float drag_center_offset_x = iCluige.iCamera2D._screen_anchor_center_x;
                        float drag_center_offset_y = iCluige.iCamera2D._screen_anchor_center_y;

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
    new_SpriteText->_allocated_text_length = -1;
    new_SpriteText->nb_lines = 0;
	new_SpriteText->_this_Node2D = new_Node2D;
	new_SpriteText->_sub_class = NULL;
	new_SpriteText->_text_changed = true;

	//virtual methods - private copies of mother class pointers
	//	constructors of all ancestors in inheritance hierarchy
	//	have already been called, so :
	//	new_Node virtual methods pointers are already pointing to
	//	overriding methods (if any)
	new_SpriteText->_delete_super = new_Node->delete_Node;
//	new_SpriteText->_enter_tree_super = new_Node->enter_tree;
//	new_SpriteText->_erase_super = new_Node->erase;//NULL
//	new_SpriteText->_post_process_super = new_Node->post_process;

    new_Node2D->_sub_class = new_SpriteText;

    free(new_Node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_Node->_class_name = iCluige.iStringBuilder.string_alloc(
            &sb, strlen("NodeNode2DSpriteText"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2DSpriteText");
    new_Node2D->_sub_class = new_SpriteText;

    new_Node->delete_Node = sprtx_delete_SpriteText;
//    new_Node->enter_tree = sprtx_enter_tree_SpriteText;
    new_Node->erase = sprtx_erase;
    new_Node->draw = sprtx_draw;

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
	int new_total_length = strlen(new_text);
	if(this_SpriteText->_allocated_text_length < new_total_length)
	{
		if(this_SpriteText->text != NULL)
		{
			free(this_SpriteText->text);
		}
		this_SpriteText->text = iCluige.checked_malloc((new_total_length + 1) * sizeof(char));
		this_SpriteText->_allocated_text_length = new_total_length;
	}
	strcpy(this_SpriteText->text, new_text);
    sprtx__bake_text(this_SpriteText);
    this_SpriteText->_text_changed = true;
}

static Node* sprtx_instanciate(const SortedDictionary* params)
{
    //mother class
    Node* res_node = iCluige.iNode2D._Node2D_factory.instanciate(params);
    Node2D* res_node2D = (Node2D*)(res_node->_sub_class);
    SpriteText* res_SpriteText = sprtx_new_SpriteText_from_Node2D(res_node2D);

    //offset = Vector2(2.265, -3.2)
    //text = "un autre label
    //sans param modifi�
    //� part texte et pos"
    utils_vector2_from_parsed(&(res_SpriteText->offset), params, "offset");
    CLUIGE_ASSERT(res_SpriteText->text == NULL, "SpriteText::instanciate() : trying to instanciate() into non empty object");
    utils_str_from_parsed(&(res_SpriteText->text), params, "text");
    CLUIGE_ASSERT(res_SpriteText->text != NULL, "SpriteText::instanciate() : text not instanciated, missing 'text' field?");
    res_SpriteText->_allocated_text_length = strlen(res_SpriteText->text);
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

