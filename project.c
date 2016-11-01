/**************************************************************************
 *   project.c  --  This file is a part of miniproject.                   *
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
/*				Mini-Project  
			
			***TEXT EDITOR***
	-> Features that this text editor consists of : 
	1> Edit the current line 
	2> Move one line up 
	3> Move one line down 
	4> Delete the current line 
	5> Display the contents of the buffer 
	6> Add a line after the line at which you are navigating 
	7> Save changes and exit to main menu 
	8> Exit discarding any changes 
	9> Show the list of commands
	and many more
*/

#include<stdio.h>
#include<stdlib.h>
#include<ncurses.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <curses.h>
#include <menu.h>
#include <assert.h>
#include "proto.h"
#include "copy.h"
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

	enum{SAVE, SAVE_AND_EXIT, OPEN_FILE, GO_BACK, EXIT, MEN};
	/***************************** Menu ****************************/
ITEM **my_items;				
MENU *my_menu;
WINDOW *my_menu_win;
WINDOW *my_menu_win2;
ITEM *cur_item;
int background = 1;

	/************************Two windows declared globally*************************/
WINDOW *textwin;
WINDOW *botwin;
	/*************************** openfile globally declareed *************************/
openfile *filedata;
	/************************ init curses ************************************/
void curses_init(){
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	start_color();			/* Start color 			*/
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_RED, COLOR_WHITE);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_CYAN, COLOR_WHITE);
	init_pair(5, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_WHITE);
	init_color(COLOR_BLUE, 0, 0, 700);
	init_color(COLOR_WHITE, 1000, 1000, 1000);
	init_color(COLOR_BLACK, 0, 0, 0);
	init_color(COLOR_RED, 1000, 0, 0);

}
/***************************create a new node and link it to the previous node if the previous node exists********************************/
filestruct *make_new_node(filestruct *prevnode){
	filestruct *newnode = (filestruct *)malloc(sizeof(filestruct));	
			/*created memory for the newnode*/
	newnode->data = NULL;								
		/*the newnode formed initally has no data so set it to NULL*/
	newnode->prev = prevnode;							
		/*now link the prev of the newnode to the prevnode..*/
	newnode->next = NULL;
	newnode->lineno = (prevnode != NULL) ? prevnode->lineno + 1 : 1;
	//newnode->type = CONTINUE;
		/* if the previous node is NULL that means the new node formed is the first node..therefore lineno=1 or increase the lineno by 1*/
	newnode->linesize = 0;
		/* linesize of the the newnode created initially set to zero*/
	newnode->posx = 0;
		/*position of x on the newline is 0*/
	filedata->totlines++;
		/*the total number of lines in the files is increased*/
   	return newnode;
}
/************************************************* created windows for text and display of commands ******************************************************************************/
void create_windows(){
	int hbot = 3;
	textwin = newwin(LINES - hbot - 2, COLS, 2, 0);
	/*window for the text*/
	botwin = newwin(hbot , COLS, LINES - hbot , 0);
	/*windows for displaying the commands*/

	wattron(textwin, COLOR_PAIR(1));
	keypad(textwin, TRUE);
	start_color();			/* Start color 	*/
	wbkgd(textwin, COLOR_PAIR(1));
	display_botwin();
}
/******************************************** display commands on the bottom window ********************************************************************/	

void display_botwin(){
	werase(botwin);
	wattron(botwin, A_BOLD);	
	mvwprintw(botwin, 1, 1, "^W Search Word	 ^F First line	^L Last line  ^D Delete line   ^_ Go to Line   ^R Replace  ^Y Copy text  ^P Paste text  ^X Exit   ESCAPE Menu");
	wrefresh(botwin);
	wattroff(botwin, A_BOLD);
	if(filedata->filename)
	        wprintw(botwin, " FILENAME: %s", filedata->filename);
}
/********************************************delete windows*********************************************************************************************/
void delete_windows(){
	delwin(textwin);
	delwin(botwin);
	delwin(my_menu_win);
	delwin(my_menu_win2);
	endwin();
}

/************************************ create the menu window ******************************************************************************************/

void create_menu_win(){
	int n_choices, i, c;
	char *choices[] = {
			"SAVE",
			"SAVE AND EXIT",
			"OPEN FILE",
			"GO BACK",
			"EXIT",
			"VIEW",
			(char *)NULL,
		  };
	
	/* Create items */
	n_choices = ARRAY_SIZE(choices);
	my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
	for(i = 0; i < n_choices; ++i)
		my_items[i] = new_item(choices[i], choices[i]);
		/* Crate menu */
	my_menu = new_menu((ITEM **)my_items);
		/* Set menu option not to show the description */
	menu_opts_off(my_menu, O_SHOWDESC);
		/* Create the window to be associated with the menu */
	my_menu_win = newwin(2, COLS, 0, 0);
	keypad(my_menu_win, TRUE);    
		/* Set main window and sub window */
	set_menu_win(my_menu, my_menu_win);
	set_menu_format(my_menu, 6, 6);
	set_menu_mark(my_menu, " * ");

		/* Print a border around the main window and print a title */
		
	attron(COLOR_PAIR(2));
	attroff(COLOR_PAIR(2));

	refresh();
		/* Post the menu */
	post_menu(my_menu);
	wrefresh(my_menu_win);

	}
