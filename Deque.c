
#include "cluige.h"
#include "Deque.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

//for convenience compare function
#include <string.h>

//just forward declarations, see implentation below
static void dq_bsearch_rec(const Deque* this_Deque, Variant searched_elem, struct _BSearchData* bsd);
static int dq_bsearch(const Deque* this_Deque, ...);

// ~static private

//int __...;

////////////////////////////////// iiDeque /////////

//constructor/destructor (don't forget to use them)

static void dq_deque_alloc(Deque* this_Deque, VariantType elems_type, int capacity)
{
    this_Deque->_capacity = capacity;
    this_Deque->_front_i = 0;
    this_Deque->_nb_elems = 0;
    this_Deque->_elems_type = elems_type;
    this_Deque->_sorted = false;
    this_Deque->_compare_func = iCluige.iDeque.default_compare_func;
    this_Deque->_compare_sub_func = iCluige.iDeque.default_compare_func;
    this_Deque->_elems = iCluige.checked_malloc(capacity * sizeof(Variant));
}

static void dq_pre_delete_Deque(Deque* this_Deque)
{
    //if elements are pointers, they must be deleted outside of this function,
    //according to the user needs
    free(this_Deque->_elems);
}

//private utils

static int dq__i_array(const Deque* this_Deque, int i_deque)
{
    // i_array from i_deque:
    return (this_Deque->_front_i + i_deque) % this_Deque->_capacity;
}

static int dq__i_array_prev(const Deque* this_Deque, int i_deque)
{
    //previous i_array (one-step closer to front) from i_deque
    int i_prev_array = this_Deque->_front_i + i_deque - 1;
    if(i_prev_array < 0)
    {
        //loop to other end of the array
        i_prev_array = this_Deque->_capacity - 1;
    }
    return i_prev_array;
}

static Variant* dq__at_ptr(const Deque* this_Deque, int i)
{
    int i_array = dq__i_array(this_Deque, i);
    return &(this_Deque->_elems[i_array]);
}

static void dq__grow(Deque* this_Deque)
{
    int new_cap = this_Deque->_capacity * 2;
    Variant* grown = iCluige.checked_malloc(new_cap * sizeof(Variant));
    for(int i=0; i < this_Deque->_nb_elems; i++)
    {
        int i_old = (this_Deque->_front_i + i) % this_Deque->_capacity;
        grown[i] = this_Deque->_elems[i_old];
    }

    this_Deque->_capacity = new_cap;
    this_Deque->_front_i = 0;
    this_Deque->_elems = grown;
}

//read

static Variant dq_at(const Deque* this_Deque, int i)
{
    assert((0 <= i) && (i < this_Deque->_nb_elems));
    int i_array = dq__i_array(this_Deque, i);
    return this_Deque->_elems[i_array];
}

static Variant dq_back(const Deque* this_Deque)
{
    assert(this_Deque->_nb_elems > 0);
    int i_array = dq__i_array(this_Deque, this_Deque->_nb_elems - 1);
    return this_Deque->_elems[i_array];
}

static Variant dq_front(const Deque* this_Deque)
{
    assert(this_Deque->_nb_elems > 0);
    return this_Deque->_elems[this_Deque->_front_i];
}

static int dq_size(const Deque* this_Deque)
{
    return this_Deque->_nb_elems;
}

static bool dq_empty(const Deque* this_Deque)
{
    return (this_Deque->_nb_elems) == 0;
}

//insertion

//variadic for easier use than Variant

//private util
static void dq__insert_va_list(Deque* this_Deque, int i, struct _Structed_va_list* s_args_elem_value)
{
    //va_start(args_elem_value, i); must be called outside
    //assert((0 <= i) &&...); relevant assert must be called outside
    if(this_Deque->_nb_elems == this_Deque->_capacity)
    {
        dq__grow(this_Deque);
    }

    Variant new_elem = iCluige.iVariant.from_s_args(this_Deque->_elems_type, s_args_elem_value);

    //shift to left or right
    if(i < (this_Deque->_nb_elems / 2))
    {
        //shift i first elems to the left
        for(int i_deque = 0; i_deque < i; i_deque++)
        {
            int prev_i_array = dq__i_array_prev(this_Deque, i_deque);
            int i_array = dq__i_array(this_Deque, i_deque);
            this_Deque->_elems[prev_i_array] = this_Deque->_elems[i_array];
        }
        this_Deque->_front_i = dq__i_array_prev(this_Deque, 0);
    }
    else
    {
        //shift i last elems to the right
        for(int i_deque = this_Deque->_nb_elems - 1; i_deque >= i; i_deque--)
        {
            int next_i_array = dq__i_array(this_Deque, i_deque + 1);
            int i_array = dq__i_array(this_Deque, i_deque);
            this_Deque->_elems[next_i_array] = this_Deque->_elems[i_array];
        }
    }
    this_Deque->_elems[dq__i_array(this_Deque, i)] = new_elem;
    this_Deque->_nb_elems++;
    //va_end(args_elem_value); call outside
}

