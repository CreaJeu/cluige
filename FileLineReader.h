#ifndef FILE_LINE_READER_H_INCLUDED
#define FILE_LINE_READER_H_INCLUDED

//for FILE*
#include <stdio.h>

typedef struct _FileLineReader FileLineReader;

//stores several lines into a (possibly growing) buffer
//separated each by a null character
struct _FileLineReader
{
	//private
	FILE* _file;
	char* _file_path;
	int _capacity;
	int _oldest_line;
	int _newest_line;
	int _eof_line;
	int _nb_chars;
	char* _buffer;
};

struct iiFileLineReader
{
	//constructor/destructor (don't forget to use them)
	void (*fileLineReader_alloc)(FileLineReader* this_FileLineReader, int capacity);
	//also inits/resets all fields, deep-copies file_path
	bool (*open_file_start_reader)(FileLineReader* this_FileLineReader, const char* path);
	void (*close_file)(FileLineReader* this_FileLineReader);
	void (*pre_delete_FileLineReader)(FileLineReader* this_FileLineReader);

	//first line of file is # 0
	//beware : buffer can be reallocated, don't store returned pointer, re-call get_line() each time
	const char* (*get_line)(FileLineReader* this_FileLineReader, int i);
	void (*forget_lines_before)(FileLineReader* this_FileLineReader, int i);//line # i is preserved
	bool (*feof)(FileLineReader* this_FileLineReader, int i);
};

void iiFileLineReader_init();

#endif //FILE_LINE_READER_H_INCLUDED
