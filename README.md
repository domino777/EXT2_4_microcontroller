EXT2_AVR
========

EXT2 library for AVR

Project for the implementation of EXT2 fs on microcontroller devices

EXT2 library support only file system formatted with ext2fs with 1kB of block size.

Maximum readable file size is 274432byte and the maximum number of storable inodes from inode table is set to 512. Library work only in read-only mode without control of root data, file privilege and only files which are seen and read by the system are regular file and directory.

EXT2.c
------
In this file is called, more than one time, a function named readBlockSD(??) ( int readBlockSD(unsigned long blkStart) ) used only for read data from a storage drive.
This function read a block of 1024bytes from a storage by a given block number relative to the storage block size, not the fs block size.
I have used this function for read data from a SD card and by default a SD block size is 512bytes.
Datas are not returned directly by the function but an 'extern volatile' variable is used.

Limitation of the library
-------------------------

For the function "EXT_ls" only firsts 12 direct inodes index adressing on the 'inode' structure are read. The maximum inode string name is limited to 20 chars
EXT_readfile can be handle the firsts 12 direct inodes index plus the first indirect inode adressing, this means that the maximum readable file size is up to 274432byte.
There is no limit about of how many chars of a file can be read ( one line at time is returned ), but if a string is too long and memory is not enought... up to you ;)


Extra info
----------
When I have develop this library my experience of C was very very very... poor but if you are interested about my progress check ----> https://github.com/linCC-wca/linCC_core


If some one is interested about this project... welcome :)
