#ifndef VARIANT_H_INCLUDED
#define VARIANT_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

typedef union _Variant Variant;
typedef enum _VariantType VariantType;
typedef struct _Pair Pair;

enum _VariantType
{
    VT_BOOL,
    VT_CHAR,
    VT_INT32,
    VT_UINT32,
    VT_INT64,
    VT_UINT64,
    VT_FLOAT,
    VT_DOUBLE,
    VT_POINTER
    //TODO : Cell, Deque, Map, char*?
};

union _Variant
{
    bool b;
    char c;
    int32_t i32;
    uint32_t ui32;
    int64_t i64;
    uint64_t ui64;
    float f;
    double d;

    void* ptr;
    //TODO : Cell, Deque, Map
};

struct _Pair
{
    Variant first;
    Variant second;
};

struct iiVariant
{
    Variant NULL_VARIANT;

    Variant (*from_val)(VariantType val_type, ...);
    Variant (*from_args)(VariantType val_type, va_list args);
    int (*compare)(VariantType val_type, Variant v1, Variant v2);//like in java
};

void iiVariant_init();

#endif //VARIANT_H_INCLUDED
