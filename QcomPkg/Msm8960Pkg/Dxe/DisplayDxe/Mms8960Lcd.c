/** @file  Lcd.c

  Copyright (c) 2011-2012, ARM Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

  **/

#include <PiDxe.h>

#include <Library/ArmPlatformSysConfigLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/LcdPlatformLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/LcdPlatformLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

#include <Protocol/Cpu.h>
#include <Protocol/EdidDiscovered.h>
#include <Protocol/EdidActive.h>

#include "LcdGraphicsOutputDxe.h"







typedef struct {
  UINT32                     Mode;
  UINT32                     HorizontalResolution;
  UINT32                     VerticalResolution;
  LCD_BPP                    Bpp;
} LCD_RESOLUTION;

#define WXGA 9

//
// WXGA Mode: 720 x 1280
//
#define WXGA_H_RES_PIXELS                 720
#define WXGA_V_RES_PIXELS                 1280

LCD_RESOLUTION mResolutions[] = {
  { 
      // Mode 0 : WXGA : 720 x 1280 x 24 bpp
      WXGA, 
      WXGA_H_RES_PIXELS, 
      WXGA_V_RES_PIXELS, 
      LCD_BITS_PER_PIXEL_24
  }
};

EFI_EDID_DISCOVERED_PROTOCOL  mEdidDiscovered = {
  0,
  NULL
};

EFI_EDID_ACTIVE_PROTOCOL      mEdidActive = {
  0,
  NULL
};


EFI_STATUS
LcdPlatformGetVram (
  OUT EFI_PHYSICAL_ADDRESS*  VramBaseAddress,
  OUT UINTN*                 VramSize
  )
{
  EFI_STATUS              Status;
  EFI_CPU_ARCH_PROTOCOL  *Cpu;

  Status = EFI_SUCCESS;
  *VramBaseAddress = (EFI_PHYSICAL_ADDRESS) 0x89000000;
  *VramSize = (UINTN)(EFI_SIZE_TO_PAGES(720*1280*3)*EFI_PAGE_SIZE);
  // Is it on the motherboard or on the daughterboard?

  //*VramBaseAddress = (EFI_PHYSICAL_ADDRESS) LCD_VRAM_CORE_TILE_BASE;
  //*VramSize = LCD_VRAM_SIZE;

  // Allocate the VRAM from the DRAM so that nobody else uses it.
  //Status = gBS->AllocatePages( AllocateAddress, EfiBootServicesData, EFI_SIZE_TO_PAGES(((UINTN)LCD_VRAM_SIZE)), VramBaseAddress);
  //if (EFI_ERROR(Status)) {
  //  return Status;
  //}

  // Ensure the Cpu architectural protocol is already installed
  //Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **)&Cpu);
  //ASSERT_EFI_ERROR(Status);

  // Mark the VRAM as un-cachable. The VRAM is inside the DRAM, which is cachable.
  //Status = Cpu->SetMemoryAttributes(Cpu, *VramBaseAddress, *VramSize, EFI_MEMORY_UC);
  //ASSERT_EFI_ERROR(Status);
  //if (EFI_ERROR(Status)) {
  //  gBS->FreePool(VramBaseAddress);
  //  return Status;
  //}

  return Status;
}

UINT32
LcdPlatformGetMaxMode (
  VOID
  )
{
  return (sizeof(mResolutions) / sizeof(LCD_RESOLUTION));
}

EFI_STATUS
LcdPlatformSetMode (
  IN UINT32                         ModeNumber
  )
{
  if (ModeNumber >= LcdPlatformGetMaxMode ()) {
    return EFI_INVALID_PARAMETER;
  }
  
  
  return EFI_SUCCESS;
}

EFI_STATUS
LcdPlatformQueryMode (
  IN  UINT32                                ModeNumber,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info
  )
{
  if (ModeNumber >= LcdPlatformGetMaxMode ()) {
    return EFI_INVALID_PARAMETER;
  }

  Info->Version = 0;
  Info->HorizontalResolution = mResolutions[ModeNumber].HorizontalResolution;
  Info->VerticalResolution = mResolutions[ModeNumber].VerticalResolution;
  Info->PixelsPerScanLine = mResolutions[ModeNumber].HorizontalResolution;

  switch (mResolutions[ModeNumber].Bpp) {
    case LCD_BITS_PER_PIXEL_24:
      Info->PixelFormat                   = PixelRedGreenBlueReserved8BitPerColor;
      Info->PixelInformation.RedMask      = LCD_24BPP_RED_MASK;
      Info->PixelInformation.GreenMask    = LCD_24BPP_GREEN_MASK;
      Info->PixelInformation.BlueMask     = LCD_24BPP_BLUE_MASK;
      Info->PixelInformation.ReservedMask = LCD_24BPP_RESERVED_MASK;
      break;
    default:
      // These are not supported
      ASSERT(FALSE);
      break;
  }
  
  return EFI_SUCCESS;
}


EFI_STATUS
LcdPlatformGetBpp (
  IN  UINT32                              ModeNumber,
  OUT LCD_BPP  *                          Bpp
  )
{
  if (ModeNumber >= LcdPlatformGetMaxMode ()) {
    return EFI_INVALID_PARAMETER;
  }

  *Bpp = mResolutions[ModeNumber].Bpp;

  return EFI_SUCCESS;
}

extern int msm_display_off(void);
extern UINT8 display_enable;
VOID
LcdShutdown (
  VOID
  )
{
  DEBUG((EFI_D_ERROR,"DisplayDxe:LcdShutdown!\n"));
  		if (display_enable) 
		{
			msm_display_off();
		}
}

