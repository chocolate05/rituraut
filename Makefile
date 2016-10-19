try: project3.o
	cc project3.o -o try -lncurses -lmenu
project.o: project3.c
	cc -Wall -c project3.c 
