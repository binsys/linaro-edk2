
#include <Base.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/ArmLib.h>
#include <Library/PrintLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>

#include <Library/qcom_lk.h>

#include <Library/qcom_msm8960_iomap.h>
#include <Library/qcom_msm8960_irqs.h>
#include <Library/qcom_msm8960_clock.h>


#include <Library/qcom_clock.h>
#include <Library/qcom_clock_pll.h>
#include <Library/qcom_clock_local.h>

#include <Library/qcom_msm8960_timer.h>
#include <Library/qcom_mmc.h>


/*
 * Function to set the capabilities for the host
 */
void target_mmc_caps(struct mmc_host *host)
{
	host->caps.ddr_mode = 1;
	host->caps.hs200_mode = 1;
	host->caps.bus_width = MMC_BOOT_BUS_WIDTH_8_BIT;
	host->caps.hs_clk_rate = MMC_CLK_96MHZ;
}

