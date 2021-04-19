################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include env_dir_path.mk
-include sources.mk
-include temperature/subdir.mk
-include gecko_sdk_3.1.1/util/third_party/printf/src/subdir.mk
-include gecko_sdk_3.1.1/util/third_party/printf/subdir.mk
-include gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/subdir.mk
-include gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_protocol_crypto/src/subdir.mk
-include gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_mbedtls_support/src/subdir.mk
-include gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_alt/source/subdir.mk
-include gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/subdir.mk
-include gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/subdir.mk
-include gecko_sdk_3.1.1/util/silicon_labs/silabs_core/memory_manager/subdir.mk
-include gecko_sdk_3.1.1/protocol/bluetooth/src/subdir.mk
-include gecko_sdk_3.1.1/protocol/bluetooth/lib/EFR32MG21/GCC/subdir.mk
-include gecko_sdk_3.1.1/platform/service/system/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/sleeptimer/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/ram_interrupt_vector_init/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/power_manager/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/mpu/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/iostream/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/hfxo_manager/src/subdir.mk
-include gecko_sdk_3.1.1/platform/service/device_init/src/subdir.mk
-include gecko_sdk_3.1.1/platform/radio/rail_lib/plugin/rail_util_rf_path/subdir.mk
-include gecko_sdk_3.1.1/platform/radio/rail_lib/plugin/rail_util_pti/subdir.mk
-include gecko_sdk_3.1.1/platform/radio/rail_lib/plugin/pa-conversions/subdir.mk
-include gecko_sdk_3.1.1/platform/emlib/src/subdir.mk
-include gecko_sdk_3.1.1/platform/emdrv/nvm3/src/subdir.mk
-include gecko_sdk_3.1.1/platform/emdrv/gpiointerrupt/src/subdir.mk
-include gecko_sdk_3.1.1/platform/driver/leddrv/src/subdir.mk
-include gecko_sdk_3.1.1/platform/driver/button/src/subdir.mk
-include gecko_sdk_3.1.1/platform/common/toolchain/src/subdir.mk
-include gecko_sdk_3.1.1/platform/common/src/subdir.mk
-include gecko_sdk_3.1.1/platform/bootloader/api/subdir.mk
-include gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG21/Source/GCC/subdir.mk
-include gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG21/Source/subdir.mk
-include gecko_sdk_3.1.1/hardware/board/src/subdir.mk
-include gecko_sdk_3.1.1/app/bluetooth/common/simple_timer/subdir.mk
-include gecko_sdk_3.1.1/app/bluetooth/common/ota_dfu/subdir.mk
-include gecko_sdk_3.1.1/app/bluetooth/common/btmesh_provisioning_decorator/subdir.mk
-include gecko_sdk_3.1.1/app/bluetooth/common/btmesh_factory_reset/subdir.mk
-include gecko_sdk_3.1.1/app/bluetooth/common/btmesh_button_press/subdir.mk
-include autogen/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: soc_btmesh_data_logging_server_xg21.axf

