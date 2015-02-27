
#ifndef __QCOM_PM8921_ROTOCOL_H__
#define __QCOM_PM8921_ROTOCOL_H__


#include <Library/qcom_lk.h>
#include <Library/qcom_ssbi.h>
#include <Library/qcom_pm8921.h>
#include <Library/qcom_pm8921_hw.h>
#include <Library/qcom_pm8921_pwm.h>
#include <Library/qcom_pm8921_rtc.h>
//
// Protocol GUID
//
// {F0843787-2270-4E68-BD83-45F38A878826}
#define QCOM_PM8921_PROTOCOL_GUID = { 0xf0843787, 0x2270, 0x4e68, { 0xbd, 0x83, 0x45, 0xf3, 0x8a, 0x87, 0x88, 0x26 } }


//
// Protocol interface structure
//
typedef struct _QCOM_PM8921_PROTOCOL  QCOM_PM8921_PROTOCOL;



typedef
EFI_STATUS
(EFIAPI *PM8921_GPIO_CONFIG)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	IN INT32 gpio,
	IN struct pm8921_gpio *param
);


typedef
EFI_STATUS
(EFIAPI *PM8921_GPIO_GET)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	IN UINT8 gpio, 
	OUT UINT8 *status
);

typedef
EFI_STATUS
(EFIAPI *PM8921_BOOT_DONE)(
	IN QCOM_PM8921_PROTOCOL   *This
);


typedef
EFI_STATUS
(EFIAPI *PM8921_LDO_SET_VOLTAGE)(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT32 ldo_id,
	IN UINT32 voltage
);


typedef
EFI_STATUS
(EFIAPI *PM8921_CONFIG_RESET_PWR_OFF)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	IN UINT32 reset
);

typedef
EFI_STATUS
(EFIAPI *PM8921_CONFIG_LED_CURRENT)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	IN enum pm8921_leds led_num, 
	IN UINT8 current, 
	IN enum led_mode sink, 
	IN INT32 enable
);


typedef
EFI_STATUS
(EFIAPI *PM8921_CONFIG_DRV_KEYPAD)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	IN UINT32 drv_flash_sel, 
	IN UINT32 flash_logic, 
	IN UINT32 flash_ensel);


typedef
EFI_STATUS
(EFIAPI *PM8921_PWRKEY_STATUS)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	OUT UINT8 *status
);


typedef
EFI_STATUS
(EFIAPI *PM8921_LOW_VOLTAGE_SWITCH_ENABLE)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	IN UINT8 lvs_id
);


typedef
EFI_STATUS
(EFIAPI *PM8921_MPP_SET_DIGITAL_OUTPUT)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	IN UINT8 mpp_id
);


typedef
EFI_STATUS
(EFIAPI *PM8921_RTC_ALARM_DISABLE)(
	IN QCOM_PM8921_PROTOCOL *This
);


typedef
EFI_STATUS
(EFIAPI *PM8921_PWM_ENABLE)(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT8 pwm_id
);


typedef
EFI_STATUS
(EFIAPI *PM8921_PWM_CONFIG)(
	IN QCOM_PM8921_PROTOCOL   *This, 
	IN UINT8 pwm_id, 
	UINT32 duty_us, 
	UINT32 period_us
);


typedef
EFI_STATUS
(EFIAPI *PM8XXX_RTC_READ_TIME)(
	IN QCOM_PM8921_PROTOCOL   *This,
	OUT UINT32* secs
);


typedef
EFI_STATUS
(EFIAPI *PM8XXX_RTC_SET_TIME)(
	IN QCOM_PM8921_PROTOCOL   *This,
	IN UINT32 secs
);


struct _QCOM_PM8921_PROTOCOL
{
	PM8921_GPIO_CONFIG                  pm8921_gpio_config;
	PM8921_GPIO_GET                     pm8921_gpio_get;
	PM8921_BOOT_DONE                    pm8921_boot_done;
	PM8921_LDO_SET_VOLTAGE              pm8921_ldo_set_voltage;
	PM8921_CONFIG_RESET_PWR_OFF         pm8921_config_reset_pwr_off;
	PM8921_CONFIG_LED_CURRENT           pm8921_config_led_current;
	PM8921_CONFIG_DRV_KEYPAD            pm8921_config_drv_keypad;
	PM8921_PWRKEY_STATUS                pm8921_pwrkey_status;
	PM8921_LOW_VOLTAGE_SWITCH_ENABLE    pm8921_low_voltage_switch_enable;
	PM8921_MPP_SET_DIGITAL_OUTPUT       pm8921_mpp_set_digital_output;
	PM8921_RTC_ALARM_DISABLE            pm8921_rtc_alarm_disable;
	PM8921_PWM_ENABLE                   pm8921_pwm_enable;
	PM8921_PWM_CONFIG                   pm8921_pwm_config;
	PM8XXX_RTC_READ_TIME                pm8xxx_rtc_read_time;
	PM8XXX_RTC_SET_TIME                 pm8xxx_rtc_set_time;
};


extern EFI_GUID gQcomPM8921ProtocolGuid;

#endif
