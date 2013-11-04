/*
 * EXT2.c
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

#include "SDMCC/SDMMC.h"
#include "EXT2.h"
#include "STRING/string_hdl.h"

#include "LCD/graphic.h"
#include "PARSER/parsing.h"

char* readInode( unsigned long inodeId );

int EXT_mount()
{
	struct MBR *dMbr;
	struct partitionDesc *partition;
	struct superblock *sBlock;
	char cc[10];
	//	Read block = FromByte / SD_blkSize
	readBlockSD(0);						// read from addr 0
	
	dMbr = (struct MBR *)buffer;
	
	if (dMbr->signature != 0xAA55)		//	this is not a MBR disk
		return 0x01;
	
	partition = ( struct partitionDesc *)(dMbr->pEntry1);
		
	EXT_base = partition->LBA_1st_sector;
	readBlockSD(EXT_base + 2);			// SD card addressing is in LBA - Ext super block start at +1024byte

	sBlock				= ( struct superblock *)buffer;

//	Inode size (byte)
	inode_size			= sBlock->s_inode_size;

//	Block size calculation
	block_size			= 1024 << sBlock->s_long_block_size;
	if ( block_size != 1024 )
		return 0x02;
	
//	Total number of inodes per block
	inode_block_count	= block_size / inode_size;
	
//	Saving the number of inode per group
	s_inodes_per_group	= sBlock->s_inodes_per_group;

//	Saving first usable inode index number
	first_ino			= sBlock->s_first_ino;
	//	Calculating first usable inode block
	first_block_ino		= first_ino * inode_size / block_size;
	
	bg_count			= sBlock->s_block_count / sBlock->s_blocks_per_gruop;
	
	unsigned int bg_per_block	= block_size / BG_SIZEOFF;
	unsigned int bg_read		= bg_count / bg_per_block;
	
	if ( (block_size / BG_SIZEOFF ) % bg_count > 0)
		bg_read++;

//	Inode table adress saving	
//	maximum inode adress saved are set to 100	
	for (int i = 0; i < bg_read; i++)
	{
		struct blockGroup *bgGroup;
		readBlockSD(EXT_base + BG_LBA_OFFSET + ( i * 2 ));								//	Read block of bg_descriptor table
		bgGroup = (struct blockGroup *)buffer;											//	populate struct
		for ( int j = 0; j < bg_per_block; j++) {
			if (i * BG_SIZEOFF + j >= 512) { i = bg_read; break;}
			inodes_addr[i * BG_SIZEOFF + j]	= ( bgGroup + j )->bg_inode_table;
		}		
	}
	
	return 0x00;
}


DIR EXT_ls(DIR fileDir, DIR_HNDL* hndl)
{
	
	DIR null_dir;
	null_dir.inode_id = 0;
	
//	No-dir type discrimination
	if ( fileDir.inode_id != 0 && fileDir.inode_type != EXT2_FT_DIR)
		return null_dir;
	
//	If no inode is specified, ROOT inode is selected
	if (fileDir.inode_id == 0)
		fileDir.inode_id = 2;
		
	struct inode *inode_struct;
	inode_struct = (struct inode *)readInode(fileDir.inode_id);
	
	//	increase index block pointer
	if ( hndl->dir_entry_byte >= block_size ) {
		hndl->dir_entry_byte	= 0;
		hndl->last_inode_block_index++;
	}	
	
	//	if next block is == 0 or index > 12 exit DIR->inode_id = 0
	if ( inode_struct->i_block[hndl->last_inode_block_index] == 0 || hndl->last_inode_block_index > 12)
		return null_dir;
	
	//	read directory entry block
	readBlockSD( EXT_base + ( inode_struct->i_block[hndl->last_inode_block_index] * 2 )); 
		
	struct dirEntry *_tempDir;
	_tempDir = (struct dirEntry *)(buffer + hndl->dir_entry_byte);
	
	DIR _returnDIR;
		
	_returnDIR.inode_id			= _tempDir->d_inode;
	_returnDIR.inode_type		= _tempDir->d_file_type;
	
	hndl->dir_entry_byte		+= _tempDir->d_rec_len;
	
	for (int i = 0; i < MAX_F_NAME && i < _tempDir->d_name_len; i++) {
		_returnDIR.inode_name[i] = _tempDir->d_file_name[i];
		_returnDIR.inode_name[i + 1] = 0x00;
	}	
		
	return _returnDIR;
}


char EXT_readfile( FILE_HNDL* fileHndl, char *text ) 
{
	struct inode *inode_struct;
	int	loop = 0;

	inode_struct					= (struct inode *)readInode(fileHndl->inode_id);
	
	//	if file inode is not a socket inode - skip
	if ( !(inode_struct->i_mode & 0x8000) )
		return 0x00;
		
	//	getting file size
	unsigned long file_size			= inode_struct->i_size;
	
	unsigned long *data_address;
	
	do {
		
		//	re-reading inode struct
		//	re-reading operation is necessary for memory saving
		if (loop == 1)
			inode_struct					= (struct inode *)readInode(fileHndl->inode_id);
		
		//	calculating block inode index
		unsigned long block_index		= fileHndl->last_byte / block_size;
	
		//	reading indirect block addresses 
		if ( block_index < 12 ) {
			*data_address				= inode_struct->i_block[block_index];
		}		
		else {
			readBlockSD( EXT_base + (inode_struct->i_block[12] ) * 2);
			data_address				= (unsigned long *)(buffer + ( block_index - 12 ) * 4 );
		}
		
		//	reading data block
		readBlockSD( EXT_base + *data_address * 2 );
	
		//	local block byte index
		unsigned int local_byte_index	= fileHndl->last_byte % block_size;
	
		if ( fileHndl->last_byte >= file_size )
			return EOF;
		
		int i = 0;
		for ( i = local_byte_index; i < block_size && buffer[i] != '\n' && fileHndl->last_byte < file_size; i++) {
			text[i - local_byte_index]			= buffer[i];
			fileHndl->last_byte++;
		}
		
		text[i - local_byte_index]		= 0x00;
	
		if (buffer[i] == '\n')
			fileHndl->last_byte++;
		
		//	Exit condition - stop reading stream
		if ( buffer[i] == '\n' || fileHndl->last_byte >= file_size)	
			return 0;
		
		loop = 1;
		
	} while( 1 );	
}

char* readInode( unsigned long inodeId ) 
{
	//	calculating in witch block group is contained the inode
	unsigned long inodes_block_gruop	= ( inodeId - 1 ) / s_inodes_per_group;
	//	calculating local block group inode index
	unsigned long local_inode_index		= ( inodeId - 1 ) % s_inodes_per_group;
	//	calculating bock index in the local block group
	unsigned long block_index			= ( local_inode_index * inode_size ) / block_size;
	//	read address
	unsigned long reading_adress		= inodes_addr[inodes_block_gruop] + block_index;
	
	readBlockSD( EXT_base + reading_adress * 2 );
	
	//	inode start byte into block
	int inode_buff_start_byte			= ( local_inode_index * inode_size ) - block_index * block_size;

	/*unsigned char _temp[inode_size];
	*
	*for (int i = inode_buff_start_byte; i < inode_buff_start_byte + inode_size; i++)
	*	_temp[ i - inode_buff_start_byte ] = buffer[ i ];
	*
	*return _temp;
	*/
	return buffer + inode_buff_start_byte;
}