################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
${USER_SS_SDK_PATH}/app/bluetooth/common/ota_dfu/sl_ota_dfu.c 

OBJS += \
./gecko_sdk_3.1.1/app/bluetooth/common/ota_dfu/sl_ota_dfu.o 

C_DEPS += \
./gecko_sdk_3.1.1/app/bluetooth/common/ota_dfu/sl_ota_dfu.d 


# Each subdirectory must supply rules for building sources it contributes
gecko_sdk_3.1.1/app/bluetooth/common/ota_dfu/sl_ota_dfu.o: ${USER_SS_SDK_PATH}/app/bluetooth/common/ota_dfu/sl_ota_dfu.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	${ARM_NONE_EABI_GCC} -g3 -gdwarf-2 -mcpu=cortex-m33 -mthumb -std=c99 '-DSL_RAIL_LIB_MULTIPROTOCOL_SUPPORT=0' '-DEFR32MG21A020F1024IM32=1' '-DSL_COMPONENT_CATALOG_PRESENT=1' '-DSL_RAIL_UTIL_PA_CONFIG_HEADER=<sl_rail_util_pa_config.h>' '-DMBEDTLS_CONFIG_FILE=<mbedtls_config.h>' -ID:/Project/GitHub/bluetooth_mesh_applications_staging/btmesh_temperature_log/inc -I"${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}\config" -I"${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}" -I"${USER_SS_SDK_PATH}//platform/common/toolchain/inc" -I"${USER_SS_SDK_PATH}//app/bluetooth/common/btmesh_factory_reset" -I"${USER_SS_SDK_PATH}//platform/service/iostream/inc" -I"${USER_SS_SDK_PATH}//platform/emdrv/nvm3/inc" -I"${USER_SS_SDK_PATH}//app/bluetooth/common/simple_timer" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_protocol_crypto/src" -I"${USER_SS_SDK_PATH}//platform/common/inc" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/mbedtls/include" -I"${USER_SS_SDK_PATH}//platform/emlib/inc" -I"${USER_SS_SDK_PATH}//platform/service/device_init/inc" -I"${USER_SS_SDK_PATH}//protocol/bluetooth/inc" -I"${USER_SS_SDK_PATH}//platform/service/hfxo_manager/inc" -I"${USER_SS_SDK_PATH}//platform/emdrv/common/inc" -I"${USER_SS_SDK_PATH}//hardware/board/inc" -I"${USER_SS_SDK_PATH}//platform/service/ram_interrupt_vector_init/inc" -I"${USER_SS_SDK_PATH}//app/bluetooth/common/app_log" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/se_manager/inc" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/se_manager/src" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/common" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/protocol/ble" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/protocol/ieee802154" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/protocol/zwave" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/protocol/mfm" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/chip/efr32/efr32xg2x" -I"${USER_SS_SDK_PATH}//app/bluetooth/common/btmesh_button_press" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_alt/include" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_mbedtls_support/inc" -I"${USER_SS_SDK_PATH}//util/third_party/printf" -I"${USER_SS_SDK_PATH}//util/third_party/printf/inc" -I"${USER_SS_SDK_PATH}//platform/driver/button/inc" -I"${USER_SS_SDK_PATH}//app/bluetooth/common/app_assert" -I"${USER_SS_SDK_PATH}//platform/service/sleeptimer/inc" -I"${USER_SS_SDK_PATH}//platform/Device/SiliconLabs/EFR32MG21/Include" -I"${USER_SS_SDK_PATH}//platform/service/mpu/inc" -I"${USER_SS_SDK_PATH}//platform/service/system/inc" -I"${USER_SS_SDK_PATH}//platform/emdrv/gpiointerrupt/inc" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/plugin/rail_util_pti" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_psa_driver/inc" -I"${USER_SS_SDK_PATH}//platform/service/power_manager/inc" -I"${USER_SS_SDK_PATH}//util/silicon_labs/silabs_core/memory_manager" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/plugin/pa-conversions" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/plugin/pa-conversions/efr32xg21" -I"${USER_SS_SDK_PATH}//app/bluetooth/common/ota_dfu" -I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/plugin/rail_util_rf_path" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_mbedtls_support/config" -I"${USER_SS_SDK_PATH}//util/third_party/crypto/mbedtls/library" -I"${USER_SS_SDK_PATH}//platform/bootloader" -I"${USER_SS_SDK_PATH}//platform/bootloader/api" -I"${USER_SS_SDK_PATH}//platform/CMSIS/Include" -I"${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}\autogen" -Os -Wall -Wextra -fno-builtin -ffunction-sections -fdata-sections -imacrossl_gcc_preinclude.h -mfpu=fpv5-sp-d16 -mfloat-abi=hard -c -fmessage-length=0 -MMD -MP -MF"gecko_sdk_3.1.1/app/bluetooth/common/ota_dfu/sl_ota_dfu.d" -MT"gecko_sdk_3.1.1/app/bluetooth/common/ota_dfu/sl_ota_dfu.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


