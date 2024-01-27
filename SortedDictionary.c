
#include "cluige.h"
#include "SortedDictionary.h"

#include <stdarg.h>
//
///*
//struct _SortedDictionary
//    SortedDictionary* _pairs;
//    VariantType _keys_type;
//    VariantType _values_type;
//
//struct iiSortedDictionary
//    void (*sorted_dictionary_alloc)(this, VariantType _keys_type, VariantType _values_type);
//    void (*pre_delete_SortedDictionary)(this);
//    //read
//    Variant (*get)(this_SortedDictionary, ...);
//    int (*size)(this_SortedDictionary);
//    bool (*is_empty)(this_SortedDictionary);
//    //insertion
//    void (*insert)(this_SortedDictionary, ...);
//    //deletion
//    void (*erase)(this_SortedDictionary, ...);
//    void (*clear)(this_SortedDictionary);
//    //search
//    bool (*has)(this_SortedDictionary, ...);
//*/
//////////////////////////////////// iiSortedDictionary /////////
//
////constructor/destructor (don't forget to use them)
//
//static void sd_sorted_dictionary_alloc(SortedDictionary* this_SortedDictionary, VariantType keys_type, VariantType values_type, int capacity)
//{
//    iCluige.iDeque.deque_alloc(&(this_SortedDictionary->_pairs), VT_POINTER, capacity);
//    this_SortedDictionary->_pairs._sorted = true;
//    this_SortedDictionary->_keys_type = keys_type;
//    this_SortedDictionary->_values_type = values_type;
//    this_SortedDictionary->compare_keys_func = NULL;
//}
//
//static void sd_pre_delete_SortedDictionary(SortedDictionary* this_SortedDictionary)
//{
//    //if elements are pointers, they must be deleted outside of this function,
//    //according to the user needs
//    iCluige.iDeque.pre_delete_Deque(&(this_SortedDictionary->_pairs));
//}
//
////private utils
////
////static int sd__i_array(const SortedDictionary* this_SortedDictionary, int i_deque)
////{
////    // i_array from i_deque:
////    return (this_SortedDictionary->_front_i + i_deque) % this_SortedDictionary->_capacity;
////}
////
////static int sd__i_array_prev(const SortedDictionary* this_SortedDictionary, int i_deque)
////{
////    //previous i_array (one-step closer to front) from i_deque
////    int i_prev_array = this_SortedDictionary->_front_i + i_deque - 1;
////    if(i_prev_array < 0)
////    {
////        //loop to other end of the array
////        i_prev_array = this_SortedDictionary->_capacity - 1;
////    }
////    return i_prev_array;
////}
////
////static Variant* sd__at_ptr(const SortedDictionary* this_SortedDictionary, int i)
////{
////    int i_array = sd__i_array(this_SortedDictionary, i);
////    return &(this_SortedDictionary->_elems[i_array]);
////}
////
////static void sd__grow(SortedDictionary* this_SortedDictionary)
////{
////    int new_cap = this_SortedDictionary->_capacity * 2;
////    Variant* grown = iCluige.checked_malloc(new_cap * sizeof(Variant));
////    for(int i=0; i < this_SortedDictionary->_nb_elems; i++)
////    {
////        int i_old = (this_SortedDictionary->_front_i + i) % this_SortedDictionary->_capacity;
////        grown[i] = this_SortedDictionary->_elems[i_old];
////    }
////
////    this_SortedDictionary->_capacity = new_cap;
////    this_SortedDictionary->_front_i = 0;
////    this_SortedDictionary->_elems = grown;
////}
//
////read
//
//static Variant sd_get(const SortedDictionary* this_SortedDictionary, ...)
//{
//    Deque* pairs = &(this_SortedDictionary->_pairs);
//    int i = iCluige.iDeque.bsearch(pairs, ...!!!****);
//    if(i == -1)
//    {
//        return iCluige.iVariant.NULL_VARIANT;
//    }
//    else
//    {
//        Pair* found = iCluige.iDeque.at(pairs, i);
//        return found->second;
//    }
//}
//
//static Variant sd_back(const SortedDictionary* this_SortedDictionary)
//{
//    assert(this_SortedDictionary->_nb_elems > 0);
//    int i_array = sd__i_array(this_SortedDictionary, this_SortedDictionary->_nb_elems - 1);
//    return this_SortedDictionary->_elems[i_array];
//}
//
//static Variant sd_front(const SortedDictionary* this_SortedDictionary)
//{
//    assert(this_SortedDictionary->_nb_elems > 0);
//    return this_SortedDictionary->_elems[this_SortedDictionary->_front_i];
//}
//
//static int sd_size(const SortedDictionary* this_SortedDictionary)
//{
//    return this_SortedDictionary->_nb_elems;
//}
//
//static bool sd_empty(const SortedDictionary* this_SortedDictionary)
//{
//    return (this_SortedDictionary->_nb_elems) == 0;
//}
//
////insertion
//
////variadic for easier use than Variant
//
////private util
//static void sd__insert_va_list(SortedDictionary* this_SortedDictionary, int i, va_list args_elem_value)
//{
//    //va_start(args_elem_value, i); must be called outside
//    //assert((0 <= i) &&...); relevant assert must be called outside
//    if(this_SortedDictionary->_nb_elems == this_SortedDictionary->_capacity)
//    {
//        sd__grow(this_SortedDictionary);
//    }
//
//    Variant new_elem = iCluige.iVariant.from_args(this_SortedDictionary->_elems_type, args_elem_value);
//
//    //shift to left or right
//    if(i < (this_SortedDictionary->_nb_elems / 2))
//    {
//        //shift i first elems to the left
//        for(int i_deque = 0; i_deque < i; i_deque++)
//        {
//            int prev_i_array = sd__i_array_prev(this_SortedDictionary, i_deque);
//            int i_array = sd__i_array(this_SortedDictionary, i_deque);
//            this_SortedDictionary->_elems[prev_i_array] = this_SortedDictionary->_elems[i_array];
//        }
//        this_SortedDictionary->_front_i = sd__i_array_prev(this_SortedDictionary, 0);
//    }
//    else
//    {
//        //shift i last elems to the right
//        for(int i_deque = this_SortedDictionary->_nb_elems - 1; i_deque >= i; i_deque--)
//        {
//            int next_i_array = sd__i_array(this_SortedDictionary, i_deque + 1);
//            int i_array = sd__i_array(this_SortedDictionary, i_deque);
//            this_SortedDictionary->_elems[next_i_array] = this_SortedDictionary->_elems[i_array];
//        }
//    }
//    this_SortedDictionary->_elems[sd__i_array(this_SortedDictionary, i)] = new_elem;
//    this_SortedDictionary->_nb_elems++;
//    //va_end(args_elem_value); call outside
//}
//
//static void sd_push_back(SortedDictionary* this_SortedDictionary, ...)
//{
//    va_list args_elem_value;
//    va_start(args_elem_value, this_SortedDictionary);
//    sd__insert_va_list(this_SortedDictionary, this_SortedDictionary->_nb_elems, args_elem_value);
//    va_end(args_elem_value);
//}
//
//static void sd_push_front(SortedDictionary* this_SortedDictionary, ...)
//{
//    va_list args_elem_value;
//    va_start(args_elem_value, this_SortedDictionary);
//    sd__insert_va_list(this_SortedDictionary, 0, args_elem_value);
//    va_end(args_elem_value);
//}
//
////same as push_back()
//static void sd_append(SortedDictionary* this_SortedDictionary, ...)
//{
//    va_list args_elem_value;
//    va_start(args_elem_value, this_SortedDictionary);
//    sd__insert_va_list(this_SortedDictionary, this_SortedDictionary->_nb_elems, args_elem_value);
//    va_end(args_elem_value);
//}
//
//static void sd_insert(SortedDictionary* this_SortedDictionary, int i, ...)
//{
//    assert((0 <= i) && (i <= this_SortedDictionary->_nb_elems));
//    va_list args_elem_value;
//    va_start(args_elem_value, i);
//    sd__insert_va_list(this_SortedDictionary, i, args_elem_value);
//    va_end(args_elem_value);
//}
//
//
////deletion
//
//static Variant sd_pop_back(SortedDictionary* this_SortedDictionary)
//{
//    assert(this_SortedDictionary->_nb_elems > 0);
//    Variant* res = sd__at_ptr(this_SortedDictionary, this_SortedDictionary->_nb_elems - 1);
//    this_SortedDictionary->_nb_elems--;
//    return *res;
//}
//
//static Variant sd_pop_front(SortedDictionary* this_SortedDictionary)
//{
//    assert(this_SortedDictionary->_nb_elems > 0);
//    Variant* res = sd__at_ptr(this_SortedDictionary, 0);
//    this_SortedDictionary->_nb_elems--;
//    this_SortedDictionary->_front_i = sd__i_array(this_SortedDictionary, 1);
//    return *res;
//}
//
//static void sd_remove(SortedDictionary* this_SortedDictionary, int i)
//{
//    assert((0 <= i) && (i < this_SortedDictionary->_nb_elems));
//    //shift to left or right
//    if(i < (this_SortedDictionary->_nb_elems / 2))
//    {
//        //shift i first elems to the right
//        for(int i_deque = i; i_deque > 0; i_deque--)
//        {
//            int prev_i_array = sd__i_array_prev(this_SortedDictionary, i_deque);
//            int i_array = sd__i_array(this_SortedDictionary, i_deque);
//            this_SortedDictionary->_elems[i_array] = this_SortedDictionary->_elems[prev_i_array];
//        }
//        this_SortedDictionary->_front_i = sd__i_array(this_SortedDictionary, 1);
//    }
//    else
//    {
//        //shift i last elems to the left
//        for(int i_deque = i; i_deque < this_SortedDictionary->_nb_elems - 1; i_deque++)
//        {
//            int next_i_array = sd__i_array(this_SortedDictionary, i_deque + 1);
//            int i_array = sd__i_array(this_SortedDictionary, i_deque);
//            this_SortedDictionary->_elems[i_array] = this_SortedDictionary->_elems[next_i_array];
//        }
//    }
//    this_SortedDictionary->_nb_elems--;
//}
//
//static void sd_clear(SortedDictionary* this_SortedDictionary)
//{
//    this_SortedDictionary->_nb_elems = 0;
//    this_SortedDictionary->_front_i = 0;//useless unless maybe for debug
//}
//
//
////search
//
////static int (*find)(const SortedDictionary* this_SortedDictionary, ...)
////{
////
////}

////////////////////////////////// iSortedDictionary /////////

void iiSortedDictionary_init()
{
//    iCluige.iSortedDictionary.sorted_dictionary_alloc = sd_sorted_dictionary_alloc;
//    iCluige.iSortedDictionary.pre_delete_SortedDictionary = sd_pre_delete_SortedDictionary;
//    iCluige.iSortedDictionary.at = sd_at;
//    iCluige.iSortedDictionary.back = sd_back;
//    iCluige.iSortedDictionary.front = sd_front;
//    iCluige.iSortedDictionary.size = sd_size;
//    iCluige.iSortedDictionary.empty = sd_empty;
//    iCluige.iSortedDictionary.push_back = sd_push_back;
//    iCluige.iSortedDictionary.push_front = sd_push_front;
//    iCluige.iSortedDictionary.append = sd_append;
//    iCluige.iSortedDictionary.insert = sd_insert;
//    iCluige.iSortedDictionary.pop_back = sd_pop_back;
//    iCluige.iSortedDictionary.pop_front = sd_pop_front;
//    iCluige.iSortedDictionary.remove = sd_remove;
//    iCluige.iSortedDictionary.clear = sd_clear;
////    iCluige.iSortedDictionary.find = sd_find;
}

