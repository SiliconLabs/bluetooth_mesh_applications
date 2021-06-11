#workspace location path

OSFLAG 	:=
OS_DISTINCT_FLAG :=
UNAME_S :=
ifeq ($(OS),Windows_NT)
	OSFLAG += -D WIN32
	OS_DISTINCT_FLAG := WINDOWS
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		OSFLAG += -D AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		OSFLAG += -D IA32
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OSFLAG += -D LINUX
		OS_DISTINCT_FLAG := LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		OSFLAG += -D OSX
		OS_DISTINCT_FLAG := MAC
	endif
endif

CUR_PWD :=
ifeq ($(OS_DISTINCT_FLAG), WINDOWS)
	CUR_PWD := $(shell cygpath -w $(shell pwd))
else
	ifeq ($(OS_DISTINCT_FLAG), LINUX)
		CUR_PWD := $(shell pwd -L)
	endif
endif

USER_SS_WORKSPACE := ${CUR_PWD}/../..
USER_SS_SDK_VER := gecko_sdk_3.1.1
USER_SS_SDK_PATH := C:/SiliconLabs/SimplicityStudio/v5/developer/sdks/gecko_sdk_suite/v3.1
USER_SS_COMPILER_PATH := C:/SiliconLabs/SimplicityStudio/v5/developer/toolchains/gnu_arm/7.2_2017q4/bin
USER_SOURCE_DIR := ${USER_SS_WORKSPACE}/..

ifndef USER_SS_SDK_PATH
$(error USER_SS_SDK_PATH is not set)
endif

ifndef USER_SS_COMPILER_PATH
$(error USER_SS_COMPILER_PATH is not set)
endif

USER_SS_PROJECT_NAME := soc_btmesh_data_logging_server_xg21

ifeq ($(OS_DISTINCT_FLAG), WINDOWS)
	# compiler set-up
	ARM_NONE_EABI_GCC := ${USER_SS_COMPILER_PATH}/arm-none-eabi-gcc.exe
	ARM_NONE_EABI_OBJCOPY := ${USER_SS_COMPILER_PATH}/arm-none-eabi-objcopy.exe
	ARM_NONE_EABI_SIZE := ${USER_SS_COMPILER_PATH}/arm-none-eabi-size.exe
else
	ifeq ($(OS_DISTINCT_FLAG), LINUX)
		# compiler set-up
		ARM_NONE_EABI_GCC := ${USER_SS_COMPILER_PATH}/arm-none-eabi-gcc
		ARM_NONE_EABI_OBJCOPY := ${USER_SS_COMPILER_PATH}/arm-none-eabi-objcopy
		ARM_NONE_EABI_SIZE := ${USER_SS_COMPILER_PATH}/arm-none-eabi-size
	endif
endif


#library
LIB_OF_SS5_PATH :=
# user's object file
#LIB_OF_USER_PATH :="${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}/dcmc/ad5940/siecg/siecg.a"

INCLUDE_PATH := \
-I"${USER_SOURCE_DIR}/inc" \
-I"${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}/config" \
-I"${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}" \
-I"${USER_SS_SDK_PATH}//platform/common/toolchain/inc" \
-I"${USER_SS_SDK_PATH}//app/bluetooth/common/btmesh_factory_reset" \
-I"${USER_SS_SDK_PATH}//platform/service/iostream/inc" \
-I"${USER_SS_SDK_PATH}//platform/emdrv/nvm3/inc" \
-I"${USER_SS_SDK_PATH}//app/bluetooth/common/simple_timer" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_protocol_crypto/src" \
-I"${USER_SS_SDK_PATH}//platform/common/inc" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/mbedtls/include" \
-I"${USER_SS_SDK_PATH}//platform/emlib/inc" \
-I"${USER_SS_SDK_PATH}//platform/service/device_init/inc" \
-I"${USER_SS_SDK_PATH}//protocol/bluetooth/inc" \
-I"${USER_SS_SDK_PATH}//platform/service/hfxo_manager/inc" \
-I"${USER_SS_SDK_PATH}//platform/emdrv/common/inc" \
-I"${USER_SS_SDK_PATH}//hardware/board/inc" \
-I"${USER_SS_SDK_PATH}//platform/service/ram_interrupt_vector_init/inc" \
-I"${USER_SS_SDK_PATH}//app/bluetooth/common/app_log" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/se_manager/inc" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/se_manager/src" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/common" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/protocol/ble" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/protocol/ieee802154" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/protocol/zwave" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/protocol/mfm" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/chip/efr32/efr32xg2x" \
-I"${USER_SS_SDK_PATH}//app/bluetooth/common/btmesh_button_press" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_alt/include" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_mbedtls_support/inc" \
-I"${USER_SS_SDK_PATH}//util/third_party/printf" \
-I"${USER_SS_SDK_PATH}//util/third_party/printf/inc" \
-I"${USER_SS_SDK_PATH}//platform/driver/button/inc" \
-I"${USER_SS_SDK_PATH}//app/bluetooth/common/app_assert" \
-I"${USER_SS_SDK_PATH}//platform/service/sleeptimer/inc" \
-I"${USER_SS_SDK_PATH}//platform/Device/SiliconLabs/EFR32MG21/Include" \
-I"${USER_SS_SDK_PATH}//platform/service/mpu/inc" \
-I"${USER_SS_SDK_PATH}//platform/service/system/inc" \
-I"${USER_SS_SDK_PATH}//platform/emdrv/gpiointerrupt/inc" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/plugin/rail_util_pti" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_psa_driver/inc" \
-I"${USER_SS_SDK_PATH}//platform/service/power_manager/inc" \
-I"${USER_SS_SDK_PATH}//util/silicon_labs/silabs_core/memory_manager" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/plugin/pa-conversions" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/plugin/pa-conversions/efr32xg21" \
-I"${USER_SS_SDK_PATH}//app/bluetooth/common/ota_dfu" \
-I"${USER_SS_SDK_PATH}//platform/radio/rail_lib/plugin/rail_util_rf_path" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/sl_component/sl_mbedtls_support/config" \
-I"${USER_SS_SDK_PATH}//util/third_party/crypto/mbedtls/library" \
-I"${USER_SS_SDK_PATH}//platform/bootloader" \
-I"${USER_SS_SDK_PATH}//platform/bootloader/api" \
-I"${USER_SS_SDK_PATH}//platform/CMSIS/Include" \
-I"${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}/autogen"

PRE_PROCESSOR_DEFINE := '-DSL_COMPONENT_CATALOG_PRESENT=1' \
'-DSL_RAIL_UTIL_PA_CONFIG_HEADER=<sl_rail_util_pa_config.h>' \
'-DMBEDTLS_CONFIG_FILE=<mbedtls_config.h>' \
'-DEFR32MG21A020F1024IM32=1'

# linker script path
LINKER_SCRIPT_PATH := "${USER_SS_WORKSPACE}/${USER_SS_PROJECT_NAME}/GNU_ARM_v7.2.1_Debug/soc_btmesh_data_logging_server_xg21.ld"