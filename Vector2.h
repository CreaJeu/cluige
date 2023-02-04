#ifndef VECTOR2_H_INCLUDED
#define VECTOR2_H_INCLUDED

typedef struct _Vector2 Vector2;

struct _Vector2
{
    float x;
    float y;
};

struct iiVector2
{
    void (*add)(const Vector2* v1, const Vector2* v2, Vector2 *result);
    void (*substract)(const Vector2* v1, const Vector2* v2, Vector2 *res);
    void (*computeLineEquation)(const Vector2* v1, const Vector2* v2, float *a, float *b, float *c);
};

void iiVector2Init();

#endif //VECTOR2_H_INCLUDED