# Tool invocations
soc_btmesh_data_logging_server_xg21.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	${ARM_NONE_EABI_GCC} -g3 -gdwarf-2 -mcpu=cortex-m33 -mthumb -T "${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}/autogen/linkerfile.ld" -Xlinker --gc-sections -Xlinker -Map="soc_btmesh_data_logging_server_xg21.map" -mfpu=fpv5-sp-d16 -mfloat-abi=hard --specs=nano.specs -o soc_btmesh_data_logging_server_xg21.axf -Wl,--start-group "./app.o" "./app_properties.o" "./main.o" "./sl_btmesh_data_logging_capi.o" "./sl_btmesh_data_logging_server.o" "./autogen/gatt_db.o" "./autogen/sl_bluetooth.o" "./autogen/sl_board_default_init.o" "./autogen/sl_btmesh.o" "./autogen/sl_btmesh_dcd.o" "./autogen/sl_device_init_clocks.o" "./autogen/sl_event_handler.o" "./autogen/sl_iostream_handles.o" "./autogen/sl_iostream_init_usart_instances.o" "./autogen/sl_power_manager_handler.o" "./autogen/sl_simple_button_instances.o" "./autogen/sl_simple_led_instances.o" "./gecko_sdk_3.1.1/app/bluetooth/common/btmesh_button_press/sl_btmesh_button_press.o" "./gecko_sdk_3.1.1/app/bluetooth/common/btmesh_factory_reset/sl_btmesh_factory_reset.o" "./gecko_sdk_3.1.1/app/bluetooth/common/btmesh_provisioning_decorator/sl_btmesh_provisioning_decorator.o" "./gecko_sdk_3.1.1/app/bluetooth/common/ota_dfu/sl_ota_dfu.o" "./gecko_sdk_3.1.1/app/bluetooth/common/simple_timer/sl_simple_timer.o" "./gecko_sdk_3.1.1/hardware/board/src/sl_board_control_gpio.o" "./gecko_sdk_3.1.1/hardware/board/src/sl_board_init.o" "./gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG21/Source/GCC/startup_efr32mg21.o" "./gecko_sdk_3.1.1/platform/Device/SiliconLabs/EFR32MG21/Source/system_efr32mg21.o" "./gecko_sdk_3.1.1/platform/bootloader/api/btl_interface.o" "./gecko_sdk_3.1.1/platform/bootloader/api/btl_interface_storage.o" "./gecko_sdk_3.1.1/platform/common/src/sl_slist.o" "./gecko_sdk_3.1.1/platform/common/toolchain/src/sl_memory.o" "./gecko_sdk_3.1.1/platform/driver/button/src/sl_button.o" "./gecko_sdk_3.1.1/platform/driver/button/src/sl_simple_button.o" "./gecko_sdk_3.1.1/platform/driver/leddrv/src/sl_led.o" "./gecko_sdk_3.1.1/platform/driver/leddrv/src/sl_simple_led.o" "./gecko_sdk_3.1.1/platform/emdrv/gpiointerrupt/src/gpiointerrupt.o" "./gecko_sdk_3.1.1/platform/emdrv/nvm3/src/nvm3_default_common_linker.o" "./gecko_sdk_3.1.1/platform/emdrv/nvm3/src/nvm3_hal_flash.o" "./gecko_sdk_3.1.1/platform/emdrv/nvm3/src/nvm3_lock.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_acmp.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_assert.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_burtc.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_cmu.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_core.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_dbg.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_emu.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_gpcrc.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_gpio.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_i2c.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_iadc.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_ldma.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_letimer.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_msc.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_prs.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_rmu.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_rtcc.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_se.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_system.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_timer.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_usart.o" "./gecko_sdk_3.1.1/platform/emlib/src/em_wdog.o" "./gecko_sdk_3.1.1/platform/radio/rail_lib/plugin/pa-conversions/pa_conversions_efr32.o" "./gecko_sdk_3.1.1/platform/radio/rail_lib/plugin/pa-conversions/pa_curves_efr32.o" "./gecko_sdk_3.1.1/platform/radio/rail_lib/plugin/rail_util_pti/sl_rail_util_pti.o" "./gecko_sdk_3.1.1/platform/radio/rail_lib/plugin/rail_util_rf_path/sl_rail_util_rf_path.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_emu_s2.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_hfrco.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_hfxo_s2.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_lfxo_s2.o" "./gecko_sdk_3.1.1/platform/service/device_init/src/sl_device_init_nvic.o" "./gecko_sdk_3.1.1/platform/service/hfxo_manager/src/sl_hfxo_manager.o" "./gecko_sdk_3.1.1/platform/service/hfxo_manager/src/sl_hfxo_manager_hal_s2.o" "./gecko_sdk_3.1.1/platform/service/iostream/src/sl_iostream.o" "./gecko_sdk_3.1.1/platform/service/iostream/src/sl_iostream_uart.o" "./gecko_sdk_3.1.1/platform/service/iostream/src/sl_iostream_usart.o" "./gecko_sdk_3.1.1/platform/service/mpu/src/sl_mpu.o" "./gecko_sdk_3.1.1/platform/service/power_manager/src/sl_power_manager.o" "./gecko_sdk_3.1.1/platform/service/power_manager/src/sl_power_manager_debug.o" "./gecko_sdk_3.1.1/platform/service/power_manager/src/sl_power_manager_hal_s2.o" "./gecko_sdk_3.1.1/platform/service/ram_interrupt_vector_init/src/sl_ram_interrupt_vector_init.o" "./gecko_sdk_3.1.1/platform/service/sleeptimer/src/sl_sleeptimer.o" "./gecko_sdk_3.1.1/platform/service/sleeptimer/src/sl_sleeptimer_hal_burtc.o" "./gecko_sdk_3.1.1/platform/service/sleeptimer/src/sl_sleeptimer_hal_prortc.o" "./gecko_sdk_3.1.1/platform/service/sleeptimer/src/sl_sleeptimer_hal_rtcc.o" "./gecko_sdk_3.1.1/platform/service/system/src/sl_system_init.o" "./gecko_sdk_3.1.1/platform/service/system/src/sl_system_process_action.o" "./gecko_sdk_3.1.1/protocol/bluetooth/src/sl_bt_mbedtls_context.o" "./gecko_sdk_3.1.1/protocol/bluetooth/src/sl_btmesh_lib.o" "./gecko_sdk_3.1.1/protocol/bluetooth/src/sl_btmesh_serdeser.o" "./gecko_sdk_3.1.1/util/silicon_labs/silabs_core/memory_manager/sl_malloc.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/aes.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/bignum.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/ccm.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/cipher.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/cipher_wrap.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/cmac.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/ctr_drbg.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/ecdh.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/ecdsa.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/ecp.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/ecp_curves.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/entropy.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/error.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/hmac_drbg.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/platform.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/platform_util.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/sha256.o" "./gecko_sdk_3.1.1/util/third_party/crypto/mbedtls/library/threading.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_attestation.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_cipher.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_entropy.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_hash.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_key_derivation.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_key_handling.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_signature.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/se_manager/src/sl_se_manager_util.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_alt/source/sl_entropy.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_alt/source/sl_mbedtls.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_alt/source/sl_se_management.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_mbedtls_support/src/mbedtls_cmac.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_mbedtls_support/src/mbedtls_ecdsa_ecdh.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_mbedtls_support/src/mbedtls_sha.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_mbedtls_support/src/se_aes.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_mbedtls_support/src/se_ccm.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_protocol_crypto/src/sli_protocol_crypto_radioaes.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_protocol_crypto/src/sli_radioaes_management.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_psa_driver_init.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_driver_aead.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_driver_builtin_keys.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_driver_cipher.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_driver_key_derivation.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_driver_key_management.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_driver_mac.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_driver_signature.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_opaque_driver_aead.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_opaque_driver_cipher.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_opaque_driver_mac.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_opaque_key_derivation.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_transparent_driver_aead.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_transparent_driver_cipher.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_transparent_driver_hash.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_transparent_driver_mac.o" "./gecko_sdk_3.1.1/util/third_party/crypto/sl_component/sl_psa_driver/src/sli_se_transparent_key_derivation.o" "./gecko_sdk_3.1.1/util/third_party/printf/printf.o" "./gecko_sdk_3.1.1/util/third_party/printf/src/iostream_printf.o" "./temperature/sl_btmesh_temperature.o" "${USER_SS_SDK_PATH}//platform/emdrv/nvm3/lib/libnvm3_CM33_gcc.a" "${USER_SS_SDK_PATH}//protocol/bluetooth/lib/EFR32MG21/GCC/libbluetooth.a" "${USER_SS_SDK_PATH}//platform/radio/rail_lib/autogen/librail_release/librail_efr32xg21_gcc_release.a" "${USER_SS_SDK_PATH}//protocol/bluetooth/lib/EFR32MG21/GCC/binapploader.o" "${USER_SS_SDK_PATH}//protocol/bluetooth/lib/EFR32MG21/GCC/libbluetooth_mesh.a" -lgcc -lc -lm -lnosys -Wl,--end-group -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: soc_btmesh_data_logging_server_xg21.hex'
	${ARM_NONE_EABI_OBJCOPY} -O ihex "soc_btmesh_data_logging_server_xg21.axf" "soc_btmesh_data_logging_server_xg21.hex"
	@echo ' '

	@echo 'Building bin file: soc_btmesh_data_logging_server_xg21.bin'
	${ARM_NONE_EABI_OBJCOPY} -O binary "soc_btmesh_data_logging_server_xg21.axf" "soc_btmesh_data_logging_server_xg21.bin"
	@echo ' '

	@echo 'Building s37 file: soc_btmesh_data_logging_server_xg21.s37'
	${ARM_NONE_EABI_OBJCOPY} -O srec "soc_btmesh_data_logging_server_xg21.axf" "soc_btmesh_data_logging_server_xg21.s37"
	@echo ' '

	@echo 'Running size tool'
	${ARM_NONE_EABI_SIZE} "soc_btmesh_data_logging_server_xg21.axf" -A
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) soc_btmesh_data_logging_server_xg21.axf
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
