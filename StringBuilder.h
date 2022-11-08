#ifndef STRINGBUILDER_H_INCLUDED
#define STRINGBUILDER_H_INCLUDED

typedef struct _StringBuilder StringBuilder;

struct _StringBuilder
{
    //size_t remainingSize;
	wchar_t* nextChar;
};

//~namespace
struct iiStringBuilder
{
    int DECIMAL_DIGITS_FOR_INT;
    int DECIMAL_DIGITS_FOR_POINTER;

    //wchar_t* stringAlloc(StringBuilder* sb, size_t maxSize)
	wchar_t* (*stringAlloc)(StringBuilder* sb, size_t maxSize);

	//void connectExistingString(StringBuilder* sb, wchar_t* dest)
	void (*connectExistingString)(StringBuilder* sb, wchar_t* dest);

	//void append(StringBuilder* sb, const wchar_t* formattedTail, ...)
	void (*append)(StringBuilder* sb, const wchar_t* formattedTail, ...);
};
//iStringBuilder : in iiCluige

//to be called only by cluigeInit() to set iStringBuilder functions pointers
void iiStringBuilderInit();

#endif // STRINGBUILDER_H_INCLUDED
