/** @file
  MMC/SD Card driver for OMAP 35xx (SDIO not supported)

  This driver always produces a BlockIo protocol but it starts off with no Media
  present. A TimerCallBack detects when media is inserted or removed and after
  a media change event a call to BlockIo ReadBlocks/WriteBlocks will cause the
  media to be detected (or removed) and the BlockIo Media structure will get
  updated. No MMC/SD Card harward registers are updated until the first BlockIo
  ReadBlocks/WriteBlocks after media has been insterted (booting with a card
  plugged in counts as an insertion event).

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  **/

#include "MMCHS.h"

EFI_BLOCK_IO_MEDIA gMMCHSMedia = 
{
	SIGNATURE_32('e', 'm', 'm', 'c'),         // MediaId
	FALSE,                                    // RemovableMedia
	TRUE,                                     // MediaPresent
	FALSE,                                    // LogicalPartition
	FALSE,                                    // ReadOnly
	FALSE,                                    // WriteCaching
	512,                                      // BlockSize
	4,                                        // IoAlign
	0,                                        // Pad
	0                                         // LastBlock
};

typedef struct
{
	VENDOR_DEVICE_PATH  Mmc;
	EFI_DEVICE_PATH     End;
} MMCHS_DEVICE_PATH;

MMCHS_DEVICE_PATH gMmcHsDevicePath =
{
	{
		HARDWARE_DEVICE_PATH,
		HW_VENDOR_DP,
		(UINT8)(sizeof(VENDOR_DEVICE_PATH)),
		(UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8),
		0xb615f1f5, 0x5088, 0x43cd, 0x80, 0x9c, 0xa1, 0x6e, 0x52, 0x48, 0x7d, 0x00
	},
	{
		END_DEVICE_PATH_TYPE,
		END_ENTIRE_DEVICE_PATH_SUBTYPE,
		{ 
			sizeof(EFI_DEVICE_PATH_PROTOCOL), 
			0 
		}
	}
};

CARD_INFO                  gCardInfo;


EFI_STATUS
SdReadWrite(
	IN EFI_BLOCK_IO_PROTOCOL    *This,
	IN  UINTN                   Lba,
	OUT VOID                    *Buffer,
	IN  UINTN                   BufferSize,
	IN  OPERATION_TYPE          OperationType
)
{
	EFI_STATUS Status = EFI_SUCCESS;

	EFI_TPL    OldTpl;

	OldTpl = gBS->RaiseTPL(TPL_NOTIFY);

DoneRestoreTPL:

	gBS->RestoreTPL(OldTpl);

Done:

	return Status;

}


/**

  Reset the Block Device.

  @param  This                 Indicates a pointer to the calling context.
  @param  ExtendedVerification Driver may perform diagnostics on reset.
  @retval EFI_SUCCESS          The device was reset.
  @retval EFI_DEVICE_ERROR     The device is not functioning properly and could
  not be reset.
  **/
EFI_STATUS
EFIAPI
MMCHSReset(
	IN EFI_BLOCK_IO_PROTOCOL          *This,
	IN BOOLEAN                        ExtendedVerification
)
{
	return EFI_SUCCESS;
}


/**

  Read BufferSize bytes from Lba into Buffer.
  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    Id of the media, changes every time the media is replaced.
  @param  Lba        The starting Logical Block Address to read from
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the destination buffer for the data. The caller is
  responsible for either having implicit or explicit ownership of the buffer.


  @retval EFI_SUCCESS           The data was read correctly from the device.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the read.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHANGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The read request contains LBAs that are not valid,

  or the buffer is not on proper alignment.

  EFI_STATUS

  **/
EFI_STATUS
EFIAPI
MMCHSReadBlocks(
	IN EFI_BLOCK_IO_PROTOCOL          *This,
	IN UINT32                         MediaId,
	IN EFI_LBA                        Lba,
	IN UINTN                          BufferSize,
	OUT VOID                          *Buffer
)
{
	EFI_STATUS Status;

	//Perform Read operation.
	Status = EFI_SUCCESS;

	return Status;

}


/**

  Write BufferSize bytes from Lba into Buffer.
  @param  This       Indicates a pointer to the calling context.
  @param  MediaId    The media ID that the write request is for.
  @param  Lba        The starting logical block address to be written. The caller is
  responsible for writing to only legitimate locations.
  @param  BufferSize Size of Buffer, must be a multiple of device block size.
  @param  Buffer     A pointer to the source buffer for the data.

  @retval EFI_SUCCESS           The data was written correctly to the device.
  @retval EFI_WRITE_PROTECTED   The device can not be written to.
  @retval EFI_DEVICE_ERROR      The device reported an error while performing the write.
  @retval EFI_NO_MEDIA          There is no media in the device.
  @retval EFI_MEDIA_CHNAGED     The MediaId does not matched the current device.
  @retval EFI_BAD_BUFFER_SIZE   The Buffer was not a multiple of the block size of the device.
  @retval EFI_INVALID_PARAMETER The write request contains LBAs that are not valid,
  or the buffer is not on proper alignment.

  **/
