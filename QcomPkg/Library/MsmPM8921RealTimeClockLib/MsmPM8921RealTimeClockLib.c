/** @file
*
*  Copyright (c) 2011, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <Uefi.h>

#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Protocol/RealTimeClock.h>
#include <Protocol/EmbeddedExternalDevice.h>
#include <Protocol/PM8921.h>


// Define EPOCH (1970-JANUARY-01) in the Julian Date representation
#define EPOCH_JULIAN_DATE	2440588

// Seconds per unit
#define SEC_PER_MIN		    ((UINTN)    60)
#define SEC_PER_HOUR		((UINTN)  3600)
#define SEC_PER_DAY		    ((UINTN) 86400)

#define SEC_PER_MONTH		((UINTN)  2,592,000)
#define SEC_PER_YEAR		((UINTN) 31,536,000)



QCOM_PM8921_PROTOCOL       * gPM8921;

/**
  Converts EFI_TIME to Epoch seconds (elapsed since 1970 JANUARY 01, 00:00:00 UTC)
  **/
static UINTN
EfiTimeToEpoch(
	IN  EFI_TIME  *Time
)
{
	UINTN a;
	UINTN y;
	UINTN m;
	UINTN JulianDate;  // Absolute Julian Date representation of the supplied Time
	UINTN EpochDays;   // Number of days elapsed since EPOCH_JULIAN_DAY
	UINTN EpochSeconds;

	a = (14 - Time->Month) / 12;
	y = Time->Year + 4800 - a;
	m = Time->Month + (12 * a) - 3;

	JulianDate = Time->Day + ((153 * m + 2) / 5) + (365 * y) + (y / 4) - (y / 100) + (y / 400) - 32045;

	ASSERT(JulianDate > EPOCH_JULIAN_DATE);
	EpochDays = JulianDate - EPOCH_JULIAN_DATE;

	EpochSeconds = (EpochDays * SEC_PER_DAY) + ((UINTN)Time->Hour * SEC_PER_HOUR) + (Time->Minute * SEC_PER_MIN) + Time->Second;

	return EpochSeconds;
}

/**
  Converts Epoch seconds (elapsed since 1970 JANUARY 01, 00:00:00 UTC) to EFI_TIME
  **/
VOID
EpochToEfiTime(
	IN  UINTN     EpochSeconds,
	OUT EFI_TIME  *Time
)
{
	INTN         a;
	INTN         b;
	INTN         c;
	INTN         d;
	INTN         g;
	INTN         j;
	INTN         m;
	INTN         y;
	INTN         da;
	INTN         db;
	INTN         dc;
	INTN         dg;
	INTN         hh;
	INTN         mm;
	INTN         ss;
	INTN         J;

	J = (EpochSeconds / 86400) + 2440588;
	j = J + 32044;
	g = j / 146097;
	dg = j % 146097;
	c = (((dg / 36524) + 1) * 3) / 4;
	dc = dg - (c * 36524);
	b = dc / 1461;
	db = dc % 1461;
	a = (((db / 365) + 1) * 3) / 4;
	da = db - (a * 365);
	y = (g * 400) + (c * 100) + (b * 4) + a;
	m = (((da * 5) + 308) / 153) - 2;
	d = da - (((m + 4) * 153) / 5) + 122;

	Time->Year = y - 4800 + ((m + 2) / 12);
	Time->Month = ((m + 2) % 12) + 1;
	Time->Day = d + 1;

	ss = EpochSeconds % 60;
	a = (EpochSeconds - ss) / 60;
	mm = a % 60;
	b = (a - mm) / 60;
	hh = b % 24;

	Time->Hour = hh;
	Time->Minute = mm;
	Time->Second = ss;
	Time->Nanosecond = 0;
}