static void dq_push_back(Deque* this_Deque, ...)
{
    struct _Structed_va_list s_args_elem_value;
    va_start(s_args_elem_value.args, this_Deque);
    dq__insert_va_list(this_Deque, this_Deque->_nb_elems, &s_args_elem_value);
    va_end(s_args_elem_value.args);
}

static void dq_push_front(Deque* this_Deque, ...)
{
    struct _Structed_va_list s_args_elem_value;
    va_start(s_args_elem_value.args, this_Deque);
    dq__insert_va_list(this_Deque, 0, &s_args_elem_value);
    va_end(s_args_elem_value.args);
}

//same as push_back()
static void dq_append(Deque* this_Deque, ...)
{
    struct _Structed_va_list s_args_elem_value;
    va_start(s_args_elem_value.args, this_Deque);
    dq__insert_va_list(this_Deque, this_Deque->_nb_elems, &s_args_elem_value);
    va_end(s_args_elem_value.args);
}

static void dq_insert(Deque* this_Deque, int i, ...)
{
    assert((0 <= i) && (i <= this_Deque->_nb_elems));
    struct _Structed_va_list s_args_elem_value;
    va_start(s_args_elem_value.args, i);
    dq__insert_va_list(this_Deque, i, &s_args_elem_value);
    va_end(s_args_elem_value.args);
}

//returns a copy of replaced elem, for example if you need to do some free/delete
//returns NULL_VARIANT if no elem was replaced
static Variant dq_insert_or_replace_sorted(Deque* this_Deque, bool replace, ...)
{
    assert(this_Deque->_sorted);
    Variant res;
    struct _Structed_va_list s_args_elem_value;
    va_start(s_args_elem_value.args, replace);
    Variant new_elem = iCluige.iVariant.from_s_args(
                this_Deque->_elems_type, &s_args_elem_value);
    va_end(s_args_elem_value.args);

    int i_insert = 0;
    int this_size = iCluige.iDeque.size(this_Deque);
    if(this_size != 0)
    {
        struct _BSearchData bsd;
        bsd._i_min = 0;
        bsd._i_max = this_size - 1;
        dq_bsearch_rec(this_Deque, new_elem, &bsd);
        if(bsd._found)
        {
            i_insert = bsd._found_deque_index + 1;
            if(replace)
            {
                res = iCluige.iDeque.at(this_Deque, bsd._found_deque_index);//copy
                iCluige.iDeque.replace(this_Deque, bsd._found_deque_index, new_elem);
                return res;
            }
        }
        else
        {
            i_insert = bsd._found_insert_deque_index;
        }
    }
    va_start(s_args_elem_value.args, replace);
    dq__insert_va_list(this_Deque, i_insert, &s_args_elem_value);
    va_end(s_args_elem_value.args);
    return iCluige.iVariant.NULL_VARIANT;
}


//deletion

static Variant dq_pop_back(Deque* this_Deque)
{
    assert(this_Deque->_nb_elems > 0);
    Variant* res = dq__at_ptr(this_Deque, this_Deque->_nb_elems - 1);
    this_Deque->_nb_elems--;
    return *res;
}

static Variant dq_pop_front(Deque* this_Deque)
{
    assert(this_Deque->_nb_elems > 0);
    Variant* res = dq__at_ptr(this_Deque, 0);
    this_Deque->_nb_elems--;
    this_Deque->_front_i = dq__i_array(this_Deque, 1);
    return *res;
}

static void dq_remove(Deque* this_Deque, int i)
{
    assert((0 <= i) && (i < this_Deque->_nb_elems));
    //shift to left or right
    if(i < (this_Deque->_nb_elems / 2))
    {
        //shift i first elems to the right
        for(int i_deque = i; i_deque > 0; i_deque--)
        {
            int prev_i_array = dq__i_array_prev(this_Deque, i_deque);
            int i_array = dq__i_array(this_Deque, i_deque);
            this_Deque->_elems[i_array] = this_Deque->_elems[prev_i_array];
        }
        this_Deque->_front_i = dq__i_array(this_Deque, 1);
    }
    else
    {
        //shift i last elems to the left
        for(int i_deque = i; i_deque < this_Deque->_nb_elems - 1; i_deque++)
        {
            int next_i_array = dq__i_array(this_Deque, i_deque + 1);
            int i_array = dq__i_array(this_Deque, i_deque);
            this_Deque->_elems[i_array] = this_Deque->_elems[next_i_array];
        }
    }
    this_Deque->_nb_elems--;
}

static void dq_clear(Deque* this_Deque)
{
    this_Deque->_nb_elems = 0;
    this_Deque->_front_i = 0;//useless unless maybe for debug
}

static void dq_replace(Deque* this_Deque, int i, Variant new_v)
{
    assert(this_Deque->_nb_elems > i);
    Variant* elem_ptr = dq__at_ptr(this_Deque, i);
    (*elem_ptr) = new_v;
}


//search

//private utility

static int dq_default_compare_func(const Deque* this_Deque, Variant va, Variant vb)
{
    return iCluige.iVariant.compare(this_Deque->_elems_type, va, vb);
}

