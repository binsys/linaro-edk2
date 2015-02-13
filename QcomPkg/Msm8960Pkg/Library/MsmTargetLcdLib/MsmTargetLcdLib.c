/** @file

  Copyright (c) 2011-2013, ARM Ltd. All rights reserved.<BR>
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

#include <Protocol/Cpu.h>
#include <Protocol/EdidDiscovered.h>
#include <Protocol/EdidActive.h>

//#include <ArmPlatform.h>

typedef struct {
  UINT32                     Mode;
  UINT32                     HorizontalResolution;
  UINT32                     VerticalResolution;
  LCD_BPP                    Bpp;
  UINT32                     OscFreq;

  UINT32                     HSync;
  UINT32                     HBackPorch;
  UINT32                     HFrontPorch;
  UINT32                     VSync;
  UINT32                     VBackPorch;
  UINT32                     VFrontPorch;
} LCD_RESOLUTION;

#define WXGA 9

//
// WXGA Mode: 720 x 1280
//
#define WXGA_H_RES_PIXELS                 720
#define WXGA_V_RES_PIXELS                 1280
#define WXGA_OSC_FREQUENCY                109000000  /* 0x067F3540 */

#define WXGA_H_SYNC                       (136 - 1)
#define WXGA_H_FRONT_PORCH                ( 80 - 1)
#define WXGA_H_BACK_PORCH                 (216 - 1)

#define WXGA_V_SYNC                       (  7 - 1)
#define WXGA_V_FRONT_PORCH                (  3 - 1)
#define WXGA_V_BACK_PORCH                 ( 29 - 1)

LCD_RESOLUTION mResolutions[] = {
  { 
      // Mode 0 : WXGA : 720 x 1280 x 24 bpp
      WXGA, 
      WXGA_H_RES_PIXELS, 
      WXGA_V_RES_PIXELS, 
      LCD_BITS_PER_PIXEL_24, 
      WXGA_OSC_FREQUENCY,
      WXGA_H_SYNC, 
      WXGA_H_FRONT_PORCH, 
      WXGA_H_BACK_PORCH,
      WXGA_V_SYNC, 
      WXGA_V_FRONT_PORCH, 
      WXGA_V_BACK_PORCH
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
LcdPlatformInitializeDisplay (
  IN EFI_HANDLE   Handle
  )
{
  DEBUG((EFI_D_ERROR,"MsmTargetLcdLibLcd:PlatformInitializeDisplay Not Impl!\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
LcdPlatformGetVram (
  OUT EFI_PHYSICAL_ADDRESS*  VramBaseAddress,
  OUT UINTN*                 VramSize
  )
{
  EFI_STATUS              Status;
  EFI_CPU_ARCH_PROTOCOL  *Cpu;

  Status = EFI_SUCCESS;
  DEBUG((EFI_D_ERROR,"MsmTargetLcdLibLcd:LcdPlatformGetVram Not Impl!\n"));
  Status = EFI_UNSUPPORTED;
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
  DEBUG((EFI_D_ERROR,"MsmTargetLcdLibLcd:LcdPlatformGetMaxMode Not Impl!\n"));
  return (sizeof(mResolutions) / sizeof(LCD_RESOLUTION)) - 1;
}

EFI_STATUS
LcdPlatformSetMode (
  IN UINT32                         ModeNumber
  )
{
  EFI_STATUS            Status;
  UINT32                LcdSite;
  UINT32                OscillatorId;
  SYS_CONFIG_FUNCTION   Function;
  UINT32                SysId;
  
  DEBUG((EFI_D_ERROR,"MsmTargetLcdLibLcd:LcdPlatformSetMode Not Impl!\n"));
  
  if (ModeNumber > LcdPlatformGetMaxMode ()) {
    return EFI_INVALID_PARAMETER;
  }
  
  
  return EFI_UNSUPPORTED;
}

EFI_STATUS
LcdPlatformQueryMode (
  IN  UINT32                                ModeNumber,
  OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info
  )
{

  DEBUG((EFI_D_ERROR,"MsmTargetLcdLibLcd:LcdPlatformQueryMode Not Impl!\n"));
  
  if (ModeNumber > LcdPlatformGetMaxMode ()) {
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

    case LCD_BITS_PER_PIXEL_16_555:
    case LCD_BITS_PER_PIXEL_16_565:
    case LCD_BITS_PER_PIXEL_12_444:
    case LCD_BITS_PER_PIXEL_8:
    case LCD_BITS_PER_PIXEL_4:
    case LCD_BITS_PER_PIXEL_2:
    case LCD_BITS_PER_PIXEL_1:
    default:
      // These are not supported
      ASSERT(FALSE);
      break;
  }
  
  return EFI_SUCCESS;
}

EFI_STATUS
LcdPlatformGetTimings (
  IN  UINT32                              ModeNumber,
  OUT UINT32*                             HRes,
  OUT UINT32*                             HSync,
  OUT UINT32*                             HBackPorch,
  OUT UINT32*                             HFrontPorch,
  OUT UINT32*                             VRes,
  OUT UINT32*                             VSync,
  OUT UINT32*                             VBackPorch,
  OUT UINT32*                             VFrontPorch
  )
{

  DEBUG((EFI_D_ERROR,"MsmTargetLcdLibLcd:LcdPlatformGetTimings Not Impl!\n"));
  if (ModeNumber > LcdPlatformGetMaxMode ()) {
    return EFI_INVALID_PARAMETER;
  }

  *HRes           = mResolutions[ModeNumber].HorizontalResolution;
  *HSync          = mResolutions[ModeNumber].HSync;
  *HBackPorch     = mResolutions[ModeNumber].HBackPorch;
  *HFrontPorch    = mResolutions[ModeNumber].HFrontPorch;
  *VRes           = mResolutions[ModeNumber].VerticalResolution;
  *VSync          = mResolutions[ModeNumber].VSync;
  *VBackPorch     = mResolutions[ModeNumber].VBackPorch;
  *VFrontPorch    = mResolutions[ModeNumber].VFrontPorch;

  return EFI_SUCCESS;
}

EFI_STATUS
LcdPlatformGetBpp (
  IN  UINT32                              ModeNumber,
  OUT LCD_BPP  *                          Bpp
  )
{
  DEBUG((EFI_D_ERROR,"MsmTargetLcdLibLcd:LcdPlatformGetBpp Not Impl!\n"));
  if (ModeNumber > LcdPlatformGetMaxMode ()) {
    return EFI_INVALID_PARAMETER;
  }

  *Bpp = mResolutions[ModeNumber].Bpp;

  return EFI_SUCCESS;
}