BOOLEAN
IsLeapYear(
	IN EFI_TIME   *Time
)
{
	if (Time->Year % 4 == 0)
	{
		if (Time->Year % 100 == 0)
		{
			if (Time->Year % 400 == 0)
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}


/**
  Returns the current time and date information, and the time-keeping capabilities
  of the hardware platform.

  @param  Time                  A pointer to storage to receive a snapshot of the current time.
  @param  Capabilities          An optional pointer to a buffer to receive the real time clock
  device's capabilities.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER Time is NULL.
  @retval EFI_DEVICE_ERROR      The time could not be retrieved due to hardware error.

  **/
EFI_STATUS
EFIAPI
LibGetTime(
OUT EFI_TIME                *Time,
OUT EFI_TIME_CAPABILITIES   *Capabilities
)
{
	EFI_STATUS            Status;
	EFI_TPL               OldTpl;
	UINT32                secs;

	if (Time == NULL)
	{
		return EFI_INVALID_PARAMETER;
	}
	//DEBUG((EFI_D_INFO, "LibGetTime start\n"));

	/* Get time and date */
	ZeroMem(Time, sizeof(EFI_TIME));


	OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
	Status = gPM8921->pm8xxx_rtc_read_time(gPM8921, &secs);
	gBS->RestoreTPL(OldTpl);

	if (Status != EFI_SUCCESS) goto EXIT;

	// Convert from internal 32-bit time to UEFI time
	EpochToEfiTime(secs, Time);

	//DEBUG((EFI_D_INFO, "LibGetTime:RTC Year:%d Month:%d Day:%d Hour:%d Minute:%d Second:%d \n", Time->Year, Time->Month, Time->Day, Time->Hour, Time->Minute, Time->Second));

	// Set capabilities

	// TODO: Set real capabilities
	if (Capabilities != NULL)
	{
		Capabilities->Resolution = 1;
		Capabilities->Accuracy = 50000000;
		Capabilities->SetsToZero = FALSE;
	}

EXIT:
	//DEBUG((EFI_D_INFO, "LibGetTime end\n"));
	return (Status == EFI_SUCCESS) ? Status : EFI_DEVICE_ERROR;
}



BOOLEAN
DayValid(
	IN  EFI_TIME  *Time
)
{
	INTN  DayOfMonth[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (Time->Day < 1 ||
		Time->Day > DayOfMonth[Time->Month - 1] ||
		(Time->Month == 2 && (!IsLeapYear(Time) && Time->Day > 28))
		)
	{
		return FALSE;
	}

	return TRUE;
}


/**
  Sets the current local time and date information.

  @param  Time                  A pointer to the current time.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware error.

  **/
EFI_STATUS
EFIAPI
LibSetTime(
	IN EFI_TIME *Time
)
{
	EFI_STATUS Status;
	EFI_TPL    OldTpl;

	UINTN       EpochSeconds;

	DEBUG((EFI_D_INFO, " LibSetTime: Year:%d  Month:%d Day:%d Hour:%d Minute:%d Nanosec:%d\n", Time->Year, Time->Month, Time->Day, Time->Hour, Time->Minute, Time->Nanosecond));
	// Time is stored in Unix Epoch format, so it starts in 1970,
	// Check the input parameters' range.
	if ((Time->Year < 1998) ||
		(Time->Year   > 2099) ||
		(Time->Month < 1) ||
		(Time->Month  > 12) ||
		(!DayValid(Time)) ||
		(Time->Hour > 23) ||
		(Time->Minute > 59) ||
		(Time->Second > 59) ||
		(Time->Nanosecond > 999999999) ||
		(!((Time->TimeZone == EFI_UNSPECIFIED_TIMEZONE) || ((Time->TimeZone >= -1440) && (Time->TimeZone <= 1440)))) ||
		(Time->Daylight & (~(EFI_TIME_ADJUST_DAYLIGHT | EFI_TIME_IN_DAYLIGHT)))
		)
	{
		Status = EFI_INVALID_PARAMETER;
		goto EXIT;
	}

	EpochSeconds = EfiTimeToEpoch(Time);


	// Adjust for the correct time zone, i.e. convert to UTC time zone
	if (Time->TimeZone != EFI_UNSPECIFIED_TIMEZONE)
	{
		EpochSeconds -= Time->TimeZone * SEC_PER_MIN;
	}

	// Adjust for the correct period
	if ((Time->Daylight & EFI_TIME_IN_DAYLIGHT) == EFI_TIME_IN_DAYLIGHT)
	{
		// Convert to un-adjusted time, i.e. fall back one hour
		EpochSeconds -= SEC_PER_HOUR;
	}

	OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
	Status = gPM8921->pm8xxx_rtc_set_time(gPM8921, EpochSeconds);
	gBS->RestoreTPL(OldTpl);
EXIT:
	return (Status == EFI_SUCCESS) ? Status : EFI_DEVICE_ERROR;
}

/**
  Returns the current wakeup alarm clock setting.

  @param  Enabled               Indicates if the alarm is currently enabled or disabled.
  @param  Pending               Indicates if the alarm signal is pending and requires acknowledgement.
  @param  Time                  The current alarm setting.

  @retval EFI_SUCCESS           The alarm settings were returned.
  @retval EFI_INVALID_PARAMETER Any parameter is NULL.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be retrieved due to a hardware error.

  **/
EFI_STATUS
EFIAPI
LibGetWakeupTime(
	OUT BOOLEAN     *Enabled,
	OUT BOOLEAN     *Pending,
	OUT EFI_TIME    *Time
)
{
	return EFI_UNSUPPORTED;
}

/**
  Sets the system wakeup alarm clock time.

  @param  Enabled               Enable or disable the wakeup alarm.
  @param  Time                  If Enable is TRUE, the time to set the wakeup alarm for.

  @retval EFI_SUCCESS           If Enable is TRUE, then the wakeup alarm was enabled. If
  Enable is FALSE, then the wakeup alarm was disabled.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The wakeup time could not be set due to a hardware error.
  @retval EFI_UNSUPPORTED       A wakeup timer is not supported on this platform.

  **/
EFI_STATUS
EFIAPI
LibSetWakeupTime(
	IN BOOLEAN      Enabled,
	OUT EFI_TIME    *Time
)
{
	return EFI_UNSUPPORTED;
}




/**
  This is the declaration of an EFI image entry point. This can be the entry point to an application
  written to this specification, an EFI boot service driver, or an EFI runtime driver.

  @param  ImageHandle           Handle that identifies the loaded image.
  @param  SystemTable           System Table for this image.

  @retval EFI_SUCCESS           The operation completed successfully.

  **/
EFI_STATUS
EFIAPI
LibRtcInitialize(
	IN EFI_HANDLE                            ImageHandle,
	IN EFI_SYSTEM_TABLE                      *SystemTable
)
{
	EFI_STATUS    Status;
	EFI_HANDLE    Handle;
	UINT8         Data;
	EFI_TPL       OldTpl;

	Status = gBS->LocateProtocol(&gQcomPM8921ProtocolGuid, NULL, (VOID **)&gPM8921);
	ASSERT_EFI_ERROR(Status);

	// Setup the setters and getters
	gRT->GetTime = LibGetTime;
	gRT->SetTime = LibSetTime;
	gRT->GetWakeupTime = LibGetWakeupTime;
	gRT->SetWakeupTime = LibSetWakeupTime;

	// Install the protocol
	Handle = NULL;
	Status = gBS->InstallMultipleProtocolInterfaces(
		&Handle,
		&gEfiRealTimeClockArchProtocolGuid, NULL,
		NULL
		);

	return Status;
}

/**
  Fixup internal data so that EFI can be call in virtual mode.
  Call the passed in Child Notify event and convert any pointers in
  lib to virtual mode.

  @param[in]    Event   The Event that is being processed
  @param[in]    Context Event Context
  **/
VOID
EFIAPI
LibRtcVirtualNotifyEvent(
IN EFI_EVENT        Event,
IN VOID             *Context
)
{
	return;
}
