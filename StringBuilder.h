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

	//shortcut function to use stringAlloc+append
	//source can be allocated on stack
	//user is responsible to free the returned char*
	//char* clone(const char* source)
	//char* clone_formatted(size_t maxSize, const char* formattedTail, ...)
	char* (*clone)(const char* source);
	char* (*clone_formatted)(size_t maxSize, const char* formattedTail, ...);

//	//utility function, like java String.split()
//	//does all malloc's to build the result
//	//char** split(const char* text, char separator);
//	char** (*split)(const char* text, const char* separator, int* nbSegments);
//	//TODO collection instead of char**+size
};
//iStringBuilder : in iiCluige

//to be called only by cluigeInit() to set iStringBuilder functions pointers
void iiStringBuilderInit();

#endif // STRINGBUILDER_H_INCLUDED
