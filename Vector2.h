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
    void (*k_mul)(const Vector2* v, float factor, Vector2 *res);
    void (*compute_line_equation)(const Vector2* v1, const Vector2* v2, float *a, float *b, float *c);
    float (*length)(const Vector2* v);
    float (*length_squared)(const Vector2* v);
    void (*normalize)(Vector2* v);
    void (*normalized)(const Vector2* this_, Vector2 *res);
    bool (*is_normalized)(const Vector2* v);
    float (*distance_to)(const Vector2* v1, const Vector2* v2);
    float (*distance_squared_to)(const Vector2* v1, const Vector2* v2);
    bool (*is_zero)(const Vector2* v);

};

void iiVector2_init();

#endif //VECTOR2_H_INCLUDED
