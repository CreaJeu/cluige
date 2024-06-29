#ifndef CLUIGE_UTILS_H_INCLUDED
#define CLUIGE_UTILS_H_INCLUDED

//for bool, true, false (standard since C99)
#include <stdbool.h>

typedef struct _SortedDictionary SortedDictionary;
typedef struct _Vector2 Vector2;

void utils_breakpoint_trick(const void* anything, bool breakpoint);

int clamp_int(int x, int min, int max);
float clamp_float(float x, float min, float max);

void utils_bool_from_parsed(bool* out,const SortedDictionary* params, const char* param_name);
void utils_char_from_parsed(char* out,const SortedDictionary* params, const char* param_name);
void utils_int_from_parsed(int* out,const SortedDictionary* params, const char* param_name);
void utils_float_from_parsed(float* out,const SortedDictionary* params, const char* param_name);
void utils_vector2_from_parsed(Vector2* out,const SortedDictionary* params, const char* param_name);
//with str deep copy
void utils_str_from_parsed(char** out,const SortedDictionary* params, const char* param_name);



#endif // CLUIGE_UTILS_H_INCLUDED
