#include "main.h"

int main(){
	/*
	struct floppy* f = make_floppy("a.bin",0,(char*)0);
	writef(f,0,11,"hello world");
	char* dest = malloc(sizeof(char)*12);
	readf(f,0,11,dest);
	printf("%s\n",dest);
	printf("%i\n",free_floppy(f));
	*/
	struct llfs_filesystem* llfs_fs = make_filesystem("tempfs");
	printf("%i\n",add_file_to_filesystem(llfs_fs, "\\temp\\file\\system\\","tempfile"));
	return 0;
}