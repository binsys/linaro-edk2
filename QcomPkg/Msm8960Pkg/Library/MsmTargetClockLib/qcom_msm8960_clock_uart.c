
#include <Base.h>
#include <Library/DebugLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Library/PrintLib.h>
#include <Library/qcom_clock.h>


/* Configure UART clock - based on the gsbi id */
void clock_config_uart_dm(UINT8 id)
{
	char gsbi_uart_clk_id[64];
	char gsbi_p_clk_id[64];
	
	UINT32 id2 = id;

	AsciiSPrint (gsbi_uart_clk_id, sizeof(gsbi_uart_clk_id), "gsbi%d_uart_clk", id2);
	clk_get_set_enable(gsbi_uart_clk_id, 1843200, 1);

	AsciiSPrint (gsbi_p_clk_id, sizeof(gsbi_uart_clk_id), "gsbi%d_pclk", id2);
	clk_get_set_enable(gsbi_p_clk_id, 0, 1);
}


