
#include "cluige.h"
#include "Utils.h"
#include <string.h>
#include <assert.h>

//for sscanf
#include <stdio.h>


void utils_breakpoint_trick(const void* anything, bool breakpoint)
{
    if(breakpoint)
    {

    }
}

void utils_bool_from_parsed(bool* out,const SortedDictionary* params, const char* param_name)
{
    //false / true
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        char* param_str = (char*)(found.v.ptr);
        int cmp = strcmp(param_str, "true");
        (*out) = (cmp == 0);
    }
}

void utils_char_from_parsed(char* out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        char* param_str = (char*)(found.v.ptr);
        (*out) = param_str[0];
    }
}

void utils_int_from_parsed(int* out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        char* param_str = (char*)(found.v.ptr);
        (*out) = atoi(param_str);
    }
}

void utils_float_from_parsed(float* out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        char* param_str = (char*)(found.v.ptr);
        (*out) = strtof(param_str, NULL);
    }
}

void utils_vector2_from_parsed(Vector2* out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        const char* str_res = (char*)(found.v.ptr);// "Vector2(2.265, -3.2)"
        float x, y;
        int ok = sscanf(str_res, "Vector2(%f, %f)", &x, &y);
        assert(ok == 2);
        if(ok == 2)
        {
            out->x = x;
            out->y = y;
        }
    }
}

void utils_str_from_parsed(char** out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        //deep copy string
        char* param_str = (char*)(found.v.ptr);
        char* copied = iCluige.checked_malloc(1 + (sizeof(char) * strlen(param_str)));
        strcpy(copied, param_str);
        (*out) = copied;
    }
}
