llfs, or linked list file system:

A small hobbyist file system written for 1.44 floppies.

Not of any real particular use, just a fun project I thought I'd write as part of the osdev community.

llfs works through linked lists of directory and file blocks which can be easily and quickly parsed
by low-level code. 

Supports:

Opening files
Writing to a file
Writing new files

open(char* name) //open file for reading / writing
read(file* f) //reads from file f
write(file* f) //writes to file f


TODO:
ADD SIZE AND TOTAL DATA ON DISK SO FAR TO FILESYSTEM STRUCT, KEEP TRACK OF CURRENT BLOCKS AND 
FILES TO MAKE SURE THERE IS NO OVERFLOW... maybe start actually tracking bits in free_current_blocks?

