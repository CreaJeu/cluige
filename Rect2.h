#ifndef RECT2_H_INCLUDED
#define RECT2_H_INCLUDED


typedef struct _Rect2 Rect2;
typedef struct _Rect2Intersection Rect2Intersection;

//the same than Godot
// => don't use negative size, use Rect2::abs() if needed
//but all x/y/min/max instead of pos+size
// => must x_min < x_max && y_min < y_max
struct _Rect2
{
//	Vector2 position;
//	Vector2 size;
	//implementation more direct than pos+size:
	float x_min;
	float y_min;
	float x_max;
	float y_max;
};

struct _Rect2Intersection
{
	bool intersects;
	Rect2 intersection;
};

//~namespace to call like : iCluige.iNode.f(my_Node, param)
struct iiRect2
{
	Rect2 ZERO;

	//constructor (no need for destructor)
	void (*rect2_alloc)(Rect2* this_Rect2, Vector2 top_left, Vector2 size);
//	void (*pre_delete_Rect2)(Rect2* this_Rect2);

	Rect2Intersection (*intersection)(const Rect2* this_Rect2, const Rect2* other);
	bool (*is_equal_approx)(const Rect2* this_Rect2, const Rect2* other);
	Rect2 (*abs)(const Rect2* this_Rect2);
	//later : merge(), get_support(), intersects other shapes
};
//iRect2 : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiRect2_init();


#endif // _H_INCLUDED
