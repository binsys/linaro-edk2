/** @file

 Copyright (c) 2011-2014, ARM Ltd. All rights reserved.<BR>
 This program and the accompanying materials
 are licensed and made available under the terms and conditions of the BSD License
 which accompanies this distribution.  The full text of the license may be found at
 http://opensource.org/licenses/bsd-license.php

 THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
 WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

 **/

#include <PiDxe.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Guid/GlobalVariable.h>

#include "LcdGraphicsOutputDxe.h"




BOOLEAN mDisplayInitialized = FALSE;


/***************************************
 * GraphicsOutput Protocol function, mapping to
 * EFI_GRAPHICS_OUTPUT_PROTOCOL.QueryMode
 ***************************************/
EFI_STATUS
EFIAPI
LcdGraphicsQueryMode(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL            *This,
	IN UINT32                                  ModeNumber,
	OUT UINTN                                  *SizeOfInfo,
	OUT EFI_GRAPHICS_OUTPUT_MODE_INFORMATION   **Info
)
{
	EFI_STATUS Status = EFI_SUCCESS;
	LCD_INSTANCE *Instance;
	DEBUG((EFI_D_ERROR, "DisplayDxe:LcdGraphicsQueryMode(ModeNumber=%d)!\n",ModeNumber));
	Instance = LCD_INSTANCE_FROM_GOP_THIS(This);

	// Setup the hardware if not already done
	if (!mDisplayInitialized)
	{
		Status = InitializeDisplay(Instance);
		if (EFI_ERROR(Status))
		{
			goto EXIT;
		}
	}

	// Error checking
	if ((This == NULL) || (Info == NULL) || (SizeOfInfo == NULL) || (ModeNumber >= This->Mode->MaxMode))
	{
		DEBUG((DEBUG_ERROR, "LcdGraphicsQueryMode: ERROR - For mode number %d : Invalid Parameter.\n", ModeNumber));
		Status = EFI_INVALID_PARAMETER;
		goto EXIT;
	}

	*Info = AllocatePool(sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION));
	if (*Info == NULL)
	{
		Status = EFI_OUT_OF_RESOURCES;
		goto EXIT;
	}

	*SizeOfInfo = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);

	Status = LcdPlatformQueryMode(ModeNumber, *Info);
	if (EFI_ERROR(Status))
	{
		FreePool(*Info);
	}

EXIT:
	return Status;
}

/***************************************
 * GraphicsOutput Protocol function, mapping to
 * EFI_GRAPHICS_OUTPUT_PROTOCOL.SetMode
 ***************************************/
EFI_STATUS
EFIAPI
LcdGraphicsSetMode(
	IN EFI_GRAPHICS_OUTPUT_PROTOCOL   *This,
	IN UINT32                         ModeNumber
)
{
	EFI_STATUS                      Status = EFI_SUCCESS;
	EFI_GRAPHICS_OUTPUT_BLT_PIXEL   FillColour;
	LCD_INSTANCE*                   Instance;
	LCD_BPP                         Bpp;

	Instance = LCD_INSTANCE_FROM_GOP_THIS(This);
	DEBUG((EFI_D_ERROR, "DisplayDxe:LcdGraphicsSetMode!\n"));
	// Setup the hardware if not already done
	if (!mDisplayInitialized)
	{
		Status = InitializeDisplay(Instance);
		if (EFI_ERROR(Status))
		{
			goto EXIT;
		}
	}

	// Check if this mode is supported
	if (ModeNumber >= This->Mode->MaxMode)
	{
		DEBUG((DEBUG_ERROR, "LcdGraphicsSetMode: ERROR - Unsupported mode number %d .\n", ModeNumber));
		Status = EFI_UNSUPPORTED;
		goto EXIT;
	}

	// Set the oscillator frequency to support the new mode
	Status = LcdPlatformSetMode(ModeNumber);
	if (EFI_ERROR(Status))
	{
		Status = EFI_DEVICE_ERROR;
		goto EXIT;
	}

	// Update the UEFI mode information
	This->Mode->Mode = ModeNumber;
	LcdPlatformQueryMode(ModeNumber, &Instance->ModeInfo);
	Status = LcdPlatformGetBpp(ModeNumber, &Bpp);
	if (EFI_ERROR(Status))
	{
		DEBUG((DEBUG_ERROR, "LcdGraphicsSetMode: ERROR - Couldn't get bytes per pixel, status: %r\n", Status));
		goto EXIT;
	}
	This->Mode->FrameBufferSize = Instance->ModeInfo.VerticalResolution
		* Instance->ModeInfo.PixelsPerScanLine
		* GetBytesPerPixel(Bpp);


	// The UEFI spec requires that we now clear the visible portions of the output display to black.

	// Set the fill colour to black
	SetMem(&FillColour, sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL), 0x0);

	// Fill the entire visible area with the same colour.
	Status = This->Blt(
		This,
		&FillColour,
		EfiBltVideoFill,
		0,
		0,
		0,
		0,
		This->Mode->Info->HorizontalResolution,
		This->Mode->Info->VerticalResolution,
		0);

EXIT:
	return Status;
}

UINTN
GetBytesPerPixel(
	IN  LCD_BPP       Bpp
)
{
	switch (Bpp)
	{
	case LCD_BITS_PER_PIXEL_24:
		return 3;

	case LCD_BITS_PER_PIXEL_16_565:
	case LCD_BITS_PER_PIXEL_16_555:
	case LCD_BITS_PER_PIXEL_12_444:
		return 2;

	case LCD_BITS_PER_PIXEL_8:
	case LCD_BITS_PER_PIXEL_4:
	case LCD_BITS_PER_PIXEL_2:
	case LCD_BITS_PER_PIXEL_1:
		return 1;

	default:
		return 0;
	}
}

