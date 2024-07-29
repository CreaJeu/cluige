
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "cluige.h"
#include "FileLineReader.h"


////////////////////////////////// iiFileLineReader /////////

//constructor/destructor (don't forget to use them)

static void flr_fileLineReader_alloc(FileLineReader* this_FileLineReader, int capacity)
{
	this_FileLineReader->_file = NULL;
	this_FileLineReader->_file_path = NULL;
	this_FileLineReader->_capacity = capacity;
	this_FileLineReader->_oldest_line = -1;
	this_FileLineReader->_newest_line = -1;
	this_FileLineReader->_nb_chars = 0;
	this_FileLineReader->_buffer = iCluige.checked_malloc(capacity * sizeof(char));
}

//also inits/resets all fields
static bool flr_open_file_start_reader(FileLineReader* this_FileLineReader, const char* path)
{
	//deep copy to prevent pointing to the stack
	int new_path_len = strlen(path) + 1;
	if(this_FileLineReader->_file_path != NULL)
	{
		free(this_FileLineReader->_file_path);
	}
	this_FileLineReader->_file_path = iCluige.checked_malloc(new_path_len * sizeof(char));
	strcpy(this_FileLineReader->_file_path, path);
	this_FileLineReader->_file = fopen(path, "r");
	if(this_FileLineReader->_file == 00)
	{
		return false;
	}
	this_FileLineReader->_oldest_line = -1;
	this_FileLineReader->_newest_line = -1;
	this_FileLineReader->_eof_line = INT_MAX;
	this_FileLineReader->_nb_chars = 0;
	return true;
}

static void flr_close_file(FileLineReader* this_FileLineReader)
{
	fclose(this_FileLineReader->_file);
}

static void flr_pre_delete_FileLineReader(FileLineReader* this_FileLineReader)
{
	if(this_FileLineReader->_file_path != NULL)
	{
		free(this_FileLineReader->_file_path);
	}
	free(this_FileLineReader->_buffer);
}

//beware : buffer can be reallocated, user should not store returned pointer, instead re-call get_line() each time
static const char* flr_get_line(FileLineReader* this_FileLineReader, int i)
{
	assert(this_FileLineReader->_oldest_line <= i);// && i <= this_FileLineReader->_newest_line);
	if(i > this_FileLineReader->_eof_line)
	{
		return NULL;
	}

	while(this_FileLineReader->_newest_line < i)
	{
		char* append_here = this_FileLineReader->_buffer + this_FileLineReader->_nb_chars;
		char* res = fgets(append_here,
				this_FileLineReader->_capacity - this_FileLineReader->_nb_chars,
				this_FileLineReader->_file);
		if(feof(this_FileLineReader->_file))
		{
			this_FileLineReader->_eof_line = this_FileLineReader->_newest_line + 1;
		}
		//final "\n" included (but no "\n" if last line of file is EOF without \n)
		if(feof(this_FileLineReader->_file) && res == NULL) // empty final line of the file
		{
			append_here[0] = '\0';
		}
		else if(res == NULL)
		{
			//assert(00 == "failed to read line"); or just a legitimate final empty line
			return NULL;
		}
		int new_line_nb_read = strlen(append_here);
		this_FileLineReader->_nb_chars += new_line_nb_read + 1;//final \0 allways added by fgets()
		int nb_read = this_FileLineReader->_nb_chars;
		char last_char_read = append_here[new_line_nb_read - 1];
		int max_len = this_FileLineReader->_capacity;
		while( (nb_read == max_len) && !feof(this_FileLineReader->_file) )
		{ // _bufer not long enough
			//re-alloc longer
			max_len *= 2;
			this_FileLineReader->_capacity = max_len;
			char* future_buffer = iCluige.checked_malloc(max_len * sizeof(char));
			int tmp_nb_chars_copied = 0;
			for(int line = this_FileLineReader->_oldest_line;
					line <= this_FileLineReader->_newest_line;
					line++)
			{
				const char* tmp_line = flr_get_line(this_FileLineReader, line);
				strcpy(future_buffer + tmp_nb_chars_copied, tmp_line);
				tmp_nb_chars_copied += strlen(tmp_line) + 1;
			}
			//and new line just partially read with fgets()
			strcpy(future_buffer + tmp_nb_chars_copied, append_here);
			// no +1 to override ending \0 from last fgets() which was incomplete
			tmp_nb_chars_copied += new_line_nb_read;
			assert(tmp_nb_chars_copied + 1 == nb_read);
			free(this_FileLineReader->_buffer);
			this_FileLineReader->_buffer = future_buffer;

			if(last_char_read != '\n')
			{
				append_here = this_FileLineReader->_buffer + tmp_nb_chars_copied;
				res = fgets(append_here, max_len - nb_read, this_FileLineReader->_file);
				if(feof(this_FileLineReader->_file) && res == NULL) // empty final line of the file
				{
					append_here[0] = '\0';
				}
				else if(res == NULL)
				{
					return NULL;
				}
				this_FileLineReader->_nb_chars += strlen(append_here);// + 1;//final \0 allways added by fgets()
				nb_read = this_FileLineReader->_nb_chars;
			}
			last_char_read = this_FileLineReader->_buffer[nb_read - 2];
		}
		this_FileLineReader->_newest_line++;
	}

	if(this_FileLineReader->_oldest_line < 0)
	{
		this_FileLineReader->_oldest_line = 0;
	}
	char* res = this_FileLineReader->_buffer;
	for(int c = this_FileLineReader->_oldest_line; c < i; c++)
	{
		res += 1 + strlen(res);
	}
	return res;
}

//line # i is preserved
static void flr_forget_lines_before(FileLineReader* this_FileLineReader, int i)
{
	iCluige.iFileLineReader.get_line(this_FileLineReader, i);
	assert(this_FileLineReader->_oldest_line <= i);
	int nb_forget = 0;
	char* line = this_FileLineReader->_buffer;
	for(int c = this_FileLineReader->_oldest_line; c < i; c++)
	{
		nb_forget += 1 + strlen(line);
		line = this_FileLineReader->_buffer + nb_forget;
	}

	for(int c = 0; c < (this_FileLineReader->_nb_chars - nb_forget); c++)
	{
		this_FileLineReader->_buffer[c] = this_FileLineReader->_buffer[c + nb_forget];
	}
	this_FileLineReader->_oldest_line = i;
	this_FileLineReader->_nb_chars -= nb_forget;
}

static bool flr_feof(FileLineReader* this_FileLineReader, int i)
{
	return i >= this_FileLineReader->_eof_line;
}


////////////////////////////////// iFileLineReader /////////

void iiFileLineReader_init()
{
	iCluige.iFileLineReader.fileLineReader_alloc = flr_fileLineReader_alloc;
	iCluige.iFileLineReader.open_file_start_reader = flr_open_file_start_reader;
	iCluige.iFileLineReader.close_file = flr_close_file;
	iCluige.iFileLineReader.pre_delete_FileLineReader = flr_pre_delete_FileLineReader;
	iCluige.iFileLineReader.get_line = flr_get_line;
	iCluige.iFileLineReader.forget_lines_before = flr_forget_lines_before;
	iCluige.iFileLineReader.feof = flr_feof;
}

