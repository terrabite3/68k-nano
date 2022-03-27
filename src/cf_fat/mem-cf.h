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



#include "mem-types.h"


//*****************************
//*****************************
//********** DEFINES **********
//*****************************
//*****************************
#ifndef CF_C_INIT		//Do only once the first time this file is used
#define	CF_C_INIT



//----------------------------------------------
//----- DEFINE TARGET COMPILER & PROCESSOR -----
//----------------------------------------------


//----------------------
//----- IO DEFINES -----
//----------------------


// TODO: move to .c file?
#define CF_DATA     (*((volatile WORD*) 0x900000))
#define CF_ERROR    (*((volatile BYTE*) 0x900003))
#define CF_FEATURE  (*((volatile BYTE*) 0x900003))
#define CF_COUNT    (*((volatile BYTE*) 0x900005))
#define CF_LBA0     (*((volatile BYTE*) 0x900007))
#define CF_LBA1     (*((volatile BYTE*) 0x900009))
#define CF_LBA2     (*((volatile BYTE*) 0x90000B))
#define CF_LBA3     (*((volatile BYTE*) 0x90000D))
#define CF_STATUS   (*((volatile BYTE*) 0x90000F))
#define CF_COMMAND  (*((volatile BYTE*) 0x90000F))

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


#endif // CF_C_INIT




//*******************************
//*******************************
//********** FUNCTIONS **********
//*******************************
//*******************************
#ifdef CF_C
//-----------------------------------
//----- INTERNAL ONLY FUNCTIONS -----
//-----------------------------------



//-----------------------------------------
//----- INTERNAL & EXTERNAL FUNCTIONS -----
//-----------------------------------------
//(Also defined below as extern)
void ffs_process (void);
void ffs_read_sector_to_buffer (DWORD sector_lba);
void ffs_write_sector_from_buffer (DWORD sector_lba);
WORD ffs_read_word (void);


#else
//------------------------------
//----- EXTERNAL FUNCTIONS -----
//------------------------------
extern void ffs_process (void);
extern void ffs_read_sector_to_buffer (DWORD sector_lba);
extern void ffs_write_sector_from_buffer (DWORD sector_lba);
extern WORD ffs_read_word (void);

#endif




//****************************
//****************************
//********** MEMORY **********
//****************************
//****************************
#ifdef CF_C
//--------------------------------------------
//----- INTERNAL ONLY MEMORY DEFINITIONS -----
//--------------------------------------------
BYTE sm_ffs_process = FFS_PROCESS_NO_CARD;
WORD file_system_information_sector;
BYTE ffs_no_of_heads;
BYTE ffs_no_of_sectors_per_track;
DWORD ffs_no_of_partition_sectors;






//--------------------------------------------------
//----- INTERNAL & EXTERNAL MEMORY DEFINITIONS -----
//--------------------------------------------------
//(Also defined below as extern)
WORD number_of_root_directory_sectors;				//Only used by FAT16, 0 for FAT32
BYTE ffs_buffer_needs_writing_to_card;
DWORD ffs_buffer_contains_lba = 0xffffffff;
DWORD fat1_start_sector;
DWORD root_directory_start_sector_cluster;			//Start sector for FAT16, start clustor for FAT32
DWORD data_area_start_sector;
BYTE disk_is_fat_32;
BYTE sectors_per_cluster;
DWORD last_found_free_cluster;
DWORD sectors_per_fat;
BYTE active_fat_table_flags;
DWORD read_write_directory_last_lba;
WORD read_write_directory_last_entry;


#else
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



#endif


