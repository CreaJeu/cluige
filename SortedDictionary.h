#ifndef SORTED_DICTIONARY_H_INCLUDED
#define SORTED_DICTIONARY_H_INCLUDED

#include "Variant.h"
#include "SortedDictionary.h"
#include <stdbool.h>


// Don't forget to call sorted_dictionary_alloc() on your new instances of SortedDictionary

typedef struct _SortedDictionary SortedDictionary;

struct _SortedDictionary
{
    //all private (users should use only functions/methods)
    Deque _pairs;
    VariantType _keys_type;
    VariantType _values_type;
    int (*compare_keys_func)(Variant v1, Variant v2);//like in java
};

struct iiSortedDictionary
{
    //constructor/destructor (don't forget to use them)
    void (*sorted_dictionary_alloc)(SortedDictionary* this_SortedDictionary, VariantType keys_type, VariantType values_type, int capacity);
    void (*pre_delete_SortedDictionary)(SortedDictionary* this_SortedDictionary);

    //read

    //(null if not found)
    Variant (*get)(const SortedDictionary* this_SortedDictionary, ...);
    int (*size)(const SortedDictionary* this_SortedDictionary);
    bool (*is_empty)(const SortedDictionary* this_SortedDictionary);

    //insertion

    void (*insert)(SortedDictionary* this_SortedDictionary, ...);

    //deletion

    void (*erase)(SortedDictionary* this_SortedDictionary, ...);
    void (*clear)(SortedDictionary* this_SortedDictionary);

    //search

    bool (*has)(const SortedDictionary* this_SortedDictionary, ...);
};

void iiSortedDictionary_init();

#endif //SORTED_DICTIONARY_H_INCLUDED
