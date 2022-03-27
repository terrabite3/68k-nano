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
//CF CARD DRIVER C CODE FILE




#define CF_C
#include "mem-cf.h"

#include "mem-ffs.h"






//************************************************************
//************************************************************
//********** DO BACKGROUND COMPACT FLASH CARD TASKS **********
//************************************************************
//************************************************************
//This function needs to be called reguarly to detect a new card being inserted so that it can be initialised ready for access.
void ffs_process (void)
{
	BYTE head;
	WORD cylinder_no;
	BYTE sector;

	BYTE b_temp;
	WORD w_temp;
	DWORD dw_temp;
	DWORD lba;
	DWORD main_partition_start_sector;
	WORD number_of_reserved_sectors;
	BYTE number_of_copies_of_fat;
	BYTE *buffer_pointer;

	// There were a bunch of stages covered below in the original driver to handle card insertion and removal.
	// Since we're using an IDE to CF adapter, we don't have access to some of these pins,
	// so we have to assume that the card is present -- at least for now.
	// I'll reproduce the relevant stuff in a simpler form.

	// First time through
	if (sm_ffs_process == FFS_PROCESS_NO_CARD)
	{
		ffs_card_ok = 0;					//Flag that card not OK

		//Reset all file handlers
		for (b_temp = 0; b_temp < FFS_FOPEN_MAX; b_temp++)
			ffs_file[b_temp].flags.bits.file_is_open = 0;

	}


	// If we already did all the initialization stuff below, skip it
	if (sm_ffs_process == FFS_PROCESS_CARD_INITIALSIED)
	{
		return;
	}


	//---------------------------------------------------
	//----- READ CARD SETUP (CF Identify Drive Cmd) -----
	//---------------------------------------------------

    // Waiting for drive not busy
    while((CF_STATUS) & 0x80);

	//Write the 'Select Card/Head' register [LBA27:24 - bits3:0]
	CF_LBA3 = 0;

	//Send 'Identify Drive' instruction
	CF_COMMAND = 0xec;

    // Waiting for DRQ
    while(!(CF_STATUS & 0x08)); 

	//CHECK GENERAL CONFIGURATION WORD (0X848A = CF CARD)
	w_temp = ffs_read_word();

	if (w_temp != 0x848a)
	{
		goto init_new_ffs_card_fail;
	}
	//Dump the next 4 words
	ffs_read_word();
	ffs_read_word();
	ffs_read_word();
	ffs_read_word();
	
	//Get number of unformatted bytes per sector (word 5)
	//(Dumped as this doesn't necessarily match the value in the boot record - we use the boot record value)
	ffs_read_word();

	//Dump the next word
	ffs_read_word();

	//Get number of sectors on the card (= the first invalid LBA address) (words 7&8) - not currently useful as we don't format a card
	ffs_read_word();
	ffs_read_word();

	//Dump the next 12 words
	for (b_temp = 0; b_temp < 12; b_temp++)
		ffs_read_word();

	//Get the buffer size (x512 bytes (NOT WORDS)) (word 21)
	ffs_read_word();

	//Dump the next 33 words
	for (b_temp = 0; b_temp < 33; b_temp++)
		ffs_read_word();

	//Get the number of heads (word 55)
	w_temp = ffs_read_word();
	if (w_temp > 0xff)						//Can't be more than 255 heads
	{
		goto init_new_ffs_card_fail;
	}
	ffs_no_of_heads = (BYTE)w_temp;

	//Get the number of sectors per track (word 56)
	w_temp = ffs_read_word();
	if (w_temp > 0xff)						//Can't be more than 64 sectors per track
	{
		goto init_new_ffs_card_fail;
	}
	ffs_no_of_sectors_per_track = (BYTE)w_temp;

	//Set the number of bytes per sector before we move on to general access
	ffs_bytes_per_sector = 512;

	//---------------------------------------
	//----- READ THE MASTER BOOT RECORD -----
	//---------------------------------------
	// read sector 1 (LBA 0x00)
	CF_LBA3 = 0xe0;
	CF_LBA2 = 0x00;
	CF_LBA1 = 0x00;
	CF_LBA0 = 0x00;

	// Write the 'Sector Count' register (no of sectors to be transfered to complete the operation)
	CF_COUNT = 0x01;

    // Waiting for drive not busy
    while((CF_STATUS) & 0x80);

	//Read sector(s) command
	CF_COMMAND = 0x20;

    // Waiting for DRQ
    while(!(CF_STATUS & 0x08)); 

	//Read and dump the first 223 words (446 bytes of boot up executable code))
	for (b_temp = 0; b_temp < 223; b_temp++)
		ffs_read_word();

	//Now at start of the partition table [0x000001be]

	//Check for Partition 1 active (0x00 = inactive, 0x80 = active) [1]
	//(We allow a value of 0x00 for partition 1 as this partition must be present and on some disks a value of 0x00 has been found)
	// b_temp = ffs_read_byte();
	//if (b_temp != 0x80)
	//	goto init_new_ffs_card_fail

	//Get 'Beginning of Partition - Head' [0x000001bf]
	// head = ffs_read_byte();
	w_temp = ffs_read_word();
	// TODO: Is this the correct byte? We're expecting 0, not 0x80
	head = w_temp >> 8;


	//Get 'Beginning of Partition - Cylinder + Sector' [0x000001c0]
	w_temp = ffs_read_word();

	cylinder_no = (w_temp >> 8);				//Yes this is correct - strange bit layout in this register!
	if (w_temp & 0x0040)
		cylinder_no += 0x0100;
	if (w_temp & 0x0080)
		cylinder_no += 0x0200;

	sector = (BYTE)(w_temp & 0x003f);

	//----- GET START ADDRESS OF PARTITION 1 -----
	//(Sectors per track x no of heads)
	w_temp = ((WORD)ffs_no_of_sectors_per_track * (WORD)ffs_no_of_heads);

	//Result x cylinder value just read
	main_partition_start_sector = ((DWORD)w_temp * (DWORD)cylinder_no);

	//Add (sectors per track x head value just read)
	w_temp = ((WORD)ffs_no_of_sectors_per_track * (WORD)head);
	main_partition_start_sector += (DWORD)w_temp;
	
	//Add sector value -1 (as sectors are numbered 1-#)
	main_partition_start_sector += (DWORD)(sector - 1);

	//WE NOW HAVE THE START ADDRESS OF THE FIRST PARTITION (THE ONLY PARTITION WE LOOK AT)


	//Read the 'Type Of Partition' [0x000001c2]
	//(We accept FAT16 or FAT32)
	// Replaced two read_byte()s with a single read_word(), and grab the byte we care about
	w_temp = ffs_read_word();
	b_temp = w_temp & 0xFF;

	if (b_temp == 0x04)						//FAT16 (smaller than 32MB)
		disk_is_fat_32 = 0;
	else if (b_temp == 0x06)				//FAT16 (larger than 32MB)
		disk_is_fat_32 = 0;
	else if (b_temp == 0x0b)				//FAT32 (Partition Up to 2048GB)
		disk_is_fat_32 = 1;
	else if (b_temp == 0x0c)				//FAT32 (Partition Up to 2048GB - uses 13h extensions)
		disk_is_fat_32 = 1;
	else if (b_temp == 0x0e)				//FAT16 (partition larger than 32MB, uses 13h extensions)
		disk_is_fat_32 = 0;
	else
	{
		goto init_new_ffs_card_fail;
	}

	//Get end of partition - head [0x000001c3]
	// We already read this byte as part of the last word

	//Get end of partition - Cylinder & Sector [0x000001c4]
	ffs_read_word();

	//Get no of sectors between MBR and the first sector in the partition [0x000001c6]
	ffs_read_word();
	ffs_read_word();

	//Get no of sectors in the partition - could be useful when we do writing of files [0x000001ca]
	ffs_no_of_partition_sectors = (DWORD)(ffs_read_word());
	ffs_no_of_partition_sectors += (((DWORD)ffs_read_word() << 16));



	//------------------------------------------
	//---- READ THE PARTITION 1 BOOT RECORD ----
	//------------------------------------------
	//Setup for finding the FAT1 table start address root directory start address and data area start address
	lba = main_partition_start_sector;

	ffs_read_sector_to_buffer(lba);
	buffer_pointer = &FFS_DRIVER_GEN_512_BYTE_BUFFER[0];

	//Dump jump code & OEM name (11 bytes)
	buffer_pointer += 11;

	//Get 'Bytes Per Sector' [# + 0x000b]
	//Check value matches value read from 'Identify Drive' - Changed - use this value as ID value can be different (it = no of Unfotmatted bytes per sector)
	//Value is usually 512, but can be 256 on some older CF cards.  Ensure <= 512 (this is the size of the sector read buffer we provide and it should not be bigger than this)
	ffs_bytes_per_sector = (WORD)*buffer_pointer++;
	ffs_bytes_per_sector |= (WORD)(*buffer_pointer++) << 8;
	if (ffs_bytes_per_sector > 512)
	{
		goto init_new_ffs_card_fail;
	}

	//Get 'Sectors Per Cluster' [# + 0x000d]
	//(Restricted to powers of 2 (1, 2, 4, 8, 16, 32�))
	sectors_per_cluster = *buffer_pointer++;
	
	b_temp = 0;											//Check its power of 2 (other functions rely on this check)
	for (w_temp = 0x01; w_temp < 0x0100; w_temp <<= 1)
	{
		if (sectors_per_cluster & (BYTE)w_temp)
			b_temp++;
	}
	if (b_temp != 1)
	{
		goto init_new_ffs_card_fail;
	}


	//Get '# of reserved sectors' [# + 0x000e]
	//Adjust the start addresses acordingly
	number_of_reserved_sectors = (WORD)*buffer_pointer++;
	number_of_reserved_sectors |= (WORD)(*buffer_pointer++) << 8;

	//Get 'no of copies of FAT' [# + 0x0010]
	//(any number >= 1 is permitted, but no value other than 2 is recomended)
	number_of_copies_of_fat = *buffer_pointer++;
	if ((number_of_copies_of_fat > 4) || (number_of_copies_of_fat == 0))		//We set a limit on there being a maximum of 4 copies of fat
	{
		goto init_new_ffs_card_fail;
	}

	//Get 'max root directory entries' [# + 0x0011]
	//(Used by FAT16, but not for FAT32)
	dw_temp = (DWORD)*buffer_pointer++;
	dw_temp |= (DWORD)(*buffer_pointer++) << 8;
	number_of_root_directory_sectors = ((dw_temp * 32) + (DWORD)(ffs_bytes_per_sector - 1)) / ffs_bytes_per_sector;		//Multiply no of entries by 32 (no of bytes per entry)
																														//This calculation rounds up
	//Get 'number of sectors in partition < 32MB' [# + 0x0013]
	//(Dump)
	buffer_pointer++;
	buffer_pointer++;

	//Get 'media descriptor' [# + 0x0015]
	//(Should be 0xF8 for hard disk)
	if (*buffer_pointer++ != 0xf8)
	{
		goto init_new_ffs_card_fail;
	}

	//Get 'sectors per fat'  [# + 0x0016]
	//(Used by FAT16, but not for FAT32 - for FAT32 the value is a double word and located later on in this table - variable will be overwritten)
	sectors_per_fat = (DWORD)*buffer_pointer++;
	sectors_per_fat |= (DWORD)(*buffer_pointer++) << 8;


	if(disk_is_fat_32 == 0)
	{
		//---------------------------------------------------------------------------------------
		//----- PARTITION IS FAT 16 - COMPLETE BOOT RECORD & INITAILISATION FOR THIS SYSTEM -----
		//---------------------------------------------------------------------------------------

		//CALCULATE THE PARTITION AREAS START ADDRESSES
		fat1_start_sector = main_partition_start_sector + (DWORD)number_of_reserved_sectors;
		root_directory_start_sector_cluster = main_partition_start_sector + (DWORD)number_of_reserved_sectors + (sectors_per_fat * number_of_copies_of_fat);
		data_area_start_sector = main_partition_start_sector + (DWORD)number_of_reserved_sectors + (sectors_per_fat * number_of_copies_of_fat) + number_of_root_directory_sectors;

		//SET THE ACTIVE FAT TABLE FLAGS
		active_fat_table_flags = 0;						// #|#|#|#|USE_FAT_TABLE_3|USE_FAT_TABLE_2|USE_FAT_TABLE_1|USE_FAT_TABLE_0
		for (b_temp = 0; b_temp < number_of_copies_of_fat; b_temp++)
		{
			active_fat_table_flags <<= 1;
			active_fat_table_flags++;
		}
		
		//SET UNUSED REGISTERS (used for FAT32)
		file_system_information_sector = 0xffff;

	}
	else
	{
		//---------------------------------------------------------------------------------------
		//----- PARTITION IS FAT 32 - COMPLETE BOOT RECORD & INITAILISATION FOR THIS SYSTEM -----
		//---------------------------------------------------------------------------------------

		//Dump sectors per track, # of heads, # of hidden sectors in partition (12 bytes)
		buffer_pointer += 12;

		//Get 'sectors per fat'  [# + 0x0024]
		sectors_per_fat = (DWORD)*buffer_pointer++;
		sectors_per_fat |= (DWORD)(*buffer_pointer++) << 8;
		sectors_per_fat |= (DWORD)(*buffer_pointer++) << 16;
		sectors_per_fat |= (DWORD)(*buffer_pointer++) << 24;

		//Get 'Flags' [# + 0x0028]
		//(Bits 0-4 Indicate Active FAT Copy)
		//(Bit 7 Indicates whether FAT Mirroring is Enabled or Disabled <Clear is Enabled>) (If FAT Mirroringis Disabled, the FAT Information is
		//only written to the copy indicated by bits 0-4)
		w_temp = (DWORD)*buffer_pointer++;
		w_temp |= (DWORD)(*buffer_pointer++) << 8;
		if (w_temp & 0x0080)
		{
			//BIT7 = 1, FAT MIRRORING IS DISABLED
			//Bits 3:0 set which FAT table is active
			if ((w_temp & 0x000f) > number_of_copies_of_fat)
			{
				goto init_new_ffs_card_fail;
			}
			
			switch (w_temp & 0x000f)
			{
			case 0:
				active_fat_table_flags = 0x01;						// #|#|#|#|USE_FAT_TABLE_3|USE_FAT_TABLE_2|USE_FAT_TABLE_1|USE_FAT_TABLE_0
				break;
			case 1:
				active_fat_table_flags = 0x02;
				break;
			case 2:
				active_fat_table_flags = 0x04;
				break;
			case 3:
				active_fat_table_flags = 0x08;
				break;
			}
		}
		else
		{
			//BIT7 = 0, FAT MIRRORING IS ENABLED INTO ALL FATS
			active_fat_table_flags = 0;						// #|#|#|#|USE_FAT_TABLE_3|USE_FAT_TABLE_2|USE_FAT_TABLE_1|USE_FAT_TABLE_0
			for (b_temp = 0; b_temp < number_of_copies_of_fat; b_temp++)
			{
				active_fat_table_flags <<= 1;
				active_fat_table_flags++;
			}
		}

		//Get 'Version of FAT32 Drive' [# + 0x002A]
		//(High Byte = Major Version, Low Byte = Minor Version)
		buffer_pointer++;
		buffer_pointer++;

		//Get 'Cluster Number of the Start of the Root Directory' [# + 0x2C]
		//(Usually 2, but not requried to be 2)
		root_directory_start_sector_cluster = (DWORD)*buffer_pointer++;
		root_directory_start_sector_cluster |= (DWORD)(*buffer_pointer++) << 8;
		root_directory_start_sector_cluster |= (DWORD)(*buffer_pointer++) << 16;
		root_directory_start_sector_cluster |= (DWORD)(*buffer_pointer++) << 24;

		//Get 'Sector Number of the File System Information Sector' [# + 0x0030]
		//(Referenced from the Start of the Partition. Usually 1, but not requried to be 1)
		file_system_information_sector = (DWORD)*buffer_pointer++;
		file_system_information_sector |= (DWORD)(*buffer_pointer++) << 8;


		//CALCULATE THE PARTITION AREAS START ADDRESSES
		fat1_start_sector = main_partition_start_sector + (DWORD)number_of_reserved_sectors;			//THE FAT START ADDRESS IS NOW GOOD (has correct offset)
		//root_directory_start_sector_cluster already done above
		data_area_start_sector = main_partition_start_sector + (DWORD)number_of_reserved_sectors + (sectors_per_fat * number_of_copies_of_fat);

		//SET UNUSED REGISTERS (used for FAT16)
		number_of_root_directory_sectors = 0;
	}

	//------------------------------------------------------------------------
	//----- BOOT RECORD IS DONE - ALL REQUIRED DISK PARAMETERS ARE KNOWN -----
	//------------------------------------------------------------------------


	//-----------------------------
	//----- CARD IS OK TO USE -----
	//-----------------------------
	ffs_card_ok = 1;				//Flag that the card is OK

	//Do CF Driver specific initialisations
	last_found_free_cluster = 0;		//When we next look for a free cluster, start from the beginning

	// Mark the card as initialized so we don't repeat this next time
	sm_ffs_process = FFS_PROCESS_CARD_INITIALSIED;

	return;


//----------------------------------
//----- CARD IS NOT COMPATIBLE -----
//----------------------------------
init_new_ffs_card_fail:
	ffs_card_ok = 0;				//Flag that the card is not OK
	return;
}









