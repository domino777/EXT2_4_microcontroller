/*
 * CFile1.c
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

#include "SDMCC/SDMMC.h"
#include "EXT2.h"
#include "STRING/string_hdl.h"

void EXT_mount()
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
	unsigned int bg_read		= bg_per_block / bg_count;
	
	if ( (block_size / BG_SIZEOFF ) % bg_count > 0)
		bg_read++;

//	Inode table adress saving	
//	maximum inode adress saved are set to 100	
	for (int i = 0; i < bg_read; i++)
	{
		struct blockGroup *bgGroup;
		readBlockSD(EXT_base + BG_LBA_OFFSET + ( i * 2 ));								//	Read block of bg_descriptor table
		for ( int j = 0; j < bg_per_block; j++) {
			unsigned char _temp[35];
				for (int b = j * BG_SIZEOFF; b < j * BG_SIZEOFF + BG_SIZEOFF; b++)		//	Taking a part of buffer and save to _temp array
					_temp[b - j * BG_SIZEOFF] = buffer[b];
			bgGroup = (struct blockGroup *)_temp;										//	populate struct
			// exit loop
			if (i * BG_LBA_OFFSET + j >= 100) { i = bg_read; break;}
			inodes_addr[i * BG_LBA_OFFSET + j]	= bgGroup->bg_inode_table;
		}		
	}
}


DIR EXT_ls(DIR fileDir, DIR_HNDL* hndl)
{
	
	DIR null_dir;
	null_dir.inode_id = 0;
	
	if ( fileDir.inode_id != 0 && fileDir.inode_type != 2)
		return null_dir;
	
//	If no inode is specified, ROOT inode is selected
	if (fileDir.inode_id == 0) {
		fileDir.inode_id = 2;
	}
		
	unsigned long inodes_block_gruop	= (fileDir.inode_id - 1) / s_inodes_per_group;
	unsigned long local_inode_index		= (fileDir.inode_id - 1) % s_inodes_per_group;
	
	unsigned long block_index			= local_inode_index / inode_block_count;
	
	unsigned long reading_adress		= inodes_addr[inodes_block_gruop] + ( block_index * 2 );
	readBlockSD( EXT_base + ( reading_adress * 2));
	
	int inode_buff_start_byte			= ( local_inode_index * inode_size ) - block_index * block_size;

	struct inode *inode_struct;	
	unsigned char _temp[512];
	
	for (int i = inode_buff_start_byte; i < inode_buff_start_byte + inode_size; i++)
		_temp[ i - inode_buff_start_byte ] = buffer[ i ];
	
	inode_struct = (struct inode *)_temp;
	
	//	increase index block pointer
	if ( hndl->dir_entry_byte >= block_size ) {
		hndl->dir_entry_byte	= 0;
		hndl->last_inode_block_index++;
	}	
	
	//	if next block is == 0 or index > 12 exit DIR->inode_id = 0
	if (inode_struct->i_block[hndl->last_inode_block_index] == 0 || hndl->last_inode_block_index > 12)
		return null_dir;
	
	//	read directory entry block
	readBlockSD( EXT_base + ( inode_struct->i_block[hndl->last_inode_block_index] * 2 )); 
		
	//	reading DIR info data
	for (int i = hndl->dir_entry_byte; i < hndl->dir_entry_byte + DIR_TEMP_OFFSET; i++)
		_temp[ i - hndl->dir_entry_byte ] = buffer[ i ];
	
	struct dirEntry *_tempDir;
	_tempDir = (struct dirEntry *)_temp;
	
	DIR _returnDIR;
	
	NULL_char(_returnDIR.inode_name, MAX_F_NAME);
	
	_returnDIR.inode_id			= _tempDir->d_inode;
	_returnDIR.inode_type		= _tempDir->d_file_type;
	
	hndl->dir_entry_byte		+= _tempDir->d_rec_len;
	
	for (int i = 0; i < MAX_F_NAME && i < _tempDir->d_name_len; i++)
		_returnDIR.inode_name[i] = _tempDir->d_file_name[i];
		
	return _returnDIR;
}