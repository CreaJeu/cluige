#include <string.h>
#include <stdio.h> //for vsnprintf()
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include "cluige.h"
#include "StringBuilder.h"


////////////////////////////////// iiStringBuilder /////////

static char* sbr_stringAlloc(StringBuilder* sb, size_t maxSize)
{
    sb->builtString = iCluige.checkedMalloc((maxSize + 1) * sizeof(char));
    sb->nextChar = sb->builtString;
    sb->remainingSize = maxSize;
    return sb->nextChar;
}

static void sbr_connectExistingString(StringBuilder* sb, char* dest)
{
    sb->builtString = dest;
    sb->nextChar = sb->builtString;
    sb->remainingSize = strlen(dest);
}

static void sbr_append(StringBuilder* sb, const char* formattedTail, ...)
{
    va_list args;
    va_start(args, formattedTail);

    int written = vsnprintf(sb->nextChar, sb->remainingSize, formattedTail, args);

    sb->remainingSize -= written;
    sb->nextChar += written;

    va_end(args);
}

static void sbr_replace(StringBuilder* sb, const char* formattedTail, ...)
{
    va_list args;
    va_start(args, formattedTail);

    sb->remainingSize = strlen(sb->builtString) + sb->remainingSize;
    int written = vsnprintf(sb->builtString, sb->remainingSize, formattedTail, args);

    sb->remainingSize -= written;
    sb->nextChar = sb->builtString + written;

    va_end(args);
}


////////////////////////////////// StringBuilder /////////

void iiStringBuilderInit()
{
    iCluige.iStringBuilder.stringAlloc = sbr_stringAlloc;
    iCluige.iStringBuilder.connectExistingString = sbr_connectExistingString;
    iCluige.iStringBuilder.append = sbr_append;
    iCluige.iStringBuilder.replace = sbr_replace;

    //+1 because of truncated results of log10
    iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT = 1 + (int)(log10(UINT_MAX));
    double pointerMaxValue = pow(2, 8 * sizeof(void*));
    iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_POINTER = 1 + (int)(log10(pointerMaxValue));
}