//*********************************************************************************************
//*********************************************************************************************
//*********************************************************************************************
//*********************************************************************************************
//****************************** DRIVER SUB FUNCTIONS BELOW HERE ******************************
//*********************************************************************************************
//*********************************************************************************************
//*********************************************************************************************
//*********************************************************************************************




//*******************************************
//*******************************************
//********** READ SECTOR TO BUFFER **********
//*******************************************
//*******************************************
//lba = start sector address
//The card must be deselected after all of the data has been read using:
//	FFS_CE = 1;					//Deselect the card
void ffs_read_sector_to_buffer (DWORD sector_lba)
{
	WORD count;
	// The original driver used a BYTE buffer, but we're going to read it by WORD,
	// so may as well treat it like a WORD buffer.
	WORD *buffer_pointer;


	//----- IF LBA MATCHES THE LAST LBA DON'T BOTHER RE-READING AS THE DATA IS STILL IN THE BUFFER -----
	if (ffs_buffer_contains_lba == sector_lba)
	{
		return;
	}


	//----- IF THE BUFFER CONTAINS DATA THAT IS WAITING TO BE WRITTEN THEN WRITE IT FIRST -----
	if (ffs_buffer_needs_writing_to_card)
	{
		if (ffs_buffer_contains_lba != 0xffffffff)			//This should not be possible but check is made just in case!
			ffs_write_sector_from_buffer(ffs_buffer_contains_lba);

		ffs_buffer_needs_writing_to_card = 0;
	}



	//----- NEW LBA TO BE LOADED -----
	CF_LBA3 = (BYTE)(0b11100000 | (sector_lba >> 24));
	CF_LBA2 = (BYTE)((sector_lba & 0x00ff0000) >> 16);
	CF_LBA1 = (BYTE)((sector_lba & 0x0000ff00) >> 8);
	CF_LBA0 = (BYTE)(sector_lba & 0x000000ff);

	//0x02 - Write the 'Sector Count' register (no of sectors to be transfered to complete the operation)
	CF_COUNT = 1;


    // Waiting for drive not busy
    while((CF_STATUS) & 0x80);

	//0x07 - Write the 'Command' register
	CF_COMMAND = 0x20;

    // Waiting for DRQ
    while(!(CF_STATUS & 0x08)); 

	ffs_buffer_contains_lba = 0xffffffff;			//Flag that buffer does not currently contain any lba


	//----- READ THE SECTOR INTO THE BUFFER -----
	buffer_pointer = (WORD*) &FFS_DRIVER_GEN_512_BYTE_BUFFER[0];

	for (count = 0; count < ffs_bytes_per_sector; count += 2)
	{
		// Swap the bytes because FAT is little-endian but M68k is big-endian
		WORD temp = CF_DATA;
		temp = (temp << 8) | (temp >> 8);
		*buffer_pointer++ = temp;
	}

	ffs_buffer_contains_lba = sector_lba;				//Flag that the data buffer currently contains data for this LBA (logged to avoid re-loading the buffer again if its not necessary)

}







