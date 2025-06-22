################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/TouchGFXProjects/HID.12/MouseHID/USB_DEVICE/Target/usbd_conf.c 

C_DEPS += \
./Application/User/USB_DEVICE/Target/usbd_conf.d 

OBJS += \
./Application/User/USB_DEVICE/Target/usbd_conf.o 


# Each subdirectory must supply rules for building sources it contributes
Application/User/USB_DEVICE/Target/usbd_conf.o: C:/TouchGFXProjects/HID.12/MouseHID/USB_DEVICE/Target/usbd_conf.c Application/User/USB_DEVICE/Target/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DUSE_BPP=16 -DDEBUG -DSTM32F429xx -c -I../../Core/Inc -I../../Drivers/CMSIS/Include -I../../Drivers/BSP -I../../TouchGFX/target -I../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../TouchGFX/App -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../TouchGFX/target/generated -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../Middlewares/ST/touchgfx/framework/include -I../../TouchGFX/generated/fonts/include -I../../TouchGFX/generated/gui_generated/include -I../../TouchGFX/generated/images/include -I../../TouchGFX/generated/texts/include -I../../TouchGFX/gui/include -I../../TouchGFX/generated/videos/include -I../../USB_DEVICE/App -I../../USB_DEVICE/Target -I../../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../../Middlewares/ST/STM32_USB_Device_Library/Class/HID/Inc -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Application-2f-User-2f-USB_DEVICE-2f-Target

clean-Application-2f-User-2f-USB_DEVICE-2f-Target:
	-$(RM) ./Application/User/USB_DEVICE/Target/usbd_conf.cyclo ./Application/User/USB_DEVICE/Target/usbd_conf.d ./Application/User/USB_DEVICE/Target/usbd_conf.o ./Application/User/USB_DEVICE/Target/usbd_conf.su

.PHONY: clean-Application-2f-User-2f-USB_DEVICE-2f-Target