int create_menu_win2(){
	ITEM **my_items2;
	int c;				
	MENU *my_menu2;
	int n_choices, i;
	ITEM *cur_item2;

	char *choices[] = {
                        "Black on White  ",
                        "White on Black  ",
                        "Yellow on Black  ",
                        "Blue on White  ",
			(char *)NULL,
                  };
	n_choices = ARRAY_SIZE(choices);
	my_items2 = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

	for(i = 0; i < n_choices; ++i)
	        my_items2[i] = new_item(choices[i], choices[i]);
	my_items2[n_choices] = (ITEM *)NULL;

	my_menu2 = new_menu((ITEM **)my_items2);
		/* Set menu option not to show the description */
		menu_opts_off(my_menu, O_SHOWDESC);

		/* Create the window to be associated with the menu */
		my_menu_win2 = newwin(5, 17, 2, 88);
		keypad(my_menu_win2, TRUE);
	     
		/* Set main window and sub window */
		set_menu_win(my_menu2, my_menu_win2);

		/* Print a border around the main window and print a title */
		
		attron(COLOR_PAIR(2));
		attroff(COLOR_PAIR(2));

		/* Post the menu */
		post_menu(my_menu2);
		wrefresh(my_menu_win2);

	refresh();
	while((c = wgetch(my_menu_win2)))
	{   switch(c)
	    {	case KEY_DOWN:
		        menu_driver(my_menu2, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu2, REQ_UP_ITEM);
				break;
			case 10:	/* Enter */
				cur_item2 = current_item(my_menu2);
				pos_menu_cursor(my_menu2);
				free_item(my_items[0]);
				free_item(my_items[1]);
				free_menu(my_menu);
				endwin();
				refresh();

				return item_index(cur_item2);

		}
	}	
}


	/***************************************** select any one of the options from the menu ******************************************************************/

int select_menu_option(){
	int c;
	int n_choices = 5;
	while((c = wgetch(my_menu_win))){
	       switch(c){
			case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case KEY_LEFT:
				menu_driver(my_menu, REQ_LEFT_ITEM);
				break;
			case KEY_RIGHT:
				menu_driver(my_menu, REQ_RIGHT_ITEM);
				break;
			case KEY_NPAGE:
				menu_driver(my_menu, REQ_SCR_DPAGE);
				break;
			case KEY_PPAGE:
				menu_driver(my_menu, REQ_SCR_UPAGE);
				break;
			case 10:	/* Enter */
				cur_item = current_item(my_menu);
				pos_menu_cursor(my_menu);
				return item_index(cur_item);
		}
	}
}


	/*****************************display the column number and the line no*********************************************************************************/


void display_line_col(filestruct *node){
	int x, y;
	getyx(textwin, y, x);

        attron(COLOR_PAIR(2));
        wmove(botwin, 1, COLS - 20);
        wclrtoeol(botwin);
        if(node)
	//	mvwprintw(botwin, 1, COLS - 20, "Ln %d, Col %d", y + 1, x + 1);
        	mvwprintw(botwin, 1, COLS - 20, "Ln %d, Col %d", node->lineno, node->posx + 1);
        attroff(COLOR_PAIR(2));
        wrefresh(botwin);
        if(node)
                wmove(textwin, y, x);
}

/************************************************* displays the line entered *****************************************************************************/

void do_output(char *input, filestruct *node){
	int x, y;
	getyx(textwin, y, x);
	wmove(textwin, y, 0);
	wclrtoeol(textwin);
	wprintw(textwin, "%s", input);
	wmove(textwin, y, node->posx);
}

/*************************************** initialises the data of the file ********************************************************************************/
void init(openfile *l){
	l->filetop = (filestruct *)malloc(sizeof(filestruct));
	l->filebot = (filestruct *)malloc(sizeof(filestruct));
	l->disfirst = l->filetop;
	l->dislast = l->filebot;
	l->copy = NULL;
	l->filename = NULL;
	l->totlines = 0;
}

/*********************************************** prints the whole file *************************************************/
void print_whole_file(openfile *l){
	filestruct *p = l->filetop;
	if(!p) {
		return;
	}
	while(p) {
		wprintw(textwin, "%s", p->data);
		p = p->next;
	} 

}
/***************************************************  prints from a specific line to another specific line **********************************************************************/

