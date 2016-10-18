try: project2.o
	cc project2.o -o try -lncurses -lmenu
project.o: project2.c
	cc -Wall -c project2.c 
