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
    void (*set)(Vector2* v, float x, float y);
    void (*add)(const Vector2* v1, const Vector2* v2, Vector2 *result);
    void (*substract)(const Vector2* v1, const Vector2* v2, Vector2 *res);
    void (*kMul)(const Vector2* v, float factor, Vector2 *res);
    void (*computeLineEquation)(const Vector2* v1, const Vector2* v2, float *a, float *b, float *c);
    float (*length)(const Vector2* v);
    float (*length_squared)(const Vector2* v);
    void (*normalize)(Vector2* v);
    void (*normalized)(const Vector2* thisV, Vector2 *res);
    bool (*is_normalized)(const Vector2* v);
    float (*distance_to)(const Vector2* v1, const Vector2* v2);
    float (*distance_squared_to)(const Vector2* v1, const Vector2* v2);

};

void iiVector2Init();

#endif //VECTOR2_H_INCLUDED
