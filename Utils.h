#ifndef CLUIGE_UTILS_H_INCLUDED
#define CLUIGE_UTILS_H_INCLUDED

//for bool, true, false (standard since C99)
#include <stdbool.h>

typedef struct _SortedDictionary SortedDictionary;
typedef struct _Vector2 Vector2;

void utils_breakpoint_trick(const void* anything, bool breakpoint);
//void utils_cluige_printf(int x, int y, const char* msg);

int clamp_int(int x, int min, int max);
float clamp_float(float x, float min, float max);

bool str_equals(const char* s1, const char* s2);

bool utils_bool_from_parsed(bool* out,const SortedDictionary* params, const char* param_name);
bool utils_char_from_parsed(char* out,const SortedDictionary* params, const char* param_name);
bool utils_int_from_parsed(int* out,const SortedDictionary* params, const char* param_name);
bool utils_float_from_parsed(float* out,const SortedDictionary* params, const char* param_name);
bool utils_vector2_from_parsed(Vector2* out,const SortedDictionary* params, const char* param_name);

//with str deep copy (malloc)
bool utils_str_from_parsed(char** out,const SortedDictionary* params, const char* param_name);

// no deep copy
bool utils_nonquoted_str_from_parsed(char** out,const SortedDictionary* params, const char* param_name);

//with str deep copy (malloc)
bool utils_id_str_from_ExtResource_parsed(char** out, const char* parsed_value);



#endif // CLUIGE_UTILS_H_INCLUDED
