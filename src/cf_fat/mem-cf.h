/*
IBEX UK LTD http://www.ibexuk.com
Electronic Product Design Specialists
RELEASED SOFTWARE

The MIT License (MIT)

Copyright (c) 2013, IBEX UK Ltd, http://ibexuk.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//Project Name:		COMPACT FLASH MEMORY CARD FAT16 & FAT 32 DRIVER
//CF CARD DRIVER C CODE HEADER FILE

#ifndef MEM_CF_H
#define MEM_CF_H

#include "mem-types.h"


#define	FFS_DRIVER_GEN_512_BYTE_BUFFER	ffs_general_buffer		//This general buffer is used by routines and may be the same as the buffer that


//PROCESS CF CARD STATE MACHINE STATES
typedef enum _FFS_PROCESS_STATE
{
    FFS_PROCESS_NO_CARD,
    FFS_PROCESS_WAIT_FOR_CARD_FULLY_INSERTED,
    FFS_PROCESS_COMPLETE_RESET,
    FFS_PROCESS_WAIT_FOR_CARD_RESET,
    FFS_PROCESS_CARD_INITIALSIED
} FFS_PROCESS_STATE;


//------------------------------
//----- EXTERNAL FUNCTIONS -----
//------------------------------
void ffs_process (void);
void ffs_read_sector_to_buffer (DWORD sector_lba);
void ffs_write_sector_from_buffer (DWORD sector_lba);
WORD ffs_read_word (void);


//---------------------------------------
//----- EXTERNAL MEMORY DEFINITIONS -----
//---------------------------------------
extern WORD number_of_root_directory_sectors;				//Only used by FAT16, 0 for FAT32
extern BYTE ffs_buffer_needs_writing_to_card;
extern DWORD ffs_buffer_contains_lba;
extern DWORD fat1_start_sector;
extern DWORD root_directory_start_sector_cluster;
extern DWORD data_area_start_sector;
extern BYTE disk_is_fat_32;
extern BYTE sectors_per_cluster;
extern DWORD last_found_free_cluster;
extern DWORD sectors_per_fat;
extern BYTE active_fat_table_flags;
extern DWORD read_write_directory_last_lba;
extern WORD read_write_directory_last_entry;

#endif // MEM_CF_H