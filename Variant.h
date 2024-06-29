#ifndef VARIANT_H_INCLUDED
#define VARIANT_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

typedef union _Variant Variant;
typedef enum _VariantType VariantType;
typedef struct _Pair Pair;
typedef struct _Checked_Variant Checked_Variant;

struct _Structed_va_list
{
    va_list args;
};

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

struct _Checked_Variant
{
    bool valid;
    Variant v;
};

struct iiVariant
{
    Variant NULL_VARIANT;

    Variant (*from_val)(VariantType val_type, ...);
    Variant (*from_s_args)(VariantType val_type, struct _Structed_va_list* sargs);
    Pair (*pair_from_args)(VariantType first_type, VariantType second_type, struct _Structed_va_list* sargs);
    int (*compare)(VariantType val_type, Variant v1, Variant v2);// res < 0  <=>  v1 < v2
    bool (*is_null)(const Variant* v);
};

void iiVariant_init();

#endif //VARIANT_H_INCLUDED
