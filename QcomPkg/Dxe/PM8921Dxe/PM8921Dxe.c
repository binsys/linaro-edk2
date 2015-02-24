
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/Timer.h>

#include <Protocol/PM8921.h>

pm8921_dev_t pmic;


EFI_STATUS EFIAPI j_pm8921_gpio_config(
	IN QCOM_PM8921_PROTOCOL *This,
	IN INT32 gpio,
	IN struct pm8921_gpio *param)
{
	return pm8921_gpio_config(gpio, param) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_gpio_get(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT8 gpio,
	OUT UINT8 *status)
{
	return pm8921_gpio_get(gpio, status) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}




EFI_STATUS EFIAPI j_pm8921_boot_done(
	IN QCOM_PM8921_PROTOCOL   *This
)
{
	pm8921_boot_done();
	return EFI_SUCCESS;
}


EFI_STATUS EFIAPI j_pm8921_ldo_set_voltage(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT32 ldo_id,
	IN UINT32 voltage
	)
{
	return pm8921_ldo_set_voltage(ldo_id, voltage) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}


EFI_STATUS EFIAPI j_pm8921_config_reset_pwr_off(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT32 reset
	)
{
	return pm8921_config_reset_pwr_off(reset) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_config_led_current(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN enum pm8921_leds led_num,
	IN UINT8 current,
	IN enum led_mode sink,
	IN INT32 enable
	)
{
	return pm8921_config_led_current(led_num, current, sink, enable) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_config_drv_keypad(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT32 drv_flash_sel,
	IN UINT32 flash_logic,
	IN UINT32 flash_ensel)
{
	return pm8921_config_drv_keypad(drv_flash_sel, flash_logic, flash_ensel) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_pwrkey_status(
	IN QCOM_PM8921_PROTOCOL   *This,
	OUT UINT8 *status
	)
{
	return pm8921_pwrkey_status(status) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_low_voltage_switch_enable(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT8 lvs_id
	)
{
	return pm8921_low_voltage_switch_enable(lvs_id) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_mpp_set_digital_output(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT8 mpp_id
	)
{
	return pm8921_mpp_set_digital_output(mpp_id) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_rtc_alarm_disable(
	IN QCOM_PM8921_PROTOCOL *This
	)
{
	return pm8921_rtc_alarm_disable() == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_pwm_enable(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT8 pwm_id
	)
{
	return pm8921_pwm_enable(pwm_id, &pmic) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}

EFI_STATUS EFIAPI j_pm8921_pwm_config(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT8 pwm_id,
	UINT32 duty_us,
	UINT32 period_us
	)
{
	return pm8921_pwm_config(pwm_id, duty_us, period_us, &pmic) == 0 ? EFI_SUCCESS : EFI_DEVICE_ERROR;
}



QCOM_PM8921_PROTOCOL gPM8921 =
{
	j_pm8921_gpio_config,
	j_pm8921_gpio_get,
	j_pm8921_boot_done,
	j_pm8921_ldo_set_voltage,
	j_pm8921_config_reset_pwr_off,
	j_pm8921_config_led_current,
	j_pm8921_config_drv_keypad,
	j_pm8921_pwrkey_status,
	j_pm8921_low_voltage_switch_enable,
	j_pm8921_mpp_set_digital_output,
	j_pm8921_rtc_alarm_disable,
	j_pm8921_pwm_enable,
	j_pm8921_pwm_config
};



/**
  @param  ImageHandle   of the loaded driver
  @param  SystemTable   Pointer to the System Table

  @retval EFI_SUCCESS           Protocol registered
  @retval EFI_OUT_OF_RESOURCES  Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      Hardware problems

  **/
EFI_STATUS
EFIAPI
PmicInitialize(
IN EFI_HANDLE         ImageHandle,
IN EFI_SYSTEM_TABLE   *SystemTable
)
{
	EFI_HANDLE  Handle = NULL;
	EFI_STATUS  Status = EFI_SUCCESS;

	ASSERT_PROTOCOL_ALREADY_INSTALLED(NULL, &gQcomPM8921ProtocolGuid);

	/* Initialize PMIC driver */
	pmic.read = (pm8921_read_func)&pa1_ssbi2_read_bytes;
	pmic.write = (pm8921_write_func)&pa1_ssbi2_write_bytes;

	pm8921_init(&pmic);


	Status = gBS->InstallMultipleProtocolInterfaces(
		&Handle,
		&gQcomPM8921ProtocolGuid, &gPM8921,
		NULL
		);

	ASSERT_EFI_ERROR(Status);

	return Status;
}