static int dq_default_compare_string_func(const Deque* this_Deque, Variant va, Variant vb)
{
    //va and vb are strings
    char* str_a = (char*)(va.ptr);
    char* str_b = (char*)(vb.ptr);
    return strcmp(str_a, str_b);
}

static int dq_default_compare_pair_key_func(const Deque* this_Deque, Variant va, Variant vb)
{
    //va and vb are pairs
    Pair* pa = (Pair*)(va.ptr);
    Pair* pb = (Pair*)(vb.ptr);
    assert(pa != NULL);
    assert(pb != NULL);
    Variant key_a = pa->first;
    Variant key_b = pb->first;
    return this_Deque->_compare_sub_func(this_Deque, key_a, key_b);
}

static void dq_bsearch_rec(const Deque* this_Deque, Variant searched_elem, struct _BSearchData* bsd)
{
    //assert(bsd->_i_min <= bsd->_i_max);
    assert(this_Deque->_nb_elems > 0);
    int comp;

    //end of recursivity
    if(bsd->_i_min >= bsd->_i_max)
    {
        Variant min_val = iCluige.iDeque.at(this_Deque, bsd->_i_min);
        comp = this_Deque->_compare_func(this_Deque, min_val, searched_elem);
        if(comp == 0)
        {
            bsd->_found = true;
            bsd->_found_deque_index = bsd->_i_min;
            return;
        }
        else if(comp < 0) //min_val < searched_elem
        {
            bsd->_found = false;
            bsd->_found_insert_deque_index = bsd->_i_min + 1;
            return;
        }
        else
        {
            bsd->_found = false;
            bsd->_found_insert_deque_index = bsd->_i_min;
            return;
        }
    }

    //bsearch general algo
    int i_middle = (bsd->_i_min + bsd->_i_max) / 2;
    Variant middle_val = iCluige.iDeque.at(this_Deque, i_middle);
    comp = this_Deque->_compare_func(this_Deque, searched_elem, middle_val);
    if(comp < 0) // searched_elem < middle_val
    {
        bsd->_i_max = i_middle - 1;
        return dq_bsearch_rec(this_Deque, searched_elem, bsd);
    }
    else if(comp == 0) // searched_elem == middle_val
    {
        bsd->_found = true;
        bsd->_found_deque_index = i_middle;
        return;
    }
    else // middle_val < searched_elem
    {
        bsd->_i_min = i_middle + 1;//recursivity
        return dq_bsearch_rec(this_Deque, searched_elem, bsd);
    }
}

static int dq_bsearch(const Deque* this_Deque, ...)
{
    assert(this_Deque->_sorted);
    int this_size = iCluige.iDeque.size(this_Deque);
    if(this_size == 0)
    {
        return -1;
    }
    //va_list args_elem_value;
    struct _Structed_va_list s_args_elem_value;
    va_start(s_args_elem_value.args, this_Deque);
    Variant searched_elem = iCluige.iVariant.from_s_args(this_Deque->_elems_type, &s_args_elem_value);
    va_end(s_args_elem_value.args);

    //add checks for first and last elems ? (when user suspects any
    // gain from it, they can code it specifically then)

    //cannot use standard c language bsearch function
    //because double-ended queue means i_start may be > i_end in the array
    struct _BSearchData bsd;
    bsd._i_min = 0;
    bsd._i_max = this_size - 1;
    dq_bsearch_rec(this_Deque, searched_elem, &bsd);
    if(bsd._found)
    {
        return bsd._found_deque_index;
    }
    else
    {
        return -1;
    }
}
//static int (*find)(const Deque* this_Deque, ...)
//{
//
//}

////////////////////////////////// iDeque /////////

void iiDeque_init()
{
    iCluige.iDeque.deque_alloc = dq_deque_alloc;
    iCluige.iDeque.pre_delete_Deque = dq_pre_delete_Deque;
    iCluige.iDeque.at = dq_at;
    iCluige.iDeque.back = dq_back;
    iCluige.iDeque.front = dq_front;
    iCluige.iDeque.size = dq_size;
    iCluige.iDeque.empty = dq_empty;
    iCluige.iDeque.push_back = dq_push_back;
    iCluige.iDeque.push_front = dq_push_front;
    iCluige.iDeque.append = dq_append;
    iCluige.iDeque.insert = dq_insert;
    iCluige.iDeque.insert_or_replace_sorted = dq_insert_or_replace_sorted;//not tested
    iCluige.iDeque.pop_back = dq_pop_back;
    iCluige.iDeque.pop_front = dq_pop_front;
    iCluige.iDeque.remove = dq_remove;
    iCluige.iDeque.clear = dq_clear;
    iCluige.iDeque.replace = dq_replace;

    iCluige.iDeque.default_compare_func = dq_default_compare_func;
    iCluige.iDeque.default_compare_pair_key_func = dq_default_compare_pair_key_func;
    iCluige.iDeque.default_compare_string_func = dq_default_compare_string_func;
    iCluige.iDeque.bsearch = dq_bsearch;
    iCluige.iDeque.bsearch_rec = dq_bsearch_rec;
//    iCluige.iDeque.find = dq_find;
}

