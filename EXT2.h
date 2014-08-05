/*
 *  EXT2.h
 *
 *  "Copyright 2013 Mauro Ghedin"
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  or any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 *
 *       @author         : Mauro Ghedin
 *       @contact        : domyno88 at gmail dot com
 *       @version        : 0.2
 *       
 */ 

#ifndef EXT2_H_
#define EXT2_H_

//	CHAR	- 1 byte
//	INT		- 2 byte
//	LONG	- 4 byte
struct MBR {
		unsigned char bootstrap[446];
		unsigned char pEntry1[16];
		unsigned char pEntry2[16];
		unsigned char pEntry3[16];
		unsigned char pEntry4[16];
		unsigned int signature;
	};

struct partitionDesc {
		unsigned char status;
		unsigned char CHS1st_Head;
		unsigned char CHS1st_sector_cylnderH;
		unsigned char CHS1st_cylinderL;
		unsigned char partType;
		unsigned char CHSLast_Head;
		unsigned char CHSLast_sector_cylnderH;
		unsigned char CHSLast_cylinderL;
		unsigned long LBA_1st_sector;
		unsigned long sectorCoun;
	};

struct superblock {
		unsigned long	s_inodes_count;			//	total number of inode;
		unsigned long	s_block_count;			//	total number of block;
		unsigned long	s_r_blocks_count;		//	total number of reserved block for superuser;
		unsigned long	s_free_blocks_count;	//	total number of free block (include super user)
		unsigned long	s_free_inodes_count;	//	total number of free inode
		unsigned long	s_first_data_block;		//	identification id of the first block that contain the superblock - for 1KB block = 1 - for block > 1KB = 0; 
		unsigned long	s_long_block_size;		//	shift value used for: block size = 1024 << s_long_block_size
		signed	 long	s_log_frag_size;		//	shift value used for: fragment size = 1024 << s_long_frag_size or fragment size = 1024 >> -s_long_frag_size
		unsigned long	s_blocks_per_gruop;		//	number of block per group
		unsigned long	s_frags_per_group;		//	number of fragment per group
		unsigned long	s_inodes_per_group;		//	number of inode per group also used for calculate the size of the inode bitmap of each block group
		unsigned long	s_mtime;				//	last time fs was mounted - UNIX time def POSIX
		unsigned long	s_wtime;				//	last time fs write access - UNIX time def POSIX
		unsigned int	s_mnt_count;			//	number of time fs was mounted
		unsigned int	s_max_mnt_count;		//	number of time fs was mounted from last full check
		unsigned int	s_magic;				//	FS identification number EXT2 = 0xEF53
		unsigned int	s_state;				//	fs state EXT2_VALID_FS = 1 | EXT2_ERROR_FS = 2
		unsigned int	s_errors;				//	to do in case of err EXT2_ERRORS_CONTINUE = 1 | EXT2_ERRORS_RO = 2 | EXT2_ERRORS_PANIC = 3
		unsigned int	s_minor_rev_level;		//	revision level
		unsigned long	s_lastcheck;			//	last file system check - UNIX time def POSIX
		unsigned long	s_checkinterval;		//	maximum unix time interval allowed between fs check - UNIX time def POSIX
		unsigned long	s_creator_os;			//	fs os creator EXT2_OS_LINUX = 0 | EXT2_OS_HURD = 1 | EXT2_OS_MASIX = 2 | EXT2_OS_FREEBSD = 3 | EXT2_OS_LITES = 4
		unsigned long	s_rev_level;			//	revision level value EXT2_GOOD_OLD_REV = 0 | EXT2_DYNAMIC_REV = 1
		unsigned int	s_def_resuid;			//	default user id for reserved blocks Ext2 default 0
		unsigned int	s_def_resgid;			//	default user group for reserved blocks Ext2 default 0	
		unsigned long	s_first_ino;			//	index first usable inode for standard file
		unsigned int	s_inode_size;			//	inode size
	};

