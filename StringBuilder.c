//#include <string.h> //no! problems with wchar.h !
#include <wchar.h>
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include "cluige.h"
#include "StringBuilder.h"


////////////////////////////////// iiStringBuilder /////////

static wchar_t* sbr_stringAlloc(StringBuilder* sb, size_t maxSize)
{
    sb->nextChar = iCluige.checkedMalloc((maxSize + 1) * sizeof(wchar_t));
    //sb->remainingSize = maxSize;
    return sb->nextChar;
}

static void sbr_connectExistingString(StringBuilder* sb, wchar_t* dest)
{
    sb->nextChar = dest;
    //sb->remainingSize = wcslen(dest);
}

static void sbr_append(StringBuilder* sb, const wchar_t* formattedTail, ...)
{
    va_list args;
    va_start(args, formattedTail);

    //not C11 //int written = vswprintf(sb->nextChar, sb->remainingSize, formattedTail, args);
    int written = vswprintf(sb->nextChar, formattedTail, args);
    //sb->remainingSize -= written;
    sb->nextChar += written;

    va_end(args);
}


////////////////////////////////// StringBuilder /////////

void iiStringBuilderInit()
{
    iCluige.iStringBuilder.stringAlloc = sbr_stringAlloc;
    iCluige.iStringBuilder.connectExistingString = sbr_connectExistingString;
    iCluige.iStringBuilder.append = sbr_append;

    //+1 because of truncated results of log10
    iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT = 1 + (int)(log10(UINT_MAX));
    double pointerMaxValue = pow(2, 8 * sizeof(void*));
    iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_POINTER = 1 + (int)(log10(pointerMaxValue));
}

