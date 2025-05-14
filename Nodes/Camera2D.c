
#include "../cluige.h"
#include "Camera2D.h"

#include <string.h>
#include <math.h>


////////////////////////////////// _Camera2D /////////

static Node* c2d__highest_node_excluding_one_nde(Node* parent, const char* class_name, const Node* camera_node);

//given in degrees not radians
static void c2d_set_rotation_degrees(Camera2D* c2d, float angle_degrees)
{
    CLUIGE_ASSERT(c2d != NULL, "Camera2D::set_rotation_degrees() : calling object is null");
    //float rota = clamp_float(rotation_angle_degrees,-360,360);
    while(angle_degrees < -360)
    {
        angle_degrees += 360;
    }
    while(360 < angle_degrees)
    {
        angle_degrees -= 360;
    }

    c2d->rotation_radians = angle_degrees * 3.1415926 / 180;
    c2d->global_tmp_cos_rotation = cosf(c2d->rotation_radians);
    c2d->global_tmp_sin_rotation = sinf(c2d->rotation_radians);
}

//in degrees
static float c2d_get_rotation_degrees(Camera2D* c2d)
{
    CLUIGE_ASSERT(c2d != NULL, "Camera2D::get_rotation_degrees() : calling object is null");
    return c2d->rotation_radians * 180 / 3.1415926;
}

//TODO not finished
static void c2d_make_current(Camera2D* c2d)
{
    CLUIGE_ASSERT(c2d != NULL, "Camera2D::make_current() : calling object is null");
    CLUIGE_ASSERT(c2d->enabled, "Camera2D::make_current() : calling object is not enabled");

    iCluige.iCamera2D.current_camera = c2d;
    c2d->_this_Node2D->_state_changes.position_changed = true;
}

//private uitl
static Camera2D* c2d__get_camera_from_node(Node* this_node)
{
    CLUIGE_ASSERT(this_node != NULL, "Camera2D::_get_camera_from_node() : calling object is null");
    Node2D* nde_2d = (Node2D*)(this_node->_sub_class);
    CLUIGE_ASSERT(nde_2d != NULL, "Camera2D::_get_camera_from_node() : calling object _sub_class is null");
    Camera2D* nde_cam_2d = (Camera2D*) (nde_2d->_sub_class);
    CLUIGE_ASSERT(nde_cam_2d != NULL, "Camera2D::_get_camera_from_node() : calling object _sub_class->_sub_class is null");
    return nde_cam_2d;
}

static void c2d_delete_Camera2D(Node* this_node)
{
    CLUIGE_ASSERT(this_node != NULL, "Camera2D::delete_camera2d() : calling object is null");
    Node2D* this_node2d = (Node2D*)(this_node->_sub_class);
    Camera2D* this_cam2d = (Camera2D*)(this_node2d->_sub_class);

    //tmp memorize function pointer before calling free(this)
    void (*delete_super)(Node*) = this_cam2d->_delete_super;

//    if(iCluige.iCamera2D.current_camera == this_cam2d && iCluige.iCamera2D.default_camera != iCluige.iCamera2D.current_camera)
//    {
//        Node* node_found = c2d__highest_node_excluding_one_nde(iCluige.public_root_2D,"NodeNode2DCamera2D",this_node);
//        if(node_found != NULL)
//        {
//            c2d_make_current(c2d__get_camera_from_node(node_found));
//        }
//        else
//        {
//            //default camera
//            c2d_make_current(iCluige.iCamera2D.default_camera);
//        }
//    }
    CLUIGE_ASSERT(this_cam2d->_sub_class == NULL, "Camera2D::delete_camera2d() : not null subclass found");
    free(this_cam2d);
    this_node2d->_sub_class = NULL;
    delete_super(this_node);
}

static Vector2 c2d_get_zoom(const Camera2D* c2d)
{
    CLUIGE_ASSERT(c2d != NULL, "Camera2D::get_zoom() : calling object is null");
    return c2d->zoom;
}

