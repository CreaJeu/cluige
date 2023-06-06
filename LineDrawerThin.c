
#include <math.h>

#include "cluige.h"
#include "LineDrawerThin.h"

/////////////////////////////////// iiLineDrawerThin //////////

void ldt_start(struct _LineDrawerThin* drawer, const Vector2* v_start, const Vector2* v_end)
{
    drawer->current_position = *v_start;
    drawer->_start_position = *v_start;
    drawer->_end_position = *v_end;

    drawer->_abs_amplitude.x = fabsf(drawer->_end_position.x - drawer->_start_position.x);
    drawer->_abs_amplitude.y = fabsf(drawer->_end_position.y - drawer->_start_position.y);

    iCluige.iVector2.compute_line_equation(&(drawer->_start_position), &(drawer->_end_position), &(drawer->_a), &(drawer->_b), &(drawer->_c));

    // Compute amplitude vector
    Vector2 amplitude_vector;
    iCluige.iVector2.substract(v_end, v_start, &amplitude_vector);

    drawer->_steep = fabsf(amplitude_vector.x) < fabsf(amplitude_vector.y);

    // Compute lateral increments
    if (drawer->_steep)
    {
        drawer->_lateral_incr.x = 0;
        drawer->_glyph_index = 0;
        if (amplitude_vector.y < 0)
        {
            drawer->_lateral_incr.y = -1;
        }
        else
        {
            drawer->_lateral_incr.y = 1;
        }
    }
    else
    {
        drawer->_lateral_incr.y = 0;
        drawer->_glyph_index = 2;
        if (amplitude_vector.x < 0)
        {
            drawer->_lateral_incr.x = -1;
        }
        else
        {
            drawer->_lateral_incr.x = 1;
        }
    }

    // Compute diagonal increments
    if (amplitude_vector.x < 0)
    {
        drawer->_diagonal_incr.x = -1;
    }
    else
    {
        drawer->_diagonal_incr.x = 1;
    }

    if (amplitude_vector.y < 0)
    {
        drawer->_diagonal_incr.y = -1;
    }
    else
    {
        drawer->_diagonal_incr.y = 1;
    }
}

bool ldt_has_finished(struct _LineDrawerThin* drawer)
{
    // Projection Y
    if(drawer->_steep)
    {
        float current_y = fabsf(drawer->current_position.y - drawer->_start_position.y);
        return current_y >= drawer->_abs_amplitude.y;
    }
    // Projection X
    else
    {
        float current_x = fabsf(drawer->current_position.x - drawer->_start_position.x);
        return current_x >= drawer->_abs_amplitude.x;
    }
}

void ldt_next(struct _LineDrawerThin* drawer)
{
    Vector2 lateral_point, diagonal_point;
    iCluige.iVector2.add(&(drawer->current_position), &(drawer->_lateral_incr), &lateral_point);
    iCluige.iVector2.add(&(drawer->current_position), &(drawer->_diagonal_incr), &diagonal_point);

    float distance_lateral = drawer->_a * lateral_point.x + drawer->_b * lateral_point.y + drawer->_c;
    float distance_diagonal = drawer->_a * diagonal_point.x + drawer->_b * diagonal_point.y + drawer->_c;

    if (fabsf(distance_lateral) < fabsf(distance_diagonal))
    {
        drawer->current_position = lateral_point;
        drawer->curr_dist = distance_lateral;
        if(drawer->_steep)
        {
            drawer->_glyph_index = 0;
        }
        else
        {
            drawer->_glyph_index = 2;
        }
    }
    else
    {
        drawer->current_position = diagonal_point;
        drawer->curr_dist = distance_diagonal;
        float incr_x_sign = copysignf(1.0, drawer->_diagonal_incr.x);
        float incr_y_sign = copysignf(1.0, drawer->_diagonal_incr.y);
        if(incr_x_sign == incr_y_sign)
        {
            drawer->_glyph_index = 3;
        }
        else
        {
            drawer->_glyph_index = 1;
        }
    }
}

const char* ldt_glyph(struct _LineDrawerThin* drawer)
{
    return iCluige.iLineDrawerThin.glyphs[drawer->_glyph_index];
}


/////////////////////////////////// LineDrawerThin //////////

void iiLineDrawerThin_init()
{
    iCluige.iLineDrawerThin.start = ldt_start;
    iCluige.iLineDrawerThin.has_finished = ldt_has_finished;
    iCluige.iLineDrawerThin.next = ldt_next;
    iCluige.iLineDrawerThin.glyph = ldt_glyph;
    iCluige.iLineDrawerThin.glyphs[0] = ":";
    iCluige.iLineDrawerThin.glyphs[1] = ",";// "/";
    iCluige.iLineDrawerThin.glyphs[2] = "-";
    iCluige.iLineDrawerThin.glyphs[3] = "`";//"\\";
}

