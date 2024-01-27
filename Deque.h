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

struct _Deque
{
    //all private (users should use only functions/methods)
    int _capacity;
    int _front_i;
    int _nb_elems;
    Variant* _elems;
    VariantType _elems_type;
    bool _sorted;
    int (*_compare_func)(Variant va, Variant vb);
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
    void (*insert_sorted)(Deque* this_Deque, ...);
    //one day? append_array()

    //deletion

    Variant (*pop_back)(Deque* this_Deque);
    Variant (*pop_front)(Deque* this_Deque);
    void (*remove)(Deque* this_Deque, int i);
    void (*clear)(Deque* this_Deque);
    //TODO
//    //one day? shuffle(), slice(iMin, iMax)
//
//    //search
//
    int (*bsearch)(const Deque* this_Deque, ...);
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
