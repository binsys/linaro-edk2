/** @file

  Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.<BR>

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/IoLib.h>
#include <Library/DebugLib.h>


VOID
TimerInit (
  VOID
  )
{
/*
  UINTN  Timer            = FixedPcdGet32(PcdOmap35xxFreeTimer);
  UINT32 TimerBaseAddress = TimerBase(Timer);

  // Set source clock for GPT3 & GPT4 to SYS_CLK
  MmioOr32 (CM_CLKSEL_PER, CM_CLKSEL_PER_CLKSEL_GPT3_SYS | CM_CLKSEL_PER_CLKSEL_GPT4_SYS);

  // Set count & reload registers
  MmioWrite32 (TimerBaseAddress + GPTIMER_TCRR, 0x00000000);
  MmioWrite32 (TimerBaseAddress + GPTIMER_TLDR, 0x00000000);

  // Disable interrupts
  MmioWrite32 (TimerBaseAddress + GPTIMER_TIER, TIER_TCAR_IT_DISABLE | TIER_OVF_IT_DISABLE | TIER_MAT_IT_DISABLE);

  // Start Timer
  MmioWrite32 (TimerBaseAddress + GPTIMER_TCLR, TCLR_AR_AUTORELOAD | TCLR_ST_ON);

  //Disable OMAP Watchdog timer (WDT2)
  MmioWrite32 (WDTIMER2_BASE + WSPR, 0xAAAA);
  DEBUG ((EFI_D_ERROR, "Magic delay to disable watchdog timers properly.\n"));
  MmioWrite32 (WDTIMER2_BASE + WSPR, 0x5555);*/
}