static void c2d_set_zoom(Camera2D* c2d, Vector2 v)
{
    CLUIGE_ASSERT(c2d != NULL, "Camera2D::set_zoom() : calling object is null");
    //CLUIGE_ASSERT(!is_near_zero(v), "Camera2D::() : ");
    if(fabs(v.x) > .01 && fabs(v.y) > .01)
    {
        //TODO gné?
        iCluige.iCamera2D._SCREEN_WIDTH = iCluige.iCamera2D._SCREEN_WIDTH * (1/v.x);
        iCluige.iCamera2D._SCREEN_HEIGHT = iCluige.iCamera2D._SCREEN_HEIGHT * (1/v.y);
        iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_X = iCluige.iCamera2D._SCREEN_WIDTH/2;
        iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_Y = iCluige.iCamera2D._SCREEN_HEIGHT/2;
        c2d->zoom = (Vector2){v.x,v.y};
    }
}

static void c2d_set_enabled(Camera2D* c2d, bool enab)
{
    CLUIGE_ASSERT(c2d != NULL, "Camera2D::set_enabled() : calling object is null");
    Camera2D* current_camera = iCluige.iCamera2D.current_camera;
    Node* node_current_camera = current_camera->_this_Node2D->_this_Node;
    Node* node_c2d = c2d->_this_Node2D->_this_Node;

    bool is_greater = iCluige.iNode.is_higher_than(node_c2d, node_current_camera);
    //true if current_camera is higher than camera in parameter

    if(enab && !is_greater)
    {
        //make_current must have in parameter a enabled camera
        c2d->enabled = enab;
        c2d_make_current(c2d);
    }
    else if (!enab && c2d == current_camera)// if user want to desactivate the current camera
    {
        Node* node_found = c2d__highest_node_excluding_one_nde(iCluige.public_root_2D,"NodeNode2DCamera2D",
                                                               current_camera->_this_Node2D->_this_Node);
        if(node_found == NULL)//no other camera in tree
        {
            c2d_make_current(iCluige.iCamera2D.default_camera);
        }
        else                    //higher camera in tree become current_camera
        {
            c2d_make_current(c2d__get_camera_from_node(node_found));
        }
    }
    c2d->enabled = enab;
}

static bool c2d_is_enabled(const Camera2D* c2d)
{
    CLUIGE_ASSERT(c2d != NULL, "Camera2D::is_enabled() : calling object is null");
    return c2d->enabled;
}

static void c2d_erase(Node* this_Node)
{
    Node2D* node2d  = (Node2D*) (this_Node->_sub_class);
    Camera2D* cam = (Camera2D*) (node2d->_sub_class);
    cam->_erase_super(this_Node);
}

