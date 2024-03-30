#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED

#include "Variant.h"
#include <stdbool.h>

// Uses Variant because no template/genericity/<T> in C
// Functions arguments can be variadic instead of Variant values,
// so users can write for example ...push_back(i+2);
// instead of ...push_back((Variant){i64 = i+2});
// Don't forget to call deque_alloc() on your new instances of Deque

typedef struct _Deque Deque;

struct _BSearchData
{
    int _i_min;
    int _i_max;
    bool _found;
    int _found_deque_index;
    int _found_insert_deque_index;
};

struct _Deque
{
    //all private (users should use only functions/methods)
    int _capacity;
    int _front_i;
    int _nb_elems;
    Variant* _elems;
    VariantType _elems_type;
    bool _sorted;

    int (*_compare_func)(const Deque* this_Deque, Variant va, Variant vb);
    //for use by custom _compare_func if needed (see example in SortedDictionary)
    int (*_compare_sub_func)(const Deque* this_Deque, Variant sub_va, Variant sub_vb);
};

struct iiDeque
{
    //constructor/destructor (don't forget to use them)

    void (*deque_alloc)(Deque* this_Deque, VariantType elems_type, int capacity);
    void (*pre_delete_Deque)(Deque* this_Deque);

    //read

    //(assert if out of bound)
    Variant (*at)(const Deque* this_Deque, int i);
    Variant (*back)(const Deque* this_Deque);
    Variant (*front)(const Deque* this_Deque);
    int (*size)(const Deque* this_Deque);
    bool (*empty)(const Deque* this_Deque);

    //insertion

    void (*push_back)(Deque* this_Deque, ...);//variadic for easier use than Variant
    void (*push_front)(Deque* this_Deque, ...);
    void (*append)(Deque* this_Deque, ...);//same as push_back()
    void (*insert)(Deque* this_Deque, int i, ...);

    //returns a copy of replaced elem, for example if you need to do some free/delete
    //or NULL_VARIANT if no elem was replaced
    Variant (*insert_or_replace_sorted)(Deque* this_Deque, bool replace, ...);
    //one day? append_array()

    //deletion

    Variant (*pop_back)(Deque* this_Deque);
    Variant (*pop_front)(Deque* this_Deque);
    void (*remove)(Deque* this_Deque, int i);
    void (*clear)(Deque* this_Deque);
    void (*replace)(Deque* this_Deque, int i, Variant new_v);
    //TODO one day? shuffle(), slice(iMin, iMax)

    //search

    int (*default_compare_func)(const Deque* this_Deque, Variant va, Variant vb);
    int (*default_compare_pair_key_func)(const Deque* this_Deque, Variant va, Variant vb);
    int (*default_compare_string_func)(const Deque* this_Deque, Variant va, Variant vb);//for convenience

    int (*bsearch)(const Deque* this_Deque, ...);
    void (*bsearch_rec)(const Deque* this_Deque, Variant searched_elem, struct _BSearchData* bsd);
//    int (*find)(const Deque* this_Deque, ...);
//    int (*rsearch)(const Deque* this_Deque, ...);//find last occurence
//    bool (*has)(const Deque* this_Deque, ...);
//    //one day? min(), max()

    //sort

    //TODO void (*sort)(Deque* this_Deque);
    //TODO void (*sort_custom)(Deque* this_Deque, int (*compare_func)(Variant, Variant));
};

void iiDeque_init();

#endif //DEQUE_H_INCLUDED
