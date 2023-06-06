#ifndef STRINGBUILDER_H_INCLUDED
#define STRINGBUILDER_H_INCLUDED

typedef struct _StringBuilder StringBuilder;

struct _StringBuilder
{
    char* built_string;
    size_t remaining_size;
	char* next_char;
};

//~namespace
struct iiStringBuilder
{
    int DECIMAL_DIGITS_FOR_INT;
    int DECIMAL_DIGITS_FOR_POINTER;

    //char* string_alloc(StringBuilder* sb, size_t max_size)
	char* (*string_alloc)(StringBuilder* sb, size_t max_size);

	//void connect_existing_string(StringBuilder* sb, char* dest)
	void (*connect_existing_string)(StringBuilder* sb, char* dest);

	//void append(StringBuilder* sb, const char* formatted_tail, ...)
	void (*append)(StringBuilder* sb, const char* formatted_tail, ...);

	//void replace(StringBuilder* sb, const char* formatted_tail, ...)
	void (*replace)(StringBuilder* sb, const char* formatted_tail, ...);

	//shortcut function to use string_alloc+append
	//source can be allocated on stack
	//user is responsible to free the returned char*
	//char* clone(const char* source)
	//char* clone_formatted(size_t max_size, const char* formatted_tail, ...)
	char* (*clone)(const char* source);
	char* (*clone_formatted)(size_t max_size, const char* formatted_tail, ...);

//	//utility function, like java String.split()
//	//does all malloc's to build the result
//	//char** split(const char* text, char separator);
//	char** (*split)(const char* text, const char* separator, int* nbSegments);
//	//TODO collection instead of char**+size
};
//iStringBuilder : in iiCluige

//to be called only by cluige_init() to set iStringBuilder functions pointers
void iiStringBuilder_init();

#endif // STRINGBUILDER_H_INCLUDED
