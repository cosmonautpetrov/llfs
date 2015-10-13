#ifndef _FLOPPY_H_
#define _FLOPPY_H_

#include "stdlib.h"
#include "string.h"

//abstract file for a "floppy"
//functions should be replaced

#define SECTOR_SIZE 512
#define SECTOR_NUM 2880

#define FLOPPY_SIZE SECTOR_SIZE*SECTOR_NUM

struct floppy{
	char* name; //name of floppy
	char** disk; //array of 512 floppies
};

struct floppy* make_floppy(char* name, char from_file, char* filename); //makes a floppy, if from_file is true then loads a floppy image from filename

int get_sector(struct floppy* f, int n, char* dest); //writes 512 bytes from sector n of floppy f into dest
int write_sector(struct floppy* f, int n, char* src); //writes 512 bytes from src to sector n of floppy f

char get_byte_from_sector(struct floppy* f, int n, int b);
int write_byte_to_sector(struct floppy* f, int n, int b, char src);

int free_floppy(struct floppy* f); //pass floppy to free

int readf(struct floppy* f, unsigned int from, unsigned int to, char* dest); //wrapper function to read from a floppy
int writef(struct floppy* f, unsigned int from, unsigned int to, char* src); //wrapper function to write to a floppy

#endif 