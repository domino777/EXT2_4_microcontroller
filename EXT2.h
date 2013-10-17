/*
 *  FAT.h
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
 *       @version        : 0.1
 *       
 */ 

#ifndef FAT_H_
#define FAT_H_

//	CHAR	- 1 byte
//	INT	- 2 byte
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
		unsigned long	s_inodes_count;			//	total number of inode
		unsigned long	s_block_count;			//	total number of block
		unsigned long	s_r_blocks_count;		//	total number of reserved block for superuser
		unsigned long	s_free_blocks_count;		//	total number of free block (include super user)
		unsigned long	s_free_inodes_count;		//	total number of free inode
		unsigned long	s_first_data_block;		//	identification id of the first block that contain the superblock - for 1KB block = 1 - for block > 1KB = 0; 
		unsigned long	s_long_block_size;		//	shift value used for: block size = 1024 << s_long_block_size
		signed	 long	s_log_frag_size;		//	shift value used for: fragment size = 1024 << s_long_frag_size or fragment size = 1024 >> -s_long_frag_size
		unsigned long	s_blocks_per_gruop;		//	number of block per group
		unsigned long	s_frags_per_group;		//	number of fragment per group
		unsigned long	s_inodes_per_group;		//	number of inode per group also used for calculate the size of the inode bitmap of each block group
		unsigned long	s_mtime;			//	last time fs was mounted - UNIX time def POSIX
		unsigned long	s_wtime;			//	last time fs write access - UNIX time def POSIX
		unsigned int	s_mnt_count;			//	number of time fs was mounted
		unsigned int	s_max_mnt_count;		//	number of time fs was mounted from last full check
		unsigned int	s_magic;			//	FS identification number EXT2 = 0xEF53
		unsigned int	s_state;			//	fs state EXT2_VALID_FS = 1 | EXT2_ERROR_FS = 2
		unsigned int	s_errors;			//	to do in case of err EXT2_ERRORS_CONTINUE = 1 | EXT2_ERRORS_RO = 2 | EXT2_ERRORS_PANIC = 3
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
		unsigned int	bg_free_block_count;		//	total number of free block of the group
		unsigned int	bg_free_inode_count;		//	total inode of free block of the group
		unsigned int	bg_used_dirs_count;		//	number of inode allocated to directories of the group
		unsigned int	bg_pad;				//	padding
		unsigned long	bg_reserved;
	};

struct inode {
		unsigned int	i_mode;
		unsigned int	i_uid;
		unsigned long	i_size;
		unsigned long	i_atime;
		unsigned long	i_ctime;
		unsigned long	i_mtime;
		unsigned long	i_dtime;
		unsigned int	i_gid;
		unsigned int	i_links_count;
		unsigned long	i_blocks;
		unsigned long	i_flags;
		unsigned long	i_osd1;
		unsigned long	i_block[15];
		unsigned long	i_generation;
		unsigned long	i_file_acl;
		unsigned long	i_dir_acl;
		unsigned long	i_faddr;
		unsigned long	i_osd2[3];
	};

struct dirEntry {
		unsigned long	d_inode;
		unsigned int	d_rec_len;
		unsigned char	d_name_len;
		unsigned char	d_file_type;
		unsigned char	d_file_name[255];
	};
typedef struct {
		unsigned long	inode_id;			//	inode id of the file/directory
		unsigned int	inode_type;			//	type of inode file/dir/link
		unsigned char	inode_name[10];			//	name of inode file_name/dir_name
	} DIR;

typedef struct {
		unsigned long	last_inode_block_index;		//	last index used of i_block
		unsigned long	dir_entry_byte;			//	next entry byte index in the selected block indexed
	} DIR_HNDL;
	

volatile unsigned long		EXT_base;		//	ext partition base address
volatile unsigned long		bg_[100];		//	block group base address
volatile unsigned int		bg_count;		//	block group count


volatile unsigned long		blockGroupNo;

volatile unsigned long		block_size;
volatile unsigned long		inodes_addr[100];	//	inode table base address
volatile unsigned long		first_ino;
volatile unsigned long		first_block_ino;	//	index first usable inode for standard file
volatile unsigned long		s_inodes_per_group;	//	inodes per group
volatile unsigned long		inode_block_count;	//	number of inote into a block
volatile unsigned int		inode_size;

extern volatile unsigned char	buffer[1024];

#define INODE_STRUCT_SIZE	128
#define BG_SIZEOFF		32
#define BG_LBA_OFFSET		4	
#define DIR_TEMP_OFFSET		264
#define MAX_F_NAME		10

void EXT_mount();
DIR EXT_ls(DIR fileDir, DIR_HNDL* hndl);

#endif /* FAT_H_ */
