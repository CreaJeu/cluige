
#include "cluige.h"
#include "Variant.h"

#include <stdarg.h>

////////////////////////////////// _Variant /////////

static Variant vrt_fromArgs(VariantType valType, va_list args)
{
    Variant res;
//    switch(valType)
    if(valType == VT_BOOL)
    {
//    case 0:
        bool val = (bool)(va_arg(args, int));
        //special case for bool : (from gcc)
    //'_Bool' is promoted to 'int' when passed through '...'
    //note: (so you should pass 'int' not '_Bool' to 'va_arg')
    //if this code [va_arg(args, bool)] was reached, the program would abort
        res.b = val;
//        break;
    }
    else if(valType == VT_DOUBLE)
    {
//    case 0:
        double val = va_arg(args, double);
        res.d = val;
//        break;
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


////////////////////////////////// iiVariant /////////


////////////////////////////////// Variant /////////

void iiVariantInit()
{
    iCluige.iVariant.fromVal = vrt_fromVal;
    iCluige.iVariant.fromArgs = vrt_fromArgs;
}

