/**
  @page ewarm EWARM Project Template for STM32L1xx Ultra Low Power devices
  
  @verbatim
  ******************* (C) COPYRIGHT 2012 STMicroelectronics ********************
  * @file    readme.txt
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   This sub-directory contains all the user-modifiable files needed
  *          to create a new project linked with the STM32L1xx Standard Peripheral 
  *          Library and working with IAR Embedded Workbench for ARM (EWARM)
  *          software toolchain (version 6.30 and later).
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
 
 @par Directory contents

 - project .ewd/.eww/.ewp: A pre-configured project file with the provided library 
                           structure that produces an executable image with IAR 
                           Embedded Workbench.
                
 - stm32l1xx_flash.icf : This file is the IAR Linker configuration file used to 
                         place program code (readonly) in internal FLASH and data
                         (readwrite, Stack and Heap)in internal SRAM. 
                         You can customize this file to your need.

 - stm32l1xx_flash_extsram.icf : This file is the IAR Linker configuration file 
                                 used to place program code (readonly) in internal 
                                 FLASH and data (readwrite, Stack and Heap)in 
                                 external SRAM. 
                                 You can customize this file to your need.

 - stm32l1xx_ram.icf:  This file is the IAR Linker configuration file used to 
                       place program code (readonly) and data (readwrite, Stack 
                       and Heap)in internal SRAM. 
                       You can customize this file to your need.
                             
 @par How to use it ?

 - Open the Project.eww workspace.
 - In the workspace toolbar select the project config:
     - STM32L152-EVAL: to configure the project for STM32L Ultra Low Power 
                       Medium-density devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: USE_STDPERIPH_DRIVER, STM32L1XX_MD, USE_STM32L152_EVAL
     - STM32L152D-EVAL: to configure the project for STM32L Ultra Low Power 
                        High-density devices.
     @note The needed define symbols for this config are already declared in the
           preprocessor section: USE_STDPERIPH_DRIVER, STM32L1XX_HD, USE_STM32L152D_EVAL

 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Debug
 - Run program: Debug->Go(F5)

@note
- Ultra Low Power Medium-density devices are STM32L151xx and STM32L152xx 
  microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
- Ultra Low Power Medium-density Plus devices are STM32L151xx, STM32L152xx and 
  STM32L162xx microcontrollers where the Flash memory density is 256 Kbytes.
- Ultra Low Power High-density devices are STM32L151xx, STM32L152xx and STM32L162xx 
  microcontrollers where the Flash memory density is 384 Kbytes.

 * <h3><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h3>
 */
