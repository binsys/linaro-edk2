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
#include <Library/qcom_uart_dm.h>


/*
 * UART Receive operation
 * Reads a word from the RX FIFO.
 */
unsigned int msm_boot_uart_dm_read(UINT32 base, unsigned int *data, int wait)
{
	static int rx_last_snap_count = 0;
	static int rx_chars_read_since_last_xfer = 0;

	if (data == NULL) {
		return MSM_BOOT_UART_DM_E_INVAL;
	}

	/* We will be polling RXRDY status bit */
	while (!(readl(MSM_BOOT_UART_DM_SR(base)) & MSM_BOOT_UART_DM_SR_RXRDY)) {
		/* if this is not a blocking call, we'll just return */
		if (!wait) {
			return MSM_BOOT_UART_DM_E_RX_NOT_READY;
		}
	}

	/* Check for Overrun error. We'll just reset Error Status */
	if (readl(MSM_BOOT_UART_DM_SR(base)) & MSM_BOOT_UART_DM_SR_UART_OVERRUN) {
		writel(MSM_BOOT_UART_DM_CMD_RESET_ERR_STAT, MSM_BOOT_UART_DM_CR(base));
	}

	/* RX FIFO is ready; read a word. */
	*data = readl(MSM_BOOT_UART_DM_RF(base, 0));

	/* increment the total count of chars we've read so far */
	rx_chars_read_since_last_xfer += 4;

	/* Rx transfer ends when one of the conditions is met:
	 * - The number of characters received since the end of the previous
	 *   xfer equals the value written to DMRX at Transfer Initialization
	 * - A stale event occurred
	 */

	/* If RX transfer has not ended yet */
	if (rx_last_snap_count == 0) {
		/* Check if we've received stale event */
		if (readl(MSM_BOOT_UART_DM_MISR(base)) & MSM_BOOT_UART_DM_RXSTALE) {
			/* Send command to reset stale interrupt */
			writel(MSM_BOOT_UART_DM_CMD_RES_STALE_INT, MSM_BOOT_UART_DM_CR(base));
		}

		/* Check if we haven't read more than DMRX value */
		else if ((unsigned int)rx_chars_read_since_last_xfer <
			readl(MSM_BOOT_UART_DM_DMRX(base))) {
			/* We can still continue reading before initializing RX transfer */
			return MSM_BOOT_UART_DM_E_SUCCESS;
		}

		/* If we've reached here it means RX
		 * xfer end conditions been met
		 */

		/* Read UART_DM_RX_TOTAL_SNAP register
		 * to know how many valid chars
		 * we've read so far since last transfer
		 */
		rx_last_snap_count = readl(MSM_BOOT_UART_DM_RX_TOTAL_SNAP(base));

	}

	/* If there are still data left in FIFO we'll read them before
	 * initializing RX Transfer again */
	if ((rx_last_snap_count - rx_chars_read_since_last_xfer) >= 0) {
		return MSM_BOOT_UART_DM_E_SUCCESS;
	}

	msm_boot_uart_dm_init_rx_transfer(base);
	rx_last_snap_count = 0;
	rx_chars_read_since_last_xfer = 0;

	return MSM_BOOT_UART_DM_E_SUCCESS;
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

	if (!word) 
	{
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

