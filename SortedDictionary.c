
#include "cluige.h"
#include "Deque.h"
#include "SortedDictionary.h"

#include <stdarg.h>



////////////////////////////////// iiSortedDictionary /////////

//constructor/destructor (don't forget to use them)

static void sd_sorted_dictionary_alloc(SortedDictionary* this_SortedDictionary, VariantType keys_type, VariantType values_type, int capacity)
{
    iCluige.iDeque.deque_alloc(&(this_SortedDictionary->_pairs), VT_POINTER, capacity);
    this_SortedDictionary->_pairs._sorted = true;
    this_SortedDictionary->_pairs._compare_func = iCluige.iDeque.default_compare_pair_key_func;
    this_SortedDictionary->_keys_type = keys_type;
    this_SortedDictionary->_values_type = values_type;
//    this_SortedDictionary->compare_keys_func = sd__deque_compare_func;
}

static void sd_pre_delete_SortedDictionary(SortedDictionary* this_SortedDictionary)
{
    //if elements are pointers, they must be deleted outside of this function,
    //according to the user needs

    //pairs are internal types, automatically malloced by SortedDictionary
    //so, must be freed
    int size = iCluige.iDeque.size(&(this_SortedDictionary->_pairs));
    for(int i=0; i < size; i++)
    {
        Variant v = iCluige.iDeque.at(&(this_SortedDictionary->_pairs), i);
        free(v.ptr);//free pair
    }

    //call quasi-destructor of _pairs
    iCluige.iDeque.pre_delete_Deque(&(this_SortedDictionary->_pairs));
}

static void sd_set_compare_keys_func(SortedDictionary* this_SortedDictionary, int (*compare_key_func)(const Deque*, Variant, Variant))
{
    this_SortedDictionary->_pairs._compare_sub_func = compare_key_func;
}


//read

static Variant sd_get(const SortedDictionary* this_SortedDictionary, ...)
{
    const Deque* _pairs = &(this_SortedDictionary->_pairs);
    struct _Structed_va_list s_args_elem_value;
    va_start(s_args_elem_value.args, this_SortedDictionary);
    Variant key = iCluige.iVariant.from_s_args(
                _pairs->_elems_type, &s_args_elem_value);
    va_end(s_args_elem_value.args);

    Pair new_pair = { key, iCluige.iVariant.NULL_VARIANT };
    int i = iCluige.iDeque.bsearch(_pairs, new_pair);
    if(i == -1)
    {
        return iCluige.iVariant.NULL_VARIANT;
    }
    else
    {
        Pair* found = (Pair*)((iCluige.iDeque.at(_pairs, i)).ptr);
        return found->second;
    }
}


static int sd_size(const SortedDictionary* this_SortedDictionary)
{
    return iCluige.iDeque.size(&(this_SortedDictionary->_pairs));
}

static bool sd_is_empty(const SortedDictionary* this_SortedDictionary)
{
    return iCluige.iDeque.empty(&(this_SortedDictionary->_pairs));
}


//insertion

//variadic for easier use than Variant
//automatically insert or replace if key already present
//returns a copy of replaced value, for example if you need to do some free/delete
//or NULL_VARIANT if no value was replaced
static Variant sd_insert(SortedDictionary* this_SortedDictionary, ...)
{
    Deque* _pairs = &(this_SortedDictionary->_pairs);
    struct _Structed_va_list s_args_key_value;
    va_start(s_args_key_value.args, this_SortedDictionary);

    Pair new_pair_local = iCluige.iVariant.pair_from_args(
            this_SortedDictionary->_keys_type, this_SortedDictionary->_values_type,
            &s_args_key_value);
    va_end(s_args_key_value.args);
    int this_size = iCluige.iDeque.size(_pairs);
    if(this_size == 0)
    {
        Pair* new_pair = iCluige.checked_malloc(sizeof(new_pair_local));
        *new_pair = new_pair_local;
        iCluige.iDeque.push_back(_pairs, new_pair);
        return iCluige.iVariant.NULL_VARIANT;
    }
    Variant new_variant_pair_local;
    new_variant_pair_local.ptr = &new_pair_local;

    struct _BSearchData bsd;
    bsd._i_min = 0;
    bsd._i_max = this_size - 1;
    iCluige.iDeque.bsearch_rec(_pairs, new_variant_pair_local, &bsd);
    if(bsd._found)
    {
        Pair* found = (Pair*)((iCluige.iDeque.at(_pairs, bsd._found_deque_index)).ptr);
        Variant replaced_value = found->second;//copy
        found->second = new_pair_local.second;
        return replaced_value;
    }
    else
    {
        Pair* new_pair = iCluige.checked_malloc(sizeof(new_pair_local));
        *new_pair = new_pair_local;
        iCluige.iDeque.insert(_pairs, bsd._found_insert_deque_index, new_pair);
        return iCluige.iVariant.NULL_VARIANT;
    }
}


//deletion

static void sd_erase(SortedDictionary* this_SortedDictionary, ...)
{
}

static void sd_clear(SortedDictionary* this_SortedDictionary)
{
}

////////////////////////////////// iSortedDictionary /////////

void iiSortedDictionary_init()
{
    iCluige.iSortedDictionary.sorted_dictionary_alloc = sd_sorted_dictionary_alloc;
    iCluige.iSortedDictionary.pre_delete_SortedDictionary = sd_pre_delete_SortedDictionary;
    iCluige.iSortedDictionary.set_compare_keys_func = sd_set_compare_keys_func;
    iCluige.iSortedDictionary.get = sd_get;
    iCluige.iSortedDictionary.size = sd_size;
    iCluige.iSortedDictionary.is_empty = sd_is_empty;
    iCluige.iSortedDictionary.insert = sd_insert;
    iCluige.iSortedDictionary.erase = sd_erase;
    iCluige.iSortedDictionary.clear = sd_clear;
}

