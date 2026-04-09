
#include <curses.h>
#include <stdio.h>
#include "cluige.h"
#include "init_non_portable.h"

void _cluige_initscr()
{
	//maybe one day : separate from core cluige via 'display server'

#ifdef CLUIGE_X11
	char cols[8];
	sprintf(cols, "%d", iCluige.window_initial_size_cols);
	char lines[8];
	sprintf(lines, "%d", iCluige.window_initial_size_lines);

	char* tmp_argv[] = {iCluige.window_title, "-cols", cols, "-lines", lines, NULL};
	Xinitscr(5, tmp_argv);
#else
	initscr();
	resize_term(iCluige.window_initial_size_lines, iCluige.window_initial_size_cols);
	PDC_set_title(iCluige.window_title);
#endif

}

