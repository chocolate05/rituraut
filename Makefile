all: project

project: project.o copy.o 
	gcc project.o copy.o -lncurses -lmenu -o project
project.o: project.c proto.h copy.h
	gcc project.c -c -lncurses -lmenu
clean: 
	rm *.o
