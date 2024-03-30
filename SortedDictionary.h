#ifndef SORTED_DICTIONARY_H_INCLUDED
#define SORTED_DICTIONARY_H_INCLUDED

#include "Variant.h"
#include "SortedDictionary.h"
#include <stdbool.h>


// Don't forget to call sorted_dictionary_alloc() on your new instances of SortedDictionary

typedef struct _SortedDictionary SortedDictionary;

struct _SortedDictionary
{
    //private (users should use only functions/methods)
    Deque _pairs;//Deque< pair* >
    VariantType _keys_type;
    VariantType _values_type;
};

struct iiSortedDictionary
{
    //constructor/destructor (don't forget to use them)
    void (*sorted_dictionary_alloc)(SortedDictionary* this_SortedDictionary, VariantType keys_type, VariantType values_type, int capacity);
    void (*pre_delete_SortedDictionary)(SortedDictionary* this_SortedDictionary);

    void (*set_compare_keys_func)(SortedDictionary* this_SortedDictionary, int (*)(const Deque*, Variant, Variant));

    //read

    //(NULL_VARIANT if not found)
    Variant (*get)(const SortedDictionary* this_SortedDictionary, ...);

    //for loops ; returned Variant .ptr is to the Pair at i
    Variant (*at)(const SortedDictionary* this_SortedDictionary, int i);

    int (*size)(const SortedDictionary* this_SortedDictionary);
    bool (*is_empty)(const SortedDictionary* this_SortedDictionary);

    //insertion

    //automatically insert or replace if key already present
    //returns a copy of replaced value, for example if you need to do some free/delete
    //or NULL_VARIANT if no value was replaced
    Variant (*insert)(SortedDictionary* this_SortedDictionary, ...);

    //inserts without any existence check
    void (*insert_first)(SortedDictionary* this_SortedDictionary, ...);
    //inserts without any existence check
    void (*insert_last)(SortedDictionary* this_SortedDictionary, ...);

    //deletion

    void (*erase)(SortedDictionary* this_SortedDictionary, ...);
    void (*clear)(SortedDictionary* this_SortedDictionary);

    //search

    //use get() instead
    //bool (*has)(const SortedDictionary* this_SortedDictionary, ...);
};

void iiSortedDictionary_init();

#endif //SORTED_DICTIONARY_H_INCLUDED
