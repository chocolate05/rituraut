/**************************************************************************
 *   proto.h  --  This file is a part of miniproject.                     *
 *                                                                        *
 *   Copyright (C) 2016  Ritu Raut                          		  *
 *                                                                        *
 *   GNU nano is free software: you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published    *
 *   by the Free Software Foundation, either version 3 of the License,    *
 *   or (at your option) any later version.                               *
 *                                                                        *
 *   GNU nano is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty          *
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.              *
 *   See the GNU General Public License for more details.                 *
 *                                                                        *
 *   You should have received a copy of the GNU General Public License    *
 *   along with this program.  If not, see http://www.gnu.org/licenses/.  *
 *                                                                        *
 **************************************************************************/

#ifndef __PROTO_H 
#define __PROTO_H 

	typedef struct filestruct {
		char *data;
		/* The text of this line. */
		int lineno;
		/* The number of this line. */
		
		int posx;
		/* The position of the character  //  Or you can say the x-coordinate of the current line*/
		int linesize;
		struct filestruct *next;
		/* Next node. */
		struct filestruct *prev;
		/* Previous node. */
		int type;
	} filestruct;


	typedef struct displayfile{
		filestruct *disfirst;

		filestruct *dislast;
	} displayfile; 

	typedef struct openfile{
		filestruct *filetop;
			/* The very first line in the file*/
		filestruct *filebot;
			/* The last line in the file*/
		filestruct *current;
			/* The current line in the file*/
		filestruct *disfirst;
			/* The first line displayed */
		filestruct *dislast;
			/* The last line displayed*/
		filestruct *copy;
			/* Copied node*/
		struct openfile *next;
			/* The next file , if any*/
		struct openfile *prev;
			/* The previous file, if any*/
		char * filename;
			/*The name of your current file*/
		int totlines;
	} openfile;

	typedef struct files{
		openfile *file1;

		openfile *file2;	
	}files;

	/*********************FUNCTION PROTOTYPES*******************************/

	void curses_init();

	filestruct *make_new_node(filestruct *prevnode);

	void create_windows();

	void delete_windows();
	
	void display_botwin();

	int select_menu_option();

	void init(openfile *l);

	filestruct *copy_node(const filestruct *src);

	void printdata(filestruct *node);

	void print_whole_file(openfile *l);

	void print_from_to(filestruct *from, filestruct *to);
	
	void print_nodes(filestruct *afterthis);

	void getdata(filestruct **node);

	void save_into_file(openfile *file);

	void open_file_to_read(char *filename);

	int stringreplace(filestruct **tmp);

	void unlink_node(filestruct *fileptr);

	void moveleft(filestruct **node);

	void moveright(filestruct **node);

	void moveup(filestruct **node);

	void movedown(filestruct **node);

	void display_line_col();

	void handle_enter(filestruct **node);

	void insert_char(filestruct **node, char c);

	void create_menu_win();

	int create_menu_win2();

	void do_output(char *input, filestruct *node);

	void search_word(filestruct **node);

	void display_line_with_nos(filestruct *node);

	void escape_mode();

	void go_to_line(filestruct **node, int pos);

	void rearrange_nodes(filestruct **node, int posx);

	int handle_commands(int choice);

	void open_file(openfile *file);

#endif

