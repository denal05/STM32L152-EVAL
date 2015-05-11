/**
  @page STM32L152_EVAL_Binary UM1009 STM32L152-EVAL Binary files Readme file
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    STM32L152-EVAL/Binary/readme.txt 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    24-January-2012
  * @brief   Description of the UM1009 "STM32L152-EVAL demonstration firmware" 
  *          Binary files.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  ******************************************************************************
   @endverbatim

@par Description

This directory contains a set of STM32L152-EVAL Demo binary files that should be
programmed to the internal FLASH using different methods: 
   - In-Circuit-Programming using the embedded bootloader
   - In-Application-Programming through USART
   - USB Device Firmware Upgrade (DFU)   

 - Using Bootloader:
    - To program the demonstration's binary image into the internal Flash memory,
      you have to use the "stm32l152_eval_fw_v1.2.0.bin" file located under 
      Project\STM32L152-EVAL\Binary with embedded Bootloader. 
      For more details, please refer to Bootloader application note AN2606 
      STM32 microcontroller system memory boot mode.

 - Using IAP:
    - To program the demonstration's binary image into the internal Flash memory,
      you have to use the "stm32l152-eval_fw_v1.2.0_offset_0x3000.bin" file 
      located under Project\STM32L152-EVAL\Binary with IAP over USART. For more 
      details, please refer to IAP application note AN3310 STM32L1xx in-application 
      programming using the USART. 

 - Using USB DFU:
    - To program the demonstration's binary image into the internal Flash memory,
      you have to use the stm32l152-eval_fw_v1.2.0.dfu file located under 
      Project\STM32L152-EVAL\Binary with USB DFU. For more details, please refer 
      to user manual UM0424 STM32 USB-FSDevice development kit.

@note
- Ultra Low Power Medium-density devices are STM32L151xx and STM32L152xx 
  microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
- Ultra Low Power Medium-density Plus devices are STM32L151xx, STM32L152xx and 
  STM32L162xx microcontrollers where the Flash memory density is 256 Kbytes.
- Ultra Low Power High-density devices are STM32L151xx, STM32L152xx and STM32L162xx 
  microcontrollers where the Flash memory density is 384 Kbytes.
 
 * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
 */