QCOM_PM8921_PROTOCOL * PM8921;

#include <Library/qcom_lk.h>
#include <msm_panel.h>
#include <mdp4.h>
#include <display.h>
#include <Library/qcom_msm8960_clock.h>
#include <Library/qcom_msm8960_timer.h>

void target_display_init(const char *panel_name);
void target_display_shutdown(void);
int target_cont_splash_screen(void);

extern void msm_clocks_init(VOID);
extern int msm_display_init(struct msm_fb_panel_data *pdata);
extern int msm_display_on(void);
extern int msm_display_off(void);
extern void mipi_lgd_cmd_hd720p_init(struct msm_panel_info *pinfo);
extern void mipi_dsi_cmd_trigger(struct msm_fb_panel_data *pdata);
UINT8 display_enable;
struct msm_fb_panel_data panel;





#define PM8921_GPIO_LCD_DCDC_EN         11
#define PM8921_GPIO_PANEL_RESET         25
#define PM8921_GPIO_PANEL_ID            12


#define PM8921_GPIO_BL_LED_EN_MITWOA           22
#define PM8921_GPIO_BL_LED_EN_MITWO           13
#define LM3530_I2C_ADDR                 (0x38)

void panel_backlight_on_mitwoa(unsigned int on)
{

}



/* Pull DISP_RST_N high to get panel out of reset */
void msm8960_mipi_panel_reset(void)
{
	struct pm8921_gpio gpio43_param = 
	{
		.direction = PM_GPIO_DIR_OUT,
		.output_buffer = 0,
		.output_value = 1,
		.pull = PM_GPIO_PULL_UP_30,
		.vin_sel = 2,
		.out_strength = PM_GPIO_STRENGTH_HIGH,
		.function = PM_GPIO_FUNC_PAIRED,
		.inv_int_pol = 0,
		.disable_pin = 0,
	};
	
	
	PM8921->pm8921_gpio_config(PM8921,PM_GPIO(43), &gpio43_param);
}

int msm8960_mipi_panel_clock(int enable)
{
	if (enable) 
	{
		mdp_clock_init();
		mmss_clock_init();
	} 
	else if(!target_cont_splash_screen()) 
	{
		mmss_clock_disable();
	}

	return 0;
}

int msm8960_mipi_panel_power(int enable)
{
	if (enable) 
	{
		/* Turn on LDO8 for lcd1 mipi vdd */
		PM8921->pm8921_ldo_set_voltage(PM8921,LDO_8, LDO_VOLTAGE_3_0V);

		PM8921->pm8921_ldo_set_voltage(PM8921,LDO_29, LDO_VOLTAGE_1_8V);
		/* Turn on LDO23 for lcd1 mipi vddio */
		PM8921->pm8921_ldo_set_voltage(PM8921,LDO_23, LDO_VOLTAGE_1_8V);

		/* Turn on LDO2 for vdda_mipi_dsi */
		PM8921->pm8921_ldo_set_voltage(PM8921,LDO_2, LDO_VOLTAGE_1_2V);

		mdelay(3);
		msm8960_mipi_panel_reset();
		mdelay(5);
	}

	return 0;
}




EFI_STATUS
InitializeDisplay(
	IN LCD_INSTANCE* Instance
)
{
	EFI_STATUS             Status = EFI_SUCCESS;
	EFI_PHYSICAL_ADDRESS   VramBaseAddress;
	UINTN                  VramSize;
	DEBUG((EFI_D_ERROR, "DisplayDxe:InitializeDisplay!\n"));
	

	PM8921 = Instance->PM8921;
	
	Status = LcdPlatformGetVram(&VramBaseAddress, &VramSize);
	if (EFI_ERROR(Status))
	{
		return Status;
	}


	
	{
		msm_clocks_init();

		
		DEBUG((EFI_D_INFO , "display_init()\n"));

		mipi_lgd_cmd_hd720p_init(&(panel.panel_info));
		panel.clk_func = msm8960_mipi_panel_clock;
		panel.power_func = msm8960_mipi_panel_power;
		panel.fb.base = (void *)((UINT32)VramBaseAddress);
		panel.fb.width =  panel.panel_info.xres;
		panel.fb.height =  panel.panel_info.yres;
		panel.fb.stride =  panel.panel_info.xres;
		panel.fb.bpp =  panel.panel_info.bpp;
		panel.fb.format = FB_FORMAT_RGB888;
		panel.mdp_rev = MDP_REV_44;


		if (msm_display_init(&panel)) 
		{
			DEBUG((EFI_D_WARN,  "Display init failed!\n"));
			Status = EFI_INVALID_PARAMETER;
			goto EXIT_ERROR_LCD_SHUTDOWN;
		}

		mipi_dsi_cmd_trigger(&panel);

		mdelay(34);

		panel_backlight_on_mitwoa(1);

		display_enable = 1;
	}

	// Setup all the relevant mode information
	Instance->Gop.Mode->SizeOfInfo = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
	Instance->Gop.Mode->FrameBufferBase = VramBaseAddress;

	// Set the flag before changing the mode, to avoid infinite loops
	mDisplayInitialized = TRUE;

	// All is ok, so don't deal with any errors
	goto EXIT;

EXIT_ERROR_LCD_SHUTDOWN:
	DEBUG((DEBUG_ERROR, "InitializeDisplay: ERROR - Can not initialise the display. Exit Status=%r\n", Status));
	LcdShutdown();

EXIT:
	return Status;
}

int target_cont_splash_screen()
{
	return 0;
}





