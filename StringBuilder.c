#include <string.h>
#include <stdio.h> //for vsnprintf()
#include <stdarg.h>
#include <limits.h>
#include <math.h>
#include "cluige.h"
#include "StringBuilder.h"


////////////////////////////////// iiStringBuilder /////////

static char* sbr_string_alloc(StringBuilder* sb, size_t max_size)
{
    sb->built_string = iCluige.checked_malloc((max_size + 1) * sizeof(char));
    sb->next_char = sb->built_string;
    sb->next_char[0] = 0;
    sb->remaining_size = max_size;
    return sb->next_char;
}

static void sbr_connect_existing_string(StringBuilder* sb, char* dest)
{
    sb->built_string = dest;
    sb->next_char = sb->built_string;
    sb->remaining_size = strlen(dest);//TODO really strlen(dest) ?
}

static void sbr__append_from_args(StringBuilder* sb, const char* formatted_tail, va_list args)
{
    int written = vsnprintf(sb->next_char, sb->remaining_size, formatted_tail, args);
    sb->remaining_size -= written;
    sb->next_char += written;
    *(sb->next_char) = '\0';
}

static void sbr_append(StringBuilder* sb, const char* formatted_tail, ...)
{
    va_list args;
    va_start(args, formatted_tail);
    sbr__append_from_args(sb, formatted_tail, args);
    va_end(args);
}

static void sbr_replace(StringBuilder* sb, const char* formatted_tail, ...)
{
    va_list args;
    va_start(args, formatted_tail);

    sb->remaining_size = strlen(sb->built_string) + sb->remaining_size;
    int written = vsnprintf(sb->built_string, sb->remaining_size, formatted_tail, args);

    sb->remaining_size -= written;
    sb->next_char = sb->built_string + written;

    va_end(args);
}

static char* sbr_clone(const char* source)
{
//    StringBuilder sb;
//    char* res = iCluige.iStringBuilder.string_alloc(&sb, strlen(source));
//    sbr_append(&sb, source);
    char* res = iCluige.checked_malloc((strlen(source) + 1) * sizeof(char));
    strcpy(res, source);
    return res;
}

static char* sbr_clone_formatted(size_t max_size, const char* formatted_tail, ...)
{
    StringBuilder sb;
    char* res = iCluige.iStringBuilder.string_alloc(&sb, max_size);
    //same as append() :
    va_list args;
    va_start(args, formatted_tail);
    sbr__append_from_args(&sb, formatted_tail, args);
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
//        char** res = iCluige.checked_malloc(nbSegments * sizeof(char*));
//        StringBuilder sb;
//        for(int i=0; i<nbSegments; i++)
//        {
//            int nextLength = segmentsPositions[i+1] - segmentsPositions[i] - separatorLength;
//            char* segment = iCluige.iStringBuilder.string_alloc(&sb, nextLength);
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

void iiStringBuilder_init()
{
    iCluige.iStringBuilder.string_alloc = sbr_string_alloc;
    iCluige.iStringBuilder.connect_existing_string = sbr_connect_existing_string;
    iCluige.iStringBuilder.append = sbr_append;
    iCluige.iStringBuilder.replace = sbr_replace;
    iCluige.iStringBuilder.clone = sbr_clone;
    iCluige.iStringBuilder.clone_formatted = sbr_clone_formatted;
    //iCluige.iStringBuilder.split = sbr_split;

    //+1 because of truncated results of log10
    iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_INT = 1 + (int)(log10(UINT_MAX));
    double pointer_max_value = pow(2, 8 * sizeof(void*));
    iCluige.iStringBuilder.DECIMAL_DIGITS_FOR_POINTER = 1 + (int)(log10(pointer_max_value));
}

