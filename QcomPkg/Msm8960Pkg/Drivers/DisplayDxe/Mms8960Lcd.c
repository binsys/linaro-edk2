/** @file  Lcd.c

  Copyright (c) 2011-2012, ARM Ltd. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/LcdPlatformLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PcdLib.h>

//#include <Drivers/HdLcd.h>

#include "LcdGraphicsOutputDxe.h"

/**********************************************************************
 *
 *  This file contains all the bits of the Lcd that are
 *  platform independent.
 *
 **********************************************************************/

EFI_STATUS
LcdInitialize (
  IN EFI_PHYSICAL_ADDRESS   VramBaseAddress
  )
{
  DEBUG((EFI_D_ERROR,"DisplayDxe:LcdInitialize Not Impl!\n"));
  return EFI_SUCCESS;
}

EFI_STATUS
LcdSetMode (
  IN UINT32  ModeNumber
  )
{
  DEBUG((EFI_D_ERROR,"DisplayDxe:LcdSetMode Not Impl!\n"));
  return EFI_SUCCESS;
}

VOID
LcdShutdown (
  VOID
  )
{
  DEBUG((EFI_D_ERROR,"DisplayDxe:LcdShutdown Not Impl!\n"));
}
