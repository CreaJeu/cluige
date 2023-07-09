#ifndef PATH2D_H_INCLUDED
#define PATH2D_H_INCLUDED

#include "Deque.h"

typedef struct _Path2D Path2D;

struct _Path2D
{
    Deque _points;//Deque<Vector2*>
    bool _visible;

    //TODO color, thick/thin
};

//~namespace to call like : iCluige.iNode.f(myNode, param)
struct iiPath2D
{
	//void path2D_alloc(Vector2* points, int nb_points, bool absolute)
	void (*path2D_alloc)(Path2D* this_Path2D, Vector2* points, int nb_points, bool absolute);
	//void path2D_alloc(Vector2* points, int nb_points, bool absolute)
	void (*path2D_alloc_from_parsed)(Path2D* this_Path2D, Deque* coordinates_sequence);
	//Path2D* new_Path2D(Vector2* points, int nb_points, bool absolute)
	Path2D* (*new_Path2D)(Vector2* points, int nb_points, bool absolute);

	//void pre_delete_Path2D(Path2D* this_Path2D)
	void (*pre_delete_Path2D)(Path2D* this_Path2D);

	Vector2* (*at)(const Path2D* this_Path2D, int i);
    int (*size)(const Path2D* this_Path2D);
};
//iPath2D : in iiCluige

//to be called only by cluige_init() to set iNode functions pointers
void iiPath2D_init();


#endif // PATH2D_H_INCLUDED
