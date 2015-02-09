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
#include <Library/DebugLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

extern int uart_putc(int port, char c);
extern int uart_getc(int port, int wait);
extern void uart_dm_init(UINT8 id,UINT32 gsbi_base,UINT32 uart_dm_base);

static UINT32 S_INIT = 0;

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
	if(!S_INIT)
	{
		//!IMPORTANT  MUST INIT(RE-INIT) UART AT HERE.BECAUSE QCOM UART_DM lib is static link,can't share var*.
		DEBUG ((EFI_D_ERROR, "SerialPortInitialize:(REMOVE THIS ON PRODUCT BUILD)!!!\n"));
		
		uart_dm_init(5, 0x16400000, 0x16440000);
		S_INIT = 1;
	}
	return RETURN_SUCCESS;
}

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
	UINTN Count = 0;
	for (Count = 0; Count < NumberOfBytes; Count++, Buffer++) 
	{
		uart_putc(0,*Buffer);
	}
	return NumberOfBytes;
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
	
	return FALSE;
}

