**** Ritu Raut ****
**** 111503059 ****
**** TEXT EDITOR **** 

	PROJECT : TEXT EDITOR
	

	This text editor is a command-shell based text editor that enable user to edit, save, and view any type of file in ASCII format. The original purpose of this program is to demonstrate pointers, memory allocations/de-allocations, file input or output, and string search techniques in C. Ncurses is used to implement this text editor.

->Features of this text editor: 
	1>Edit the current line 
	2>Move one line up 
	3>Move one line down 
	4>Delete the current line 
	5>Display the contents of the buffer 
	6>Add a line after the line at which you are navigating 
	7>Save changes and exit to main menu 
	8>Exit discarding any changes 
	9>Show the list of commands
	10>Search a word
	11>Replace text
	12>Go to a specified line
	13>Copy text
	14>Paste text
	15>Window resizing handled
	16>Can change the view of the text editor
	17>Cursor traversing handled properly
	and many more

->The Structure of the Program:
	 A huge buffer is used to read from and store all the text. The buffer is written in a doubly linked- list structure, the text file is break down and represented in list of nodes. Adding, replacing, deleting a line or a block involves simple iteration processes that involve O(1) complexity.Otherwise, rearranging the lines, Saving, reading, involves O(n) complexity.


->Limitations of the program would be: 
	The text program only reads text files. Any other file would not be able to work properly with the editor.


->How to use the program? 
	After compilation, an executable would be automatically generated. Run the program with desired text file name in shell command line.

Check "./project -h" for more information.
The program takes a single file for opens it for editing the contents in the file.
Example: 
    ./project -h <filename>
		or
    ./project <filename>
    >> File with a filename <filename> will be created if it does not exist already.
	else it will read the file and open the file for editing.

Example:
    ./project 
    >> When no argument given, the editor asks whether to open any file or not.


