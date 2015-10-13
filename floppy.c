#include "floppy.h"

struct floppy* make_floppy(char* name, char from_file, char* filename){
	struct floppy* retf = malloc(sizeof(struct floppy));

	retf->name=strdup(name); 

	retf->disk = malloc(sizeof(char*)*SECTOR_NUM);

	//allocate sectors per disk
	int i;
	for(i=0;i<SECTOR_NUM;i++){
		retf->disk[i] = malloc(sizeof(char)*SECTOR_SIZE);
	}


	return retf;
}

int free_floppy(struct floppy* f){
	if(f){ //if f isn't null 
		//free all sectors, then sector pointers
		int i;
		for(i=0;i<SECTOR_NUM;i++){
			free(f->disk[i]);
		}
		free(f->disk);
		free(f->name);
		free(f);
		return 0;
	}else
		return -1;
}

char get_byte_from_sector(struct floppy* f, int n, int b){
	if(f){
		return f->disk[n][b];
	}else
		return 0;
}

int write_byte_to_sector(struct floppy* f, int n, int b, char src){
	if(f){
		f->disk[n][b]=src;
	}else
		return -1;
}

//write a full sector at a time
int get_sector(struct floppy* f, int n, char* dest){
	int i;
	for(i=0;i<SECTOR_SIZE;i++){
		dest[i]=get_byte_from_sector(f,n,i);
	}
	return 0;
}

//read a full sector at a time
int write_sector(struct floppy* f, int n, char* src){
	int i;
	for(i=0;i<SECTOR_SIZE;i++){
		write_byte_to_sector(f,n,i,src[i]);
	}
	return 0;
}


int readf(struct floppy* f, unsigned int from, unsigned int to, char* dest){
	//wrapper function to read bytes from a floppy without worrying about sectors
	if(to > SECTOR_NUM*SECTOR_SIZE || from >= SECTOR_NUM*SECTOR_SIZE){
		return -1; //out of bounds read, terminate
	}else{
		//which sect to read from
		int sect;


		//read from floppy
		int i;
		for(i=from;i<to;i++){
			sect=i/SECTOR_SIZE; //get sect number
			dest[i-from]=get_byte_from_sector(f,sect,i%SECTOR_SIZE); //modulus for internal sector offset
		}
		return 0;
	}
}

int writef(struct floppy* f, unsigned int from, unsigned int to, char* src){
	//wrapper function to WRITE bytes to a floppy without worrying about sectors
	if(to > SECTOR_NUM*SECTOR_SIZE || from >= SECTOR_NUM*SECTOR_SIZE){
		return -1; //out of bounds write, terminate
	}else{
		//which sect to write to 
		int sect;


		//write to floppy
		int i;
		for(i=from;i<to;i++){
			sect=i/SECTOR_SIZE; //get sect number
			write_byte_to_sector(f,sect,i%SECTOR_SIZE,src[i-from]); //modulus for internal sector offset
		}
		return 0;
	}
}