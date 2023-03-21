#ifndef DEQUE_H_INCLUDED
#define DEQUE_H_INCLUDED

#include "Variant.h"
#include <stdbool.h>

// Uses Variant because no template/genericity/<T> in C
// Functions arguments can be variadic instead of Variant values,
// so users can write for example ...push_back(i+2);
// instead of ...push_back((Variant){i64 = i+2});
// Don't forget to call dequeAlloc() on your new instances of Deque

typedef struct _Deque Deque;

struct _Deque
{
    //all private (users should use only functions/methods)
    int _capacity;
    int _frontI;
    int _nbElems;
    Variant* _elems;
    VariantType _elemsType;
    bool _sorted;
    int (*_compareFunc)(Variant va, Variant vb);
};

struct iiDeque
{
    //constructor/destructor (don't forget to use them)

    void (*dequeAlloc)(Deque* thisDeque, VariantType elemsType, int capacity);
    void (*deleteDeque)(Deque* thisDeque);

    //read

    //(assert if out of bound)
    Variant (*at)(const Deque* thisDeque, int i);
    Variant (*back)(const Deque* thisDeque);
    Variant (*front)(const Deque* thisDeque);
    int (*size)(const Deque* thisDeque);
    bool (*empty)(const Deque* thisDeque);

    //insertion

    void (*push_back)(Deque* thisDeque, ...);//variadic for easier use than Variant
    void (*push_front)(Deque* thisDeque, ...);
    void (*append)(Deque* thisDeque, ...);//same as push_back()
    void (*insert)(Deque* thisDeque, int i, ...);
    //TODO void (*insertSorted)(Deque* thisDeque, ...);
    //one day? append_array()

    //deletion

    Variant (*pop_back)(Deque* thisDeque);
    Variant (*pop_front)(Deque* thisDeque);
    void (*remove)(Deque* thisDeque, int i);
    void (*clear)(Deque* thisDeque);
    //TODO
//    //one day? shuffle(), slice(iMin, iMax)
//
//    //search
//
//    int (*bsearch)(const Deque* thisDeque, ...);
//    int (*find)(const Deque* thisDeque, ...);
//    int (*rsearch)(const Deque* thisDeque, ...);//find last occurence
//    bool (*has)(const Deque* thisDeque, ...);
//    //one day? min(), max()

    //sort

    //TODO void (*sort)(Deque* thisDeque);
    //TODO void (*sort_custom)(Deque* thisDeque, int (*compareFunc)(Variant, Variant));
};

void iiDequeInit();

#endif //DEQUE_H_INCLUDED
