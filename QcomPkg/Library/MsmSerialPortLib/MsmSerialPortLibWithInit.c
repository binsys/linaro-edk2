/** @file
  Serial I/O Port library functions with no library constructor/destructor


  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Base.h>
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Library/qcom_lk.h>
#include <Library/qcom_gsbi.h>
#include <Library/qcom_uart_dm.h>


/*
  Programmed hardware of Serial port.
  @return    Always return EFI_UNSUPPORTED.
**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
	// Set up system clocking,now only uart.
	msm_clocks_init();
	
	// Init Uart
	uart_dm_init(FixedPcdGet8(PcdQcomDebugGsbiID), FixedPcdGet32(PcdQcomDebugGsbiBaseAddress), FixedPcdGet32(PcdQcomDebugUartDmBaseAddress));
	return RETURN_SUCCESS;
}


