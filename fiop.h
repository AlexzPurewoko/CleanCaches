/*
 * Copyright (C) 2018 Alexzander Purwoko Widiantoro
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <ctype.h>
typedef struct __fio{
	char data[512];
	int types;
}FIOread;
void __setToFirst(FILE *f){
	fseek(f, 0, SEEK_SET);
}
void __setToEnd(FILE *f){
	fseek(f, 0, SEEK_END);
}
size_t __fiolen(FILE *f){
	if(f == NULL)return 0;
	fseek(f, 0, SEEK_SET);
	size_t len = 0, count = 0;
	int read;
	while((read = getc(f)) != -1){
		if(read == '\n')len++;
		count++;
	}
	return (len || count)?len+1:0;
}
// remove a path from index
FILE *__fioremove(FILE *f, const char *fpath, int start, int end){
	int len = __fiolen(f);
	if(len == 0 || start > end || end == len)return f;
	int _fpos = 0;
	int read;
	__setToFirst(f);
	char buffer[strlen(fpath)+4];
	sprintf(buffer, "%s.tmp", fpath);
	FILE *tmp = fopen(buffer, "w+");
	while((read = getc(f)) != -1){
		if(read == '\n'){
			_fpos++;
			//skipping newline if start is positioned from zero
			if(start == 0 && _fpos >= start && _fpos-1 <= end)continue;
			//skipping newline if start is positioned greater than zero and fpos-1 is less than end
			else if(start > 0 && _fpos >= start && _fpos-1 < end)continue;
			// if somewhat else, no skipping and write newline char
			else {
				putc('\n', tmp);
				continue;
			}
		}
		// skipping content from start into end
		if(_fpos >= start &&  _fpos <= end)continue;
		putc(read, tmp);
	}
	fclose(tmp);
	fclose(f);
	rename(buffer, fpath);
	f = fopen(fpath, "r+");
	__setToFirst(f);
	return f;
}
// write to File
void __fiowrite(FILE *f, FIOread *data){
	if(!data || !f)return;
	size_t len = __fiolen(f);
	__setToEnd(f);
	if(len > 0)
		fprintf(f, "\n%d%s", data -> types, data -> data);
	else
		fprintf(f, "%d%s", data -> types, data -> data);
	__setToFirst(f);
}
// read with index
int __fioreadi(FILE *f, FIOread *fr, int index){
	if(!fr)return -1;
	size_t len = __fiolen(f);
	if(index >= len)return -1;
	char *tmp = fr -> data;
	int read;
	fr -> types = -1;
	int x = 0, y = 0;
	__setToFirst(f);
	while((read = getc(f)) != -1 ){
		if(read == '\n'){
			if(++y > index)break;
			continue;
		}
		if(index == y){
			switch(fr -> types){
				case -1:
					fr -> types = read - '0';
					break;
				default:
					tmp[x++] = ((char)read);
			}
		}
	}
	tmp[x] = '\0';
	return 1;
}
// read iterator(until next newline)
int __fioread(FILE *f, FIOread *fr){
	if(!fr)return -1;
	char *tmp = fr -> data;
	int read;
	fr -> types = -1;
	int x = 0;
	while((read = getc(f)) != -1 && read != '\n'){
		switch(fr -> types){
			case -1:
				fr -> types = read - '0';
				break;
			default:
				tmp[x++] = ((char)read);
		}
	}
	tmp[x] = '\0';
	return (!x)? -1:1;
}
/**
 * This function is used to check the file is exists or not
 * how to use
 * checkExists(filename<string>);
 * This method will return 0 if no error occurs if searching the file
 * and the file is exists in directory
 * return 1 if the file isn't exists and the error is occurs if searching the file
 */
int checkExists(const char *filename){
    struct stat v;
    errno = 0;
    return (stat(filename, &v) != 0 && errno == ENOENT) ? 1 : 0;
}
