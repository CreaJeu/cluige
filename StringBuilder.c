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
    sb->nextChar[0] = 0;
    sb->remainingSize = maxSize;
    return sb->nextChar;
}

static void sbr_connectExistingString(StringBuilder* sb, char* dest)
{
    sb->builtString = dest;
    sb->nextChar = sb->builtString;
    sb->remainingSize = strlen(dest);
}

static void sbr__append_from_args(StringBuilder* sb, const char* formattedTail, va_list args)
{
    int written = vsnprintf(sb->nextChar, sb->remainingSize, formattedTail, args);
    sb->remainingSize -= written;
    sb->nextChar += written;
    *(sb->nextChar) = '\0';
}

static void sbr_append(StringBuilder* sb, const char* formattedTail, ...)
{
    va_list args;
    va_start(args, formattedTail);
    sbr__append_from_args(sb, formattedTail, args);
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

static char* sbr_stack_to_heap(const char* source)
{
//    StringBuilder sb;
//    char* res = iCluige.iStringBuilder.stringAlloc(&sb, strlen(source));
//    sbr_append(&sb, source);
    char* res = iCluige.checkedMalloc((strlen(source) + 1) * sizeof(char));
    strcpy(res, source);
    return res;
}

static char* sbr_formatted_to_heap(size_t maxSize, const char* formattedTail, ...)
{
    StringBuilder sb;
    char* res = iCluige.iStringBuilder.stringAlloc(&sb, maxSize);
    //same as append() :
    va_list args;
    va_start(args, formattedTail);
    sbr__append_from_args(&sb, formattedTail, args);
    va_end(args);
    return res;
}

//char** sbr_split(const char* text, const char* separator, int* outNbSegments)
//{
//    int totalLength = strlen(text);
//    int separatorLength = strlen(separator);
//    //quick and dirty, ok if text not too long
//    int* segmentsPositions = calloc(totalLength, sizeof(int));
//    if(segmentsPositions == NULL)
//    {
//        printf("\n\n\n    fatal memory alloc error !\n\n\n");
//        exit(EXIT_FAILURE);
//    }
//    int nbSegments = 0;
//    int lastPositionTested = 0;
//    int nextSegmentLength = -1;
//    char* currPos = text;
//    //find segments
//    while(lastPositionTested < totalLength - 1)
//    {
//        nextSegmentLength = strcspn(currPos, separator);
//        segmentsPositions[nbSegments] = lastPositionTested;
//        nbSegments++;
//        lastPositionTested += nextSegmentLength + separatorLength;
//        currPos += nextSegmentLength + separatorLength;
//    }
//    segmentsPositions[nbSegments] = totalLength + separatorLength;
//
//    //malloc and copy segments
//    (*outNbSegments) = nbSegments;
//    char** res = NULL;
//    currPos = text;
//    if(nbSegments != 0)
//    {
//        //array of char*
//        char** res = iCluige.checkedMalloc(nbSegments * sizeof(char*));
//        StringBuilder sb;
//        for(int i=0; i<nbSegments; i++)
//        {
//            int nextLength = segmentsPositions[i+1] - segmentsPositions[i] - separatorLength;
//            char* segment = iCluige.iStringBuilder.stringAlloc(&sb, nextLength);
//            res[i] = segment;
//            strncpy(segment, currPos, nextLength);
//            segment[nextLength] = 0;//needed?
//            currPos += nextLength + separatorLength;
//        }
//    }
//    free(segmentsPositions);
//    return res;
//}

////////////////////////////////// StringBuilder /////////

void iiStringBuilderInit()
{
    iCluige.iStringBuilder.stringAlloc = sbr_stringAlloc;
    iCluige.iStringBuilder.connectExistingString = sbr_connectExistingString;
    iCluige.iStringBuilder.append = sbr_append;
    iCluige.iStringBuilder.replace = sbr_replace;
    iCluige.iStringBuilder.stack_to_heap = sbr_stack_to_heap;
    iCluige.iStringBuilder.formatted_to_heap = sbr_formatted_to_heap;
    //iCluige.iStringBuilder.split = sbr_split;

    //+1 because of truncated results of log10
    iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT = 1 + (int)(log10(UINT_MAX));
    double pointerMaxValue = pow(2, 8 * sizeof(void*));
    iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_POINTER = 1 + (int)(log10(pointerMaxValue));
}