//apply offsets and limits on camera
static void c2d_pre_draw(Node* this_Node)
{
    Node2D* node2d  = (Node2D*) (this_Node->_sub_class);
    Camera2D* cam = (Camera2D*) (node2d->_sub_class);
	//backup state
	if(iCluige.iCamera2D._state_changes.state_changed)
	{
		iCluige.iCamera2D._state_changes.anchor_mode = cam->anchor_mode;
		iCluige.iCamera2D._state_changes._tmp_limited_offseted_global_position = cam->_tmp_limited_offseted_global_position;
		iCluige.iCamera2D._state_changes.zoom = cam->zoom;
		iCluige.iCamera2D._state_changes.ignore_rotation = cam->ignore_rotation;
		iCluige.iCamera2D._state_changes.global_tmp_cos_rotation = cam->global_tmp_cos_rotation;
		iCluige.iCamera2D._state_changes.global_tmp_sin_rotation = cam->global_tmp_sin_rotation;
	}
	if(cam != iCluige.iCamera2D.current_camera)
	{
		return;
	}
    cam->_pre_draw_super(this_Node);//super

    //updates values
    if(cam->anchor_mode == ANCHOR_MODE_FIXED_TOP_LEFT)
    {
        cam->_tmp_limited_offseted_global_position.x =
            clamp_float(node2d->_tmp_global_position.x , cam->limit_left,cam->limit_right)+ cam->offset.x;

        cam->_tmp_limited_offseted_global_position.y =
            clamp_float(node2d->_tmp_global_position.y , cam->limit_top,cam->limit_bottom) + cam->offset.y;

    }
    else
    {
        float offset_adapted_x = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_X;
        float offset_adapted_y = iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_Y;

        cam->_tmp_limited_offseted_global_position.x =
            clamp_float(node2d->_tmp_global_position.x , cam->limit_left,cam->limit_right)+ cam->offset.x - offset_adapted_x;

        cam->_tmp_limited_offseted_global_position.y =
            clamp_float(node2d->_tmp_global_position.y , cam->limit_top,cam->limit_bottom) + cam->offset.y - offset_adapted_y;
    }

	iCluige.iCamera2D._state_changes.state_changed =
		!iCluige.iVector2.is_equal_approx(&(cam->_tmp_limited_offseted_global_position),
			&(iCluige.iCamera2D._state_changes._tmp_limited_offseted_global_position)) ||
		!is_equal_approx(cam->global_tmp_cos_rotation, iCluige.iCamera2D._state_changes.global_tmp_cos_rotation) ||
		!iCluige.iVector2.is_equal_approx(&(cam->zoom), &(iCluige.iCamera2D._state_changes.zoom)) ||
		cam->anchor_mode != iCluige.iCamera2D._state_changes.anchor_mode ||
		cam->ignore_rotation != iCluige.iCamera2D._state_changes.ignore_rotation;
}


/////////////////////////////////// iiCamera2D //////////

/*
@param Node* parent any node in the tree, the search will start from this node
@param char* class_name the name of the class you want to search
@param Node* excluded_node the node you want to exclude of the search


@returns the highest node in tree hierarchy by class_name excluding one node of the tree
*/
//TODO not sure it works perfectly + not sure if it's relevant for Cluige
static Node* c2d__highest_node_excluding_one_nde(Node* parent, const char* class_name, const Node* excluded_node) {
    CLUIGE_ASSERT(parent != NULL, "Camera2D::_highest_node_excluding_one_nde() : parent is null");
    Node* highest_camera = NULL;
    int highest_depth = -1;

    Deque queue;
    iCluige.iDeque.deque_alloc(&queue, VT_POINTER, 10);
    iCluige.iDeque.append(&queue, parent);

    int current_depth = 0;
    while (iCluige.iDeque.size(&queue) > 0)
    {
        int queue_size = iCluige.iDeque.size(&queue);
        for (int i = 0; i < queue_size; i++)
        {
            Variant front_variant = iCluige.iDeque.front(&queue);
            Node* current_node = (Node*)front_variant.ptr;
            iCluige.iDeque.pop_front(&queue);

            if (current_node != excluded_node && strcmp(current_node->_class_name, class_name) == 0)
            {
                Node2D* node2Dfound = current_node->_sub_class;
                Camera2D* camera_found = node2Dfound->_sub_class;
                if (camera_found->enabled && current_depth > highest_depth)
                {
                    highest_camera = current_node;
                    highest_depth = current_depth;
                }
            }

            if (current_node->children != NULL)
            {
                iCluige.iDeque.append(&queue, current_node->children);
            }

            if (current_node->next_sibling != NULL)
            {
                iCluige.iDeque.append(&queue, current_node->next_sibling);
            }
        }
        current_depth++;
    }
    iCluige.iDeque.pre_delete_Deque(&queue);
    return highest_camera;
}

static struct _Camera2D* c2d_new_Camera2D()
{
    Node2D* new_Node2D = iCluige.iNode2D.new_Node2D();
	Node* new_Node = new_Node2D->_this_Node;
    Camera2D* new_camera2D = iCluige.checked_malloc(sizeof(Camera2D));

