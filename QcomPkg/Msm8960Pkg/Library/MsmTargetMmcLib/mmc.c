/* Copyright (c) 2010-2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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


#include <Base.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>

#include <Library/qcom_lk.h>

#include <Library/qcom_msm8960_iomap.h>
#include <Library/qcom_msm8960_irqs.h>
#include <Library/qcom_msm8960_clock.h>


#include <Library/qcom_clock.h>
#include <Library/qcom_clock_pll.h>
#include <Library/qcom_clock_local.h>

//#include <uart_dm.h>
//#include <gsbi.h>
#include <Library/qcom_mmc.h>


//#include <partition_parser.h>
//#include <platform/timer.h>


#define USEC_PER_SEC           (1000000L)

unsigned int mmc_boot_mci_base = 0;
struct mmc_host mmc_host;

/*
 * Enable MCI CLK
 */
void mmc_boot_mci_clk_enable()
{
	UINT32 reg = 0;

	reg |= MMC_BOOT_MCI_CLK_ENABLE;
	reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	reg |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
	writel(reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();

	/* Enable power save */
	reg |= MMC_BOOT_MCI_CLK_PWRSAVE;
	writel(reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();
}

/*
 * Disable MCI clk
 */
void mmc_boot_mci_clk_disable()
{
	UINT32 reg = 0;

	reg |= MMC_BOOT_MCI_CLK_DISABLE;
	writel(reg, MMC_BOOT_MCI_CLK);

	/* Wait for the MMC_BOOT_MCI_CLK write to go through. */
	mmc_mclk_reg_wr_delay();
}


void mmc_mclk_reg_wr_delay()
{
	if (mmc_host.mmc_cont_version)
	{
		/* Wait for the MMC_BOOT_MCI register write to go through. */
		while(readl(MMC_BOOT_MCI_STATUS2) & MMC_BOOT_MCI_MCLK_REG_WR_ACTIVE);
	}
	else
		udelay((1 + ((3 * USEC_PER_SEC) / (mmc_host.mclk_rate? mmc_host.mclk_rate : MMC_CLK_400KHZ))));
}
