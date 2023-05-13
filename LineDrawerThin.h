#ifndef LINE_DRAWER_THIN_H_INCLUDED
#define LINE_DRAWER_THIN_H_INCLUDED

#include "cluige.h"

typedef struct _LineDrawerThin LineDrawerThin;

struct _LineDrawerThin
{
    // Public
    Vector2 current_position;
    float curr_dist;

    // Private
    Vector2 _diagonal_incr;
    Vector2 _lateral_incr;
    Vector2 _start_position;
    Vector2 _end_position;
    Vector2 _abs_amplitude;
    bool _steep;
    int _glyphIndex;

    // Line equation
    float _a, _b, _c;
};

//~namespace to call like : iCluige.iLineDrawerThin.f(myLineDrawerThin, param)
struct iiLineDrawerThin
{
    void (*start)(struct _LineDrawerThin* drawer, const Vector2* v_start, const Vector2* v_end);
    _Bool (*hasFinished)(struct _LineDrawerThin* drawer);
    void (*next)(struct _LineDrawerThin* drawer);
    const char* (*glyph)(struct _LineDrawerThin* drawer);
    char* glyphs[4]; // '|' '/' '-' '\'
};

//to be called by cluigeInit(), to set iLineDrawerThin functions pointers
void iiLineDrawerThin_init();


#endif // LINE_DRAWER_THIN_H_INCLUDED
