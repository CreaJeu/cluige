

#include "cluige.h"
#include "Path2D.h"


////////////////////////////////// _Path2D /////////


////////////////////////////////// iiPath2D /////////


static void p2D_path2D_alloc(Path2D* this_Path2D, Vector2* points, int nb_points, bool absolute)
{
    Deque* new_path = &(this_Path2D->_points);
    iCluige.iDeque.deque_alloc(new_path, VT_POINTER, nb_points);

    Vector2 prev = (Vector2){0,0};
    for(int i=0; i<nb_points; i++)
    {
        Vector2* pointI = iCluige.checked_malloc(sizeof(Vector2));
        if(absolute)
        {
            *pointI = points[i];
        }
        else
        {
            iCluige.iVector2.add(&prev, &(points[i]), pointI);
            prev = *pointI;
        }
        iCluige.iDeque.push_back(new_path, pointI);
    }
}

static Path2D* p2D_new_Path2D(Vector2* points, int nb_points, bool absolute)
{
    Path2D* new_Path2D = iCluige.checked_malloc(sizeof(Path2D));
    p2D_path2D_alloc(new_Path2D, points, nb_points, absolute);
    return new_Path2D;
}

static void p2D_pre_delete_Path2D(Path2D* this_Path2D)
{
    Deque* points = &(this_Path2D->_points);
    int nb_points = iCluige.iDeque.size(points);
    for(int j=0; j<nb_points; j++)
    {
        Vector2* point_j = iCluige.iDeque.at(points, j).ptr;
        free(point_j);
    }
    iCluige.iDeque.pre_delete_Deque(points);
}

static Vector2* p2D_at(const Path2D* this_Path2D, int i)
{
    const Deque* points = &(this_Path2D->_points);
    return iCluige.iDeque.at(points, i).ptr;
}

static int p2D_size(const Path2D* this_Path2D)
{
    const Deque* points = &(this_Path2D->_points);
    return iCluige.iDeque.size(points);
}

/////////////////////////////////// iPath2D //////////

void iiPath2D_init()
{
	iCluige.iPath2D.path2D_alloc = p2D_path2D_alloc;
	iCluige.iPath2D.pre_delete_Path2D = p2D_pre_delete_Path2D;
    iCluige.iPath2D.new_Path2D = p2D_new_Path2D;
    iCluige.iPath2D.at = p2D_at;
    iCluige.iPath2D.size = p2D_size;
}

