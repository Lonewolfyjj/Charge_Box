/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */

#include "hl_drv_flash.h"
#include "stdio.h"

/* Definitions of physical drive number for each drive */
#define DEV_FLASH	0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;

	switch (pdrv) {
	case DEV_FLASH :
		//result = RAM_disk_status();

		return stat;

	case DEV_MMC :
		//result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		//result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = RES_OK;

	switch (pdrv) {
	case DEV_FLASH :
		//result = RAM_disk_initialize();
		// translate the reslut code here
		if (hl_drv_flash_init() != FLASH_RET_OK) {
			printf("[error] disk flash already init !\r\n");
			return stat;
    	}

		return stat;

	case DEV_MMC :
		//result = MMC_disk_initialize();

		// translate the reslut code here

		return stat;

	case DEV_USB :
		//result = USB_disk_initialize();

		// translate the reslut code here

		return stat;
	}
	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	DRESULT res = RES_OK;

	switch (pdrv) {
	case DEV_FLASH :
		// translate the arguments here
		//result = RAM_disk_read(buff, sector, count);
		// translate the reslut code here
		sector++;
		
		for(;count > 0;count--)
		{										    
			hl_drv_flash_read(sector * 4096, (uint8_t *)buff, 4096);
			sector++;
			buff += 4096;
		}
		return res;

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	DRESULT res = RES_OK;

	switch (pdrv) {
	case DEV_FLASH :
		// translate the arguments here
		//result = RAM_disk_write(buff, sector, count);
		// translate the reslut code here
		sector++;

		
		for(;count > 0;count--)
		{										    
			hl_drv_flash_write(sector * 4096, (uint8_t *)buff, 4096);
			sector++;
			buff += 4096;
		}

		return res;

	case DEV_MMC :
		// translate the arguments here

		//result = MMC_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;

	case DEV_USB :
		// translate the arguments here

		//result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

		return res;
	}

	return RES_PARERR;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res = RES_OK;

	switch (pdrv) {
	case DEV_FLASH :

		// Process of the command for the RAM drive
		switch (cmd) {
			// /* 扇区数量：3584*4096/1024/1024=14(MB) */
			// case GET_SECTOR_COUNT: // FLASH_SECTOR_SIZE - 512 
			// *(DWORD * )buff = 3584; // 这个值来源于前面512扇区给了文件系统表，后面的才是可以使用的空间  w25q128 4096 扇区 - 512 扇区 =	3584
			// break;
			// /* 扇区大小  */
			// case GET_SECTOR_SIZE :
			// *(WORD * )buff = 4096; //FLASH_SECTOR_SIZE
			// break;
			// /* 同时擦除扇区个数 */
			// case GET_BLOCK_SIZE :
			// *(DWORD * )buff = 1;
			// break;  
			/* 扇区数量：3584*4096/1024/1024=14(MB) */
			case GET_SECTOR_COUNT: // FLASH_SECTOR_SIZE - 512 
			*(DWORD * )buff = 1023;
			break;
			/* 扇区大小  */
			case GET_SECTOR_SIZE :
			*(WORD * )buff = 4096; //FLASH_SECTOR_SIZE
			break;
			/* 同时擦除扇区个数 */
			case GET_BLOCK_SIZE :
			*(DWORD * )buff = 1;
			break;       
		}
		return res;

	case DEV_MMC :

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB :

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}

DWORD get_fattime (void)
{
    return (DWORD)(2022 - 80) << 25 |
           (DWORD)(10 + 1) << 21 |
           (DWORD)9 << 16 |
           (DWORD)20 << 11 |
           (DWORD)20 << 5 |
           (DWORD)0 >> 1;
}