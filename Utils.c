
#include "cluige.h"
#include "Utils.h"
#include <string.h>
#include <assert.h>
#include <math.h>

//for sscanf
#include <stdio.h>

////for utils_cluige_printf() TODO replace curses depndcy with ~'godot server'
//#include <curses.h>


void utils_breakpoint_trick(const void* anything, bool breakpoint)
{
    if(breakpoint)
    {
        //put your breakpoint here
        breakpoint = true;
    }
}

//void utils_cluige_printf(int x, int y, const char* msg)
//{
//	mvaddstr(y, x, msg);
//}

bool is_equal_approx(float a, float b)
{
	return fabs(a - b) < iCluige.EPSILON;
}

int clamp_int(int x, int min, int max)
{
    if(x < min)
    {
        return min;
    }
    else if(max < x)
    {
        return max;
    }
    else
    {
        return x;
    }
}

float clamp_float(float x, float min, float max)
{
    if(x < min)
    {
        return min;
    }
    else if(max < x)
    {
        return max;
    }
    else
    {
        return x;
    }
}

bool str_equals(const char* s1, const char* s2)
{
	return (strcmp(s1, s2) == 0);
}

bool utils_bool_from_parsed(bool* out,const SortedDictionary* params, const char* param_name)
{
    //false / true
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        char* param_str = (char*)(found.v.ptr);
        int cmp = strcmp(param_str, "true");
        (*out) = (cmp == 0);
        return true;
    }
    return false;
}

bool utils_char_from_parsed(char* out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        char* param_str = (char*)(found.v.ptr);
        (*out) = param_str[0];
        return true;
    }
    return false;
}

bool utils_int_from_parsed(int* out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        char* param_str = (char*)(found.v.ptr);
        (*out) = atoi(param_str);
        return true;
    }
    return false;
}

bool utils_float_from_parsed(float* out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        char* param_str = (char*)(found.v.ptr);
        (*out) = strtof(param_str, NULL);
        return true;
    }
    return false;
}

bool utils_vector2_from_parsed(Vector2* out,const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        const char* str_res = (char*)(found.v.ptr);// "Vector2(2.265, -3.2)"
        float x, y;
        int ok = sscanf(str_res, "Vector2(%f, %f)", &x, &y);
        CLUIGE_ASSERT(ok == 2, "Utils::utils_vector2_from_parsed() : not a parsable Vector2");
        if(ok == 2)
        {
            out->x = x;
            out->y = y;
        }
        return true;
    }
    return false;
}

bool utils_str_from_parsed(char** out, const SortedDictionary* params, const char* param_name)
{
    Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
    if(found.valid)//param found
    {
        //beware : strings values are quoted!
        // =>here we remove those \"
        char* param_str = (char*)(found.v.ptr);// ""text""
        size_t length_param = strlen(param_str);
        //crash //param_str[length_param - 1] = '\0';// ""text\0"
        size_t length_res_null_termin = length_param - 1;//-2+1 // "text\0"
        //deep copy string
        char* copied = iCluige.checked_malloc(sizeof(char) * length_res_null_termin);// "text\0"
        strncpy(copied, param_str + 1, length_res_null_termin - 1);
        copied[length_res_null_termin - 1] = '\0';// "text\0"
        (*out) = copied;
        return true;
    }
    return false;
}

bool utils_nonquoted_str_from_parsed(char** out, const SortedDictionary* params, const char* param_name)
{
	Checked_Variant found = iCluige.iSortedDictionary.get(params, param_name);
	if(found.valid)//param found
	{
		char* param_str = (char*)(found.v.ptr);// "text\0"
//		size_t length_res_null_termin =  strlen(param_str);
//		char* copied = iCluige.checked_malloc(sizeof(char) * length_res_null_termin);// "text\0"
//		strcpy(copied, param_str);
//		(*out) = copied;
		(*out) = param_str;
		return true;
	}
	return false;
}

bool utils_id_str_from_ExtResource_parsed(char** out, const char* parsed_value)
{
	// "ExtResource("2_efpur")\0"
	const char* from = parsed_value;
	int tmp_len = strlen("ExtResource(\"");
	from += tmp_len;//2_efpur")\0"
	tmp_len = strcspn(from, "\"");
	char* copied = iCluige.checked_malloc((tmp_len +  1) * sizeof(char));
	strncpy(copied, from, tmp_len);
	copied[tmp_len] = '\0';
	(*out) = copied;
	return true;
}

void _cluige_assert(bool right, const char* msg)
{
	if(!right)
	{
		printf("ASSERT FAILED %s\n", msg);// TODO see utils_cluige_printf() in .h
		utils_breakpoint_trick(msg, true);
		assert(right);
	}
	//memo to 'replace all' with regex in code::blocks :
	//Ctrl+R
	//check 'regular expression' and 'match case'
	//assert(\(.*\));
	//CLUIGE_ASSERT(\1, "XXXX::() : ");
}
