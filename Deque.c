
#include "cluige.h"
#include "Deque.h"

#include <assert.h>
#include <stdarg.h>


////////////////////////////////// iiDeque /////////

//constructor/destructor (don't forget to use them)

static void dq_dequeAlloc(Deque* thisDeque, VariantType elemsType, int capacity)
{
    thisDeque->_capacity = capacity;
    thisDeque->_frontI = 0;
    thisDeque->_nbElems = 0;
    thisDeque->_elemsType = elemsType;
    thisDeque->_sorted = false;
    thisDeque->_compareFunc = NULL;
    thisDeque->_elems = iCluige.checkedMalloc(capacity * sizeof(Variant));
}

static void dq_deleteDeque(Deque* thisDeque)
{
    //if elements are pointers, they must be deleted outside of this function,
    //according to the user needs
    free(thisDeque->_elems);
}

//private utils

static int dq__iArray(const Deque* thisDeque, int iDeque)
{
    // iArray from iDeque:
    return (thisDeque->_frontI + iDeque) % thisDeque->_capacity;
}

static int dq__iArrayPrev(const Deque* thisDeque, int iDeque)
{
    //previous iArray (one-step closer to front) from iDeque
    int iPrevArray = thisDeque->_frontI + iDeque - 1;
    if(iPrevArray < 0)
    {
        //loop to other end of the array
        iPrevArray = thisDeque->_capacity - 1;
    }
    return iPrevArray;
}

static Variant* dq__atPtr(const Deque* thisDeque, int i)
{
    int iArray = dq__iArray(thisDeque, i);
    return &(thisDeque->_elems[iArray]);
}

static void dq__grow(Deque* thisDeque)
{
    int newCap = thisDeque->_capacity * 2;
    Variant* grown = iCluige.checkedMalloc(newCap * sizeof(Variant));
    for(int i=0; i < thisDeque->_nbElems; i++)
    {
        int iOld = (thisDeque->_frontI + i) % thisDeque->_capacity;
        grown[i] = thisDeque->_elems[iOld];
    }

    thisDeque->_capacity = newCap;
    thisDeque->_frontI = 0;
    thisDeque->_elems = grown;
}

//read

static Variant dq_at(const Deque* thisDeque, int i)
{
    assert((0 <= i) && (i < thisDeque->_nbElems));
    int iArray = dq__iArray(thisDeque, i);
    return thisDeque->_elems[iArray];
}

static Variant dq_back(const Deque* thisDeque)
{
    assert(thisDeque->_nbElems > 0);
    int iArray = dq__iArray(thisDeque, thisDeque->_nbElems - 1);
    return thisDeque->_elems[iArray];
}

static Variant dq_front(const Deque* thisDeque)
{
    assert(thisDeque->_nbElems > 0);
    return thisDeque->_elems[thisDeque->_frontI];
}

static int dq_size(const Deque* thisDeque)
{
    return thisDeque->_nbElems;
}

static bool dq_empty(const Deque* thisDeque)
{
    return (thisDeque->_nbElems) == 0;
}

//insertion

//variadic for easier use than Variant

//private util
static void dq__insert_va_list(Deque* thisDeque, int i, va_list args_elemValue)
{
    //va_start(args_elemValue, i); must be called outside
    //assert((0 <= i) &&...); relevant assert must be called outside
    if(thisDeque->_nbElems == thisDeque->_capacity)
    {
        dq__grow(thisDeque);
    }

    Variant newElem = iCluige.iVariant.fromArgs(thisDeque->_elemsType, args_elemValue);

    //shift to left or right
    if(i < (thisDeque->_nbElems / 2))
    {
        //shift i first elems to the left
        for(int iDeque = 0; iDeque < i; iDeque++)
        {
            int prevI_array = dq__iArrayPrev(thisDeque, iDeque);
            int i_array = dq__iArray(thisDeque, iDeque);
            thisDeque->_elems[prevI_array] = thisDeque->_elems[i_array];
        }
        thisDeque->_frontI = dq__iArrayPrev(thisDeque, 0);
    }
    else
    {
        //shift i last elems to the right
        for(int iDeque = thisDeque->_nbElems - 1; iDeque >= i; iDeque--)
        {
            int nextI_array = dq__iArray(thisDeque, iDeque + 1);
            int i_array = dq__iArray(thisDeque, iDeque);
            thisDeque->_elems[nextI_array] = thisDeque->_elems[i_array];
        }
    }
    thisDeque->_elems[dq__iArray(thisDeque, i)] = newElem;
    thisDeque->_nbElems++;
    //va_end(args_elemValue); call outside
}

