#include "filesystem.h"

unsigned int first_free_block(struct llfs_filesystem* fs){
	return 0;
}

int is_block_free(struct llfs_filesystem* fs, unsigned int num){
	//take num, divide by 8 to get byte offset
	//use offset to find byte in free block table
	//shift byte left by num modulo 8 and use and 0x01 to see value of bit
	//return bit
	unsigned int n=num/8;
	unsigned int shiftn = num%8;

	char is_free = fs->boot_sector->free_blocks[n];

	is_free = (is_free >> shiftn) && 0x01;


	return (int)is_free;
} 

int flip_block_bit(struct llfs_filesystem* fs, unsigned int num){
	//take num, divide by 8 to get byte offset
	//use offset to find byte in free block table
	//shift byte left by num modulo 8 and use and 0x01 to see value of bit
	//flip bit
	//shift back and or
	return 0;
}

struct llfs_filesystem* make_filesystem(char* name){
	struct llfs_filesystem* rllfs= malloc(sizeof(struct llfs_filesystem));
	rllfs->boot_sector = malloc(sizeof(struct root_directory_block)); //make space for file_system and boot_sector

	if(strlen(name)>11){ //if the name is longer than the max allowed name, trunctuate
		char* trunc_name = malloc(sizeof(char)*12);
		memcpy(trunc_name,name,11);
		trunc_name[11]='\0';
		rllfs->boot_sector->disk_name = trunc_name;
	}else{
		rllfs->boot_sector->disk_name = name;
	}

	//allocate free block table
	rllfs->boot_sector->free_blocks = calloc(SECTOR_SIZE*FREE_BLOCK_TABLE_SECTOR_LENGTH,sizeof(char));

	//allocate and set up empty root file directory
	struct directory_block* root = rllfs->boot_sector->root;
	root = malloc(sizeof(struct directory_block));
	root->name_of_directory = "root";
	root->total_subdirectory_num = 0;
	root->total_file_num = 0;
	root->is_extended = 0;
	root->upper =0;

	return rllfs;
}

int free_filesystem(struct llfs_filesystem* fs){
	free(fs->boot_sector->disk_name);
	free(fs->boot_sector->root);
	free(fs->boot_sector->free_blocks);
	free(fs->boot_sector);
	free(fs);
	return 0;
}

int add_file_to_filesystem(struct llfs_filesystem* fs, char* path, char* name){
	//first break up the path into a blocks of text;
	char** path_names = malloc(sizeof(char*)*2);
	int path_num=0;

	//parser to break up \blah\blah\ format into array of strings
	int i=0;
	while(path[i]!='\0'){
		if(path[i] != '\\'){ //mismatch, return -1
			return -1;
		}
		i++;
		if(path[i]!='\0'){
			char* temps = malloc(sizeof(char)*2);
			int tempi=0;
			for(i;path[i]!='\\';i++){
				if(path[i]=='\0'){
					free(temps);
					break;
					//return -2; //oops
				}
				temps[tempi]=path[i];
				temps=realloc(temps,sizeof(char)*(tempi)+2);
				tempi++;
			}
			temps[tempi]='\0';
			path_names[path_num]=strdup(temps);
			path_names = realloc(path_names,sizeof(char*)*(path_num+2));
			path_num++;
		}
	}

	struct directory_block* finder = fs->boot_sector->root;
	int curr_path_level=0;
	while(1){ //while doesn't match up
		if(!strcmp(path_names[curr_path_level],finder->name_of_directory)){
			//we're in the right directory;
			break;
		}else if(finder->total_subdirectory_num != 0){ //if directory has subdirectories
			int i;
			for(i=0;i<total_subdirectory_num;i++){

				if(!strcmp(path_names[curr_path_level+1],finder->dir_pointers_in_mem[i]->name_of_directory)){
					//matched! go down one
					curr_path_level++;
					break;
				}

				//if i >= 10 
				//check if directory is extended
				//if isn't break and exit
				//if it is, load extended directory block i/10 (if it exists), and subdirectory i%10
			}
		}else{
			//no more subdirectories to check, path does not exist
		}
	}

	//free path_names at the end
	for(i=0;i<path_num;i++){
		free(path_names[i]);
	}
	free(path_names);
 
	return 0;
}