EFI_STATUS
EFIAPI
MMCHSWriteBlocks(
	IN EFI_BLOCK_IO_PROTOCOL          *This,
	IN UINT32                         MediaId,
	IN EFI_LBA                        Lba,
	IN UINTN                          BufferSize,
	IN VOID                           *Buffer
)
{
	EFI_STATUS  Status;
	Status = EFI_SUCCESS;
	return Status;

}


/**

  Flush the Block Device.
  @param  This              Indicates a pointer to the calling context.
  @retval EFI_SUCCESS       All outstanding data was written to the device
  @retval EFI_DEVICE_ERROR  The device reported an error while writting back the data
  @retval EFI_NO_MEDIA      There is no media in the device.

  **/
EFI_STATUS
EFIAPI
MMCHSFlushBlocks(
	IN EFI_BLOCK_IO_PROTOCOL  *This
)
{
	return EFI_SUCCESS;
}


EFI_BLOCK_IO_PROTOCOL gBlockIo = {
	EFI_BLOCK_IO_INTERFACE_REVISION,   // Revision
	&gMMCHSMedia,                      // *Media
	MMCHSReset,                        // Reset
	MMCHSReadBlocks,                   // ReadBlocks
	MMCHSWriteBlocks,                  // WriteBlocks
	MMCHSFlushBlocks                   // FlushBlocks
};

static unsigned mmc_sdc_base[] =
{ 
	MSM_SDC1_BASE, 
	MSM_SDC2_BASE, 
	MSM_SDC3_BASE, 
	MSM_SDC4_BASE 
};

EFI_STATUS
EFIAPI
MMCHSInitialize(
	IN EFI_HANDLE         ImageHandle,
	IN EFI_SYSTEM_TABLE   *SystemTable
)
{
	EFI_STATUS  Status;

	ZeroMem(&gCardInfo, sizeof(CARD_INFO));

	msm_clocks_init();

	unsigned base_addr;
	unsigned char slot;
	UINT32 blocksize;

	/* Trying Slot 1 first */
	slot = 1;
	base_addr = mmc_sdc_base[slot - 1];
	if (mmc_boot_main(slot, base_addr))
	{

		DEBUG((EFI_D_ERROR, "MMCHSInitialize eMMC slot 1 init failed!\n"));
		/* Trying Slot 3 next */
		slot = 3;
		base_addr = mmc_sdc_base[slot - 1];
		if (mmc_boot_main(slot, base_addr))
		{
			DEBUG((EFI_D_ERROR, "MMCHSInitialize eMMC slot 3 init failed!\n"));
			return EFI_DEVICE_ERROR;
		}
		else
		{
			DEBUG((EFI_D_ERROR, "MMCHSInitialize eMMC slot 3 init ok!\n"));
		}
	}
	else
	{
		DEBUG((EFI_D_ERROR, "MMCHSInitialize eMMC slot 1 init ok!\n"));
	}

	blocksize = mmc_get_device_blocksize();

	DEBUG((EFI_D_INFO, "eMMC Block Size:%d\n", blocksize));

	VOID * data;


	Status = gBS->AllocatePool(EfiBootServicesData, (blocksize), &data);

	if (EFI_ERROR(Status)) {
		DEBUG((EFI_D_INFO, "test memory alloc failed!\n"));
		return Status;
	}

	int ret = 0;
	ret = mmc_read(blocksize, (UINT32 *)data, blocksize);

	if (ret != 0)
	{
		DEBUG((EFI_D_INFO, "mmc_read failed! ret = %d\n", ret));
		return EFI_DEVICE_ERROR;
	}

	UINT8 * STR = (UINT8 *)data;

	DEBUG((EFI_D_INFO, "First 8 Bytes = %c%c%c%c%c%c%c%c\n", STR[0], STR[1], STR[2], STR[3], STR[4], STR[5], STR[6], STR[7]));



	Status = gBS->FreePool(data);


	//Publish BlockIO.
	Status = gBS->InstallMultipleProtocolInterfaces(
		&ImageHandle,
		&gEfiBlockIoProtocolGuid, &gBlockIo,
		&gEfiDevicePathProtocolGuid, &gMmcHsDevicePath,
		NULL
		);
	return Status;
}
