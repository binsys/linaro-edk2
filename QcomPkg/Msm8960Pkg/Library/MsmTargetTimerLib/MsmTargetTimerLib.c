/** @file

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>

#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <Library/qcom_lk.h>

#include <Library/qcom_msm8960_iomap.h>
#include <Library/qcom_msm8960_irqs.h>
#include <Library/qcom_msm8960_clock.h>
#include <Library/qcom_msm8960_timer.h>


#define GPT_ENABLE_CLR_ON_MATCH_EN        2
#define GPT_ENABLE_EN                     1


//doc
//https://github.com/torvalds/linux/blob/master/Documentation/devicetree/bindings/arm/msm/timer.txt
//https://github.com/tsgan/qualcomm/blob/master/timer.c
//https://github.com/groeck/coreboot/blob/a234f45601e6e85a5179ec9cc446f070b86f425b/src/soc/qualcomm/ipq806x/timer.c



//ms «∫¡√Î=0.001√Î
//us «Œ¢√Î=0.000001√Î
//ns «ƒ…√Î=0.000000001√Î
//http://blog.163.com/arm_linux008/blog/static/13780414220107462045150/


//1√Î=1000∫¡√Î=1000000Œ¢√Î=1000000000ƒ…√Î=1000000000000∆§√Î=1000000000000000∑…√Î
//1s=1000ms=1000000us=1000000000ns=1000000000000ps=1000000000000000fs
//ƒ⁄∫À≤„£∫
//   include 
//   1°¢void ndelay(unsigned long nsecs);         ƒ…√Îº∂£∫1/10^-10
//   2°¢void udelay(unsigned long usecs);         Œ¢√Îº∂: 1/10^-6
//   3°¢void mdelay(unsigned long msecs);         ∫¡√Îº∂£∫1/10^-3

//   * With 32KHz clock, minimum possible delay is 31.25 Micro seconds and
//   * its multiples. In Rumi GPT clock is 32 KHz


//ref 
// clock is 32768Hz = 33k
// 1ms = 33 ticks
// 1us = 0.033 ticks
// 1ns = 0.000033 ticks
//∫¡√Î—” ±
//     1000us        30us
//     33 ticks      1 ticks
void mdelay(unsigned msecs)
{
	msecs *= 33;

	writel(0, GPT_CLEAR);
	writel(0, GPT_ENABLE);
	while (readl(GPT_COUNT_VAL) != 0);

	writel(GPT_ENABLE_EN, GPT_ENABLE);
	while (readl(GPT_COUNT_VAL) < msecs);

	writel(0, GPT_ENABLE);
	writel(0, GPT_CLEAR);
}

//Œ¢√Î—” ±◊Ó–°—”≥Ÿ «30us£¨usecs = 1
void udelay(unsigned usecs)
{
	usecs = (usecs * 33 + 1000 - 33) / 1000;

	writel(0, GPT_CLEAR);
	writel(0, GPT_ENABLE);
	while (readl(GPT_COUNT_VAL) != 0);

	writel(GPT_ENABLE_EN, GPT_ENABLE);
	while (readl(GPT_COUNT_VAL) < usecs);

	writel(0, GPT_ENABLE);
	writel(0, GPT_CLEAR);
}


RETURN_STATUS
EFIAPI
TimerConstructor (
  VOID
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:TimerConstructor\n"));
	return EFI_SUCCESS;
}

//∫¡√Î
UINTN
EFIAPI
MicroSecondDelay (
  IN  UINTN MicroSeconds
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:MicroSecondDelay MicroSeconds=%d start\n",MicroSeconds));
	mdelay((UINT32)MicroSeconds);
	DEBUG ((EFI_D_ERROR, "TimerLib:MicroSecondDelay MicroSeconds=%d end\n",MicroSeconds));
	return MicroSeconds;
}

//Œ¢√Î
UINTN
EFIAPI
NanoSecondDelay (
  IN  UINTN NanoSeconds
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:NanoSecondDelay NanoSeconds=%d,use udelay start\n",NanoSeconds));
	udelay((UINT32)NanoSeconds);
	DEBUG ((EFI_D_ERROR, "TimerLib:NanoSecondDelay NanoSeconds=%d,use udelay end\n",NanoSeconds));
	return NanoSeconds;
}

UINT64
EFIAPI
GetPerformanceCounter (
  VOID
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:GetPerformanceCounter need mpl!!!\n"));
	ASSERT(FALSE);
	return 0;
}

UINT64
EFIAPI
GetPerformanceCounterProperties (
  OUT UINT64  *StartValue,  OPTIONAL
  OUT UINT64  *EndValue     OPTIONAL
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:GetPerformanceCounterProperties need mpl!!!\n"));
	ASSERT(FALSE);
	if (StartValue != NULL) {
	// Timer starts with the reload value
	//*StartValue = (UINT64)MmioRead32 (TimerBase(PcdGet32(PcdOmap35xxFreeTimer)) + GPTIMER_TLDR);
	}

	if (EndValue != NULL) {
		// Timer counts up to 0xFFFFFFFF
		*EndValue = 0xFFFFFFFF;
	}
	return PcdGet64(PcdEmbeddedPerformanceCounterFrequencyInHz);
}
