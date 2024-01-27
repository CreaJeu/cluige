
#include "cluige.h"
#include "Deque.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>


////////////////////////////////// iiDeque /////////

//constructor/destructor (don't forget to use them)

static void dq_deque_alloc(Deque* this_Deque, VariantType elems_type, int capacity)
{
    this_Deque->_capacity = capacity;
    this_Deque->_front_i = 0;
    this_Deque->_nb_elems = 0;
    this_Deque->_elems_type = elems_type;
    this_Deque->_sorted = false;
    this_Deque->_compare_func = NULL;
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
static void dq__insert_va_list(Deque* this_Deque, int i, va_list args_elem_value)
{
    //va_start(args_elem_value, i); must be called outside
    //assert((0 <= i) &&...); relevant assert must be called outside
    if(this_Deque->_nb_elems == this_Deque->_capacity)
    {
        dq__grow(this_Deque);
    }

    Variant new_elem = iCluige.iVariant.from_args(this_Deque->_elems_type, args_elem_value);

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
    va_list args_elem_value;
    va_start(args_elem_value, this_Deque);
    dq__insert_va_list(this_Deque, this_Deque->_nb_elems, args_elem_value);
    va_end(args_elem_value);
}

static void dq_push_front(Deque* this_Deque, ...)
{
    va_list args_elem_value;
    va_start(args_elem_value, this_Deque);
    dq__insert_va_list(this_Deque, 0, args_elem_value);
    va_end(args_elem_value);
}

//same as push_back()
static void dq_append(Deque* this_Deque, ...)
{
    va_list args_elem_value;
    va_start(args_elem_value, this_Deque);
    dq__insert_va_list(this_Deque, this_Deque->_nb_elems, args_elem_value);
    va_end(args_elem_value);
}

static void dq_insert(Deque* this_Deque, int i, ...)
{
    assert((0 <= i) && (i <= this_Deque->_nb_elems));
    va_list args_elem_value;
    va_start(args_elem_value, i);
    dq__insert_va_list(this_Deque, i, args_elem_value);
    va_end(args_elem_value);
}

//just forward declaration, see implentation below
static int dq_bsearch_rec(const Deque* this_Deque, Variant searched_elem, int* i_min, int i_max);

static void dq_insert_sorted(Deque* this_Deque, ...)
{
    assert(this_Deque->_sorted);
    va_list args_elem_value;
    va_start(args_elem_value, this_Deque);
    int i_min = 0;
    int i_max = iCluige.iDeque.size(this_Deque) - 1;
    Variant new_elem = iCluige.iVariant.from_args(
                this_Deque->_elems_type, args_elem_value);
    va_end(args_elem_value);

    //find insert pos, store it in i_min
    dq_bsearch_rec(this_Deque, new_elem, &i_min, i_max);
    va_start(args_elem_value, this_Deque);
    dq__insert_va_list(this_Deque, i_min, args_elem_value);
    va_end(args_elem_value);
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


//search

//private utility
static int dq_compare(const Deque* this_Deque, Variant va, Variant vb)
{
    if((this_Deque->_compare_func) == NULL)
    {
        return iCluige.iVariant.compare(this_Deque->_elems_type, va, vb);
    }
    else
    {
        return this_Deque->_compare_func(va, vb);
    }
}

//i_min is changed at the end of the search to be the
// index at which searched_elem should be inserted if
// needed for a sorted insertion
static int dq_bsearch_rec(const Deque* this_Deque, Variant searched_elem, int* i_min, int i_max)
{
    int comp;

    if(*i_min > i_max)
    {
        return -1;
    }
    Variant min_val = iCluige.iDeque.at(this_Deque, *i_min);
    if(*i_min == i_max)
    {
        comp = dq_compare(this_Deque, min_val, searched_elem);
        if(comp == 0)
        {
            (*i_min)++;
            return i_max;
        }
        else if(comp < 0) //min_val < searched_elem
        {
            (*i_min)++;
            return -1;
        }
        else
        {
            return -1;
        }
    }

    //not really bsearch algo, optimisation for cases where big
    //sorted deques are built by many appends already in right order
    Variant max_val = iCluige.iDeque.at(this_Deque, i_max);
    comp = dq_compare(this_Deque, searched_elem, max_val);
    if(comp > 0) //searched_elem > max_val
    {
        (*i_min) = i_max + 1;
        return -1;
    }

    //bsearch general algo
    int i_middle = (*i_min + i_max) / 2;
    Variant middle_val = iCluige.iDeque.at(this_Deque, i_middle);
    comp = dq_compare(this_Deque, searched_elem, middle_val);
    if(comp < 0) // searched_elem < middle_val
    {
        return dq_bsearch_rec(this_Deque, searched_elem, i_min, i_middle - 1);
    }
    else if(comp == 0) // searched_elem == middle_val
    {
        *i_min = i_middle + 1;
        return i_middle;
    }
    else // middle_val < searched_elem
    {
        *i_min = i_middle + 1;//recursivity
        return dq_bsearch_rec(this_Deque, searched_elem, i_min, i_max);
    }
}

static int dq_bsearch(const Deque* this_Deque, ...)
{
    assert(this_Deque->_sorted);
    va_list args_elem_value;
    va_start(args_elem_value, this_Deque);
    Variant searched_elem = iCluige.iVariant.from_args(this_Deque->_elems_type, args_elem_value);

    //cannot use standard c language bsearch function
    //because double-ended queue means i_start may be > i_end in the array
    int i_min = 0;
    int i_max = iCluige.iDeque.size(this_Deque) - 1;
    int res = dq_bsearch_rec(this_Deque, searched_elem, &i_min, i_max);
    va_end(args_elem_value);
    return res;
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
    iCluige.iDeque.insert_sorted = dq_insert_sorted;
    iCluige.iDeque.pop_back = dq_pop_back;
    iCluige.iDeque.pop_front = dq_pop_front;
    iCluige.iDeque.remove = dq_remove;
    iCluige.iDeque.clear = dq_clear;
    iCluige.iDeque.bsearch = dq_bsearch;
//    iCluige.iDeque.find = dq_find;
}

