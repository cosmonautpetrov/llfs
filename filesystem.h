#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

//linked list file system 
#define BLOCK_SIZE 128

#include "stdio.h"
#include "floppy.h"
#include "string.h"

#define FREE_BLOCK_TABLE_SECTOR_LENGTH 3 //block length, in blocks, of free block table
#define MAX_FILES_PER_BLOCK 10
#define MAX_DIRECTORIES_PER_BLOCK 10

#define DEBUG() printf("DEBUG: %i\n", __LINE__)

struct directory_block;
struct file_block;
struct root_directory_block;

//up to 128 bytes in size
struct directory_block{ //main block for each directory

	// to be written into file
	char* name_of_directory;
	unsigned int total_subdirectory_num;
	unsigned int total_file_num;
	//10 each of directory and file pointers
	unsigned int* file_pointers_on_disk;
	unsigned int* dir_pointers_on_disk;

	unsigned int upper_directory_block_addr;//address of above directory
	unsigned int previous_dir_block_addr; // if zero, is head
	unsigned int extended_dir_block_addr;

	unsigned int is_extended;

	//used for parsing
	struct directory_block* upper; 
	struct directory_block* prev;
	struct file_block** file_pointers_in_mem;
	struct directory_block** dir_pointers_in_mem;
	struct directory_block* next;
};

struct file_block{
	//written into file
	char* name; //up to 64 bytes long
	unsigned int size;
	unsigned int file_offset; //offset on disk in bytes
	unsigned int local_block_size; //size on disk offset
	unsigned int is_extended; //is extended or not
	unsigned int previous_file_block_pointer; //if zero, is head
	unsigned int ext_file_block_pointer; //pointer to extended file block

	//used for parsing
	char* data;
	struct file_block* next;
	struct file_block* prev;
};

struct root_directory_block{
	//written to disk
	char* free_blocks; //3 sectors used for finding free space
	char* disk_name; //12 byte name of disk
	unsigned int root_directory_offset; //offset of root directory (top block)

	//used for parsing
	struct directory_block* root;
};

struct llfs_filesystem{
	struct root_directory_block* boot_sector;
};

struct llfs_filesystem* make_filesystem(char* name); //generates an empty file-system with name name and blank root directory
int free_filesystem(struct llfs_filesystem* fs); //frees a filesystem from memory
int dump_filesystem(struct llfs_filesystem* fs); //dumps a filesystem ls style
int add_file_to_filesystem(struct llfs_filesystem* fs, char* path, char* name); //adds file name to filesystem fs in path
int mkdr_filesystem(struct llfs_filesystem, char* path, char* name); //makes a directory in path with name name 

unsigned int first_free_block(struct llfs_filesystem* fs); //returns num of first free  block bit

int is_block_free(struct llfs_filesystem* fs, unsigned int num); //checks if block bit is free
int flip_block_bit(struct llfs_filesystem* fs, unsigned int num); //flips block bit

/*
	1. generate filesystem in memory (using blocks as linked lists)
	2. write filesystem to floppy using writef
	3. write floppy to file 
	4. read floppy from file
	5. load filesystem into memory

*/

//helper functions to clean up functions
char** path_to_string_arr(char* src); //converts a pathname string to a string array
struct directory_block* path_to_directory_block(struct llfs_filesystem* fs,char* path); //returns a directory block from path

#endif