void print_from_to(filestruct *from, filestruct *to){
	werase(textwin);

	while(from != to){
		wprintw(textwin, "%s", from->data);
		//wgetch(textwin);
		from = from->next;
	}
	if(from->data){
		wprintw(textwin, "%s", from->data);
	}
}
/**************************************************SEARCH WORD ***************************************************************************/
void search_word(filestruct **node){
	char *str = (char *)malloc(sizeof(char)*COLS);
	char *p;
	p = (char *)malloc(sizeof(char)*COLS);
	int n;
	filestruct *tmp;
	tmp = (filestruct *)malloc(sizeof(filestruct));
	tmp = filedata->filetop;
	wmove(botwin, 1, 0);
	wclrtoeol(botwin);
	mvwprintw(botwin, 1, 5, "search: ");
	echo(); 
	wscanw(botwin, "%s", str);
	wmove(botwin, 1, 10);
	wclrtoeol(botwin);
	noecho();

	while(tmp){
	p = strstr(tmp->data, str);
	n = p - &tmp->data[0];
	wmove(textwin, tmp->lineno - 1, n);
		if(!p){
		tmp = tmp->next;
	}
	else{
		*node = tmp;
		(*node)->posx = n;
		wmove(botwin, 1, 1);
		wclrtoeol(botwin);
		mvwprintw(botwin, 1, 1, "%s found :Ln %d Col %d", str, (*node)->lineno, (*node)->posx + 1);
		break;
		}		
	}
	if(!p && !tmp){
		wmove(botwin, 1, 0);
		wclrtoeol(botwin);
		mvwprintw(botwin, 1, 7, "%s ", str);
		wprintw(botwin, "Not found\n");
		free(p);
		free(str);
		free(tmp);
		return;
	}
}


/*********************************************  handles all the commands and the inputs of the menu **************************************************************************/

int handle_commands(int choice){
	char c;
	switch(choice){
		case EXIT:
			wmove(botwin, 1, 1);
			wclrtoeol(botwin);
			mvwprintw(botwin, 1, 1, "Are you sure you want to exit: y / n ");
			echo();
			wscanw(botwin, "%c", &c);
			noecho();
			if(c == 'y' || c == 'Y')
				return 1;
			display_botwin();
			return 0;
		case SAVE:
			save_into_file(filedata);
			display_botwin();
			return 0;
		case SAVE_AND_EXIT:
			save_into_file(filedata);
			return 1;
		case OPEN_FILE:
			if(!(filedata->filename)){
				werase(botwin);
				save_into_file(filedata);
			}
			else{
				werase(botwin);
				mvwprintw(botwin, 1, 1, "%s file is already opened", filedata->filename);
				wgetch(botwin);
				save_into_file(filedata);
			}
			display_botwin();
			
			return 0;
		case GO_BACK:
			return 0;
		case 5:
			switch(create_menu_win2()){
				case 0:
					background = 1;
					wbkgd(textwin, COLOR_PAIR(1));
					return 0;
				case 1:	
					background = 3;
					wbkgd(textwin, COLOR_PAIR(3));
					return 0;
				case 2:
					background = 5;
					wbkgd(textwin, COLOR_PAIR(5));
					return 0;
				case 3:
					background = 2;
					wbkgd(textwin, COLOR_PAIR(2));
					return 0;

			
			}

				
	}
}
/********************************************************** saves the contents into a file ***************************************************************************/
void save_into_file(openfile *file){
	if(!(file->filename)){
		wmove(botwin, 1, 1);
		wclrtoeol(botwin);
		mvwprintw(botwin, 1, 1, "Enter the filename: ");
		echo();
		file->filename = (char *)malloc(sizeof(char) * 100);
		wscanw(botwin, "%s", file->filename);
		noecho();
		mvwprintw(botwin, 1, 1, "Saved your contents in %s file", file->filename);
	}
	FILE *fp;
	fp = fopen(file->filename, "w");
	if(!fp){
		mvwprintw(botwin, 1, 1,  "Error opening the file");
	}
	filestruct *head;
	head = file->filetop;
	while(head != NULL){
		fwrite(head->data, head->linesize, 1, fp);
		head = head->next;
	}

	wmove(botwin, 1, 1);
	wclrtoeol(botwin);
	mvwprintw(botwin, 1, 1, "Saved your contents in %s file", file->filename);
	wgetch(botwin);
	fclose(fp);
}
/*********************************************************** open file ************************/
	
void open_file(openfile *file){
	if((file->filename)){
		;
	}
	FILE *fp;
	fp = fopen(file->filename, "a+");
	if(!fp){
		mvwprintw(botwin, 1, 1, "Error opening the file");
	}
}
/************************************************************ unlinks a node from the list *************************************************************************/

void unlink_node(filestruct *fileptr){
	if(!fileptr)
	        return;
	if(fileptr->prev != NULL)
		fileptr->prev->next = fileptr->next;
	if(fileptr->next != NULL)
		fileptr->next->prev = fileptr->prev;
	filestruct *temp;
	temp = fileptr->next;
	while(temp){
		temp->lineno--;
		temp = temp->next;
	}
	filedata->totlines--;
		
}

/**************************************************************   move left right up down***************************************************************/

