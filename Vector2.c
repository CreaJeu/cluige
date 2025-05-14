
#include "cluige.h"
#include "Vector2.h"

#include <math.h>
#include <float.h>

////////////////////////////////// iiVector2 /////////

static void vct2_set(Vector2* v, float x, float y)
{
    v->x = x;
    v->y = y;
}

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

static void vct2_k_mul(const Vector2* v, float factor, Vector2 *result)
{
    result->x = factor * v->x;
    result->y = factor * v->y;
}

static void vct2_compute_line_equation(const Vector2* v1, const Vector2* v2, float *a, float *b, float *c)
{
    *a = v2->y - v1->y;
    *b = v1->x - v2->x;
    *c = v2->x * v1->y - v1->x * v2->y;
}

static float vct2_length(const Vector2* v)
{
	//return sqrtf(v->x * v->x + v->y * v->y);
	return hypotf(v->x, v->y);
}

static float vct2_length_squared(const Vector2* v)
{
	return v->x * v->x + v->y * v->y;
}

static void vct2_normalize(Vector2* v)
{
	float l = v->x * v->x + v->y * v->y;
	if(l != 0)
    {
		l = sqrtf(l);
		v->x /= l;
		v->y /= l;
	}
}

static void vct2_normalized(const Vector2* thisV, Vector2 *res)
{
	*res = *thisV;
	vct2_normalize(res);
}

static bool vct2_is_normalized(const Vector2* v)
{
	return vct2_length_squared(v) <= FLT_EPSILON;
}

static float vct2_distance_to(const Vector2* v1, const Vector2* v2)
{
    float dx = v2->x - v1->x;
    float dy = v2->y - v1->y;
	return sqrtf(dx * dx + dy * dy);
}

static float vct2_distance_squared_to(const Vector2* v1, const Vector2* v2)
{
    float dx = v2->x - v1->x;
    float dy = v2->y - v1->y;
	return dx * dx + dy * dy;
}

static bool vct2_is_zero_approx(const Vector2* v)
{
    return fabs(v->x) < iCluige.EPSILON && fabs(v->y) < iCluige.EPSILON;
}

static bool vct2_is_equal_approx(const Vector2* v1, const Vector2* v2)
{
	Vector2 diff;
	vct2_substract(v1, v2, &diff);
	return vct2_is_zero_approx(&diff);
}

////////////////////////////////// iVector2 /////////

void iiVector2_init()
{
    iCluige.iVector2.set = vct2_set;
    iCluige.iVector2.add = vct2_add;
    iCluige.iVector2.substract = vct2_substract;
    iCluige.iVector2.k_mul = vct2_k_mul;
    iCluige.iVector2.compute_line_equation = vct2_compute_line_equation;
    iCluige.iVector2.length = vct2_length;
    iCluige.iVector2.length_squared = vct2_length_squared;
    iCluige.iVector2.normalize = vct2_normalize;
    iCluige.iVector2.normalized = vct2_normalized;
    iCluige.iVector2.is_normalized = vct2_is_normalized;
    iCluige.iVector2.distance_to = vct2_distance_to;
    iCluige.iVector2.distance_squared_to = vct2_distance_squared_to;
    iCluige.iVector2.is_zero_approx = vct2_is_zero_approx;
    iCluige.iVector2.is_equal_approx = vct2_is_equal_approx;
}

