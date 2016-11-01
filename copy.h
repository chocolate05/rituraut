/**************************************************************************
 *   copy.h  --  This file is a part of miniproject.                   	  *
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

#ifndef __COPY_H 
#define __COPY_H 
#define MAX 1024
typedef struct queue {
	char arr[MAX];
	int i;
}queue;
void qinit(queue *q);
void enqueue(queue *q, char d);
char dequeue(queue *q);
int qempty(queue *q);
int qfull(queue *q);
int qlen(queue *q);
void qprint(queue *q);
typedef struct cstack{
	char a[MAX];
	int i;
}cstack;
void cpush(cstack *s, char num);
char cpop(cstack *s);
int cempty(cstack *s);
int cfull(cstack *s);
void cinit(cstack *s);
void cprint(cstack *s);
#endif