struct blockGroup {
		unsigned long	bg_block_bitmap;		//	first block bitmap id of the group
		unsigned long	bg_inode_bitmap;		//	first inode bitmap id of the group
		unsigned long	bg_inode_table;			//	first inode table id of the group
		unsigned int	bg_free_block_count;	//	total number of free block of the group
		unsigned int	bg_free_inode_count;	//	total inode of free block of the group
		unsigned int	bg_used_dirs_count;		//	number of inode allocated to directories of the group
		unsigned int	bg_pad;					//	padding
		unsigned long	bg_reserved[3];
	};

struct inode {
		unsigned int	i_mode;					//	indicate the format of the described file and the access rights
		unsigned int	i_uid;					//	user id associated
		unsigned long	i_size;					//	size of the file in byte - lower 32bit
		unsigned long	i_atime;				//	access time (s) from 01/01/1970
		unsigned long	i_ctime;				//	creation time (s) from 01/01/1970
		unsigned long	i_mtime;				//	modification time (s) from 01/01/1970
		unsigned long	i_dtime;				//	deleted time (s) from 01/01/1970
		unsigned int	i_gid;					//	value of the POSIX group having access to this file
		unsigned int	i_links_count;			//	indicating how many times this this particular inode is linked (referred to)
		unsigned long	i_blocks;				//	total number of 512-byte blocks reserved to contain the data of this inode
		unsigned long	i_flags;				//	
		unsigned long	i_osd1;					//	OS dependent
		unsigned long	i_block[15];			//	first 12block - pointer to block id where data are stored | 13 - single-indirect blocks | 14 - doubly-indirect blocks | 15 - triply-indirect blocks
		unsigned long	i_generation;			//	used to indicate the file version
		unsigned long	i_file_acl;				//	block number of extended attribute
		unsigned long	i_dir_acl;				//	size of the file in byte - high 32bit
		unsigned long	i_faddr;				//	location of file fragment (deprecated)
		unsigned long	i_osd2[3];				//	OS dependent structure
	};

struct dirEntry {
		unsigned long	d_inode;						//	inode id of the file/dir
		unsigned int	d_rec_len;						//	total record length used stote this record
		unsigned char	d_name_len;						//	name length
		unsigned char	d_file_type;					//	file type dir|regulatFile
		unsigned char	d_file_name[255];				//	name
	};
	
typedef struct {
		unsigned long	inode_id;						//	node id of the file/dir
		unsigned int	inode_type;						//	inode type
		unsigned char	inode_name[20];					//	inode name
	} DIR;

typedef struct {
		unsigned long	last_inode_block_index;			//	last index used of i_block
		unsigned long	dir_entry_byte;					//	next entry byte (!= only for directory not empty)
	} DIR_HNDL;
	
typedef struct {
		unsigned long	inode_id;						//	file inode id
		unsigned long	last_byte;						//	last byte read
	} FILE_HNDL;


volatile unsigned long			EXT_base;				//	ext partition base address
volatile unsigned long			bg_[100];				//	block group base address
volatile unsigned int			bg_count;				//	block group count


volatile unsigned long			blockGroupNo;

volatile unsigned long			block_size;
volatile unsigned long			inodes_addr[512];		//	inode table base address
volatile unsigned long			first_ino;
volatile unsigned long			first_block_ino;		//	index first usable inode for standard file
volatile unsigned long			s_inodes_per_group;		//	inodes per group
volatile unsigned long			inode_block_count;		//	number of inode into a block
volatile unsigned int			inode_size;

extern volatile unsigned char	buffer[1024];

#define INODE_STRUCT_SIZE	128
#define BG_SIZEOFF			32
#define BG_LBA_OFFSET		4	
#define DIR_TEMP_OFFSET		264
#define MAX_F_NAME			20
#define EXT2_FT_DIR			2
#define EXT2_FT_REG_FILE	1
#define EOF					0x04
#define MAX_FLINE_LENGHT	1024

int		EXT_mount();
DIR		EXT_ls(DIR fileDir, DIR_HNDL* hndl);
char	EXT_readfile( FILE_HNDL *fileHndl, char* text );

#endif /* EXT2_H_ */
