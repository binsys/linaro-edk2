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
 * Helper function to replace Line Feed char "\n" with
 * Carriage Return "\r\n".
 * Currently keeping it simple than efficient
 */
unsigned int msm_boot_uart_replace_lr_with_cr(char *data_in,int num_of_chars,char *data_out, int *num_of_chars_out)
{
	int i = 0, j = 0;

	if ((data_in == NULL) || (data_out == NULL) || (num_of_chars < 0)) {
		return MSM_BOOT_UART_DM_E_INVAL;
	}

	for (i = 0, j = 0; i < num_of_chars; i++, j++) {
		if (data_in[i] == '\n') {
			data_out[j++] = '\r';
		}

		data_out[j] = data_in[i];
	}

	*num_of_chars_out = j;

	return MSM_BOOT_UART_DM_E_SUCCESS;
}



#define PACK_CHARS_INTO_WORDS(a, cnt, word)  {\
                                               word = 0;\
                                               for(j = 0; j < (int)cnt; j++) \
                                               {\
                                                   word |= (a[j] & 0xff) << (j * 8);\
                                               }\
                                              }
                                              
                                              
/*
 * UART transmit operation
 */
unsigned int msm_boot_uart_dm_write(UINT32 base, char *data, unsigned int num_of_chars)
{
	unsigned int tx_word_count = 0;
	unsigned int tx_char_left = 0, tx_char = 0;
	unsigned int tx_word = 0;
	int i = 0;
	int j = 0;
	char *tx_data = NULL;
	char new_data[1024];

	if ((data == NULL) || (num_of_chars <= 0)) {
		return MSM_BOOT_UART_DM_E_INVAL;
	}

	/* Replace line-feed (/n) with carriage-return + line-feed (/r/n) */

	msm_boot_uart_replace_lr_with_cr(data, num_of_chars, new_data, &i);

	tx_data = new_data;
	num_of_chars = i;

	/* Write to NO_CHARS_FOR_TX register number of characters
	 * to be transmitted. However, before writing TX_FIFO must
	 * be empty as indicated by TX_READY interrupt in IMR register
	 */

	/* Check if transmit FIFO is empty.
	 * If not we'll wait for TX_READY interrupt. */
	if (!(readl(MSM_BOOT_UART_DM_SR(base)) & MSM_BOOT_UART_DM_SR_TXEMT)) {
		while (!(readl(MSM_BOOT_UART_DM_ISR(base)) & MSM_BOOT_UART_DM_TX_READY)) {
			udelay(1);
			/* Kick watchdog? */
		}
	}

	/* We are here. FIFO is ready to be written. */
	/* Write number of characters to be written */
	writel(num_of_chars, MSM_BOOT_UART_DM_NO_CHARS_FOR_TX(base));

	/* Clear TX_READY interrupt */
	writel(MSM_BOOT_UART_DM_GCMD_RES_TX_RDY_INT, MSM_BOOT_UART_DM_CR(base));

	/* We use four-character word FIFO. So we need to divide data into
	 * four characters and write in UART_DM_TF register */
	tx_word_count = (num_of_chars % 4) ? ((num_of_chars / 4) + 1) :
	    (num_of_chars / 4);
	tx_char_left = num_of_chars;

	for (i = 0; i < (int)tx_word_count; i++) {
		tx_char = (tx_char_left < 4) ? tx_char_left : 4;
		
		
		PACK_CHARS_INTO_WORDS(tx_data, tx_char, tx_word);

		/* Wait till TX FIFO has space */
		while (!(readl(MSM_BOOT_UART_DM_SR(base)) & MSM_BOOT_UART_DM_SR_TXRDY)) {
			udelay(1);
		}

		/* TX FIFO has space. Write the chars */
		writel(tx_word, MSM_BOOT_UART_DM_TF(base, 0));
		tx_char_left = num_of_chars - (i + 1) * 4;
		tx_data = tx_data + 4;
	}

	return MSM_BOOT_UART_DM_E_SUCCESS;
}

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
