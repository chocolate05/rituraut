#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
//#include <stdarg.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>

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
	int totsize;

} openfile;

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

void getdata(filestruct *node, char *temp);

void save_into_file(filestruct *head, char *filename);

void unlink_node(filestruct *fileptr);

void delete_node(filestruct *node);

void moveleft(filestruct *node);

void moveright(filestruct *node);

void moveup(filestruct *node);

void movedown(filestruct *node);

void display_line_col();

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);

void create_menu_win();

void do_output(char *input, filestruct *node);
////////////////////////////////////////////////////////////////////////////////////////////////////////


void create_menu_win()
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
        my_menu_win = newwin(10, 40, 4, 4);
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
//printw("Hey newnode is created properly and now i'm on line %d\n",newnode->lineno);
	newnode->type = CONTINUE;
		/* if the previous node is NULL that means the new node formed is the first node..therefore lineno=1 or increase the lineno by 1*/
	newnode->linesize = 0;
	newnode->posx = 0;
	//newnode->position = 0; 

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
void do_output(char *input, filestruct *node){
	move(node->lineno - 1, 0);
	clrtoeol();
	printw("%s", input);
	move(node->lineno - 1, node->posx);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void moveleft(filestruct *node){
	int x, y;
	getyx(stdscr, y, x);

	if(x -1 >= 0 ){
		x = x - 1;
		move(y, x);
		//node->posx--;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void moveright(filestruct *node){
	int x, y;
	getyx(stdscr, y, x);
	if(x + 1 < COLS){ //&& x + 1 <= node->linesize){
		x++;
		move(y, x);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void movedown(filestruct *node){
	int x, y;

	getyx(stdscr, y, x);
	if(y + 1 < LINES ){//&& node->next != NULL){
		y++;
		move(y, x);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void moveup(filestruct *node){
/*	if(node->lineno == 1)
		;

	else if(node->prev->linesize - 1 < node->posx ){
		node = node->prev;
//printw("\b%d", node->lineno);
		move(node->lineno - 1, node->linesize - 1);
		node->posx = node->linesize - 1;
	}
	else if(node->lineno == 1)
		;
	else{
		moveup(node);
		int x, y;
		getyx(stdscr, y, x);
		if(y - 1 >= 0 && node->prev != NULL){
			y--;
			move(y, x);
		}	
		node = node->prev;
		node->posx = node->next->posx;	
	}

//printw("%d", node->posx);
}
/**/		int x, y;
	getyx(stdscr, y, x);
	if(y - 1 >= 0 && node->prev != NULL){
		y--;
		move(y, x);
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void display_line_col(filestruct *node){
	int x, y;
	getyx(stdscr, y, x);
//	x = node->posx;
	attron(COLOR_PAIR(2));
	move(LINES - 3, COLS - 20);
	 clrtoeol();
	//ctrtoeol();
	attron(A_REVERSE);
	mvprintw(LINES - 3, 0, "  ^G Get Help     ^O Write Out    ^W Where Is     ^K Cut Text     ^J Justify      ^C Cur Pos      ^Y Prev Page    M-1 First Line  M-W WhereIs Next^^ Mark Text    M-} Indent Text\n   ^X Exit         ^R Read File    ^L Replace      ^U Uncut Text   ^T To Spell     ^_ Go To Line   ^V Next Page    M-/ Last Line   M-] To Bracket  M-^ Copy Text   M-{ Unindent Text");
	attroff(A_REVERSE);	
	mvprintw(LINES - 3, COLS - 20, "Ln %d, Col %d", node->lineno, node->posx + 1);
	attroff(COLOR_PAIR(2));
	refresh();
	move(node->lineno - 1, node->posx);

}
void print_whole_file(filestruct *node){
	while(node != NULL){
		printw("%s", node->data);
		node = node->next;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void printdata(filestruct *node){
	int x, y, i;
	
	getyx(stdscr, y, x);
	for(i = 0; i < strlen(node->data); i++){
		printw("%c", node->data[i]);
		refresh();
	}
	printw("\n");
printw("This is %d node\n", node->lineno);

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void print_nodes(filestruct *afterthis){
	filestruct *temp;
	temp = (filestruct *)malloc(sizeof(filestruct));
	temp = afterthis;
	while(temp != NULL){
		printw("%s", temp->data);
		clrtoeol();
//		ctrtoeol();
		temp = temp->next;
	}

}
void rearrange_nodes(filestruct *node, int posx){
	filestruct *insertnode;
	insertnode = make_new_node(node);
	filestruct *temp;
        temp = (filestruct *)malloc(sizeof(filestruct));
        temp = node->next;

//	insertnode->prev = node;
	insertnode->next = node->next;
	node->next->prev = insertnode;
	node->next = insertnode;
	while(temp != NULL){
		temp->lineno++;
		temp = temp->next;
	}
}



void getdata(filestruct *node, char *temp){
	int i = 0, x, y;
	int c;
	char insert;
	/*set node->data memory*/
	node->data = (char *)malloc(sizeof(char)*MAX);
	while((c = getch()) != -1){
                if(!strcmp(keyname(c), "^P")){
	
			while(node->data[node->posx] != ' '){
                        getyx(stdscr, y, x);
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
			// do_output(node->data, node);

                }
               else if(!strcmp(keyname(c), "^W")){
			char *str = (char *)malloc(sizeof(char)*COLS);
			char *p;
			p = (char *)malloc(sizeof(char)*COLS);
			int n;

		//	scanw("%s", string);
			mvprintw(LINES - 4, 5, "search: ");
getyx(stdscr, y, x);
//printw("%d %d", y, x ); 
			scanw("%s", str);
			printw("%s", str);
			refresh();
			p = strstr(node->data, str);
			n = p - &node->data[0];
			move(node->lineno, n);
//                        printw("hey\n");
			if(!p)
				printw("Not found\n");
                

//	}
			else{
				attron(A_STANDOUT);
				attron(COLOR_PAIR(1));
				mvprintw(node->lineno - 1, n, "%s", str);
				attroff(COLOR_PAIR(1));
				attroff(A_STANDOUT);
			}
		}

		/* When you press ENTER KEY i.e. the newline character occurs then return the node with the data that is taken the next node of this node points to NULL */
			else if(node->posx == MAX - 1 || c == '\n'){
			node->data[node->posx] = c;
                        node->linesize++;
                        node->posx++;
		//	node->linesize++;
		//	node->posx++;
//printw("%d", node->posx);
			do_output(node->data, node);
			if(c == '\n'){
				getyx(stdscr, y, x);
				if(x == node->linesize){
					move(y + 1, 0);
				}
		/*		else{
					rearrange_nodes(node, node->posx);					
				}*/
			}
                //      i++;
                        node->data[node->posx + 1] = '\0';
                        node->next == NULL;
                        node->type = CONTINUE;
			
			break;
		}
		
		else if (c == KEY_LEFT){
			if(node->posx == 0 && node->lineno != 1){	//if it reaches the left most end of the line then move the cursor to the rightmost end of the previous line
				node = node->prev;
				node->posx = node->linesize - 1;
				move(node->lineno - 1, node->linesize - 1);
			}
	
			else if(node->posx == 0 && node->lineno == 1)
				;
			else{
				moveleft(node);
				node->posx--;
//printw("%d", node->posx);
			}
			

		}
		else if(c == KEY_RIGHT ){
			if(node->posx == node->linesize - 1 && node->next != NULL){
				node = node->next;
//printw("%d", node->lineno);
				node->posx = 0; 
				move(node->lineno - 1, 0);
//printw("%d", node->posx);

			}
			else if (node->posx < node->linesize && node->next == NULL){
				moveright(node);
				node->posx++;
			}
			else if(node->posx < node->linesize - 1){
				moveright(node);
				node->posx++;
			}
//printw("%d", node->posx);
			
		}
		else if(c == KEY_UP){  					//should have added node->prev != NULL
                        if(node->lineno == 1)
                               ;

			else if(node->prev->linesize - 1 < node->posx ){
				node = node->prev;
//printw("\b%d", node->lineno);
				move(node->lineno - 1, node->linesize - 1);
				node->posx = node->linesize - 1;
			}
			else if(node->lineno == 1)
				;
			else{
			moveup(node);
			node = node->prev;
			node->posx = node->next->posx;	
			}

//printw("%d", node->posx);*/
			moveup(node);
		}
		else if(c == KEY_DOWN ){		//should have added node->next != NULL
//printw("%d", node->lineno);
                        if(node->next == NULL)
                                ;

			else if(node->next->linesize - 1 < node->posx){
				node = node->next;
//printw("%d", node->lineno);
				move(node->lineno - 1, node->linesize - 1);
				node->posx = node->linesize - 1;
			}
			else{
				movedown(node);
				node = node->next;
				node->posx = node->prev->posx;			
			}
//printw("%d", node->posx);		
		}
		else if(c == KEY_BACKSPACE /*&&(node->posx != -1 || node->lineno != 1)*/){
			getyx(stdscr, y, x);
		//	printw(" \b");
			/* Here you are removing the characters only from the screen*/
		//	fflush(stdout);
			if(node->posx != 0){
				memmove(&(node->data[x - 1]), &(node->data[x]), strlen(node->data) - x + 1);

			/* Here you removed the character from the original node*/
//printw("%d\n%d", strlen(node->data), node->linesize);
//printw("\n%d\n%d",strlen(node->data), node->linesize);
				node->linesize--;
				node->posx--;
			}
			else if(node->posx == 0)
				;
			do_output(node->data, node);
//			move(node->lineno, node->posx++);
//printw("%d", node->posx);

		}
		else if (c == KEY_END){
			
			node->next = NULL;
			node->type = END;
			//system("clear");
			//delwin(stdscr);
			free(temp);
			erase();
			refresh();
			getch();
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
//printw("%d", node->posx);
			//display_line_col(node);
			}

			else{
/*				insert = c;
				char *temp;
				temp = (char *)malloc(node->linesize + 1);
				strncpy(temp, node->data, node->posx);
			//	temp = &node->data[node->posx];
				sprintf(temp + 1, "%c%s", insert, &node->data[node->posx]);
				node->posx++;
				node->linesize++;
				strcpy(node->data, temp);*/
				insert = c;
//				char *temp;
//				temp = (char *)malloc(node->linesize + 1);
				temp[0] = '\0';
                		strncpy(temp,node->data,node->posx);
                		temp[node->posx] = '\0';
               			 sprintf(temp + node->posx, "%c%s", insert, &node->data[node->posx]);
			//	node->data = (char *)malloc(node->linesize + 1);
				node->data[0] = '\0';
        		        strcpy(node->data, temp);
                                node->posx++;
                                node->linesize++;
			
//				free(temp);
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
//	delete_node(fileptr);
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void save_into_file(filestruct *head, char *filename){

	FILE *fp;
	fp = fopen(filename, "a+");
	if(!fp){
		printw("Error opening the file\n");
		exit(1);
	}
	while(head != NULL){
		fwrite(head->data, head->linesize, 1, fp);
		//fwrite("\n", 1, 1, fp);
		head = head->next;
	}
	clrtoeol();
mvprintw(LINES - 3, 0, "Your file is saved successfully in file named %s", filename);
refresh();
	fclose(fp);

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void open_file_to_read(char *filename){
	FILE *fp;
	char c;
	fp = fopen(filename, "r");
	if(!fp){
		printw("Error opening the file\n");
		exit(1);
	}
        erase();

	while(!feof(fp)){
		fread(&c, 1, 1, fp);
		printw("%c", c);
	}
}
openfile *filedata;



int main(int argc, char *argv[]){
	int i = 0;
	char *filename;
	curses_init();
	filestruct *newnode;

	newnode = make_new_node(NULL);
char *temp;
temp = (char *)malloc(sizeof(char)* 1024);

	getdata(newnode, temp);
//printw("%d\n", newnode->linesize);

	filestruct *node;
	node = make_new_node(newnode);
	display_line_col(node);
	link_node(newnode, node);
	while(node != NULL){
//			node->linesize++;
//			node->posx++;
//printw("%d", node->posx);

		getdata(node, temp);
//printw("%d\n", node->linesize);
		if(node->type == END){
			create_menu_win();
			break;
			}
		node->next = make_new_node(node);
		node->next->prev = node;
		node = node->next;
//printw("\n%d %d", node->lineno, node->posx + 1);
		display_line_col(node);		
	}
//filedata->filetop = make_new_node(NULL);
//filedata->filetop = newnode;
//filedata->filebot = make_new_node(node->prev->prev);
//filedata->filebot = node->prev;
	if(choice == SAVE){
//		system("clear");
		mvprintw(55, 45,"%d ", newnode->lineno);
		save_into_file(newnode, argv[1]);
	}
	else if(choice == GO_BACK ){
		erase();
//		system("clear");
		filestruct *copyhead = newnode;
		print_whole_file(copyhead);
		display_line_col(copyhead);
		refresh();
	}
	else if(choice == OPEN_FILE){
		printw("Enter the name of the file you want to open \n");
		scanw("%s", filename);
		open_file_to_read(filename);
		
	}
	free(temp);
	refresh();
	getch();
	endwin();

	return 0;
}