void moveleft(filestruct **node){
	int x, y;
	getyx(textwin, y, x);

	if((*node)->posx == 0 && (*node)->lineno != 1){	//if it reaches the left most end of the line then move the cursor to the rightmost end of the previous line
		(*node) = (*node)->prev;
		(*node)->posx = (*node)->linesize - 1;
		wmove(textwin, (*node)->lineno - 1, (*node)->linesize - 1);
	}
		
	else if((*node)->posx == 0 && (*node)->lineno == 1) //left corner position
		;
	else{
		if(x -1 >= 0 ){				//middle point
			x = x - 1;
			wmove(textwin, y, x);
			(*node)->posx--;
		}

	}

}

void moveright(filestruct **node){
	int x, y;
	getyx(textwin, y, x);
	if((*node)->posx == (*node)->linesize - 1 && (*node)->next != NULL){ //at the end of the line
		(*node) = (*node)->next;
		(*node)->posx = 0; 
		wmove(textwin, (*node)->lineno - 1, 0);

	}
	else if ((*node)->posx < (*node)->linesize - 1 && (*node)->next == NULL){ //
		if(x + 1 < COLS){ //&& x + 1 <= node->linesize){
			x++;
			wmove(textwin, y, x);
		}

		(*node)->posx++;
	}
	else if((*node)->posx < (*node)->linesize - 1){
		if(x + 1 < COLS){ //&& x + 1 <= node->linesize){
			x++;
			wmove(textwin, y, x);
		}

		(*node)->posx++;

	}


}
void moveup(filestruct **node){
	int x, y;
	getyx(textwin, y, x);
	if((*node) == filedata->disfirst){
		if((*node)->prev != NULL){
			filedata->disfirst = filedata->disfirst->prev;
			filedata->dislast = filedata->dislast->prev;

			print_from_to(filedata->disfirst, filedata->dislast);
			//wmove(textwin, 0, (*node)->);

		}
	}
	

	if((*node)->lineno == 1) //firstline  -dont move
		;

	else if((*node)->prev->linesize - 1 < (*node)->posx ){ //previous line length smaller than the current postion 
		(*node) = (*node)->prev;
		if((*node) == filedata->disfirst){
			y = 0;
		}
		else
			y--;
		wmove(textwin, y, (*node)->linesize - 1);
		(*node)->posx = (*node)->linesize - 1;
	}
	else{
		if(y - 1 >= 0 && ((*node)->prev) != NULL){
		*node = (*node)->prev;
			if((*node) == filedata->disfirst){

				y = 0;
			}
			else
				y--;
			wmove(textwin, y, x);
		}
		if((*node)->next->linesize == 0){
			(*node)->posx = 0;
		}
		else
			(*node)->posx = (*node)->next->posx;	
	}
}

void movedown(filestruct **node){
	int x, y;
	getyx(textwin, y, x);
	/* if the cursor is on the last line of the screen the change the disfirst and the dislast */
	if((*node) == filedata->dislast){
		if((*node)->next != NULL){
			filedata->disfirst = filedata->disfirst->next;
			filedata->dislast = filedata->dislast->next;
			print_from_to(filedata->disfirst, filedata->dislast);
		}
	}
	
	if((*node)->next == NULL){
		return;
	}

	else if((*node)->next->linesize - 1 < (*node)->posx){
		(*node) = (*node)->next;
		if((*node) == filedata->dislast){
			y = LINES - 6;
		}
		else
			y++;
		if((*node)->data[0] == 0){
			wmove(textwin, y, 0);
		}
		else{
			wmove(textwin, y, (*node)->linesize - 1);
			(*node)->posx = (*node)->linesize - 1;
		}
	}
	else{
		if(y + 1 < LINES ){//&& (*node)->next != NULL){
		(*node) = (*node)->next;
			if((*node) == filedata->dislast){
				y = LINES - 6;
			}
			else
				y++;
			wmove(textwin, y, x);
		}

		(*node)->posx = (*node)->prev->posx;			
	}
}
/*************************************************************** go to line *************************************************************************/
			
			/* The node is changed to the node to which the #line indicates */

void go_to_line(filestruct **node, int pos){
	if(pos > filedata->totlines || pos <= 0){
		display_botwin();
		return;
	}
	*node = filedata->filetop;
	int i = 1;
	while(i < pos && *node){
		*node = (*node)->next;
		i++;
	}
	(*node)->posx = 0;
	wmove(textwin, i - 1, 0);
	display_botwin();
}

/**************************************************************** string replace ********************************************************************/

