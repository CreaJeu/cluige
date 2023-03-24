
#include "cluige.h"
#include "Variant.h"

#include <assert.h>
#include <stdarg.h>

////////////////////////////////// _Variant /////////

static Variant vrt_fromArgs(VariantType valType, va_list args)
{
    Variant res;

    switch(valType)
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
        assert(false);//wrong type given to Variant.fromArgs(t,...)
    }

    return res;
}

static Variant vrt_fromVal(VariantType valType, ...)
{
    va_list args;
    va_start(args, valType);
    Variant res = vrt_fromArgs(valType, args);
    va_end(args);
    return res;
}

static int vrt_compare(VariantType valType, Variant v1, Variant v2)
{
    int res = 0;
    switch(valType)
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
        assert(false);//wrong type given to Variant.fromArgs(t,...)
    }
    return res;
}

////////////////////////////////// iiVariant /////////


////////////////////////////////// Variant /////////

void iiVariantInit()
{
    iCluige.iVariant.fromVal = vrt_fromVal;
    iCluige.iVariant.fromArgs = vrt_fromArgs;
    iCluige.iVariant.compare = vrt_compare;
}

