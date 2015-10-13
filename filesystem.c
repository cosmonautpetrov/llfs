#include "filesystem.h"

int first_free_block(struct llfs_filesystem* fs){
	int i;
	for(i=0;i<BLOCKS_ON_DISK;i++){
		if(is_block_free(fs,i)
			return i;
	}
	return -1; //no free blocks
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

	unsigned int n=num/8;
	unsigned int shiftn = num%8;

	char bit = fs->boot_sector->free_blocks[n];

	bit = (bit >> shiftn) && 0x01;

	bit ^= 1;

	fs->boot_sector->free_blocks[n] ^= (-bit ^ fs->boot_sector->free_blocks[n]) & (1); //flip bit

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
	rllfs->boot_sector->root = malloc(sizeof(struct directory_block));

	struct directory_block* root = rllfs->boot_sector->root;
	root->name_of_directory = strdup("root");
	root->total_subdirectory_num = 0;
	root->total_file_num = 0;
	root->is_extended = 0;
	root->upper =0;

	return rllfs;
}

int free_filesystem(struct llfs_filesystem* fs){
	free(fs->boot_sector->disk_name);
	free(fs->boot_sector->root->name_of_directory);
	free(fs->boot_sector->root);
	free(fs->boot_sector->free_blocks);
	free(fs->boot_sector);
	free(fs);
	return 0;
}

int add_file_to_filesystem(struct llfs_filesystem* fs, char* path, char* name){

	struct directory_block* dest = path_to_directory_block(fs,path);

	if(!dest) //if dest does not exist 
		return -1;

	if(dest->total_file_num == 0){ //no files written to director yet
		dest->file_pointers_in_mem = malloc(sizeof(struct file_block*)); //allocate file_pointers in mem
	}

	//load file

	FILE* srcf = fopen(name, "r");
	if(!srcf) //no file
		return -2;

	//make new file)block
	struct file_block* f = malloc(sizeof(struct file_block*));
	//set name
	f->name = name;
	//find size
	fseek(srcf,0,SEEK_END);
	f->size = ftell(srcf);
	rewind(srcf);
	//read it in
	f->data = malloc(sizeof(char)*f->size);
	int bytes_read = fread(f->data,1,f->size,srcf);


	//add it to directory block
	if(dest->total_file_num >= 10){ //needs extension
		//write to extended block
	}else{
		//write it to current block
		dest->total_file_num++;
		dest->file_pointers_in_mem[dest->total_file_num-1] = f;
	}

	return 0;
}

struct directory_block* path_to_directory_block(struct llfs_filesystem* fs,char* path){
	char** path_names = path_to_string_arr(path); //break up path names

	struct directory_block* finder = fs->boot_sector->root;
	int curr_path_level=0;
	while(1){ //while doesn't match up
		if(!strcmp(path_names[curr_path_level],finder->name_of_directory)){
			//we're in the right directory;
			//free path names and return directory

			int i;
			for(i=0;path_names[i]!=0;i++){
				free(path_names[i]);;
			}
			free(path_names);

			return finder;	

		}else if(finder->total_subdirectory_num != 0){ //if directory has subdirectories
			int i;
			for(i=0;i<finder->total_subdirectory_num;i++){

				//if i >= 10 
				//check if directory is extended
				//if isn't break and exit
				//if it is, load extended directory block i/10 (if it exists), and subdirectory i%10

				if(i>=10){
					struct directory_block* ext_block = finder; //extended block
					int y;
					for(y=0;y<(i/10);y++){ //navigate to extended directory block with our subdir
						if(ext_block->is_extended){ //can we actually extend?
							ext_block = ext_block->next;
						}else{
							//no extended block here, free path names and exit
							int i;
							for(i=0;path_names[i]!=0;i++){
								free(path_names[i]);;
							}
							free(path_names);		

							return (struct directory_block*) 0;						
						}
					}

					if(!strcmp(path_names[curr_path_level+1],ext_block->dir_pointers_in_mem[i%10]->name_of_directory)){
						curr_path_level++;
						finder = ext_block->dir_pointers_in_mem[i%10]; //if path names match up go down a level
						break;
					}
				}else{
					if(!strcmp(path_names[curr_path_level+1],finder->dir_pointers_in_mem[i]->name_of_directory)){
						//matched! go down one
						curr_path_level++;
						finder = finder->dir_pointers_in_mem[i]; //if path names match up go down a level
						break;
					}
				}
			}
		}else{
			//no more subdirectories to check, path does not exist
			//free path names and exit 

			int i;
			for(i=0;path_names[i]!=0;i++){
				free(path_names[i]);;
			}
			free(path_names);	

			return (struct directory_block*)0;
		}
	}
	//free path_names at the end
	
}

char** path_to_string_arr(char* path){
	char** path_names = malloc(sizeof(char*)*2);
	int path_num=0;

	//parser to break up \blah\blah\ format into array of strings
	int i=0;
	while(path[i]!='\0'){
		if(path[i] != '\\'){ //mismatch, return 0
			return (char**)0;
		}
		i++;
		if(path[i]!='\0'){
			char* temps = malloc(sizeof(char)*2);
			int tempi=0;
			for(i;path[i]!='\\';i++){
				if(path[i]=='\0'){
					free(temps);
					return (char**)0;
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
			path_names[path_num] = '\0';
		}
	}

	return path_names;
}