int stringreplace(filestruct **tmp){
	char *text, *orig, *new;
	orig = (char *)malloc(sizeof(char) * COLS);
	new = (char *)malloc(sizeof(char) * COLS);
	wmove(botwin, 1, 1);
	wclrtoeol(botwin);
	mvwprintw(botwin, 1, 1, "replace :");
	echo();
	wscanw(botwin, "%s", orig);
	wprintw(botwin, " with : ");
	wscanw(botwin, "%s", new);
	noecho();
	char *ch;
	char temp[128];
	int count = 0, lenorig = strlen(orig), lennew = strlen(new);
	(*tmp) = filedata->filetop;
	while(*tmp){
		text = (*tmp)->data;
		while(ch = strstr(text, orig)){
			count ++;
			strncpy(temp, text, ch - text);
			temp[ch - text] = '\0';
			sprintf(temp + (ch - text), "%s%s", new, ch + lenorig);
			text[0] = '\0';
			strcpy(text, temp);
			(*tmp)->linesize += lennew - lenorig;
		}
		if((*tmp)->next)
			(*tmp) = (*tmp)->next;
		else
			break;
	}
	(*tmp)->posx = (*tmp)->linesize -1;
	wmove(textwin, (*tmp)->lineno - 1, (*tmp)->posx);
	return count;
}


/**************************************  inserts a line in between two lines and then rearranges all the other nodes  **********************************************************/
void rearrange_nodes(filestruct **node, int posx){
	filestruct *insertnode;
	insertnode = (filestruct *)malloc(sizeof(filestruct));
	filestruct *temp;
	insertnode->prev = *node;
	insertnode->next = (*node)->next;
	insertnode->linesize = (*node)->linesize - (*node)->posx - 1;
	insertnode->posx = insertnode->linesize;
	insertnode->lineno = (*node)->lineno + 1;
	insertnode->data = (char *)malloc(sizeof(char) * insertnode->linesize);
	int i = posx;
	int j = 0;
	while((*node)->data[i]){
		insertnode->data[j] = (*node)->data[i];
		i++;
		j++;
	}
	insertnode->data[j] = 0;
	(*node)->linesize = (*node)->posx + 1;
	(*node)->data[(*node)->posx] = '\n';
	(*node)->data[(*node)->posx + 1] = '\0';
	if((*node)->next)
		(*node)->next->prev = insertnode;
	(*node)->next = insertnode;
	temp = insertnode->next;
	while(temp != NULL){
		temp->lineno++;
		temp = temp->next;
	}
	wclrtoeol(textwin);
	print_nodes(insertnode);
	(*node) = insertnode;
	filedata->totlines++;
}
/******************************  print nodes after a node ***********************************************/
void print_nodes(filestruct *afterthis){
	filestruct *temp;
	temp = (filestruct *)malloc(sizeof(filestruct));
	temp = afterthis;
	while(temp != NULL){
		mvwprintw(textwin, temp->lineno -1, 0, "%s", temp->data);
		wclrtoeol(textwin);
		temp = temp->next;
	}
}

void handle_enter(filestruct **node){
					/* Enter pressed at the end of the line creates a new node representing new line*/
	if((*node)->posx == (*node)->linesize){
		(*node)->data[(*node)->posx] = '\n';
		(*node)->linesize++;
		(*node)->posx++;
		(*node)->data[(*node)->posx] = '\0';
		do_output((*node)->data, (*node));

		if(filedata->filebot->lineno < (*node)->lineno)
			filedata->filebot = (*node);

		(*node)->next = (filestruct *)malloc(sizeof(filestruct));
		(*node)->next = make_new_node(*node);
		wmove(textwin, (*node)->lineno, 0);
						//(*node)->data[(*node)->posx + 1] = '\0';
		if((*node) == filedata->dislast){
			filedata->dislast = (*node)->next;
			filedata->disfirst = filedata->disfirst->next;
			print_from_to(filedata->disfirst, filedata->dislast);
		}
		(*node) = (*node)->next;
		(*node)->data = (char *)malloc(sizeof(char) * 1024);
		(*node)->data[0] = '\0';
						
	}

					/* Enter pressed in the middle of the line inserts a new node in between the current line and the next line*/
	else{
		(*node)->linesize++;
		rearrange_nodes(node, (*node)->posx);
		(*node)->posx = 0;

		if(filedata->filebot->lineno < (*node)->lineno)
			filedata->filebot = (*node);
						
		wmove(textwin, (*node)->lineno - 1, 0);
						

	}

}

void insert_char(filestruct **node, char c){
	char insert;
	char *temp;
	temp = (char *)malloc(sizeof(char) * 1024);
	if((*node)->posx == (*node)->linesize){

		(*node)->data[(*node)->posx] = c;
		(*node)->linesize++;
		(*node)->posx++;
		(*node)->data[(*node)->posx] = 0; //if you dont do this it prints junk
		do_output((*node)->data, (*node));
	}

	else{
					/*insert = c;
					strcpy(node->data, temp);*/					/* TODO PROBLEM TO FIX: If the nodelinesize exceeds the maxlength of line*/
		insert = c;
		temp[0] = '\0';
		strncpy(temp,(*node)->data,(*node)->posx);
		temp[(*node)->posx] = '\0';
		sprintf(temp + (*node)->posx, "%c%s", insert, &(*node)->data[(*node)->posx]);
		(*node)->data[0] = '\0';
		strcpy((*node)->data, temp);
		(*node)->posx++;
		(*node)->linesize++;
		do_output(&(*node)->data[0], (*node));		
	}

}



