
#include "cluige.h"
#include "Deque.h"

#include <assert.h>
#include <stdarg.h>


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

static void dq_delete_Deque(Deque* this_Deque)
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

//static int (*find)(const Deque* this_Deque, ...)
//{
//
//}

////////////////////////////////// iDeque /////////

void iiDeque_init()
{
    iCluige.iDeque.deque_alloc = dq_deque_alloc;
    iCluige.iDeque.delete_Deque = dq_delete_Deque;
    iCluige.iDeque.at = dq_at;
    iCluige.iDeque.back = dq_back;
    iCluige.iDeque.front = dq_front;
    iCluige.iDeque.size = dq_size;
    iCluige.iDeque.empty = dq_empty;
    iCluige.iDeque.push_back = dq_push_back;
    iCluige.iDeque.push_front = dq_push_front;
    iCluige.iDeque.append = dq_append;
    iCluige.iDeque.insert = dq_insert;
    iCluige.iDeque.pop_back = dq_pop_back;
    iCluige.iDeque.pop_front = dq_pop_front;
    iCluige.iDeque.remove = dq_remove;
    iCluige.iDeque.clear = dq_clear;
//    iCluige.iDeque.find = dq_find;
}

