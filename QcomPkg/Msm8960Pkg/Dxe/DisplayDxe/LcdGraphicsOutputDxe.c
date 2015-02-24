/** @file

 Copyright (c) 2011-2014, ARM Ltd. All rights reserved.<BR>
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/

#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Guid/GlobalVariable.h>

#include "LcdGraphicsOutputDxe.h"


LCD_INSTANCE mLcdTemplate = 
{
	LCD_INSTANCE_SIGNATURE,
	NULL, // Handle
	{ // ModeInfo
		0, // Version
		0, // HorizontalResolution
		0, // VerticalResolution
		PixelBltOnly, // PixelFormat
		{ 0 }, // PixelInformation
		0, // PixelsPerScanLine
	},
	{
		0, // MaxMode;
		0, // Mode;
		NULL, // Info;
		0, // SizeOfInfo;
		0, // FrameBufferBase;
		0 // FrameBufferSize;
	},
	{ // Gop
		LcdGraphicsQueryMode,  // QueryMode
		LcdGraphicsSetMode,    // SetMode
		LcdGraphicsBlt,        // Blt
		NULL                     // *Mode
	},
	{ // DevicePath
		{
			{
				HARDWARE_DEVICE_PATH, HW_VENDOR_DP,
				{ (UINT8)(sizeof(VENDOR_DEVICE_PATH)), (UINT8)((sizeof(VENDOR_DEVICE_PATH)) >> 8) },
			},
			// Hardware Device Path for Lcd
			EFI_CALLER_ID_GUID // Use the driver's GUID
		},

		{
			END_DEVICE_PATH_TYPE,
			END_ENTIRE_DEVICE_PATH_SUBTYPE,
			{ sizeof(EFI_DEVICE_PATH_PROTOCOL), 0 }
		}
	},
	(EFI_EVENT)NULL, // ExitBootServicesEvent
	NULL//PM8921
};

EFI_STATUS
LcdInstanceContructor(
	OUT LCD_INSTANCE** NewInstance
)
{
	EFI_STATUS  Status = EFI_SUCCESS;
	LCD_INSTANCE* Instance;
	DEBUG((EFI_D_ERROR, "DisplayDxe:LcdInstanceContructor!\n"));

	Instance = AllocateCopyPool(sizeof(LCD_INSTANCE), &mLcdTemplate);
	if (Instance == NULL)
	{
		return EFI_OUT_OF_RESOURCES;
	}

	Instance->Gop.Mode = &Instance->Mode;
	Instance->Gop.Mode->MaxMode = LcdPlatformGetMaxMode();
	Instance->Mode.Info = &Instance->ModeInfo;
	
	Status = gBS->LocateProtocol (&gQcomPM8921ProtocolGuid, NULL, (VOID **)&(Instance->PM8921));
	if (EFI_ERROR(Status))
	{
		DEBUG((DEBUG_ERROR, "LocateProtocol: gQcomPM8921ProtocolGuid Error. Exit Status=%r\n", Status));
		return Status;
	}

	*NewInstance = Instance;
	
	

	return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
LcdGraphicsOutputDxeInitialize(
	IN EFI_HANDLE         ImageHandle,
	IN EFI_SYSTEM_TABLE   *SystemTable
)
{
	EFI_STATUS  Status = EFI_SUCCESS;
	LCD_INSTANCE* Instance;
	DEBUG((EFI_D_ERROR, "DisplayDxe:LcdGraphicsOutputDxeInitialize!\n"));
	Status = LcdInstanceContructor(&Instance);
	if (EFI_ERROR(Status))
	{
		DEBUG((DEBUG_ERROR, "LcdInstanceContructor: Error. Exit Status=%r\n", Status));
		goto EXIT;
	}

	// Install the Graphics Output Protocol and the Device Path
	Status = gBS->InstallMultipleProtocolInterfaces(
		&Instance->Handle,
		&gEfiGraphicsOutputProtocolGuid, &Instance->Gop,
		&gEfiDevicePathProtocolGuid, &Instance->DevicePath,
		NULL
		);

	if (EFI_ERROR(Status))
	{
		DEBUG((DEBUG_ERROR, "GraphicsOutputDxeInitialize: Can not install the protocol. Exit Status=%r\n", Status));
		goto EXIT;
	}

	// Register for an ExitBootServicesEvent
	// When ExitBootServices starts, this function here will make sure that the graphics driver will shut down properly,
	// i.e. it will free up all allocated memory and perform any necessary hardware re-configuration.
	Status = gBS->CreateEvent(
		EVT_SIGNAL_EXIT_BOOT_SERVICES,
		TPL_NOTIFY,
		LcdGraphicsExitBootServicesEvent, NULL,
		&Instance->ExitBootServicesEvent
		);

	if (EFI_ERROR(Status))
	{
		DEBUG((DEBUG_ERROR, "GraphicsOutputDxeInitialize: Can not install the ExitBootServicesEvent handler. Exit Status=%r\n", Status));
		goto EXIT_ERROR_UNINSTALL_PROTOCOL;
	}

	// To get here, everything must be fine, so just exit
	goto EXIT;

EXIT_ERROR_UNINSTALL_PROTOCOL:
	/* The following function could return an error message,
	 * however, to get here something must have gone wrong already,
	 * so preserve the original error, i.e. don't change
	 * the Status variable, even it fails to uninstall the protocol.
	 */
	gBS->UninstallMultipleProtocolInterfaces(
		Instance->Handle,
		&gEfiGraphicsOutputProtocolGuid, &Instance->Gop, // Uninstall Graphics Output protocol
		&gEfiDevicePathProtocolGuid, &Instance->DevicePath,     // Uninstall device path
		NULL
		);

EXIT:
	return Status;

}

/***************************************
 * This function should be called
 * on Event: ExitBootServices
 * to free up memory, stop the driver
 * and uninstall the protocols
 ***************************************/
VOID
LcdGraphicsExitBootServicesEvent(
	IN EFI_EVENT  Event,
	IN VOID       *Context
)
{
	DEBUG((EFI_D_ERROR, "DisplayDxe:LcdGraphicsExitBootServicesEvent!\n"));
	// By default, this PCD is FALSE. But if a platform starts a predefined OS that
	// does not use a framebuffer then we might want to disable the display controller
	// to avoid to display corrupted information on the screen.
	if (FeaturePcdGet(PcdGopDisableOnExitBootServices))
	{
		// Turn-off the Display controller
		LcdShutdown();
	}
}






