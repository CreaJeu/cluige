
#include "cluige.h"
#include "Vector2.h"


////////////////////////////////// iiVector2 /////////

static void vct2_add(const Vector2* v1, const Vector2* v2, Vector2 *result)
{
    result->x = v1->x + v2->x;
    result->y = v1->y + v2->y;
}

static void vct2_substract(const Vector2* v1, const Vector2* v2, Vector2 *result)
{
    result->x = v1->x - v2->x;
    result->y = v1->y - v2->y;
}

static void vct2_computeLineEquation(const Vector2* v1, const Vector2* v2, float *a, float *b, float *c)
{
    *a = v2->y - v1->y;
    *b = v1->x - v2->x;
    *c = v2->x * v1->y - v1->x * v2->y;
}


////////////////////////////////// iVector2 /////////

void iiVector2Init()
{
    iCluige.iVector2.add = vct2_add;
    iCluige.iVector2.substract = vct2_substract;
    iCluige.iVector2.computeLineEquation = vct2_computeLineEquation;
}

