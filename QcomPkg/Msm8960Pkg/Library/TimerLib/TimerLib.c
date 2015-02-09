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

#include <qcom_lk.h>

#include <qcom_msm8960_iomap.h>
#include <qcom_msm8960_irqs.h>
#include <qcom_msm8960_clock.h>


#include "platform_shared_timer.h"

//doc
//https://github.com/torvalds/linux/blob/master/Documentation/devicetree/bindings/arm/msm/timer.txt
//https://github.com/tsgan/qualcomm/blob/master/timer.c
//https://github.com/groeck/coreboot/blob/a234f45601e6e85a5179ec9cc446f070b86f425b/src/soc/qualcomm/ipq806x/timer.c

RETURN_STATUS
EFIAPI
TimerConstructor (
  VOID
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:TimerConstructor need re-mpl!!!\n"));

	/* Initialize DGT timer */
	/* disable timer */
	//writel(0, DGT_ENABLE);
	MmioWrite32(DGT_ENABLE,0);
	

	/* DGT uses LPXO source which is 27MHz.
	 * Set clock divider to 4.
	 */
	//writel(3, DGT_CLK_CTL);
	MmioWrite32(DGT_CLK_CTL,3);

	//ticks_per_sec = 6750000;	/* (27 MHz / 4) */
	return EFI_SUCCESS;
}

//∫¡√Î
UINTN
EFIAPI
MicroSecondDelay (
  IN  UINTN MicroSeconds
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:MicroSecondDelay need re-mpl!!!\n"));
	mdelay((UINT32)MicroSeconds);
	return MicroSeconds;
}

//Œ¢√Î
UINTN
EFIAPI
NanoSecondDelay (
  IN  UINTN NanoSeconds
  )
{
	DEBUG ((EFI_D_ERROR, "TimerLib:NanoSecondDelay need re-mpl!!!\n"));
	udelay((UINT32)NanoSeconds);
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
