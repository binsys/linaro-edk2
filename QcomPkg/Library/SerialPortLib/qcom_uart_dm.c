/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>


#include <Library/ArmLib.h>
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Library/qcom_lk.h>
#include <Library/qcom_gsbi.h>
#include <qcom_uart_dm.h>

/* Note:
 * This is a basic implementation of UART_DM protocol. More focus has been
 * given on simplicity than efficiency. Few of the things to be noted are:
 * - RX path may not be suitable for multi-threaded scenaraio because of the
 *   use of static variables. TX path shouldn't have any problem though. If
 *   multi-threaded support is required, a simple data-structure can
 *   be maintained for each thread.
 * - Right now we are using polling method than interrupt based.
 * - We are using legacy UART protocol without Data Mover.
 * - Not all interrupts and error events are handled.
 * - While waiting Watchdog hasn't been taken into consideration.
 */


/* UART_DM uses four character word FIFO where as UART core
 * uses a character FIFO. so it's really inefficient to try
 * to write single character. But that's how dprintf has been
 * implemented.
 */
int uart_putc(int port, char c)
{
	UINT32 uart_base = FixedPcdGet32(PcdQcomDebugUartDmBaseAddress);


	msm_boot_uart_dm_write(uart_base, &c, 1);

	return 0;
}

/* UART_DM uses four character word FIFO whereas uart_getc
 * is supposed to read only one character. So we need to
 * read a word and keep track of each character in the word.
 */
int uart_getc(int port, int wait)
{
	int byte;
	static unsigned int word = 0;
	UINT32 uart_base = FixedPcdGet32(PcdQcomDebugUartDmBaseAddress);


	if (!word) {
		/* Read from FIFO only if it's a first read or all the four
		 * characters out of a word have been read */
		if (msm_boot_uart_dm_read(uart_base, &word, wait) != MSM_BOOT_UART_DM_E_SUCCESS) {
			return -1;
		}

	}

	byte = (int)word & 0xff;
	word = word >> 8;

	return byte;
}
