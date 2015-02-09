

/*
* Copyright (c) 2008, Google Inc.
* All rights reserved.
*
* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*  * Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  * Neither the name of Google, Inc. nor the names of its contributors
*    may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

#include <Library/IoLib.h>
#include <Library/DebugLib.h>

#include <qcom_lk.h>

#include <qcom_msm8960_iomap.h>
#include <qcom_msm8960_irqs.h>

#include "platform_shared_timer.h"




#define GPT_ENABLE_CLR_ON_MATCH_EN        2
#define GPT_ENABLE_EN                     1
#define DGT_ENABLE_CLR_ON_MATCH_EN        2
#define DGT_ENABLE_EN                     1

#define SPSS_TIMER_STATUS_DGT_EN    (1 << 0)

//static platform_timer_callback timer_callback;
//static void *timer_arg;
//static time_t timer_interval;

static volatile UINT32 ticks;

//static enum handler_return timer_irq(void *arg)
//{
//	ticks += timer_interval;
//	return timer_callback(timer_arg, ticks);
//}

//status_t
//platform_set_periodic_timer(platform_timer_callback callback,
//void *arg, time_t interval)
//{
//	uint32_t tick_count = interval * platform_tick_rate() / 1000;
//
//	enter_critical_section();
//
//	timer_callback = callback;
//	timer_arg = arg;
//	timer_interval = interval;
//
//	writel(tick_count, DGT_MATCH_VAL);
//	writel(0, DGT_CLEAR);
//	writel(DGT_ENABLE_EN | DGT_ENABLE_CLR_ON_MATCH_EN, DGT_ENABLE);
//
//	register_int_handler(INT_DEBUG_TIMER_EXP, timer_irq, 0);
//	unmask_interrupt(INT_DEBUG_TIMER_EXP);
//
//	exit_critical_section();
//	return 0;
//}
//
//time_t current_time(void)
//{
//	return ticks;
//}

//static void wait_for_timer_op(void)
//{
//	while (readl(SPSS_TIMER_STATUS) & SPSS_TIMER_STATUS_DGT_EN);
//}

//void platform_uninit_timer(void)
//{
//	writel(0, DGT_ENABLE);
//	wait_for_timer_op();
//	writel(0, DGT_CLEAR);
//	wait_for_timer_op();
//}



//msÊÇºÁÃë=0.001Ãë
//usÊÇÎ¢Ãë=0.000001Ãë
//nsÊÇÄÉÃë=0.000000001Ãë
//http://blog.163.com/arm_linux008/blog/static/13780414220107462045150/


//1Ãë=1000ºÁÃë=1000000Î¢Ãë=1000000000ÄÉÃë=1000000000000Æ¤Ãë=1000000000000000·ÉÃë
//1s=1000ms=1000000us=1000000000ns=1000000000000ps=1000000000000000fs
//ÄÚºË²ã£º
//   include 
//   1¡¢void ndelay(unsigned long nsecs);         ÄÉÃë¼¶£º1/10^-10
//   2¡¢void udelay(unsigned long usecs);         Î¢Ãë¼¶: 1/10^-6
//   3¡¢void mdelay(unsigned long msecs);         ºÁÃë¼¶£º1/10^-3

//   * With 32KHz clock, minimum possible delay is 31.25 Micro seconds and
//   * its multiples. In Rumi GPT clock is 32 KHz


//ref 
// clock is 32768Hz = 33k
// 1ms = 33 ticks
// 1ns = 0.033 ticks
//ºÁÃëÑÓÊ±
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

//Î¢ÃëÑÓÊ±×îÐ¡ÑÓ³ÙÊÇ30us£¬usecs = 1
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

/* Return current time in micro seconds */
bigtime_t current_time_hires(void)
{
	return ticks * 1000ULL;
}

