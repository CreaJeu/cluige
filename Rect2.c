
#include "cluige.h"
#include "Rect2.h"
#include <math.h>


////////////////////////////////// _Rect2 /////////

//no method


////////////////////////////////// iiRect2 /////////

static void rct2_rect2_alloc(Rect2* this_Rect2, Vector2 top_left, Vector2 size)
{
	this_Rect2->x_min = top_left.x;
	this_Rect2->y_min = top_left.y;
	this_Rect2->x_max = size.x + top_left.x;
	this_Rect2->y_max = size.y + top_left.y;
}

static Rect2Intersection rct2_intersection(const Rect2* this_Rect2, const Rect2* other)
{
	CLUIGE_ASSERT(this_Rect2->x_min <= this_Rect2->x_max && this_Rect2->y_min <= this_Rect2->y_max, "Rect2::intersection() : 'this_Rect2' has negative size, use Rect2::abs() to prevent errors");
	CLUIGE_ASSERT(other->x_min <= other->x_max && other->y_min <= other->y_max, "Rect2::intersection(this, other) : 'other' has negative size, use Rect2::abs() to prevent errors");

	Rect2Intersection res;
	if(this_Rect2->x_max < other->x_min || other->x_max < this_Rect2->x_min
			|| this_Rect2->y_max < other->y_min || other->y_max < this_Rect2->y_min)
	{
		res.intersects = false;
		return res;
	}
	else
	{
		res.intersects = true;
		res.intersection.x_min = fmaxf(this_Rect2->x_min, other->x_min);
		res.intersection.y_min = fmaxf(this_Rect2->y_min, other->y_min);
		res.intersection.x_max = fminf(this_Rect2->x_max, other->x_max);
		res.intersection.y_max = fminf(this_Rect2->y_max, other->y_max);
		return res;
	}
}

static bool rct2_is_equal_approx(const Rect2* this_Rect2, const Rect2* other)
{
	return fabs(this_Rect2->x_min - other->x_min) < iCluige.EPSILON
			&& fabs(this_Rect2->y_min - other->y_min) < iCluige.EPSILON
			&& fabs(this_Rect2->x_max - other->x_max) < iCluige.EPSILON
			&& fabs(this_Rect2->y_max - other->y_max) < iCluige.EPSILON;
}

static Rect2 rct2_abs(const Rect2* this_Rect2)
{
	Rect2 res = *this_Rect2;
	if(res.x_max < res.x_min)
	{
		res.x_min = res.x_max;
		res.x_max = this_Rect2->x_min;
	}
	if(res.y_max < res.y_min)
	{
		res.y_min = res.y_max;
		res.y_max = this_Rect2->y_min;
	}
	return res;
}


/////////////////////////////////// iRect2 //////////

void iiRect2_init()
{
	iCluige.iRect2.ZERO = (Rect2){0, 0, 0, 0};
	iCluige.iRect2.rect2_alloc = rct2_rect2_alloc;
	iCluige.iRect2.intersection = rct2_intersection;
	iCluige.iRect2.is_equal_approx = rct2_is_equal_approx;
	iCluige.iRect2.abs = rct2_abs;
}