static void dq_push_back(Deque* thisDeque, ...)
{
    va_list args_elemValue;
    va_start(args_elemValue, thisDeque);
    dq__insert_va_list(thisDeque, thisDeque->_nbElems, args_elemValue);
    va_end(args_elemValue);
}

static void dq_push_front(Deque* thisDeque, ...)
{
    va_list args_elemValue;
    va_start(args_elemValue, thisDeque);
    dq__insert_va_list(thisDeque, 0, args_elemValue);
    va_end(args_elemValue);
}

//same as push_back()
static void dq_append(Deque* thisDeque, ...)
{
    va_list args_elemValue;
    va_start(args_elemValue, thisDeque);
    dq__insert_va_list(thisDeque, thisDeque->_nbElems, args_elemValue);
    va_end(args_elemValue);
}

static void dq_insert(Deque* thisDeque, int i, ...)
{
    assert((0 <= i) && (i <= thisDeque->_nbElems));
    va_list args_elemValue;
    va_start(args_elemValue, i);
    dq__insert_va_list(thisDeque, i, args_elemValue);
    va_end(args_elemValue);
}


//deletion

static Variant dq_pop_back(Deque* thisDeque)
{
    assert(thisDeque->_nbElems > 0);
    Variant* res = dq__atPtr(thisDeque, thisDeque->_nbElems - 1);
    thisDeque->_nbElems--;
    return *res;
}

static Variant dq_pop_front(Deque* thisDeque)
{
    assert(thisDeque->_nbElems > 0);
    Variant* res = dq__atPtr(thisDeque, 0);
    thisDeque->_nbElems--;
    thisDeque->_frontI = dq__iArray(thisDeque, 1);
    return *res;
}

static void dq_remove(Deque* thisDeque, int i)
{
    assert((0 <= i) && (i < thisDeque->_nbElems));
    //shift to left or right
    if(i < (thisDeque->_nbElems / 2))
    {
        //shift i first elems to the right
        for(int iDeque = i; iDeque > 0; iDeque--)
        {
            int prevI_array = dq__iArrayPrev(thisDeque, iDeque);
            int i_array = dq__iArray(thisDeque, iDeque);
            thisDeque->_elems[i_array] = thisDeque->_elems[prevI_array];
        }
        thisDeque->_frontI = dq__iArray(thisDeque, 1);
    }
    else
    {
        //shift i last elems to the left
        for(int iDeque = i; iDeque < thisDeque->_nbElems - 1; iDeque++)
        {
            int nextI_array = dq__iArray(thisDeque, iDeque + 1);
            int i_array = dq__iArray(thisDeque, iDeque);
            thisDeque->_elems[i_array] = thisDeque->_elems[nextI_array];
        }
    }
    thisDeque->_nbElems--;
}

static void dq_clear(Deque* thisDeque)
{
    thisDeque->_nbElems = 0;
    thisDeque->_frontI = 0;//useless unless maybe for debug
}


//search

//static int (*find)(const Deque* thisDeque, ...)
//{
//
//}

////////////////////////////////// iDeque /////////

void iiDequeInit()
{
    iCluige.iDeque.dequeAlloc = dq_dequeAlloc;
    iCluige.iDeque.deleteDeque = dq_deleteDeque;
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

