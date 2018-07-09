/*
 * Copyright (C) 2018 Alexzander Purwoko Widiantoro
 */
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "colors.h"
#define update() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", x, y)
#define PR_(buffer, s, showPrint) \
	if(showPrint){ \
		sprintf(buffer, ANSI_COLOR_BLUE "[" ANSI_COLOR_RESET "" ANSI_COLOR_YELLOW "Deleting" ANSI_COLOR_RESET " " ANSI_COLOR_RED "%s" ANSI_COLOR_RESET "" ANSI_COLOR_BLUE "]" ANSI_COLOR_RESET , s); \
    	update(); \
   		gotoxy(0,0); \
    	puts(buffer); \
	}
#define BUFFER_SIZE 1024
int __ncount_ct(char *dir){
	DIR *d;
	struct dirent *dp;
	if((d = opendir(dir)) == NULL){
		return 0;
	}
	int count = 0;
	while((dp = readdir(d)) != NULL){
		if(!strcmp(dp -> d_name, ".") ||!strcmp(dp -> d_name, ".."))continue;
	    count++;
	}
	closedir(d);
	return count;
}
void __goToParent(char *s){
	int x = strlen(s) - 1;
	for(; x > 0; x--){
		if(s[x] == '/'){
			s[x] = '\0';
			return;
		}
		s[x] = '\0';
	}
}
/*
 * __nclean_dirs() -> for clean a content of directories...
 * @params dir The directory to be cleaned
 * @params shouldCleanRoot Are we Should clean the directories after cleaning its content?(yes = true, otherwise false)
 * @noreturn
 */
void __nclean_dirs(const char *dir, short shouldCleanRoot, char *_rbuf, short showPrint){
	char *_nbuffer = malloc(BUFFER_SIZE);
	char *_buffer = _nbuffer;
	int _d_currdir = 0;
	DIR *d;
	struct dirent *dp;
	if((d = opendir(dir)) == NULL){
		goto out;
	}
	_buffer[0] = '\0';
	while(1){
		if((dp = readdir(d)) == NULL){
			if(!_d_currdir){
				closedir(d);
				if(shouldCleanRoot){
					PR_(_rbuf, dir, showPrint);
					rmdir(dir);
				}
				break;
				
			}
			else {
				PR_(_rbuf, dir, showPrint);
				closedir(d);
				rmdir(_buffer);
				_d_currdir--;
				__goToParent(_buffer);
				d = opendir(_buffer);
				continue;
			}
		}
		if(!strcmp(dp -> d_name, ".") ||!strcmp(dp -> d_name, ".."))continue; 
		if(_buffer[0] == '\0')
		    sprintf(_buffer, "%s/%s", dir, dp -> d_name);
		else
		    sprintf(_buffer, "%s/%s", _buffer, dp -> d_name);
		switch(dp -> d_type){
			case DT_DIR :
				if(!__ncount_ct(_buffer)){
					PR_(_rbuf, dir, showPrint);
					rmdir(_buffer);
				   __goToParent(_buffer);
				}
				else {
					_d_currdir++;
					closedir(d);
					d = opendir(_buffer);		
				}
				break;
			case DT_REG :
				PR_(_rbuf, dir, showPrint);
				unlink(_buffer);
				__goToParent(_buffer);
		}
	}
	out:
	free(_buffer);
}
