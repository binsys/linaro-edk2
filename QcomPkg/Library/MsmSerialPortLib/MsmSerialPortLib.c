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

extern int uart_putc(int port, char c);
extern int uart_getc(int port, int wait);


/**
  Write data to serial device.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Write data failed.
  @retval !0               Actual number of bytes writed to serial device.

**/
UINTN
EFIAPI
SerialPortWrite (
	IN UINT8     *Buffer,
	IN UINTN     NumberOfBytes
)
{
	//UINTN Count = 0;
	//for (Count = 0; Count < NumberOfBytes; Count++, Buffer++) 
	//{
	//	uart_putc(0,*Buffer);
	//}
	UINT32 uart_base = FixedPcdGet32(PcdQcomDebugUartDmBaseAddress);
	return msm_boot_uart_dm_write(uart_base,(INT8 *)Buffer,NumberOfBytes) == MSM_BOOT_UART_DM_E_SUCCESS?NumberOfBytes:0;
}


/**
  Read data from serial device and save the datas in buffer.

  @param  Buffer           Point of data buffer which need to be writed.
  @param  NumberOfBytes    Number of output bytes which are cached in Buffer.

  @retval 0                Read data failed.
  @retval !0               Aactual number of bytes read from serial device.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
)
{
	UINTN Count = 0;
	for (Count = 0; Count < NumberOfBytes; Count++, Buffer++) 
	{
		*Buffer = (UINT8)uart_getc(0,0); 
	}
	return NumberOfBytes;
}


/**
  Check to see if any data is avaiable to be read from the debug device.

  @retval EFI_SUCCESS       At least one byte of data is avaiable to be read
  @retval EFI_NOT_READY     No data is avaiable to be read
  @retval EFI_DEVICE_ERROR  The serial device is not functioning properly

**/
BOOLEAN
EFIAPI
SerialPortPoll (
	VOID
	)
{
	//UINT32 LSR = UartBase(PcdGet32(PcdOmap35xxConsoleUart)) + UART_LSR_REG;

	//if ((MmioRead8(LSR) & UART_LSR_RX_FIFO_E_MASK) == UART_LSR_RX_FIFO_E_NOT_EMPTY) 
	//{
	//	return TRUE;
	//} 
	//else 
	//{
	//	return FALSE;
	//}
	
	UINT32 uart_base = FixedPcdGet32(PcdQcomDebugUartDmBaseAddress);
	
	if(!(readl(MSM_BOOT_UART_DM_SR(uart_base)) & MSM_BOOT_UART_DM_SR_RXRDY))
	{
		return EFI_NOT_READY;
	}
	
	return EFI_SUCCESS;
}

