
#include "cluige.h"
#include "Variant.h"

#include <assert.h>
#include <stdarg.h>
#include <string.h> //for memcmp()

////////////////////////////////// iiVariant /////////


static Variant vrt_from_s_args(VariantType val_type, struct _Structed_va_list* sargs)
{
    Variant res;

    switch(val_type)
    {
    case VT_BOOL:
        res.b = (bool)(va_arg(sargs->args, int));
        //special case for bool : (from gcc)
    //'_Bool' is promoted to 'int' when passed through '...'
    //note: (so you should pass 'int' not '_Bool' to 'va_arg')
    //if this code [va_arg(args, bool)] was reached, the program would abort
        break;
    case VT_CHAR:
        res.c = (char)(va_arg(sargs->args, int));
        //special case too, char is passed to va_args as int
        break;
    case VT_INT32:
        res.i32 = va_arg(sargs->args, int32_t);
        break;
    case VT_UINT32:
        res.ui32 = va_arg(sargs->args, uint32_t);
        break;
    case VT_INT64:
        res.i64 = va_arg(sargs->args, int64_t);
        break;
    case VT_UINT64:
        res.ui64 = va_arg(sargs->args, uint64_t);
        break;
    case VT_FLOAT:
        res.f = (float)(va_arg(sargs->args, double));
        //special case too, float is passed to va_args as double
        break;
    case VT_DOUBLE:
        res.d = va_arg(sargs->args, double);
        break;
    case VT_POINTER:
        res.ptr = va_arg(sargs->args, void*);
        break;
    default:
        assert(false);//wrong type given to Variant.from_args(t,...)
    }

    return res;
}

static Variant vrt_from_val(VariantType val_type, ...)
{
//    va_list args;
    struct _Structed_va_list s_args;
    va_start(s_args.args, val_type);
    Variant res = vrt_from_s_args(val_type, &s_args);
    va_end(s_args.args);
    return res;
}

static Pair vrt_pair_from_args(VariantType first_type, VariantType second_type, struct _Structed_va_list* sargs)
{
    Variant key = vrt_from_s_args(first_type, sargs);
    Variant val = vrt_from_s_args(second_type, sargs);

    Pair res;
    res.first = key;
    res.second = val;
    return res;
}

static int vrt_compare(VariantType val_type, Variant v1, Variant v2)
{
    int res = 0;
    switch(val_type)
    {
    case VT_BOOL:
        if((v1.b) < (v2.b))
        {
            res = -1;
        }
        else if((v1.b) > (v2.b))
        {
            res = 1;
        }
        break;
    case VT_CHAR:
        if((v1.c) < (v2.c))
        {
            res = -1;
        }
        else if((v1.c) > (v2.c))
        {
            res = 1;
        }
        break;
    case VT_INT32:
        if((v1.i32) < (v2.i32))
        {
            res = -1;
        }
        else if((v1.i32) > (v2.i32))
        {
            res = 1;
        }
        break;
    case VT_UINT32:
        if((v1.ui32) < (v2.ui32))
        {
            res = -1;
        }
        else if((v1.ui32) > (v2.ui32))
        {
            res = 1;
        }
        break;
    case VT_INT64:
        if((v1.i64) < (v2.i64))
        {
            res = -1;
        }
        else if((v1.i64) > (v2.i64))
        {
            res = 1;
        }
        break;
    case VT_UINT64:
        if((v1.ui64) < (v2.ui64))
        {
            res = -1;
        }
        else if((v1.ui64) > (v2.ui64))
        {
            res = 1;
        }
        break;
    case VT_FLOAT:
        if((v1.f) < (v2.f))
        {
            res = -1;
        }
        else if((v1.f) > (v2.f))
        {
            res = 1;
        }
        break;
    case VT_DOUBLE:
        if((v1.d) < (v2.d))
        {
            res = -1;
        }
        else if((v1.d) > (v2.d))
        {
            res = 1;
        }
        break;
    case VT_POINTER:
        if((v1.ptr) < (v2.ptr))
        {
            res = -1;
        }
        else if((v1.ptr) > (v2.ptr))
        {
            res = 1;
        }
        break;
    default:
        assert(00 == "wrong type given to Variant.from_args(t,...)");
    }
    return res;
}

static bool vrt_is_null(const Variant* v)
{
    return memcmp(v, &(iCluige.iVariant.NULL_VARIANT), sizeof(*v)) == 0;
}


////////////////////////////////// Variant /////////

void iiVariant_init()
{
    //iCluige.iVariant.NULL_VARIANT = (Variant)0;//not standard C
    iCluige.iVariant.NULL_VARIANT.d = 0;
    iCluige.iVariant.NULL_VARIANT.ptr = 0;///TODO better way to be sure to fully fill with 0? memcpy from int[]{0,0,0..}?
    iCluige.iVariant.from_val = vrt_from_val;
    iCluige.iVariant.from_s_args = vrt_from_s_args;
    iCluige.iVariant.pair_from_args = vrt_pair_from_args;
    iCluige.iVariant.compare = vrt_compare;
    iCluige.iVariant.is_null = vrt_is_null;
}

