
#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Library/PrintLib.h>
#include <Library/qcom_clock.h>




UINT32 qcom_uart_dm_target_UART_DM_CLK_RX_TX_BIT_RATE = 0xff;


/* Configure UART clock - based on the gsbi id */
void clock_config_uart_dm(UINT8 id)
{
	char gsbi_uart_clk_id[64];
	char gsbi_p_clk_id[64];

	AsciiSPrint (gsbi_uart_clk_id, 64, "gsbi%u_uart_clk", id);
	clk_get_set_enable(gsbi_uart_clk_id, 1843200, 1);

	AsciiSPrint (gsbi_p_clk_id, 64, "gsbi%u_pclk", id);
	clk_get_set_enable(gsbi_p_clk_id, 0, 1);
}