    //enabled must be set with the method set_enabled
    new_camera2D->enabled = true;
    new_camera2D->ignore_rotation = true;
    new_camera2D->rotation_radians = 0;

    //Zoom must be set with the method set_zoom
    new_camera2D->zoom = (Vector2){ 1., 1. };
    new_camera2D->offset = (Vector2){ 0., 0. };
    new_camera2D->limit_bottom = 100000.0;
    new_camera2D->limit_top = -100000.0;
    new_camera2D->limit_right = 100000.0;
    new_camera2D->limit_left = -100000.0;
    new_camera2D->anchor_mode = ANCHOR_MODE_DRAG_CENTER;

    //respectively Left Top Right Bottom
    new_camera2D->limits[0] = &new_camera2D->limit_left;
    new_camera2D->limits[1] = &new_camera2D->limit_top;
    new_camera2D->limits[2] = &new_camera2D->limit_right;
    new_camera2D->limits[3] = &new_camera2D->limit_bottom;
    new_camera2D->global_tmp_sin_rotation = 0;
    new_camera2D->global_tmp_cos_rotation = 1;

    //new_camera2D->_state_changes.state_changed = true;//see make_current()

    //private attributes of camera2D
	new_camera2D->_this_Node2D = new_Node2D;
	new_camera2D->_sub_class = NULL;

    //private attribute of Node2D
    new_Node2D->_sub_class = new_camera2D;

	//virtual methods - private copies of mother class pointers
	//	constructors of all ancestors in inheritance hierarchy
	//	have already been called, so :
	//	new_Node virtual methods pointers are already pointing to
	//	overriding methods (if any)
    new_camera2D->_delete_super = new_Node->delete_Node;
    new_camera2D->_erase_super = new_Node->erase;
    new_camera2D->_pre_draw_super = new_Node->pre_draw;
//    new_camera2D->_post_process_super = new_Node->post_process;

    free(new_Node->_class_name); //TODO static value to avoid free
    StringBuilder sb;
    new_Node->_class_name = iCluige.iStringBuilder.string_alloc(
            &sb, strlen("NodeNode2DCamera2D"));
    iCluige.iStringBuilder.append(&sb, "NodeNode2DCamera2D");

    Camera2D* current_camera = iCluige.iCamera2D.current_camera;

    //checks if the new camera should be the current or not
    if(current_camera != NULL)
    {
        if(current_camera == iCluige.iCamera2D.default_camera)//default camera is the current_camera
        {
            c2d_make_current(new_camera2D);
        }
    }
    else //first camera created
    {
        iCluige.iCamera2D.current_camera = new_camera2D;
    }
    new_Node->delete_Node = c2d_delete_Camera2D;
    new_Node->erase = c2d_erase;
    new_Node->pre_draw = c2d_pre_draw;

    return new_camera2D;
}


/////////////////////////////////// global //////////


void iiCamera2D_init()
{
    iCluige.iCamera2D._SCREEN_HEIGHT = 100.0;
    iCluige.iCamera2D._SCREEN_WIDTH = 200.0;
    iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_X = iCluige.iCamera2D._SCREEN_WIDTH / 2;
    iCluige.iCamera2D._SCREEN_ANCHOR_CENTER_Y = iCluige.iCamera2D._SCREEN_HEIGHT / 2;

    iCluige.iCamera2D.new_Camera2D = c2d_new_Camera2D;
    iCluige.iCamera2D.get_zoom = c2d_get_zoom;
    iCluige.iCamera2D.set_zoom = c2d_set_zoom;
    iCluige.iCamera2D.set_enabled = c2d_set_enabled;
    iCluige.iCamera2D.is_enabled = c2d_is_enabled;
    iCluige.iCamera2D.make_current = c2d_make_current;
    iCluige.iCamera2D.set_rotation_degrees = c2d_set_rotation_degrees;
    iCluige.iCamera2D.get_rotation_degrees = c2d_get_rotation_degrees;
}
