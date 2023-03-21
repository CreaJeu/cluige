#ifndef VARIANT_H_INCLUDED
#define VARIANT_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

typedef union _Variant Variant;
typedef enum _VariantType VariantType;

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
    //TODO : Cell, Deque, Map
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

struct iiVariant
{
    Variant (*fromVal)(VariantType valType, ...);
    Variant (*fromArgs)(VariantType valType, va_list args);
    //void (*add)(const Variant* v1, const Variant* v2, Variant *result);
};

void iiVariantInit();

#endif //VARIANT_H_INCLUDED
