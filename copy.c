/**************************************************************************
 *   copy.c  --  This file is a part of miniproject.                   *
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

#include "copy.h"
void qinit(queue *q){
	q->i = 0;
}
void enqueue(queue *q, char d){
	q->arr[q->i] = d;
	(q->i)++;
}
char dequeue(queue *q){
	int j = 0;
	char tmp;
	tmp = q->arr[0];
	while(j < (q->i - 1)) {
		q->arr[j] = q->arr[j + 1];
		j++;
	}
	(q->i)--;
	return tmp;
			
}
int qempty(queue *q){
	return q->i == 0;
}
int qfull(queue *q){
	return q->i == MAX;
}
int qlen(queue *q){
	return q->i;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cpush(cstack *s, char num) {
	s->a[s->i]= num;
	(s->i)++;
}
/* the caller should check that the cstack is not empty before calling pop() 
 */
char cpop(cstack *s) {
	char t = s->a[s->i - 1];
	(s->i)--;
	return t;
}
int cempty(cstack *s) {
	return s->i == 0;
}
int cfull(cstack *s) {
	return s->i == MAX;
}
/* It is mandatory to call init() on a cstack before calling push, pop or any other function on the cstack 
 */
void cinit(cstack *s) {
	s->i = 0;
}

