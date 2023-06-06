
#include "cluige.h"
#include "Variant.h"

#include <assert.h>
#include <stdarg.h>

////////////////////////////////// _Variant /////////

static Variant vrt_from_args(VariantType val_type, va_list args)
{
    Variant res;

    switch(val_type)
    {
    case VT_BOOL:
        res.b = (bool)(va_arg(args, int));
        //special case for bool : (from gcc)
    //'_Bool' is promoted to 'int' when passed through '...'
    //note: (so you should pass 'int' not '_Bool' to 'va_arg')
    //if this code [va_arg(args, bool)] was reached, the program would abort
        break;
    case VT_CHAR:
        res.c = (char)(va_arg(args, int));
        //special case too, char is passed to va_args as int
        break;
    case VT_INT32:
        res.i32 = va_arg(args, int32_t);
        break;
    case VT_UINT32:
        res.ui32 = va_arg(args, uint32_t);
        break;
    case VT_INT64:
        res.i64 = va_arg(args, int64_t);
        break;
    case VT_UINT64:
        res.ui64 = va_arg(args, uint64_t);
        break;
    case VT_FLOAT:
        res.f = (float)(va_arg(args, double));
        //special case too, float is passed to va_args as double
        break;
    case VT_DOUBLE:
        res.d = va_arg(args, double);
        break;
    case VT_POINTER:
        res.ptr = va_arg(args, void*);
        break;
    default:
        assert(false);//wrong type given to Variant.from_args(t,...)
    }

    return res;
}

static Variant vrt_from_val(VariantType val_type, ...)
{
    va_list args;
    va_start(args, val_type);
    Variant res = vrt_from_args(val_type, args);
    va_end(args);
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
            res = 1;
        }
        else if((v1.b) > (v2.b))
        {
            res = -1;
        }
        break;
    case VT_CHAR:
        if((v1.c) < (v2.c))
        {
            res = 1;
        }
        else if((v1.c) > (v2.c))
        {
            res = -1;
        }
        break;
    case VT_INT32:
        if((v1.i32) < (v2.i32))
        {
            res = 1;
        }
        else if((v1.i32) > (v2.i32))
        {
            res = -1;
        }
        break;
    case VT_UINT32:
        if((v1.ui32) < (v2.ui32))
        {
            res = 1;
        }
        else if((v1.ui32) > (v2.ui32))
        {
            res = -1;
        }
        break;
    case VT_INT64:
        if((v1.i64) < (v2.i64))
        {
            res = 1;
        }
        else if((v1.i64) > (v2.i64))
        {
            res = -1;
        }
        break;
    case VT_UINT64:
        if((v1.ui64) < (v2.ui64))
        {
            res = 1;
        }
        else if((v1.ui64) > (v2.ui64))
        {
            res = -1;
        }
        break;
    case VT_FLOAT:
        if((v1.f) < (v2.f))
        {
            res = 1;
        }
        else if((v1.f) > (v2.f))
        {
            res = -1;
        }
        break;
    case VT_DOUBLE:
        if((v1.d) < (v2.d))
        {
            res = 1;
        }
        else if((v1.d) > (v2.d))
        {
            res = -1;
        }
        break;
    case VT_POINTER:
        if((v1.ptr) < (v2.ptr))
        {
            res = 1;
        }
        else if((v1.ptr) > (v2.ptr))
        {
            res = -1;
        }
        break;
    default:
        assert(false);//wrong type given to Variant.from_args(t,...)
    }
    return res;
}

////////////////////////////////// iiVariant /////////


////////////////////////////////// Variant /////////

void iiVariant_init()
{
    iCluige.iVariant.from_val = vrt_from_val;
    iCluige.iVariant.from_args = vrt_from_args;
    iCluige.iVariant.compare = vrt_compare;
}