//**********************************************
//**********************************************
//********** WRITE SECTOR FROM BUFFER **********
//**********************************************
//**********************************************
void ffs_write_sector_from_buffer (DWORD sector_lba)
{
	WORD count;
	WORD *buffer_pointer;
	
	buffer_pointer = (WORD*) &FFS_DRIVER_GEN_512_BYTE_BUFFER[0];

	ffs_buffer_needs_writing_to_card = 0;			//Flag that buffer is no longer waiting to write to card (must be at top as this function
													//calls other functions that check this flag and would call the function back)

	
	//----- SETUP TO WRITE THE SECTOR -----

	CF_LBA3 = (BYTE)(0b11100000 | (sector_lba >> 24));
	CF_LBA2 = (BYTE)((sector_lba & 0x00ff0000) >> 16);
	CF_LBA1 = (BYTE)((sector_lba & 0x0000ff00) >> 8);
	CF_LBA0 = (BYTE)(sector_lba & 0x000000ff);

	//0x02 - Write the 'Sector Count' register (no of sectors to be transfered to complete the operation)
	CF_COUNT = 1;


    // Waiting for drive not busy
    while((CF_STATUS) & 0x80);

	//0x07 - Write the 'Command' register
	CF_COMMAND = 0x30;

    // Waiting for DRQ
    while(!(CF_STATUS & 0x08)); 

	//----- WRITE THE BUFFER TO THE CARD SECTOR -----
	for (count = 0; count < ffs_bytes_per_sector; count += 2)
	{
		WORD temp = *buffer_pointer++;
		temp = (temp >> 8) | (temp << 8);
		CF_DATA = temp;
	}
	

}




//*****************************************
//*****************************************
//********** READ WORD FROM CARD **********
//*****************************************
//*****************************************
WORD ffs_read_word (void)
{
	WORD data;
	data = CF_DATA;
	return (data);
}
