	////////////////////////CORRECTIONS///////////////

	/*
		BACKSPACE AT LEFT CORNER
		BACKSPACE AT LEFT SIDE WITH A STRING FOLLOWING IT
		ENTER CURSOR AFTER ENTER
		
	*/

	#include<stdio.h>
	#include<stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <assert.h>
	#include <ncurses.h>
	#include <stdio.h>
	#include <stdarg.h>
	#include <unistd.h>
	#include <string.h>
	//#include <fcntl.h>
	#include <errno.h>
	#include <ctype.h>
	//#include <locale.h>


	#include <curses.h>
	#include <menu.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
	#define CTRLD 	4


	#define KEY_ESC 177 
	enum{SAVE, EDIT, OPEN_FILE, GO_BACK, EXIT};
	enum{ END, CONTINUE };
	#define MAX COLS
	int choice;
	WINDOW *textwin;
	WINDOW *botwin;
	/* More structure types. */
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

	openfile *filedata;
	char *choices[] = {
				"SAVE",
				"EDIT",
				"OPEN FILE",
				"GO BACK",
				"EXIT",
				(char *)NULL,
			  };


	/*********************FUNCTION PROTOTYPES*******************************/

	void curses_init();

	filestruct *make_new_node(filestruct *prevnode);

	filestruct *copy_node(const filestruct *src);

	void link_node(filestruct *node1, filestruct *node2);

	void printdata(filestruct *node);

	void print_whole_file(filestruct *node);

	void getdata(filestruct *node, char *temp);

	void save_into_file(filestruct *head, char *filename);

	void open_file_to_read(char *filename);

	void unlink_node(filestruct *fileptr);

	void delete_node(filestruct *node);

	void moveleft(filestruct *node);

	void moveright(filestruct *node);

	void moveup(filestruct *node);

	void movedown(filestruct *node);

	void display_line_col();

	void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

	void create_menu_win();

	void create_menu_win2();

	void do_output(char *input, filestruct *node);

	void search_word(filestruct **node);

	void display_line_with_nos(filestruct *node);

	void escape_mode();

	void after_end(filestruct *newnode, char *filename);

	void go_to_line(filestruct **node, int pos);

	void get_help();

	void rearrange_nodes(filestruct *node, int posx);

	void recursive_part(filestruct *node, char *filename);

	void save();

	void exit();
	////////////////////////////////////////////////////////////////////////////////////////////////////////

		ITEM **my_items;
	//	int c;				
		MENU *my_menu;
		WINDOW *my_menu_win;
	       // int n_choices;
		ITEM *cur_item;
	void create_menu_win(){
	/*	ITEM **my_items;
		int c;				
		MENU *my_menu;
		WINDOW *my_menu_win;
		int n_choices, i;
		ITEM *cur_item;
	*/	
		int n_choices, i;
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
	       // set_menu_sub(my_menu, derwin(my_menu_win, 6, 68, 3, 1));
		set_menu_format(my_menu, 5, 5);
		set_menu_mark(my_menu, " * ");

		/* Print a border around the main window and print a title */
	       // box(my_menu_win, 0, 0);
		
		attron(COLOR_PAIR(2));
		//mvprintw(LINES - 3, 0, "Use PageUp and PageDown to scroll");
		//mvprintw(LINES - 2, 0, "Use Arrow Keys to navigate (F1 to Exit)");
		attroff(COLOR_PAIR(2));

		refresh();
		/* Post the menu */
		post_menu(my_menu);
		wrefresh(my_menu_win);
		
	/*	while((c = wgetch(my_menu_win)) != KEY_F(1))
		{       switch(c)
			{	case KEY_DOWN:
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
	/*				cur_item = current_item(my_menu);
					move(LINES - 2, 0);
					clrtoeol();
					mvprintw(LINES - 2, 0, "You have chosen %d item with name %s and description %s", 
					item_index(cur_item) + 1,  item_name(cur_item), 
					item_description(cur_item));
					
					refresh();
					choice = item_index(cur_item);- 
					pos_menu_cursor(my_menu);
					break;
			


			}
			wrefresh(my_menu_win);
		}*/	

		/* Unpost and free all the memory taken up */
	       // unpost_menu(my_menu);
		//free_menu(my_menu);
	       // for(i = 0; i < n_choices; ++i)
		 //       free_item(my_items[i]);
		//endwin();

	}

	void after_end(filestruct *newnode, char *filename){
		int c;
		int chosen = 0;
	int j = 5;
		while((c = wgetch(my_menu_win)) != KEY_F(1)){
		chosen = 0;
		       switch(c)
			{	case KEY_DOWN:
					menu_driver(my_menu, REQ_DOWN_ITEM);
					break;
				case KEY_UP:
					menu_driver(my_menu, REQ_UP_ITEM);
					break;
				case KEY_LEFT:
	wprintw(textwin, "pressed left key");
					menu_driver(my_menu, REQ_LEFT_ITEM);
					break;
				case KEY_RIGHT:
	wprintw(textwin, "pressed right key");
					menu_driver(my_menu, REQ_RIGHT_ITEM);
					break;
				case KEY_NPAGE:
					menu_driver(my_menu, REQ_SCR_DPAGE);
					break;
				case KEY_PPAGE:
					menu_driver(my_menu, REQ_SCR_UPAGE);
					break;
				case 10:	/* Enter */
	wprintw(textwin, "presssesd enter");
					cur_item = current_item(my_menu);
					move(LINES - 2, 0);
					wclrtoeol(textwin);
					mvwprintw(textwin, LINES - 10, 0, "You have chosen %d item with name %s and description %s", 
					item_index(cur_item) + 1,  item_name(cur_item), 
					item_description(cur_item));
					
					wrefresh(textwin);
					choice = item_index(cur_item);
	//mvwprintw(textwin, 12, 12, "item chosen is %d\n", choice);
					pos_menu_cursor(my_menu);
					chosen = 1;
					break;
			


			}
			wrefresh(my_menu_win);
			if(chosen){
			wprintw(textwin, "chosen!!!!");	
				break;
			}
		}
	wgetch(textwin);
	wprintw(textwin, "out of menu");
	wgetch(textwin);
			if(choice == SAVE){
				save_into_file(newnode, filename);
			}
			if(choice == GO_BACK ){
				werase(textwin);
				filestruct *copyhead;
				copyhead = filedata->filetop;
				print_whole_file(copyhead);
				display_line_col(copyhead);
				wrefresh(textwin);
				mvwprintw(botwin, 1, 2, "you can start typing now");
				
				wmove(textwin, 0, 0 );
			}
			if(choice == OPEN_FILE){
				wprintw(textwin, "Enter the name of the file you want to open \n");
				wscanw(textwin, "%s", filename);
				open_file_to_read(filename);		
			}
			if(choice == EXIT){
			if(c == 'Y' || c == 'y'){		
				delwin(textwin);
				delwin(botwin);
				unpost_menu(my_menu);
				free_menu(my_menu);
	       //	for(int i = 0; i < n_choices; ++i)
		 //       free_item(my_items[i]);

				delwin(my_menu_win);
				endwin();
			}
			else{
				noecho();
				werase(textwin);
				filestruct *node2;
				node2 = filedata->filetop;
				print_whole_file(node2);
		//	wmove(textwin, filedata->filebot->lineno - 1, filedata->filebot->linesize - 1);
				recursive_part(filedata->filebot, filedata->filename);
			}
		}

	}

	void get_help(){
		wmove(botwin, 1, 1);
		wclrtoeol(botwin);
		mvwprintw(botwin, 1, 1, "Press q to quit this window");
		wrefresh(botwin);
		WINDOW *help;
		int hbot = 5;
		int c;
		help = newwin(LINES - hbot, COLS, 2, 0);
		wprintw(help, " The notation for shortcuts is as follows: Control-key sequences are\nnotated with a caret (^) symbol and can be entered either by using the\nControl (Ctrl) key or pressing the Escape (Esc) key twice.  Escape-key\nsequences are notated with the Meta (M-) symbol and can be entered using\neither the Esc, Alt, or Meta key depending on your keyboard setup.  Also,\npressing Esc twice and then typing a three-digit decimal number from 000\nto 255 will enter the character with the corresponding value.  The\nfollowing keystrokes are available in the main editor window.\nAlternative keys are shown in parentheses:");
		while(c = wgetch(help)){
			if(c == 'q'){
			werase(help);
//			system("clear");
			delwin(help);
	//		wgetch(textwin);
			werase(textwin);
			filestruct *temp;
			temp = filedata->filetop;
			print_whole_file(temp);
			wrefresh(textwin);
			return;
			}
		}
/*		werase(help);
		delwin(help); */
	}
	void create_menu_win2()
	{	ITEM **my_items;
		int c;				
		MENU *my_menu;
		WINDOW *my_menu_win;
		int n_choices, i;
		ITEM *cur_item;
		
		/* Initialize curses */
		initscr();
		start_color();
		cbreak();
		noecho();
		keypad(stdscr, TRUE);
		init_pair(1, COLOR_RED, COLOR_WHITE);

		/* Create items */
		n_choices = ARRAY_SIZE(choices);
		my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
		for(i = 0; i < n_choices; ++i)
			my_items[i] = new_item(choices[i], "");

		/* Crate menu */
		my_menu = new_menu((ITEM **)my_items);

		/* Create the window to be associated with the menu */
		my_menu_win = newwin(40, 40, 10, 15);
		keypad(my_menu_win, TRUE);
	     
		/* Set main window and sub window */
		set_menu_win(my_menu, my_menu_win);
		set_menu_sub(my_menu, derwin(my_menu_win, 6, 38, 3, 1));

		/* Set menu mark to the string " * " */
		set_menu_mark(my_menu, " * ");

		/* Print a border around the main window and print a title */
		box(my_menu_win, 0, 0);
		print_in_middle(my_menu_win, 1, 0, 40, "My Menu", COLOR_PAIR(1));
		mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
		mvwhline(my_menu_win, 2, 1, ACS_HLINE, 38);
		mvwaddch(my_menu_win, 2, 39, ACS_RTEE);
		mvprintw(LINES - 2, 0, "F1 to exit");
		refresh();
		
		/* Post the menu */
		post_menu(my_menu);
		wrefresh(my_menu_win);

		while((c = wgetch(my_menu_win)) != KEY_F(1))
		{       switch(c)
			{	case KEY_DOWN:
					menu_driver(my_menu, REQ_DOWN_ITEM);
					break;
				case KEY_UP:
					menu_driver(my_menu, REQ_UP_ITEM);
					break;
				case 10:	/* Enter */
					cur_item = current_item(my_menu);
					move(LINES - 2, 0);
					clrtoeol();
					mvprintw(LINES - 2, 0, "You have chosen %d item with name %s and description %s", 
					item_index(cur_item) + 1,  item_name(cur_item), 
					item_description(cur_item));
					
					refresh();
					choice = item_index(cur_item);- 
					pos_menu_cursor(my_menu);
					break;
			

			}
			wrefresh(my_menu_win);
		}	

		/* Unpost and free all the memory taken up */
		unpost_menu(my_menu);
		free_menu(my_menu);
		for(i = 0; i < n_choices; ++i)
			free_item(my_items[i]);
		wclear(my_menu_win);
		//endwin();
	}

	void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
	{	int length, x, y;
		float temp;

		if(win == NULL)
			win = stdscr;
		getyx(win, y, x);
		if(startx != 0)
			x = startx;
		if(starty != 0)
			y = starty;
		if(width == 0)
			width = 80;

		length = strlen(string);
		temp = (width - length)/ 2;
		x = startx + (int)temp;
		wattron(win, color);
		mvwprintw(win, y, x, "%s", string);
		wattroff(win, color);
		refresh();
	}


	/************************init curses************************************/

	void curses_init(){
		initscr();
		noecho();
		keypad(stdscr, TRUE);
		start_color();			/* Start color 			*/
		init_pair(1, COLOR_BLUE, COLOR_WHITE);
		init_pair(2, COLOR_RED, COLOR_WHITE);
		init_pair(3, COLOR_WHITE, COLOR_BLACK);
		init_color(COLOR_WHITE, 1000, 1000, 1000);
		init_color(COLOR_RED, 1000, 0, 0);
		attron(COLOR_PAIR(1));
		wbkgd(stdscr, COLOR_PAIR(1));

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
		newnode->type = CONTINUE;
			/* if the previous node is NULL that means the new node formed is the first node..therefore lineno=1 or increase the lineno by 1*/
		newnode->linesize = 0;
		newnode->posx = 0;
		filedata->totlines++;
	    return newnode;
	}
	/************************************** Make a copy of a filestruct node. *********i.e copy all the elements of the source node into another created node***************/
	filestruct *copy_node(const filestruct *src){
		filestruct *dst;

		if(src != NULL){		
			/*check if the node you want to copy is not NULL*/

			dst = (filestruct *)malloc(sizeof(filestruct));	

			dst->data = (char *)malloc(sizeof(src->data));
			strcpy(dst->data, src->data);		
				/*copy the data of the source node into the newnode created*/
			dst->next = src->next;					
				/*as it is a copy point the prev and next nodes of the created node where the src node is poinitng*/
			dst->prev = src->prev;
			dst->lineno = src->lineno;				
				/*also copy the lineno*/
		
			return dst;
		}
		return NULL; //////////////////////doubtfull about this condition
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void do_output(char *input, filestruct *node){
		wmove(textwin, node->lineno - 1, 0);
		wclrtoeol(textwin);
		wprintw(textwin, "%s", input);
		wmove(textwin, node->lineno - 1, node->posx);
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void moveleft(filestruct *node){
		int x, y;
		getyx(textwin, y, x);

		if(x -1 >= 0 ){
			x = x - 1;
			wmove(textwin, y, x);
			//node->posx--;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void moveright(filestruct *node){
		int x, y;
		getyx(textwin, y, x);
		if(x + 1 < COLS){ //&& x + 1 <= node->linesize){
			x++;
			wmove(textwin, y, x);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void movedown(filestruct *node){
		int x, y;

		getyx(textwin, y, x);
		if(y + 1 < LINES ){//&& node->next != NULL){
			y++;
			wmove(textwin, y, x);
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void moveup(filestruct *node){
			int x, y;
		getyx(textwin, y, x);
		if(y - 1 >= 0 && node->prev != NULL){
			y--;
			wmove(textwin, y, x);
		}
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void go_to_line(filestruct **node, int pos){
		mvwprintw(botwin, 1, 12, "%d", pos);
		*node = filedata->filetop;
		int i = 1;
		while(i < pos && *node){
			*node = (*node)->next;
			i++;
		}
		wmove(textwin, i, 0);	
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void display_line_col(filestruct *node){
		attron(COLOR_PAIR(2));
		wmove(botwin, 2, COLS - 20);
		 wclrtoeol(botwin);
		if(node)
			mvwprintw(botwin, 2, COLS - 20, "Ln %d, Col %d", node->lineno, node->posx + 1);
		attroff(COLOR_PAIR(2));
		wrefresh(botwin);
		if(node)
			wmove(textwin, node->lineno - 1, node->posx);
		else{
	//		wmove(textwin, 0, 0);
	//		filestruct *newnode;
	//		newnode = make_new_node(NULL);
			
		}

	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void print_whole_file(filestruct *node){
		while(node != NULL){
			wprintw(textwin, "%s", node->data);
			node = node->next;
		}
	//	free(node);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void printdata(filestruct *node){
	//	int x, y;
		int i;
		
	//	getyx(stdscr, y, x);
		for(i = 0; i < strlen(node->data); i++){
			printw("%c", node->data[i]);
			refresh();
		}
		printw("\n");

	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void print_nodes(filestruct *afterthis){
		filestruct *temp;
		temp = (filestruct *)malloc(sizeof(filestruct));
		temp = afterthis;
		while(temp != NULL){
			mvwprintw(textwin, temp->lineno -1, 0, "%s", temp->data);
			wclrtoeol(textwin);
			int x, y;
			getyx(stdscr, y, x);
		//	printw("x = %d, y = %d lineno = %d", x, y, temp->lineno);
			//wprintw(textwin, "%d", temp->lineno);
			temp = temp->next;
		}

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void rearrange_nodes(filestruct *node, int posx){
		filestruct *insertnode;
	//	insertnode = make_new_node(node);
		insertnode = (filestruct *)malloc(sizeof(filestruct));
		filestruct *temp;
		temp = (filestruct *)malloc(sizeof(filestruct));
	//        temp = node->next;
		insertnode->prev = node;
		insertnode->next = node->next;
		insertnode->linesize = node->linesize - node->posx - 1;
	//	node->linesize = node->linesize - insertnode->linesize + 1;
		insertnode->lineno = node->lineno + 1;
		insertnode->data = (char *)malloc(sizeof(char) * insertnode->linesize);
		int i = posx;
		int j = 0;
		while(node->data[i]){
			insertnode->data[j] = node->data[i];
			i++;
			j++;
		}
		node->linesize = node->posx + 1;
		node->data[node->posx] = '\n';
		node->data[node->posx + 1] = '\0';
		if(node->next)
			node->next->prev = insertnode;
		node->next = insertnode;
		temp = insertnode->next;
		while(temp != NULL){
			temp->lineno++;
			temp = temp->next;
		}
		free(temp);
		wclrtoeol(textwin);
		print_nodes(insertnode);
		wmove(textwin, insertnode->lineno - 1, 0);
		node = insertnode;
	//wprintw(textwin, " %d", node->lineno);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void search_word(filestruct **node){
		//		int x, y;
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
	//getyx(stdscr, y, x);
				echo(); 
				wscanw(botwin, "%s", str);
				wmove(botwin, 1, 10);
				wclrtoeol(botwin);
				noecho();
				//refresh();
		while(tmp){
			p = strstr(tmp->data, str);
			n = p - &tmp->data[0];
			wmove(textwin, tmp->lineno, n);
			if(!p){
				tmp = tmp->next;
			}
			else{
	//			do_output(tmp->data, tmp);
				wattron(textwin, A_STANDOUT);
				wattron(textwin, COLOR_PAIR(1));
				mvwprintw(textwin, tmp->lineno - 1, n, "%s", str);

				wattroff(textwin, A_STANDOUT);
				wattroff(textwin, COLOR_PAIR(1));
	//			wgetch(textwin);
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
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void display_line_with_no(filestruct *node){
		while(node != NULL){
			printw("%d  %s",node->lineno, node->data);
			node = node->next;
		}
		free(node);

	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void escape_mode(){
		werase(textwin);
		wbkgd(textwin, COLOR_PAIR(3));
		attron(COLOR_PAIR(3));
		print_whole_file(filedata->filetop);
		attroff(COLOR_PAIR(3));
		int c;
		filestruct *node;
		node = (filestruct *)malloc(sizeof(filestruct));
		node = filedata->filetop;
		wmove(textwin, 0,0);
		while((c = wgetch(textwin)) != -1){
			if (c == KEY_LEFT){
				if(node->posx == 0 && node->lineno != 1){	//if it reaches the left most end of the line then move the cursor to the rightmost end of the previous line
					node = node->prev;
					node->posx = node->linesize - 1;
					wmove(textwin, node->lineno - 1, node->linesize - 1);
				}
		
				else if(node->posx == 0 && node->lineno == 1)
					;
				else{
					moveleft(node);
					node->posx--;
				}
				

			}
			else if(c == KEY_RIGHT ){
				if(node->posx == node->linesize - 1 && node->next != NULL){
					node = node->next;
					node->posx = 0; 
					wmove(textwin, node->lineno - 1, 0);

				}
				else if (node->posx < node->linesize && node->next == NULL){
					moveright(node);
					node->posx++;
				}
				else if(node->posx < node->linesize - 1){
					moveright(node);
					node->posx++;
				}
				
			}
			else if(c == KEY_UP){  					//should have added node->prev != NULL
				if(node->lineno == 1)
				       ;

				else if(node->prev->linesize - 1 < node->posx ){
					node = node->prev;
					wmove(textwin, node->lineno - 1, node->linesize - 1);
					node->posx = node->linesize - 1;
				}
				else if(node->lineno == 1)
					;
				else{
				moveup(node);
				node = node->prev;
				node->posx = node->next->posx;	
				}

				moveup(node);
			}
			else if(c == KEY_DOWN ){		//should have added node->next != NULL
				if(node->next == NULL)
					;

				else if(node->next->linesize - 1 < node->posx){
					node = node->next;
					wmove(textwin, node->lineno - 1, node->linesize - 1);
					node->posx = node->linesize - 1;
				}
				else{
					movedown(node);
					node = node->next;
					node->posx = node->prev->posx;			
				}
			}
			
			else if(c == 'n'){
				display_line_with_no(filedata->filetop);
			}	

		}
		

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void getdata(filestruct *node, char *temp){
		//wbkgd(stdscr, COLOR_PAIR(1));
		int  x, y;
		int c;
		char insert;
		/*set node->data memory*/
		node->data = (char *)malloc(sizeof(char)*MAX);
		
		while((c = wgetch(textwin)) != -1){
			if(!strcmp(keyname(c), "^P")){
		
				while(node->data[node->posx] != ' '){
				getyx(textwin, y, x);
				/* Here you are removing the characters only from the screen*/
				if(node->posx != 0){
					memmove(&(node->data[x - 1]), &(node->data[x]), strlen(node->data) - x + 1);

				/* Here you removed the character from the original node*/
					node->linesize--;
					node->posx--;
				}
				else if(node->posx == 0)
					;
				do_output(node->data, node);

				}

			}
			/*else if(!strcmp(keyname(c), "^[")){
				escape_mode();
			}*/
		       else if(!strcmp(keyname(c), "^W")){

				search_word(&node);
			}
			else if(!strcmp(keyname(c), "^F")){
				node =filedata->filetop;
				wmove(textwin, 0, 0);
			}
			else if(!strcmp(keyname(c), "^_")){
				int pos = 0;
				mvwprintw(botwin, 1, 1, "GO TO LINE : ");				
				wscanw(botwin, "%d", &pos);
				go_to_line(&node, pos);
			}
			else if(!strcmp(keyname(c), "^G")){
				get_help();				
			}
			else if(!strcmp(keyname(c), "^F")){
				go_to_line(&node, 1);
			}
			else if(!strcmp(keyname(c), "^L")){
				go_to_line(&node, filedata->totlines);
			}
			/* When you press ENTER KEY i.e. the newline character occurs then return the node with the data that is taken the next node of this node points to NULL */
			else if(node)
			 if((node->posx == MAX - 1 || c == '\n')){
				if(c == '\n'){
					getyx(textwin, y, x);
					if(x == node->linesize){
						node->data[node->posx] = c;
						node->linesize++;
						node->posx++;
						do_output(node->data, node);
						wmove(textwin, y + 1, 0);
						node->data[node->posx + 1] = '\0';
						node->next == NULL;
						node->type = CONTINUE;
						filedata->totlines++;
	//					wprintw(textwin, "linesize = %d", node->linesize);	
						break;
					}
					else if(node->posx == MAX - 1){
						node->data[node->posx] = c;
						node->linesize++;
						node->posx++;
						do_output(node->data, node);
						node->data[node->posx + 1] = '\0';
						node->next == NULL;
						node->type = CONTINUE;
						filedata->totlines++;	
						break;

					}
					else{
						node->linesize++;
	//					node->posx++;					
						rearrange_nodes(node, node->posx);
	//					node->prev->linesize++;
	//wprintw(textwin, " %d", node->lineno);
						wmove(textwin, node->lineno, 0);
						
					}
				}
	/*                        node->data[node->posx + 1] = '\0';
				node->next == NULL;
				node->type = CONTINUE;
				filedata->totlines++;	
				break;*/
			}
			
			else if (c == KEY_LEFT){
				if(node->posx == 0 && node->lineno != 1){	//if it reaches the left most end of the line then move the cursor to the rightmost end of the previous line
					node = node->prev;
					node->posx = node->linesize - 1;
					wmove(textwin, node->lineno - 1, node->linesize - 1);
				}
		
				else if(node->posx == 0 && node->lineno == 1)
					;
				else{
					moveleft(node);
					node->posx--;
				}
				

			}
			else if(c == KEY_RIGHT ){
				if(node->posx == node->linesize - 1 && node->next != NULL){
					node = node->next;
					node->posx = 0; 
					wmove(textwin, node->lineno - 1, 0);

				}
				else if (node->posx < node->linesize && node->next == NULL){
					moveright(node);
					node->posx++;
				}
				else if(node->posx < node->linesize - 1){
					moveright(node);
					node->posx++;
				}
				
			}
			else if(c == KEY_UP){				//should have added node->prev != NULL
				if(node->lineno == 1)
				       ;

				else if(node->prev->linesize - 1 < node->posx ){
					node = node->prev;
					wmove(textwin, node->lineno - 1, node->linesize - 1);
					node->posx = node->linesize - 1;
				}
				else if(node->lineno == 1)
					;
				else{
				moveup(node);
				node = node->prev;
				node->posx = node->next->posx;	
				}

				moveup(node);
			}
			else if(c == KEY_DOWN ){		//should have added node->next != NULL
				if(node->next == NULL)
					;

				else if(node->next->linesize - 1 < node->posx){
					node = node->next;
					wmove(textwin, node->lineno - 1, node->linesize - 1);
					node->posx = node->linesize - 1;
				}
				else{
					movedown(node);
					node = node->next;
					node->posx = node->prev->posx;			
				}
			}
			else if(c == KEY_BACKSPACE /*&&(node->posx != -1 || node->lineno != 1)*/){
				getyx(textwin, y, x);
				/* Here you are removing the characters only from the screen*/
				if(node->posx != 0){
					memmove(&(node->data[x - 1]), &(node->data[x]), strlen(node->data) - x + 1);

				/* Here you removed the character from the original node*/
					node->linesize--;
					node->posx--;
					do_output(node->data, node);
				}
				else if(node->posx == 0){
				//	mvwprintw(textwin, 5, 5, "\nhey");
					wmove(textwin, node->lineno - 2, node->linesize - 1);
					node = node->prev;
					if(node)
						unlink_node(node->next);
					filedata->totlines--;
					werase(textwin);
					print_nodes(filedata->filetop);	
				}
				
				//do_output(node->data, node);
				
			
	//			move(node->lineno, node->posx++);

			}
			else if (c == KEY_END){
				
				node->next = NULL;
				node->type = END;
				//system("clear");
				//delwin(stdscr);
				free(temp);
				werase(textwin);
				wrefresh(textwin);
				//getch();
				break;
			}
			/*Else keep on adding the data to the node until you reach the line limit ends.*/
			else if(node->posx != MAX ){
				if(node->posx == node->linesize){

					node->data[node->posx] = c;
				//	i++;
					node->linesize++;
					node->posx++;
					do_output(node->data, node);
				}

				else{
	/*				insert = c;
					strcpy(node->data, temp);*/					/*PROBLEM TO FIX: If the nodelinesize exceeds the maxlength of line*/
					insert = c;
					temp[0] = '\0';
					strncpy(temp,node->data,node->posx);
					temp[node->posx] = '\0';
					sprintf(temp + node->posx, "%c%s", insert, &node->data[node->posx]);
					node->data[0] = '\0';
					strcpy(node->data, temp);
					node->posx++;
					node->linesize++;
					do_output(&node->data[0], node);		
				}
			}
		/*	else if(!strcmp(keyname(c), "^G")){
				printw("hey");
				
			}*/
			display_line_col(node);
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void link_node(filestruct *node1, filestruct *node2){
		node1->next = node2;
		/* The very next node of node1 is node2 */
		node2->prev = node1;
		/*The node before node2 is node1*/
		node2->next = NULL;

	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void unlink_node(filestruct *fileptr){
		assert(fileptr != NULL);
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
	//	delete_node(fileptr);
		free(fileptr->data);
		free(fileptr);
		
		
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void save_into_file(filestruct *head, char *filename){

	//	mvwprintw(botwin, 1, 1, "Saved your contents in %s file", filename);
		
		FILE *fp;
		//	mvprintw(55, 45,"%d ", newnode->lineno);
		fp = fopen(filename, "w");
		if(!fp){
			mvwprintw(textwin, 27, 27,  "Error opening the file\nEnter a correct filename\n");
			
			filename[0] = '\0';
			wscanw(textwin, "%s", filename);
			save_into_file(head, filename);
		}
		while(head != NULL){
			fwrite(head->data, head->linesize, 1, fp);
			head = head->next;
		}
		wmove(botwin, 1, 1);
		wclrtoeol(botwin);
		mvwprintw(botwin, 1, 1, "Saved your contents in %s file", filename);
		wgetch(textwin);
		fclose(fp);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void open_file_to_read(char *filename){
		FILE *fp;
		char c;
		fp = fopen(filename, "r");
		if(!fp){
			wprintw(textwin, "Error opening the file\n");
			exit(1);
		}
		werase(textwin);

		while(!feof(fp)){
			fread(&c, 1, 1, fp);
			wprintw(textwin, "%c", c);
		}
	}
	//openfile *filedata;
	//filedata = (openfile *)malloc(sizeof(openfile));

	void recursive_part(filestruct *node, char *filename){
		char *temp;
		temp = (char *)malloc(sizeof(char)* 1024);

		while(node != NULL){
			getdata(node, temp);
			if(node->type == END){
				after_end(filedata->filetop, filename);
				break;
				}
			node->next = make_new_node(node);
			node->next->prev = node;
			node = node->next;
			display_line_col(node);		
		}
		filedata->filebot = node->prev;

	wprintw(textwin, "Out of afterend");
		if(wgetch(textwin) ==  KEY_F(1)){
			mvwprintw(botwin, 1, 1, "Are you sure you want to exit: Y/N");
			echo();
			int c;
			c = wgetch(botwin);
			if(c == 'Y' || c == 'y'){		
				free(temp);
				delwin(textwin);
				delwin(botwin);
				unpost_menu(my_menu);
				free_menu(my_menu);
	       //	for(int i = 0; i < n_choices; ++i)
		 //       free_item(my_items[i]);

				delwin(my_menu_win);
				endwin();
			}
			else{
				noecho();
				werase(textwin);
				filestruct *node2;
				node2 = filedata->filetop;
				print_whole_file(node2);
		//	wmove(textwin, filedata->filebot->lineno - 1, filedata->filebot->linesize - 1);
				recursive_part(filedata->filebot, filename);
			}
		}
		else{
			noecho();
			werase(textwin);
			filestruct *node2;
			node2 = filedata->filetop;
			print_whole_file(node2);
		//	wmove(textwin, filedata->filebot->lineno - 1, filedata->filebot->linesize - 1);
			recursive_part(filedata->filebot, filename);
		}

	}

	int main(int argc, char *argv[]){
		//openfile *filedata;
		if(argc != 2){
			errno = EINVAL;
			perror("usage: ./try <filename>\n");
			return errno;
		}
		filedata = (openfile *)malloc(sizeof(openfile));
		//int i = 0;
		char *filename;
		curses_init();
		int hbot = 7;
		//create_menu_win();
		textwin = newwin(LINES - hbot - 2, COLS, 2, 0);
		botwin = newwin(hbot , COLS, LINES - hbot , 0); // window for score
	//	box(botwin, 0, 0);
		keypad(textwin, TRUE);
		create_menu_win();
		//box(botwin, 0, 0);
		start_color();			/* Start color 			*/
			
		wbkgd(textwin, COLOR_PAIR(1));
	
		wattron(botwin, A_BOLD);	
		mvwprintw(botwin, 3, 5, "^G Get Help				^W Search Word			^F First line			^F First line");
		mvwprintw(botwin, 4, 5, "^C Exit without saving			^_ Go to Line			^L Last Line			KEY_END Menu");
		wattroff(botwin, A_BOLD);
		wmove(textwin, 0, 0);

		//wgetch(field);
		wrefresh(textwin);
		wrefresh(botwin);
		//wgetch(textwin);
		wrefresh(botwin);

		filestruct *newnode;

		newnode = make_new_node(NULL);
		display_line_col(newnode);
		char *temp;
		temp = (char *)malloc(sizeof(char)* 1024);

		getdata(newnode, temp);
		filedata->filetop = make_new_node(NULL);
		filedata->filetop = newnode;
		filedata->filename = (char *)malloc(sizeof(char)*100);
	////////////////////////////////////////////////////////////////	strcpy(filedata->filename, argv[1]);
		filestruct *node;
		node = make_new_node(newnode);
		display_line_col(node);
		link_node(newnode, node);
		while(node != NULL){
			getdata(node, temp);
			if(node->type == END){
				after_end(newnode, argv[1]);
				break;
				}
			node->next = make_new_node(node);
			node->next->prev = node;
			node = node->next;
			display_line_col(node);		
		}
	//filedata->filetop = make_new_node(NULL);
	//filedata->filetop = newnode;
		filedata->filebot = make_new_node(node->prev->prev);
		filedata->filebot = node->prev;

	wprintw(textwin, "Out of afterend");
		if(wgetch(textwin) ==  KEY_F(1)){
			mvwprintw(botwin, 1, 1, "Are you sure you want to exit: Y/N");
			echo();
			int c;
			c = wgetch(botwin);
			if(c == 'Y' || c == 'y'){		
				free(temp);
				delwin(textwin);
				delwin(botwin);
				unpost_menu(my_menu);
				free_menu(my_menu);
	       //	for(int i = 0; i < n_choices; ++i)
		 //       free_item(my_items[i]);

				delwin(my_menu_win);
				endwin();
			}
			else{
				noecho();
				werase(textwin);
				filestruct *node2;
				node2 = filedata->filetop;
				print_whole_file(node2);
		//	wmove(textwin, filedata->filebot->lineno - 1, filedata->filebot->linesize - 1);
				recursive_part(filedata->filebot, argv[1]);
			}
		}
		else{
			noecho();
			werase(textwin);
			filestruct *node2;
			node2 = filedata->filetop;
			print_whole_file(node2);
		//	wmove(textwin, filedata->filebot->lineno - 1, filedata->filebot->linesize - 1);
			recursive_part(filedata->filebot, argv[1]);
		}
		return 0;
}
