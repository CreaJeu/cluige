#ifndef STRINGBUILDER_H_INCLUDED
#define STRINGBUILDER_H_INCLUDED

typedef struct _StringBuilder StringBuilder;

struct _StringBuilder
{
    char* builtString;
    size_t remainingSize;
	char* nextChar;
};

//~namespace
struct iiStringBuilder
{
    int DECIMAL_DIGITS_FOR_INT;
    int DECIMAL_DIGITS_FOR_POINTER;

    //char* stringAlloc(StringBuilder* sb, size_t maxSize)
	char* (*stringAlloc)(StringBuilder* sb, size_t maxSize);

	//void connectExistingString(StringBuilder* sb, char* dest)
	void (*connectExistingString)(StringBuilder* sb, char* dest);

	//void append(StringBuilder* sb, const char* formattedTail, ...)
	void (*append)(StringBuilder* sb, const char* formattedTail, ...);

	//void replace(StringBuilder* sb, const char* formattedTail, ...)
	void (*replace)(StringBuilder* sb, const char* formattedTail, ...);
};
//iStringBuilder : in iiCluige

//to be called only by cluigeInit() to set iStringBuilder functions pointers
void iiStringBuilderInit();

#endif // STRINGBUILDER_H_INCLUDED
