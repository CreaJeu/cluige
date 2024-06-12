#ifndef CAMERA2D_H_INCLUDED
#define CAMERA2D_H_INCLUDED

#include "../Vector2.h"

typedef struct _Camera2D Camera2D;

struct _Camera2D
{

    //vector which permits to slide by this vector the position of the camera ((Vector2){-90,-45}; will center)
	Vector2 offset;


	//if false camera won't be current if true and if the camera is the highest camera in tree becomes current camera
	//MUST use methods to modify
	bool enabled;

	//allow to zoom on the camera, can't be equal to zero
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
    //
    int limit_top;
    int limit_right;
    int limit_left;
    int limit_bottom;

    //respectively Left Top Right Bottom
    int* limits[4];


	//virtual methods
	void (*delete_Node2D)(Node*);
	void (*post_process_Node2D)(Node*);
};



struct iiCamera2D
{
    //default camera is the camera in private_root
    //is updated automatically when disabling and enabling cameras
    // or by deleting a camera from tree
    //tho if you insert a new camera in the tree and there is already one the current camera will always be the first camera created,
    // it's not updated when you add a new camera to the tree
	Camera2D* current_camera;




	Camera2D* default_camera;

	Camera2D* (*new_Camera2D)();

	//private method
	void (*_predraw)(Node* this_node);


	Vector2 (*get_zoom)(const Camera2D* c2d);

	//must be used, don't directly access zoom;
    void (*set_zoom)(Camera2D* c2d, Vector2 v);

    //must be used, don't directly access enabled;
    void (*set_enabled)(Camera2D* c2d, bool enab);


    bool (*is_enabled)(const Camera2D* c2d);
    void (*make_current)(Camera2D* c2d);




};



void iiCamera2D_init();


#endif  // CAMERA2D_H_INCLUDED
