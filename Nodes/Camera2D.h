#ifndef CAMERA2D_H_INCLUDED
#define CAMERA2D_H_INCLUDED

#include "../Vector2.h"

typedef struct _Camera2D Camera2D;

enum AnchorMode
{
    ANCHOR_MODE_FIXED_TOP_LEFT,
    ANCHOR_MODE_DRAG_CENTER
};

struct _Camera2D
{
    //vector which permits to slide by this vector the position of the camera ((Vector2){-90,-45}; will center)
	Vector2 offset;

	//if false camera won't be current if true and if the camera is the highest camera in tree becomes current camera
	//MUST use methods to modify
	bool enabled;

	bool ignore_rotation;

	//rotation angle value if ignore_rotation allows it, in radians
	float rotation_radians;

	//allows to zoom on the camera, can't be equal to zero
	//(Vector2){0.5,0.5}; will zoom out
	//(Vector2){2,2}; will zoom in
    //MUST use methods to modify
	Vector2 zoom;

	//private attribute shouldn't be modified by user
	Vector2 _tmp_limited_offseted_global_position;

    //for inheritance + private
	Node2D* _this_Node2D; //link to super
	void* _sub_class;     //to access to potential son/sub_class

    //Limits of the camera if the node off which the camera is attached goes off limits, the camera won't follow
    //the node can still move out of the camera bounds
    float limit_top;
    float limit_right;
    float limit_left;
    float limit_bottom;

    //cosf(rotation) and sinf(rotation)
    float global_tmp_sin_rotation;
    float global_tmp_cos_rotation;

    /*way of upgrading the camera
    float drag_left_margin;
    float drag_right_margin;
    float drag_top_margin;
    float drag_bottom_margin;
    bool drag_horizontal_enabled;
    bool drag_vertical_enabled;*/

    //0 if anchor up left corner
    //1 if anchor screen center
    enum AnchorMode anchor_mode;

    //respectively Left Top Right Bottom
    float* limits[4];

	//virtual methods - private copies of mother class pointers
	void (*_delete_super)(Node*);
	void (*_erase_super)(Node*);
//	void (*_post_process_super)(Node*);
};

struct iiCamera2D
{
    //default camera is the camera in private_root
    //is updated automatically when disabling and enabling cameras
    // or by deleting a camera from tree
    //tho if you insert a new camera in the tree and there is already one the current camera will always be the first camera created,
    // it's not updated when you add a new camera to the tree
	Camera2D* current_camera;

    //default camera which is created at every lunch of cluige
	Camera2D* default_camera;

	//private
	//not in _Camera2D because current_camera can change
	struct
	{
		bool state_changed;
		enum AnchorMode anchor_mode;
		Vector2 _tmp_limited_offseted_global_position;
		Vector2 zoom;
		bool ignore_rotation;
		float global_tmp_cos_rotation;
		float global_tmp_sin_rotation;
	} _state_changes;

	Camera2D* (*new_Camera2D)();

	//private method
	void (*_predraw)(Node* this_node);

	//temporary TODO
	float _SCREEN_HEIGHT;
	float _SCREEN_ANCHOR_CENTER_Y;
	float _SCREEN_ANCHOR_CENTER_X;
	float _SCREEN_WIDTH;


	Vector2 (*get_zoom)(const Camera2D* c2d);

	//must be used, don't directly access zoom;
    void (*set_zoom)(Camera2D* c2d, Vector2 v);

    //must be used, don't directly access enabled;
    void (*set_enabled)(Camera2D* c2d, bool enab);

    bool (*is_enabled)(const Camera2D* c2d);
    void (*make_current)(Camera2D* c2d);

    //Must use this methods to set the rotation and get the rotation
    //give degrees, not radians
    void (*set_rotation_degrees)(Camera2D* c2d, float rotation_angle_degrees);
    float (*get_rotation_degrees)(Camera2D* c2d);
};


void iiCamera2D_init();


#endif  // CAMERA2D_H_INCLUDED
