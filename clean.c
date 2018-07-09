/*
 * Copyright (C) 2018 Alexzander Purwoko Widiantoro
 */

 /*
  * Main file for CleanCache
  * Build :
  * 	gcc clean.c -o clean
  *
  */
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "colors.h"
#include "cleandir.h"
#include "fiop.h"
#include "arrconv.h"
//version number
#define VERSION "2.5"
// for identifying that the path is folder or regular files
// because the method for cleaning folder and removing files is not same
#define TP_FOLDERS 1
#define TP_FILES 0
// for store a list of path dirs/files for cleaning its content.
#define DATA_PTH "/etc/.cacheflist.cdt"
// wizard for adding absolutepath into list
void add_data();
// for operation cleaning
void clean_caches(short showPrint);
// display a help information
void help();
// display a list of absolute path 
void list();
// wizard for removing absolutepath from list
void setup_remove();
// clean dirs for selected position in an array format
void clean_cachef(const char *list, short showPrint);
// for displaying a list of absolutepath that have been listed.
// This method is used by setup_remove() and list()
void _nmlist(FILE *f, FIOread *rd);

// argc : count of argument
// argv : list of argument
int main(const int argc, const char *argv[]){
	const char *print_info = "Cache clean v" VERSION "\n" ANSI_COLOR_RED "Perintah Salah!" ANSI_COLOR_RESET "\n" ANSI_COLOR_GREEN "Silahkan ketik" ANSI_COLOR_RESET " " ANSI_COLOR_YELLOW  "clean -help" ANSI_COLOR_RESET " " ANSI_COLOR_GREEN "untuk info lanjut!" ANSI_COLOR_RESET "\n";
	if(argc >= 2)
		if(!strcmp(argv[1], "-add"))add_data();
		else if(!strcmp(argv[1], "-a"))
			if(argc > 2 && !strcmp(argv[2], "-v"))clean_caches(1);
			else clean_caches(0);
		else if(!strcmp(argv[1], "-av"))clean_caches(1);
		else if(!strcmp(argv[1], "-d"))
			if(argc > 2 && !strcmp(argv[2], "-v"))clean_cachef(argv[3], 1);
			else clean_cachef(argv[2], 0);
		else if(!strcmp(argv[1], "-dv"))clean_cachef(argv[2], 1);
		else if(!strcmp(argv[1], "-help"))help();
		else if(!strcmp(argv[1], "-list"))list();
		else if(!strcmp(argv[1], "-remove"))setup_remove();
		else printf(print_info);
	else printf(print_info);
	return 0;
}
void clean_cachef(const char *list, short showPrint){
	int (*checkFile) (const char *);
	checkFile = checkExists;
	int check = (*checkFile)(DATA_PTH);
	if(check){
		exit(1);
	}
	size_t len_list = __getlenis(list, ',');
	if(!len_list)exit(1);
	FILE *fg = fopen(DATA_PTH, "r+");
	size_t len = __fiolen(fg);
	if(len <= 0){
		fclose(fg);
		exit(1);
	}
	FIOread rd;
	__setToFirst(fg);
	int *_list = __strtointp(list, ',');
	char *_rbuf = malloc(BUFFER_SIZE);
	for(int x = 0; x < len_list; x++){
		if(_list[x] >= len)continue;
		if(__fioreadi(fg, &rd, _list[x]) == -1)continue;
		char *fpth = rd.data;
		int tp = rd.types;
		switch(tp){
			case TP_FOLDERS:{
					PR_(_rbuf, fpth, showPrint);
					if(showPrint)sleep(1);
					__nclean_dirs(fpth, 0, _rbuf, showPrint);
				}
				break;
			case TP_FILES:
				PR_(_rbuf, fpth, showPrint);
				if(showPrint)sleep(1);
				remove(fpth);
				break;
		}
	}
	free(_list);
	if(showPrint)printf("\n" ANSI_COLOR_MAGENTA "[CleanDirs finished!]\nTouch keyboard to end..." ANSI_COLOR_RESET );
	free(_rbuf);
	fclose(fg);
	if(showPrint)getch();
	printf("\n");
}
void setup_remove(){
	clrscr();
	FIOread rd;
	int (*checkFile) (const char *);
	checkFile = checkExists;
	int check = (*checkFile)(DATA_PTH);
	if(check){
		fprintf(stderr, ANSI_COLOR_RED "No data that has been added! Please add first!" ANSI_COLOR_RESET );
		exit(1);
	}
	FILE *fg = fopen(DATA_PTH, "r+");
	size_t len = __fiolen(fg);
	if(len <= 0){
		fclose(fg);
		exit(1);
	}
	printf( ANSI_COLOR_RED "Remove specified Dirs/files from list" ANSI_COLOR_RESET "\n" );
	_nmlist(fg, &rd);
	printf("\n\n" ANSI_COLOR_YELLOW "Type a list number to remove!" ANSI_COLOR_RESET " (" ANSI_COLOR_RED "-1 cancel" ANSI_COLOR_RESET ") : ");
	int r;
	scanf("%d", &r);
	if(r == -1)goto o;
	__fioremove(fg, DATA_PTH, r, r);
	clrscr();
	getch();
	printf(ANSI_COLOR_MAGENTA "Are you sure to remove again?(y/n):" ANSI_COLOR_RESET );
	int re = getch();
	if(re == 'y')setup_remove();
	o:
	clrscr();
}
void list(){
	int (*checkFile) (const char *);
	checkFile = checkExists;
	int check = (*checkFile)(DATA_PTH);
	if(check){
		fprintf(stderr, ANSI_COLOR_RED "No data that has been added! Please add first!" ANSI_COLOR_RESET );
		exit(1);
	}
	FILE *fg = fopen(DATA_PTH, "r+");
	size_t len = __fiolen(fg);
	if(len <= 0){
		fclose(fg);
		exit(1);
	}
	FIOread rd;
	_nmlist(fg, &rd);
}
void _nmlist(FILE *f, FIOread *rd){
	int x = 0;
	clrscr();
	printf( ANSI_COLOR_BLUE "-------- LIST FOR AVAILABLE CLEAN DIRS/FILES --------" ANSI_COLOR_RESET );
	printf("\n" ANSI_COLOR_RED "|" ANSI_COLOR_RESET " " ANSI_COLOR_CYAN "No" ANSI_COLOR_RESET " " ANSI_COLOR_RED "|" ANSI_COLOR_RESET " " ANSI_COLOR_YELLOW "Types" ANSI_COLOR_RESET " " ANSI_COLOR_RED "|" ANSI_COLOR_RESET "\t " ANSI_COLOR_GREEN "PATH" ANSI_COLOR_RESET " \t" ANSI_COLOR_RED "|" ANSI_COLOR_RESET);
	__setToFirst(f);
	while(__fioread(f, rd) != -1){
		char *fpth = rd -> data;
		int id = rd ->types;
		switch(id){
			case TP_FOLDERS:{
					printf("\n" ANSI_COLOR_RED "|" ANSI_COLOR_RESET " " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET " " ANSI_COLOR_RED "|" ANSI_COLOR_RESET " " ANSI_COLOR_YELLOW "FOLDERS" ANSI_COLOR_RESET " " ANSI_COLOR_RED "|" ANSI_COLOR_RESET " " ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET " " ANSI_COLOR_RED "|" ANSI_COLOR_RESET, x, fpth);

				}
				break;
			case TP_FILES:
					printf("\n" ANSI_COLOR_RED "|" ANSI_COLOR_RESET " " ANSI_COLOR_CYAN "%d" ANSI_COLOR_RESET " " ANSI_COLOR_RED "|" ANSI_COLOR_RESET " " ANSI_COLOR_MAGENTA "FILES" ANSI_COLOR_RESET " " ANSI_COLOR_RED "|" ANSI_COLOR_RESET " " ANSI_COLOR_GREEN "%s" ANSI_COLOR_RESET " " ANSI_COLOR_RED "|" ANSI_COLOR_RESET, x, fpth);
				break;
		}
		x++;
	}
	printf("\n" ANSI_COLOR_BLUE "-----------------------------------------------------" ANSI_COLOR_RESET "\n");
}
void clean_caches(short showPrint){
	int (*checkFile) (const char *);
	checkFile = checkExists;
	int check = (*checkFile)(DATA_PTH);
	if(check){
		exit(1);
	}
	FILE *fg = fopen(DATA_PTH, "r+");
	size_t len = __fiolen(fg);
	if(len <= 0){
		fclose(fg);
		exit(1);
	}
	FIOread rd;
	__setToFirst(fg);
	char *_rbuf = malloc(BUFFER_SIZE);
	for(int x = 0; __fioread(fg, &rd) != -1; x++){
		char *fpth = rd.data;
		int tp = rd.types;
		switch(tp){
			case TP_FOLDERS:{
					PR_(_rbuf, fpth, showPrint);
					if(showPrint)sleep(1);
					__nclean_dirs(fpth, 0, _rbuf, showPrint);
				}
				break;
			case TP_FILES:
				PR_(_rbuf, fpth, showPrint);
				if(showPrint)sleep(1);
				remove(fpth);
				break;
		}
	}
	
	if(showPrint)printf("\n" ANSI_COLOR_MAGENTA "[CleanDirs finished!]\nTouch keyboard to end..." ANSI_COLOR_RESET );
	free(_rbuf);
	fclose(fg);
	if(showPrint)getch();
	printf("\n");
}
void esetup_addn(FIOread *res){
	clrscr();
	printf( ANSI_COLOR_MAGENTA "File(0)" ANSI_COLOR_RESET " " ANSI_COLOR_GREEN "atau" ANSI_COLOR_RESET " " ANSI_COLOR_YELLOW "folder(1)" ANSI_COLOR_RESET " " ANSI_COLOR_GREEN "yg ditambahkan ?:" ANSI_COLOR_RESET );
	scanf("%d", &(res -> types));
	printf( ANSI_COLOR_GREEN "Masukkan absolute path! : " ANSI_COLOR_RESET );
	getch();
	fgets(res -> data, 512, stdin);
	res -> data[strlen(res -> data) - 1] = '\0';
	clrscr();
}
void add_data(){
	int (*checkFile) (const char *);
	checkFile = checkExists;
	int check = (*checkFile)(DATA_PTH);
	if(check)
		creat(DATA_PTH, 0777);
	FIOread res;
	FILE *fg = fopen(DATA_PTH, "r+");
	__setToFirst(fg);
	while(1){
		esetup_addn(&res);
		__fiowrite(fg, &res);
		printf( ANSI_COLOR_GREEN "Apakah Anda ingin tambah lagi?(y/n) : " ANSI_COLOR_RESET );
		char n = getch();
		if(n != 'y')break;
	}
	printf( ANSI_COLOR_GREEN "\nTerimakasih!\n" ANSI_COLOR_RESET );
}
void help(){
	printf( ANSI_COLOR_GREEN "CleanCache -v" VERSION " By Alexzander Purwoko Widiantoro, Licensed under Apache License" ANSI_COLOR_RESET );
	printf( ANSI_COLOR_GREEN "\nBuild Time :" ANSI_COLOR_RESET " " ANSI_COLOR_YELLOW "%s at %s" ANSI_COLOR_RESET , __DATE__, __TIME__);
	printf( ANSI_COLOR_GREEN "\n\nCleanCache is used to delete any files/directories from list" ANSI_COLOR_RESET );
	printf( ANSI_COLOR_GREEN "\nPlease be carefull and read this help before using." ANSI_COLOR_RESET );
	printf( ANSI_COLOR_GREEN "\nUsages : " ANSI_COLOR_RESET );
	printf( ANSI_COLOR_YELLOW "clean [args]" ANSI_COLOR_RESET );
	printf("\n\t" ANSI_COLOR_YELLOW "-help" ANSI_COLOR_RESET " \t : " ANSI_COLOR_CYAN "Shows this help" ANSI_COLOR_RESET );
	printf("\n\t" ANSI_COLOR_YELLOW "-list" ANSI_COLOR_RESET " \t : " ANSI_COLOR_CYAN "to show the list of all configurations" ANSI_COLOR_RESET );
	printf("\n\t" ANSI_COLOR_YELLOW "-add" ANSI_COLOR_RESET " \t : " ANSI_COLOR_CYAN "This wizard to add new directories/files to list for auto delete" ANSI_COLOR_RESET );
	printf("\n\t" ANSI_COLOR_YELLOW "-a" ANSI_COLOR_RESET "\t : " ANSI_COLOR_CYAN "Deletes a specified dirs/files from the list" ANSI_COLOR_RESET );
	printf("\n\t" ANSI_COLOR_YELLOW "-d" ANSI_COLOR_RESET "\t : " ANSI_COLOR_CYAN "Deletes a specified dirs/files from the selected index of list(separated by commas)" ANSI_COLOR_RESET );
	printf("\n\t" ANSI_COLOR_YELLOW "-v" ANSI_COLOR_RESET "\t : " ANSI_COLOR_CYAN "Verbose an operation in " ANSI_COLOR_RESET " " ANSI_COLOR_YELLOW "-a" ANSI_COLOR_RESET " " ANSI_COLOR_CYAN "/" ANSI_COLOR_RESET " " ANSI_COLOR_YELLOW "-d" ANSI_COLOR_RESET " " ANSI_COLOR_CYAN "commands" ANSI_COLOR_RESET );
	printf("\n\t" ANSI_COLOR_YELLOW "-remove" ANSI_COLOR_RESET "\t : " ANSI_COLOR_CYAN "Setup for remove specified dir/files from the list" ANSI_COLOR_RESET );
	printf("\n");
}