/*********************************   getdata    **********************************************/

void getdata(filestruct **node){
	FILE *fp;
	queue copy_queue;
	qinit(&copy_queue);
	cstack copy_stack;
	cinit(&copy_stack);
	//fp = NULL;
	int  x, y, fileopened = 0, start_copy;
	int i, c, j = 0, k = 0, m = 0, copy = 0;
	char insert;
	/*set node->data memory*/
	char *temp;
	temp = (char *)malloc(sizeof(char) * 1024);
	(*node)->data = (char *)malloc(sizeof(char) * 1024);
		/*open file only if argument is given that is filename is specified*/
	if(filedata->filename != NULL){
		fp = fopen(filedata->filename, "a+");
		fileopened = 1;
	}
	else
		fp = NULL;
	if(fp == NULL){
		fileopened = 0;
	}
	while(1){
		getyx(textwin, y, x);
		if(filedata->totlines > COLS - 6){
				
		}
		if(y == 0){
			filedata->disfirst = (*node);
		}
		if(y == (LINES - 6)){
			filedata->dislast = (*node);
		}
			/*if file is opened only then start reading the characters one by one*/
		if(fileopened){
			if((c = getc(fp)) != EOF){
				;
			}
			else{
				fclose(fp);
				fileopened = 0;
				c = wgetch(textwin);
			}
		}
		else if(((c = wgetch(textwin)) != EOF)){
			;
		}
		if(!(strcmp(keyname(c), "^X"))){
			if(handle_commands(EXIT)){ // gets out of the while loop ... closes the all the windows
				break;
			}
			else
				wmove(textwin, (*node)->lineno - 1, (*node)->posx);						
		}
		/* Escape KEY to move to the menu*/
		else if(!(strcmp(keyname(c), "^["))){
			if(handle_commands(select_menu_option())){
				break;
			}
			else
				wmove(textwin, (*node)->lineno - 1, (*node)->posx);						
		}
		/* CTRL+D Removes the node from the list */
		else if(!(strcmp(keyname(c), "^D"))){
			filestruct *remove;
			remove = (*node);
				/* set the node to the next if next node is available or else set it to the prev node */
			if((*node)->next)
				(*node) = (*node)->next;
			/* this condition is for the last node in the list which does not have a next node */
			else if((*node)->prev)
				(*node) = (*node)->prev;
			/* if the node left is the only node then dont remove it instead set the data to null */
			else{
				(*node)->data[0] = 0;
				do_output((*node)->data, (*node));
			}
			/* the node which is to be removed should not be the only node */
			if((*node)->prev || (*node)->next){
				(*node)->posx = 0;
				unlink_node(remove);
				werase(textwin);
				print_whole_file(filedata);
				wmove(textwin, (*node)->lineno - 1, 0);
			}
		
		}
		/* CTRL+W To search a word in a file */
		else if(!(strcmp(keyname(c), "^W"))){
			search_word(node);
			wgetch(botwin);
			display_botwin();
		}
		/* CTRL+_ To go to a specific line*/
		else if(!(strcmp(keyname(c), "^_"))){
			int pos = 0;
			werase(botwin);
			mvwprintw(botwin, 1, 1, "GO TO LINE: ");
			echo();				
			wscanw(botwin, "%d", &pos);
			noecho();

			go_to_line(node, pos);
		}
		else if(!(strcmp(keyname(c), "^F"))){
			go_to_line(node, 1);
		}
		else if(!(strcmp(keyname(c), "^L"))){
			go_to_line(node, filedata->filebot->lineno);
		}
		/* CTRL+R To replace a given word in a file */
		else if(!(strcmp(keyname(c), "^R"))){
			stringreplace(node);
			werase(textwin);
			print_whole_file(filedata);
			if((*node)->linesize == 0){
				(*node)->posx = 0;
			}

			display_botwin();
		}
		/* CTRL+Y To start copying text */
		else if(!(strcmp(keyname(c), "^Y"))){
			copy = 1;
			werase(botwin);
			mvwprintw(botwin, 1, 1, "COPY MODE ON:  use SHIFT + LEFT key to move to the left and SHIFT + RIGHT key to move to the right  ^P Paste text");
			
		}
		/*CTRL+P Paste the copied text */
		else if(!(strcmp(keyname(c), "^P"))){
			copy = 0;
			while(!qempty(&copy_queue)){
				c = dequeue(&copy_queue);
				switch(c){
					case 10:
						handle_enter(node);
						break;
					default:
						insert_char(node, c);

				}
			}
			while(!cempty(&copy_stack)){
				c = cpop(&copy_stack);
				switch(c){
					case 10:
						handle_enter(node);
						break;
					default:
						insert_char(node, c);
				}
			}

			werase(botwin);
			display_botwin();

		}

		else{
			switch(c){
			case KEY_LEFT:
					moveleft(node);
					break;
			case KEY_RIGHT:
					moveright(node);
					break;
			case KEY_UP:
					moveup(node);
					break;
			case KEY_DOWN:
					movedown(node);
					break;
			case KEY_SLEFT:
					moveleft(node);
					if(copy){
						if((*node)->posx == 0 && (*node)->lineno == 1){
							;
						}
						else if(!qempty(&copy_queue)){
							wchgat(textwin, 1, A_BLINK, background, NULL);
							queue qtemp;
							qinit(&qtemp);
							while(qlen(&copy_queue) != 1){
								enqueue(&qtemp, dequeue(&copy_queue));
							}
							dequeue(&copy_queue);
							queue qtemp2;
							qinit(&qtemp2);
							qtemp2 = qtemp;
							qtemp = copy_queue;
							copy_queue = qtemp2;
						}
						else{
							wchgat(textwin, 1, A_REVERSE, background, NULL);
							cpush(&copy_stack, (*node)->data[(*node)->posx]);
						}
					}
					
					break;

			case KEY_SRIGHT:if(copy){
						if((*node)->posx == 0 && (*node)->lineno == 1){
							;
						}	
						else if(!cempty(&copy_stack)){
							wchgat(textwin, 1, A_BLINK, background, NULL);
							cpop(&copy_stack);
						}
						else{
							wchgat(textwin, 1, A_REVERSE, background, NULL);
							enqueue(&copy_queue, (*node)->data[(*node)->posx]);
						}
					}
					
					moveright(node);
					break;
			case '\t':	i = 0;
					if((*node)->posx == (*node)->linesize){
						while(i < 4){
							(*node)->data[(*node)->posx] = ' ';
							(*node)->posx++;
							(*node)->linesize++;
							(*node)->data[(*node)->posx] = 0;
							i++;
							//do_output((*node)->data, (*node));
						}
					}
					else{
						while(i < 4){
							insert = ' ';
							temp[0] = '\0';
							strncpy(temp,(*node)->data,(*node)->posx);
							temp[(*node)->posx] = '\0';
							sprintf(temp + (*node)->posx, "%c%s", insert, &(*node)->data[(*node)->posx]);
							(*node)->data[0] = '\0';
							strcpy((*node)->data, temp);
							(*node)->posx++;
							(*node)->linesize++;
							i++;
						}
					}
					do_output((*node)->data, (*node));	
					break;
					/*Enter key*/
			case 10:
					/* Enter pressed at the end of the line creates a new node representing new line*/
					if((*node)->posx == (*node)->linesize){
						(*node)->data[(*node)->posx] = c;
						(*node)->linesize++;
						(*node)->posx++;
						(*node)->data[(*node)->posx] = '\0';
						do_output((*node)->data, (*node));

						if(filedata->filebot->lineno < (*node)->lineno)
							filedata->filebot = (*node);

						(*node)->next = (filestruct *)malloc(sizeof(filestruct));
						(*node)->next = make_new_node(*node);
						wmove(textwin, (*node)->lineno, 0);
						if((*node) == filedata->dislast){
							filedata->dislast = (*node)->next;
							filedata->disfirst = filedata->disfirst->next;
							print_from_to(filedata->disfirst, filedata->dislast);
						}
						(*node) = (*node)->next;
						(*node)->data = (char *)malloc(sizeof(char) * 1024);
						(*node)->data[0] = '\0';
					}

					/* Enter pressed in the middle of the line inserts a new node in between the current line and the next line*/
					else{
						(*node)->linesize++;
						rearrange_nodes(node, (*node)->posx);
						(*node)->posx = 0;

						if(filedata->filebot->lineno < (*node)->lineno)
							filedata->filebot = (*node);
						
						wmove(textwin, (*node)->lineno - 1, 0);
						

					}
					break;
			case KEY_BACKSPACE:
				getyx(textwin, y, x);
				/* Here you are removing the characters only from the screen*/
				if((*node)->posx != 0){
					memmove(&((*node)->data[x - 1]), &((*node)->data[x]), strlen((*node)->data) - x + 1);

				/* Here you removed the character from the original node*/
					(*node)->linesize--;
					(*node)->posx--;
					do_output((*node)->data, (*node));
				}
				/**/
				else if((*node)->posx == 0 && (*node)->prev){
					filestruct *remove;//= (filestruct *)malloc(sizeof(filestruct));
					remove = (*node);
					if((*node)->prev){
						(*node)->prev->data[(*node)->prev->linesize - 1] = 0;
						strcat((*node)->prev->data, (*node)->data);
						(*node)->prev->posx = (*node)->prev->linesize - 1;
						(*node)->prev->linesize += (*node)->linesize - 1;
						*node = (*node)->prev;
					}
					if(*node)
						unlink_node(remove);
					werase(textwin);
					print_whole_file(filedata);
					wmove(textwin, y - 1, (*node)->posx);
					
				}
				else{					/*After backspacing the first line*/
					(*node)->data[0] = 0;  
					(*node)->posx = 0;		/*this part removed the junk part that was printed*/
					(*node)->linesize = 0;
				}
					
				break;
			case KEY_RESIZE:
				endwin();
				refresh();
				print_whole_file(filedata);
				create_windows();
				display_botwin();
				wclear(textwin);
				create_menu_win();
				wrefresh(textwin);
				print_whole_file(filedata);
				go_to_line(node, (*node)->lineno);
				break;
			default:
				if((*node)->posx == (*node)->linesize){

					(*node)->data[(*node)->posx] = c;
					(*node)->linesize++;
					(*node)->posx++;
					(*node)->data[(*node)->posx] = 0; //if you dont do this it prints junk
					do_output((*node)->data, (*node));
				}

				else{
					/*insert = c;
					strcpy(node->data, temp);*/					/* TODO PROBLEM TO FIX: If the nodelinesize exceeds the maxlength of line*/
					insert = c;
					temp[0] = '\0';
					strncpy(temp,(*node)->data,(*node)->posx);
					temp[(*node)->posx] = '\0';
					sprintf(temp + (*node)->posx, "%c%s", insert, &(*node)->data[(*node)->posx]);
					(*node)->data[0] = '\0';
					strcpy((*node)->data, temp);
					(*node)->posx++;
					(*node)->linesize++;
					do_output(&(*node)->data[0], (*node));		
				}
					break;

			}
		}

		display_line_col(*node);
	}

}
/**************************************************************int main*********************************************************************************/
int main(int argc, char *argv[]){
	if(argc > 1 && !(strcmp(argv[1], "-h"))){
		char c;
		printf("\nUsage: ./project <filename> 	To open the file with the text editor\n\n\t\t\t***EDITOR COMMANDS***:\n\n");
		printf("KEY_LEFT		-Move left\n");
		printf("KEY_RIGHT		-Move right\n");
		printf("KEY_UP			-Move up\n");
		printf("KEY_DOWN		-Move down\n");
		printf("CTRL+D			-Delete the current line\n");
		printf("CTRL+Y			-Copy text\n");
		printf("SHIFT+KEY_LEFT		-Move left in copy text mode\n");
		printf("SHIFT+KEY_RIGHT		-Move right in copy text mode\n");
		printf("SHIFT+KEY_UP		-Move up in copy text mode\n");
		printf("SHIFT+KEY_DOWN		-Move down in copy text mode\n");
		printf("CTRL+P			-Paste text\n");
		printf("CTRL+_			-Go to line\n");
		printf("CTRL+W			-Search word\n");
		printf("CTRL+F			-Go to the first line of the file\n");
		printf("CTRL+L			-Go to the last line of the file\n");
		printf("CTRL+R			-Replace text\n");
		printf("KEY_ESCAPE		-Go to the Menu\n");
		printf("CTRL+X			-EXIT\n\n");
		printf("\t\t\t***MENU OPTIONS***\n\nVIEW:\t\tCan change the background color and the text color\nSAVE:\t\tSaves the contents in a file\nEXIT:\t\tExits from the program\nOPEN FILE:\tOpens a file for editing it\n\n\t\t PRESS ANY KEY TO CONTINUE WITH THE EDITOR\n");
		fflush(stdin);
		scanf("%c", &c);
	}

	filedata = (openfile *)malloc(sizeof(openfile));
	filestruct *newnode;
	curses_init();
	create_menu_win();
	init(filedata);				
	create_windows();
		

		/*created first node*/
	newnode = make_new_node(NULL);
		/*The totlines = 0 filetop & filebot memory*/

	filedata->filetop = newnode;
	filedata->filetop = newnode;
		
	if(argc > 1 && (strcmp(argv[1], "-h"))){ /*	if the filename is given read the contents of the file and save it into the list*/
		filedata->filename = (char *)malloc(sizeof(char) * 100);
		strcpy(filedata->filename, argv[1]);
	}
	else if(argc > 1 && (!(strcmp(argv[1], "-h"))) && argv[2]){
		filedata->filename = (char *)malloc(sizeof(char) * 100);
		strcpy(filedata->filename, argv[2]);
	}
//	else you can open a file through menu 
	else{
	        char c;
	        wmove(botwin, 1, 1);
	        wclrtoeol(botwin);
	        mvwprintw(botwin, 1, 1, "Do you want to open any file? y / n ");
	        
	        echo();
	        wscanw(botwin, "%c", &c);
	        if(c == 'y' || c == 'Y'){
	                filedata->filename = (char *)malloc(sizeof(char ) * 100);
	                wmove(botwin, 1, 1);
	                wclrtoeol(botwin);
	                mvwprintw(botwin, 1, 1, "Enter your filename ");
	                wscanw(botwin, "%s", filedata->filename);
	        }
	        noecho();
	        display_botwin();
	}
	display_line_col(newnode);
	getdata(&newnode);
	delete_windows();
}

