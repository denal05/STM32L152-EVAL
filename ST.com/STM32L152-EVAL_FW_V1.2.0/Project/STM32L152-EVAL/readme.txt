/**
  @page STM32L152_EVAL_Demo UM1009 STM32L152-EVAL demonstration firmware Readme file
  
  @verbatim
  ******************** (C) COPYRIGHT 2012 STMicroelectronics *******************
  * @file    STM32L152-EVAL/readme.txt 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    24-January-2012
  * @brief   Description of the UM1009 "STM32L152-EVAL demonstration firmware".
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

This directory contains a set of sources files and pre-configured projects that
describes the demonstration firmware running on the STM32L152-EVAL evaluation 
board, which can be used to evaluate the capabilities of the STM32L ultra low power
medium density STM32L152VB microcontroller and on-board peripherals.

@par Directory contents 

 - "STM32L152-EVAL\inc": contains the STM32L152-EVAL firmware header files 
    - STM32L152-EVAL/inc/calendar.h                 Header for calendar.c
    - STM32L152-EVAL/inc/hw_config.h                Header for hw_config.c
    - STM32L152-EVAL/inc/iddmeasurement.h           Header for iddmeasurement.c
    - STM32L152-EVAL/inc/main.h                     Main header file
    - STM32L152-EVAL/inc/mass_mal.h                 Header for mass_mal.c
    - STM32L152-EVAL/inc/mass_storage.h             Header for mass_storage.c
    - STM32L152-EVAL/inc/memory.h                   Header for memory.c
    - STM32L152-EVAL/inc/menu.h                     Header for menu.c
    - STM32L152-EVAL/inc/platform_config.h          Evaluation board specific configuration file
    - STM32L152-EVAL/inc/stm32l1xx_clock_config.h   Header for stm32l1xx_clock_config.c
    - STM32L152-EVAL/inc/stm32l1xx_conf.h           Library Configuration file
    - STM32L152-EVAL/inc/stm32l1xx_it.h             Header for stm32l1xx_it.c  
    - STM32L152-EVAL/inc/storage.h                  Header for storage.c
    - STM32L152-EVAL/inc/usb_bot.h                  Header for usb_bot.c
    - STM32L152-EVAL/inc/usb_conf.h                 Mass Storage Demo configuration header
    - STM32L152-EVAL/inc/usb_desc.h                 Header for usb_desc.c
    - STM32L152-EVAL/inc/usb_istr.h                 Header for usb_istr.c
    - STM32L152-EVAL/inc/usb_prop.h                 Header for usb_prop.c
    - STM32L152-EVAL/inc/usb_pwr.h                  Header for usb_pwr.c
    - STM32L152-EVAL/inc/usb_scsi.h                 Header for usb_scsi.c
    - STM32L152-EVAL/inc/waveplayer.h               Header for waveplayer.c    
            
 - "STM32L152-EVAL\EWARM": contains pre-configured project for EWARM toolchain

 - "STM32L152-EVAL\HiTOP": contains pre-configured project for HiTOP toolchain

 - "STM32L152-EVAL\MDK-ARM": contains pre-configured project for MDK-ARM toolchain
 
 - "STM32L152-EVAL\RIDE": contains pre-configured project for RIDE toolchain
  
 - "STM32L152-EVAL\TrueSTUDIO": contains pre-configured project for TrueSTUDIO toolchain
 
 - "STM32L152-EVAL\src": contains the STM32L152-EVAL firmware source files
    - STM32L152-EVAL/src/calendar.c                 This file provides the firmware which includes the calendar driver.
    - STM32L152-EVAL/src/diskio.c                   This file provides the firmware which includes the FatFs driver.
    - STM32L152-EVAL/src/hw_config.c                This file provides the firmware which includes the USB Library Hardware Configuration & Setup.
    - STM32L152-EVAL/src/iddmeasurement.c           This file provides the firmware which includes the idd measurement driver.
    - STM32L152-EVAL/src/main.c                     Main program
    - STM32L152-EVAL/src/mass_mal.c                 This file provides the firmware which includes the USB Library Medium Access Layer interface            
    - STM32L152-EVAL/src/mass_storage.c             This file provides the firmware which includes the USB Library a set of functions needed to manage the communication between the STM32L15x USB and the SD Card 
    - STM32L152-EVAL/src/memory.c                   This file provides the firmware which includes the USB Library Memory management layer                       
    - STM32L152-EVAL/src/menu.c                     This file provides the firmware which includes the menu navigation driver.    
    - STM32L152-EVAL/src/scsi_data.c                This file provides the firmware which includes the USB Library Initialization of the SCSI data.
    - STM32L152-EVAL/src/stm32l1xx_clock_config.c   This file provides the firmware which includes the STM32L1xx different ready to use clock configuration.
    - STM32L152-EVAL/src/stm32l1xx_it.c             Interrupt handlers
    - STM32L152-EVAL/src/storage.c                  This file provides the firmware which includes the Storage (FatFs) driver.
    - STM32L152-EVAL/src/usb_bot.c                  This file provides the firmware which includes the USB Library BOT State Machine management. 
    - STM32L152-EVAL/src/usb_desc.c                 This file provides the firmware which includes the USB Library Descriptors for Mass Storage Device. 
    - STM32L152-EVAL/src/usb_endp.c                 This file provides the firmware which includes the USB Library Endpoint routines.
    - STM32L152-EVAL/src/usb_istr.c                 This file provides the firmware which includes the USB Library ISTR events interrupt service routines.      
    - STM32L152-EVAL/src/usb_prop.c                 This file provides the firmware which includes the USB Library All processing related to Mass Storage Demo (Endpoint 0). 
    - STM32L152-EVAL/src/usb_pwr.c                  This file provides the firmware which includes the USB Library Connection/disconnection & power management.
    - STM32L152-EVAL/src/usb_scsi.c                 This file provides the firmware which includes the USB Library All processing related to the SCSI commands.   
    - STM32L152-EVAL/src/waveplayer.c               This file provides the firmware which includes the wave player driver. 
    - STM32L152-EVAL/src/system_stm32l1xx.c         STM32L1xx system source file

@note The "system_stm32l1xx.c" is generated by an automatic clock configuration 
      system and can be easily customized to your own configuration. 
      To select different clock setup, use the "STM32L1xx_Clock_Configuration_V1.1.0.xls" 
      provided with the AN3309 package available on <a href="http://www.st.com/internet/mcu/family/141.jsp">  ST Microcontrollers </a>
       
@par Hardware and Software environment  

  - This firmware runs on STM32L15x Ultra Low Power Medium-Density Devices.
  
  - This firmware has been tested with STMicroelectronics STM32L152-EVAL 
    (STM32L15x Ultra Low Power Medium-Density) evaluation board and can be easily 
    tailored to any other supported device and development board.
       
@par How to use it ?       
      
In order to load the demo code, you have do the following:
 - EWARM
    - Open the STM32L152-EVAL_Demo.eww workspace
    - Rebuild all files: Project->Rebuild all
    - Load project image: Project->Debug
    - Run program: Debug->Go(F5)
   
 - HiTOP 
    - Open the HiTOP toolchain.
    - Browse to open the STM32L152-EVAL_Demo.htp
    - A "Download application" window is displayed, click "cancel".
    - Rebuild all files: Project->Rebuild all
    - Load project image : Click "ok" in the "Download application" window.
    - Run the "RESET_GO_MAIN" script to set the PC at the "main"
    - Run program: Debug->Go(F5).   

 - MDK-ARM 
    - Open the STM32L152-EVAL_Demo.Uv2 project
    - Rebuild all files: Project->Rebuild all target files
    - Load project image: Debug->Start/Stop Debug Session
    - Run program: Debug->Run (F5)

 - RIDE
    - Open the STM32L152-EVAL_Demo.rprj project
    - Load project image: Debug->start(ctrl+D)
    - Run program: Debug->Run(ctrl+F9) 
 
 - TrueSTUDIO 
    - Open the TrueSTUDIO toolchain.
    - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace 
      directory.
    - Click on File->Import, select General->'Existing Projects into Workspace' 
      and then click "Next". 
    - Browse to the TrueSTUDIO workspace directory and select the project: 
        - STM32L152-EVAL: to configure the project for STM32L Ultra Low Power 
           Medium-density devices
    - Rebuild all project files: Select the project in the "Project explorer" 
      window then click on Project->build project menu.
    - Run program: Select the project in the "Project explorer" window then click 
      Run->Debug (F11)  

@note
- Ultra Low Power Medium-density devices are STM32L151xx and STM32L152xx 
  microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
- Ultra Low Power Medium-density Plus devices are STM32L151xx, STM32L152xx and 
  STM32L162xx microcontrollers where the Flash memory density is 256 Kbytes.
- Ultra Low Power High-density devices are STM32L151xx, STM32L152xx and STM32L162xx 
  microcontrollers where the Flash memory density is 384 Kbytes.
  
 * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
 */

