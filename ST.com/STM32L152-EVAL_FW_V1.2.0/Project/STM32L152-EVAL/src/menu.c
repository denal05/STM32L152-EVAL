/**
  ******************************************************************************
  * @file    menu.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    24-January-2012
  * @brief   This file includes the menu navigation driver for the STM32L152-EVAL demonstration.
  ******************************************************************************
  * @attention
  *
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
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L152_EVAL_Demo
  * @{
  */

/** @defgroup MENU
  * @brief    This file includes the menu navigation driver for the STM32L152-EVAL
  *           demonstration.
  * @{
  */

/** @defgroup MENU_Private_Types
  * @{
  */
typedef void (* tMenuFunc)(void);
typedef struct sMenuItem * tMenuItem;
typedef struct sMenu * tMenu;
/**
  * @}
  */


/** @defgroup MENU_Private_Defines
  * @{
  */
#define NUMBER_OF_ICONS         9
#define NUMBER_OF_SLIDES        14
#define TEMPERATURE_THYS        31
#define TEMPERATURE_TOS         32
/**
  * @}
  */

/** @defgroup MENU_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup MENU_Private_Variables
  * @{
  */

uint8_t MenuItemIndex = 0, nMenuLevel = 0;
uint8_t ItemNumb[MAX_MENU_LEVELS];
uint8_t NumberOfFiles = 0;
uint32_t bmplen = 0;
extern uint8_t Buffer1[];
extern UINT BytesWritten;
extern UINT BytesRead;
char* DirectoryFiles[MAX_FILES];
tMenuItem psMenuItem, psCurrentMenuItem;
tMenu psPrevMenu[MAX_MENU_LEVELS];
tMenu psCurrentMenu;
extern FIL F;
extern FATFS fs;
static uint8_t TempCelsiusDisplay[]     = "       +abc.d C     ";
uint8_t TempFahrenheitDisplay[]  = "       +abc.d F     ";
static int32_t TempValue = 0, TempValueCelsius = 0, TempValueFahrenheit = 0;
__IO uint8_t SMbusAlertOccurred, JumperConfIndex = 0;
struct sMenuItem
{
  uint8_t* pszTitle;
  tMenuFunc pfMenuFunc;
  tMenuFunc pfUpDownMenuFunc;
  tMenu psSubMenu;
};
struct sMenu
{
  uint8_t* pszTitle;
  tMenuItem psItems;
  uint8_t nItems;
};
const uint8_t* Images1Addr[3] =
  {"STFILES/STLOGO.BMP", "STFILES/HELP.BMP", "STFILES/STM32L.BMP"
  };
const uint8_t* Images2Addr[2] =
  {"STFILES/ALARM.BMP", "STFILES/WATCH.BMP"
  };
const uint8_t* IconsAddr[NUMBER_OF_ICONS] =
  {
    "STFILES/ICON1.BMP",
    "STFILES/ICON2.BMP",
    "STFILES/ICON3.BMP",
    "STFILES/ICON4.BMP",
    "STFILES/ICON5.BMP",
    "STFILES/ICON6.BMP",
    "STFILES/ICON7.BMP",
    "STFILES/ICON8.BMP",
    "STFILES/ICON9.BMP",
  };
const uint8_t* SlideAddr[14] =
  {
    "STFILES/SLIDE1.BMP",
    "STFILES/SLIDE2.BMP",
    "STFILES/SLIDE3.BMP",
    "STFILES/SLIDE4.BMP",
    "STFILES/SLIDE5.BMP",
    "STFILES/SLIDE6.BMP",
    "STFILES/SLIDE7.BMP",
    "STFILES/SLIDE8.BMP",
    "STFILES/SLIDE9.BMP",
    "STFILES/SLIDE10.BMP",
    "STFILES/SLIDE11.BMP",
    "STFILES/SLIDE12.BMP",
    "STFILES/SLIDE13.BMP",
    "STFILES/SLIDE14.BMP"
  };

const uint32_t SlidesSpeech[14] =
  {
    0x000000, 0x031A00, 0x06A200, 0x079000, 0x0A7600, 0x104000, 0x132600,
    0x176600, 0x1B9C00, 0x1D8200, 0x21F400, 0x24CE00, 0x28E400, 0x2BB200
  };

const uint32_t DelayTiming[14] =
  {
    2650, 3020, 800, 2480, 4950, 2480, 3640, 3600, 1630, 3800, 2520, 3500, 2400, 1770
  };

const uint8_t SlidesCheck[6] =
  {
    0x42, 0x4D, 0x42, 0x58, 0x02, 0x00
  };
const uint8_t Icons64Check[6] =
  {
    0x42, 0x4D, 0x42, 0x20, 0x00, 0x00
  };
const uint8_t Icons128Check[6] =
  {
    0x42, 0x4D, 0x42, 0x80, 0x00, 0x00
  };

#define SCROLL_SPEED  2500 /* Low value gives higher speed */
#define SCROLL_NUM    1
uint8_t LCD_String[8] = " STM32L ";

/*------------------------------ Menu level 4 -------------------------------*/
/*------------------------------ Menu level 3 -------------------------------*/

struct sMenuItem SettingTimeMenuItems[] =
  {
    {"       Adjust       ", Calendar_TimeAdjust, Menu_IdleFunc},
    {"        Show        ", Calendar_TimeShow, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu TimeMenu =
  {"        Time        ", SettingTimeMenuItems, countof(SettingTimeMenuItems)
  };

struct sMenuItem SettingDateMenuItems[] =
  {
    {"       Adjust       ", Calendar_DateAdjust, Menu_IdleFunc},
    {"        Show        ", Calendar_DateShow, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu DateMenu =
  {"        Date        ", SettingDateMenuItems, countof(SettingDateMenuItems)
  };

struct sMenuItem SettingAlarmBMenuItems[] =
  {
    {"       Adjust       ", Calendar_AlarmAdjust_B, Menu_IdleFunc},
    {"        Show        ", Calendar_AlarmShow_B, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu AlarmBMenu =
  {"       AlarmB       ", SettingAlarmBMenuItems, countof(SettingAlarmBMenuItems)
  };

struct sMenuItem SettingAlarmAMenuItems[] =
  {
    {"       Adjust       ", Calendar_AlarmAdjust_A, Menu_IdleFunc},
    {"        Show        ", Calendar_AlarmShow_A, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu AlarmAMenu =
  {"       AlarmA       ", SettingAlarmAMenuItems, countof(SettingAlarmAMenuItems)
  };

struct sMenuItem SettingBiasMeasurementItems[] =
  {
    {"    Measure Bias    ", IDD_Measurement_Calibrate, Menu_IdleFunc},
    {"  Reset Bias Value  ", IDD_Measurement_Bias_Reset, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu BiasMeasurement =
  {"  Bias Measurement  ", SettingBiasMeasurementItems, countof(SettingBiasMeasurementItems)
  };

/*------------------------------ Menu level 2 -------------------------------*/
struct sMenuItem AboutMenuItems[] =
  {
    {"        About       ", Menu_AboutFunc, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu AboutMenu =
  {"        About       ", AboutMenuItems, countof(AboutMenuItems)
  };

struct sMenuItem HelpMenuItems[] =
  {
    {"    Menu Navigation ", Menu_MenuNavigationFunc, Menu_IdleFunc},
    {"    Jumpers Config  ", Menu_JumpConfFunc, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu HelpMenu =
  {
    "        Help        ", HelpMenuItems, countof(HelpMenuItems)
  };

struct sMenuItem USBMassMenuItems[] =
  {
    {"       Start        ", Mass_Storage_Start, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu USBMassMenu =
  {"  USB Mass Storage  ", USBMassMenuItems, countof(USBMassMenuItems)
  };

struct sMenuItem TempSensorMenuItems[] =
  {
    {"    Temperature     ", Menu_ThermometerFun, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu TempSensorMenu =
  {"    Thermometer     ", TempSensorMenuItems, countof(TempSensorMenuItems)
  };

struct sMenuItem IDDMeasureMenuItems[] =
  {
    {" IDD RUN 32MHz Mode ", IDD_Measurement_Run_HCLKTo32, Menu_IdleFunc},
    {" IDD RUN 1MHz Mode  ", IDD_Measurement_Run_HCLKToMSI_1MHz, Menu_IdleFunc},
    {"  IDD RUN LP Mode   ", IDD_Measurement_RunLP, Menu_IdleFunc},
    {"   IDD SLEEP Mode   ", IDD_Measurement_Sleep, Menu_IdleFunc},
    {" IDD SLEEP LP Mode  ", IDD_Measurement_SleepLP, Menu_IdleFunc},
    {"   IDD STOP Mode    ", IDD_Measurement_Stop, Menu_IdleFunc},
    {"  IDD STANDBY Mode  ", IDD_Measurement_EnterStandby, Menu_IdleFunc},
    {"  Bias Measurement  ", Menu_IdleFunc, Menu_IdleFunc, &BiasMeasurement},    
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu IDDMeasureMenu =
  {"    IDD Measure     ", IDDMeasureMenuItems, countof(IDDMeasureMenuItems)
  };

struct sMenuItem AudioMenuItems[] =
  {
    {"      WavePlayer    ", Menu_WavePlayerFunc, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu AudioMenu =
  {"        Audio       ", AudioMenuItems, countof(AudioMenuItems)
  };

struct sMenuItem ImagesViewerMenuItems[] =
  {
    {"    Images Viewer   ", Menu_ImagesViewerFunc, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu ImagesViewerMenu =
  {"    Images Viewer   ", ImagesViewerMenuItems, countof(ImagesViewerMenuItems)
  };

struct sMenuItem CalendarMenuItems[] =
  {
    {"        Time        ", Menu_IdleFunc, Menu_IdleFunc, &TimeMenu
    },
    {"        Date        ", Menu_IdleFunc, Menu_IdleFunc, &DateMenu},
    {"       AlarmA       ", Menu_IdleFunc, Menu_IdleFunc, &AlarmAMenu},
    {"       AlarmB       ", Menu_IdleFunc, Menu_IdleFunc, &AlarmBMenu},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu CalendarMenu =
  {"       Calendar     ", CalendarMenuItems, countof(CalendarMenuItems)
  };

struct sMenuItem ProdPresMenuItems[] =
  {
    {"       Start        ", Menu_ProductPres, Menu_IdleFunc},
    {"       Return       ", Menu_ReturnFunc, Menu_IdleFunc}
  };

struct sMenu ProdPresMenu =
  {"Product Presentation", ProdPresMenuItems, countof(ProdPresMenuItems)
  };

/*------------------------------ Menu level 1 -------------------------------*/
struct sMenuItem MainMenuItems[] =
  {
    {"Product Presentation", Menu_IdleFunc, Menu_IdleFunc, &ProdPresMenu
    },
    {"       Calendar     ", Menu_IdleFunc, Menu_IdleFunc, &CalendarMenu},
    {"    Images Viewer   ", Menu_IdleFunc, Menu_IdleFunc, &ImagesViewerMenu},
    {"        Audio       ", Menu_IdleFunc, Menu_IdleFunc, &AudioMenu},
    {"    IDD Measure     ", Menu_IdleFunc, Menu_IdleFunc, &IDDMeasureMenu},
    {"    Thermometer     ", Menu_IdleFunc, Menu_IdleFunc, &TempSensorMenu},
    {"  USB Mass Storage  ", Menu_IdleFunc, Menu_IdleFunc, &USBMassMenu},
    {"        Help        ", Menu_IdleFunc, Menu_IdleFunc, &HelpMenu},
    {"        About       ", Menu_IdleFunc, Menu_IdleFunc, &AboutMenu}
  };

struct sMenu MainMenu =
  {"     Main menu      ", MainMenuItems, countof(MainMenuItems)
  };

/**
  * @}
  */

/** @defgroup MENU_Private_FunctionPrototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup MENU_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the navigation menu.
  * @param  None
  * @retval None
  */
void Menu_Init(void)
{
  psCurrentMenu = &MainMenu;
  psPrevMenu[nMenuLevel] = psCurrentMenu;
  psMenuItem = MainMenuItems;
}


/**
  * @brief  Displays the current menu.
  * @param  None
  * @retval None
  */
void Menu_DisplayMenu(void)
{
  uint32_t Line = 0, index = 0;

  tMenuItem psMenuItem2;
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  /* Clear the LCD Screen */
  LCD_Clear(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(Line, psCurrentMenu->pszTitle);
  Line += 24;
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_WHITE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_BLUE);
  while (!(index >= (psCurrentMenu->nItems)))
  {
    psMenuItem2 = &(psCurrentMenu->psItems[index]);
    LCD_DisplayStringLine(Line, psMenuItem2->pszTitle);
    index++;
    Line += 24;
  }
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_GREEN);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  /* Get the current menu */
  psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
  LCD_DisplayStringLine(((MenuItemIndex + 1) * 24), psMenuItem->pszTitle);
}

/**
  * @brief  This function is executed when "SEL" push-buttton is pressed.
  * @param  None
  * @retval None
  */
void Menu_SelFunc(void)
{
  psCurrentMenuItem = psMenuItem;
  if (psMenuItem->psSubMenu != '\0')
  {
    /* Update the current Item by the submenu */
    MenuItemIndex = 0;
    psCurrentMenu = psMenuItem->psSubMenu;
    psMenuItem = &(psCurrentMenu->psItems)[MenuItemIndex];
    Menu_DisplayMenu();
    nMenuLevel++;
    psPrevMenu[nMenuLevel] = psCurrentMenu;
  }
  psCurrentMenuItem->pfMenuFunc();
}


/**
  * @brief  This function is executed when any of "UP" push-butttons is pressed.
  * @param  None
  * @retval None
  */
void Menu_UpFunc(void)
{
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_WHITE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_BLUE);
  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  LCD_DisplayStringLine(((MenuItemIndex + 1) * 24), psMenuItem->pszTitle);
  if (MenuItemIndex > 0)
  {
    MenuItemIndex--;
  }
  else
  {
    MenuItemIndex = psCurrentMenu->nItems - 1;
  }
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_GREEN);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  LCD_DisplayStringLine(((MenuItemIndex + 1) * 24), psMenuItem->pszTitle);
  ItemNumb[nMenuLevel] = MenuItemIndex;
}

/**
  * @brief  This function is executed when any of "Down" push-butttons is pressed.
  * @param  None
  * @retval None
  */
void Menu_DownFunc(void)
{
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_WHITE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_BLUE);
  psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
  LCD_DisplayStringLine(((MenuItemIndex + 1) * 24), psMenuItem->pszTitle);

  /* Test on the MenuItemIndex value before incrementing it */
  if (MenuItemIndex >= ((psCurrentMenu->nItems) - 1))
  {
    MenuItemIndex = 0;
  }
  else
  {
    MenuItemIndex++;
  }
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_GREEN);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  /* Get the current menu */
  psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
  LCD_DisplayStringLine(((MenuItemIndex + 1) * 24), psMenuItem->pszTitle);
  ItemNumb[nMenuLevel] = MenuItemIndex;
}

/**
  * @brief  This function is executed when the "RETURN" menu is selected.
  * @param  None
  * @retval None
  */
void Menu_ReturnFunc(void)
{
  psMenuItem->pfUpDownMenuFunc();
  if (nMenuLevel == 0)
  {
    nMenuLevel++;
  }
  psCurrentMenu = psPrevMenu[nMenuLevel-1];
  psMenuItem = &psCurrentMenu->psItems[0];
  ItemNumb[nMenuLevel] = 0;
  MenuItemIndex = 0;
  nMenuLevel--;
  if (nMenuLevel != 0)
  {
    Menu_DisplayMenu();
  }
  else
  {
    Menu_ShowMenuIcons();
  }
}

/**
  * @brief  Reads key from demoboard.
  * @param  None
  * @retval Return RIGHT, LEFT, SEL, UP, DOWN or NOKEY
  */
uint8_t Menu_ReadKey(void)
{
  /* "right" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_RIGHT))
  {
    while (STM_EVAL_PBGetState(BUTTON_RIGHT) == Bit_RESET);
    return RIGHT;
  }
  /* "left" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_LEFT))
  {
    while (STM_EVAL_PBGetState(BUTTON_LEFT) == Bit_RESET);
    return LEFT;
  }
  /* "up" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_UP))
  {
    while (STM_EVAL_PBGetState(BUTTON_UP) == Bit_RESET);
    return UP;
  }
  /* "down" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_DOWN))
  {
    while (STM_EVAL_PBGetState(BUTTON_DOWN) == Bit_RESET);
    return DOWN;
  }
  /* "sel" key is pressed */
  if (!STM_EVAL_PBGetState(BUTTON_SEL))
  {
    while (STM_EVAL_PBGetState(BUTTON_SEL) == Bit_RESET);
    return SEL;
  }
  /* No key is pressed */
  else
  {
    return NOKEY;
  }
}

/**
  * @brief  Idle function.
  * @param  None
  * @retval None
  */
void Menu_IdleFunc(void)
{
  /* Nothing to execute: return */
  return;
}

/**
  * @brief  Display menu icons.
  * @param  None
  * @retval None
  */
void Menu_DisplayIcons(void)
{
  uint32_t i = 0, j = 0, l = 0,  iconline = 0, iconcolumn = 0;

  iconline = 98;
  iconcolumn = 290 ;

  for (i = 0; i < 3; i++)
  {
    for (j = 0; j < 4; j++)
    {
      LCD_SetDisplayWindow(iconline, iconcolumn, 64, 64);
      Storage_OpenReadFile(iconline, iconcolumn, (const char*)IconsAddr[l]);
      iconcolumn -= 65;
      l++;
      if (l == NUMBER_OF_ICONS )
      {
        return;
      }
    }
    iconline += 65;
    iconcolumn = 290;
  }
}

/**
  * @brief  Show the main menu icon.
  * @param  None
  * @retval None
  */
void Menu_ShowMenuIcons(void)
{
  uint32_t pressedkey = 0, i = 0;
  uint16_t IconRect[12][2] = {{34, 290}, {34, 225}, {34, 160}, {34, 95},
                              {99, 290}, {99, 225}, {99, 160}, {99, 95},
                              {164, 290}, {164, 225}, {164, 160}, {164, 95}};

  /* Disable the JoyStick interrupts */
  Demo_IntExtOnOffConfig(DISABLE);

  while (Menu_ReadKey() != NOKEY)
  {}

  /* Initializes the Menu state machine */
  Menu_Init();

  MenuItemIndex = 0;

  /* Clear*/
  LCD_Clear(LCD_COLOR_WHITE);

  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);

  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);

  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_WHITE);

  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_BLUE);

  /* Displays Icons */
  Menu_DisplayIcons();

  /* Disable LCD Window mode */
  LCD_WindowModeDisable();


  LCD_DrawRect(IconRect[0][0], IconRect[0][1], 64, 65);


  /* Endless loop */
  while (1)
  {
    /* Check which key is pressed */
    pressedkey = Menu_ReadKey();

    /* If "UP" pushbutton is pressed */
    if (pressedkey == UP)
    {
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);

      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 64, 65);

      if (i <= 3)
      {
        i += 8;
        if (i >= NUMBER_OF_ICONS)
        {
          i = (NUMBER_OF_ICONS - 1);
        }
      }
      else
      {
        i -= 4;
      }
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_BLUE);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 64, 65);

      /* Set the Back Color */
      LCD_SetBackColor(LCD_COLOR_BLUE);
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      /* Test on the MenuItemIndex value before incrementing it */
      if (MenuItemIndex <= 3)
      {
        MenuItemIndex += 8;
        if (MenuItemIndex >= NUMBER_OF_ICONS)
        {
          MenuItemIndex = (NUMBER_OF_ICONS - 1);
        }
      }
      else
      {
        MenuItemIndex -= 4;
      }
      /* Get the current menu */
      psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
      LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
      ItemNumb[nMenuLevel] = MenuItemIndex;
    }
    /* If "DOWN" pushbutton is pressed */
    if (pressedkey == DOWN)
    {
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 64, 65);
      if (i >= 8)
      {
        i -= 8;
      }
      else
      {
        i += 4;
        if (i >= NUMBER_OF_ICONS)
        {
          i = (NUMBER_OF_ICONS - 1);
        }
      }
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_BLUE);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 64, 65);

      /* Set the Back Color */
      LCD_SetBackColor(LCD_COLOR_BLUE);
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      /* Test on the MenuItemIndex value before incrementing it */
      if (MenuItemIndex >= 8)
      {
        MenuItemIndex -= 8;
      }
      else
      {
        MenuItemIndex += 4;
        if (MenuItemIndex >= NUMBER_OF_ICONS)
        {
          MenuItemIndex = (NUMBER_OF_ICONS - 1);
        }
      }
      /* Get the current menu */
      psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
      LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
      ItemNumb[nMenuLevel] = MenuItemIndex;
    }
    /* If "LEFT" pushbutton is pressed */
    if (pressedkey == LEFT)
    {
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 64, 65);
      if (i == 0)
      {
        i = (NUMBER_OF_ICONS - 1);
      }
      else
      {
        i--;
      }
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_BLUE);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 64, 65);

      /* Set the Back Color */
      LCD_SetBackColor(LCD_COLOR_BLUE);
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      if (MenuItemIndex > 0)
      {
        MenuItemIndex--;
      }
      else
      {
        MenuItemIndex = psCurrentMenu->nItems - 1;
      }

      psMenuItem = &psCurrentMenu->psItems[MenuItemIndex];
      LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
      ItemNumb[nMenuLevel] = MenuItemIndex;
    }
    /* If "RIGHT" pushbutton is pressed */
    if (pressedkey == RIGHT)
    {
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 64, 65);
      if (i == (NUMBER_OF_ICONS - 1))
      {
        i = 0x00;
      }
      else
      {
        i++;
      }
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_BLUE);
      LCD_DrawRect(IconRect[i][0], IconRect[i][1], 64, 65);

      /* Set the Back Color */
      LCD_SetBackColor(LCD_COLOR_BLUE);
      /* Set the Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      /* Test on the MenuItemIndex value before incrementing it */
      if (MenuItemIndex >= ((psCurrentMenu->nItems) - 1))
      {
        MenuItemIndex = 0;
      }
      else
      {
        MenuItemIndex++;
      }
      /* Get the current menu */
      psMenuItem = &(psCurrentMenu->psItems[MenuItemIndex]);
      LCD_DisplayStringLine(LCD_LINE_0, psMenuItem->pszTitle);
      ItemNumb[nMenuLevel] = MenuItemIndex;
    }
    /* If "SEL" pushbutton is pressed */
    if (pressedkey == SEL)
    {
      Menu_SelFunc();
      Demo_IntExtOnOffConfig(ENABLE);
      return;
    }
  }
}

/**
  * @brief  Display the STM32 introduction.
  * @param  None
  * @retval None
  */
void Menu_STM32Intro(void)
{
  /* Disable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, DISABLE);

  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_WHITE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_BLUE);

  LCD_SetDisplayWindow(239, 0x13F, 240, 320);
  Storage_OpenReadFile(239, 319, "STFILES/STLOGO.BMP");
  /*Display " STM32L " string on LCD glass in scrolling mode*/
  LCD_GLASS_ScrollString("   ST   ", SCROLL_NUM, SCROLL_SPEED);

  LCD_SetDisplayWindow(239, 0x13F, 240, 320);
  Storage_OpenReadFile(239, 319, "STFILES/STM32L.BMP");
  /*Display " STM32L " string on LCD glass in scrolling mode*/
  LCD_GLASS_ScrollString(LCD_String, SCROLL_NUM, SCROLL_SPEED);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

}

/**
  * @brief  Display the push buttons configuration menu.
  * @param  None
  * @retval None
  */
void Menu_MenuNavigationFunc(void)
{
  Demo_IntExtOnOffConfig(DISABLE);
  LCD_SetDisplayWindow(239, 0x13F, 240, 320);
  Storage_OpenReadFile(239, 319, "STFILES/HELP.BMP");
  while (Menu_ReadKey() == NOKEY)
  {}
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_0, "UP, DOWN, RIGHT and ");
  LCD_DisplayStringLine(LCD_LINE_1, "LEFT push-buttons   ");
  LCD_DisplayStringLine(LCD_LINE_2, "perform circular    ");
  LCD_DisplayStringLine(LCD_LINE_3, "navigation in the   ");
  LCD_DisplayStringLine(LCD_LINE_4, "main menu, current  ");
  LCD_DisplayStringLine(LCD_LINE_5, "menu items. SEL     ");
  LCD_DisplayStringLine(LCD_LINE_6, "push-button selects ");
  LCD_DisplayStringLine(LCD_LINE_7, "the current item. UP");
  LCD_DisplayStringLine(LCD_LINE_8, "and DOWN perform    ");
  LCD_DisplayStringLine(LCD_LINE_9, "vertical navigation.");

  while (Menu_ReadKey() == NOKEY)
  {}
  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  Demo_IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Display the needed Jumpers configuration menu.
  * @param  None
  * @retval None
  */
void Menu_JumpConfFunc(void)
{
  uint8_t pressedkey = 0;
  LCD_Clear(LCD_COLOR_WHITE);
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_BLACK);
  LCD_DisplayStringLine(LCD_LINE_0, " The needed Jumpers ");
  LCD_DisplayStringLine(LCD_LINE_1, "    Configuration:  ");
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  
  while (pressedkey != SEL)
  {
    if (JumperConfIndex == 0x00)
    { 
      LCD_DisplayStringLine(LCD_LINE_3, "Use RIGHT and LEFT  ");
      LCD_DisplayStringLine(LCD_LINE_4, "to go to the        ");
      LCD_DisplayStringLine(LCD_LINE_5, "next/previous slide ");
      LCD_DisplayStringLine(LCD_LINE_6, "and SEL to exit     ");
    }  
    
    if (JumperConfIndex == 0x01)
    {
      LCD_DisplayStringLine(LCD_LINE_2, "- VDD Voltage:      ");
      LCD_DisplayStringLine(LCD_LINE_3, "JP13 fitted pos 3.3V");
      LCD_DisplayStringLine(LCD_LINE_4, "- IDD Measure demo: ");
      LCD_DisplayStringLine(LCD_LINE_5, "JP14 fitted and JP4 ");
      LCD_DisplayStringLine(LCD_LINE_6, "fitted pos 2<->3    ");
      LCD_DisplayStringLine(LCD_LINE_7, "- Thermometre demo: ");
      LCD_DisplayStringLine(LCD_LINE_8, "JP11: fitted        ");
      LCD_DisplayStringLine(LCD_LINE_9, "                    ");
    }

    if (JumperConfIndex == 0x02)
    {
      LCD_DisplayStringLine(LCD_LINE_2, "- LED3 and LED4:     ");
      LCD_DisplayStringLine(LCD_LINE_3, "JP18 and JP19 fitted");
      LCD_DisplayStringLine(LCD_LINE_4, "- SD card detect:   ");
      LCD_DisplayStringLine(LCD_LINE_5, "JP2 fitted          ");
      LCD_DisplayStringLine(LCD_LINE_6, "- LCD Glass:        ");
      LCD_DisplayStringLine(LCD_LINE_7, "JP7 and JP8 fitted  ");
      LCD_DisplayStringLine(LCD_LINE_8, "pos 1<->2           ");
      LCD_DisplayStringLine(LCD_LINE_9, "                    ");
    }
    
    if (JumperConfIndex == 0x03)
    {
      LCD_DisplayStringLine(LCD_LINE_2, "- LCD Glass Contrast");
      LCD_DisplayStringLine(LCD_LINE_3, "JP10 fitted pos LDR ");
      LCD_DisplayStringLine(LCD_LINE_4, "- Audio Out:        ");
      LCD_DisplayStringLine(LCD_LINE_5, "JP16 not fitted     ");
      LCD_DisplayStringLine(LCD_LINE_6, "- JP5, JP6 and JP9: ");
      LCD_DisplayStringLine(LCD_LINE_7, "not used by the demo");
      LCD_DisplayStringLine(LCD_LINE_8, "                    ");
      LCD_DisplayStringLine(LCD_LINE_9, "                    ");
    }
    
    if (JumperConfIndex == 0x04)
    {
      LCD_DisplayStringLine(LCD_LINE_2, "The IDD bias current");
      LCD_DisplayStringLine(LCD_LINE_3, "in stop and standby ");
      LCD_DisplayStringLine(LCD_LINE_4, "modes is evaluated  ");
      LCD_DisplayStringLine(LCD_LINE_5, "to 0.2uA.           ");
      LCD_DisplayStringLine(LCD_LINE_6, "Refer to section:   ");
      LCD_DisplayStringLine(LCD_LINE_7, "IDD measurement     ");
      LCD_DisplayStringLine(LCD_LINE_8, "improvment procedure");
      LCD_DisplayStringLine(LCD_LINE_9, "in the user manual. ");
    }
    
    pressedkey = Menu_ReadKey();
    while (pressedkey == NOKEY)
    {
      pressedkey = Menu_ReadKey();
    }
    if (pressedkey == RIGHT)
    {
      if (JumperConfIndex == 0x04)
      {
        JumperConfIndex = 0x01;
      }
      else
      {
        JumperConfIndex++;
      }
    }
    if (pressedkey == LEFT)
    {
      if (JumperConfIndex == 0x01)
      {
        JumperConfIndex = 0x04;
      }
      else
      {
        JumperConfIndex--;
      }
    }
  }
  JumperConfIndex = 0x00;
  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  Demo_IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Display the Images stored on the SDcard.
  * @param  None
  * @retval None
  */
void Menu_ImagesViewerFunc(void)
{
  uint8_t str[20], pressedkey = 0;
  int8_t i = 0;

  LCD_Clear(LCD_COLOR_WHITE);
  Demo_IntExtOnOffConfig(DISABLE);

  while (Menu_ReadKey() != NOKEY)
  {}

  for (i = 0; i < MAX_FILES; i++)
  {
    DirectoryFiles[i] = malloc(13); /** Initialize the DirectoryFiles pointers (heap) */
  }

  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  NumberOfFiles = Storage_GetDirectoryBitmapFiles ("/USER", DirectoryFiles); /** Get the BMP file names on root directory  Maximum 10 files */
  i = 0;

  if (NumberOfFiles == 0)
  {
    for (i = 0; i < MAX_FILES; i++)
    {
      free(DirectoryFiles[i]);
    }
    LCD_Clear(LCD_COLOR_WHITE);
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLACK);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_0, "No Bitmap files...  ");
    LCD_DisplayStringLine(LCD_LINE_1, "Exit:  Push JoyStick");
    while (Menu_ReadKey() == NOKEY)
    {}
    LCD_Clear(LCD_COLOR_WHITE);
    Menu_DisplayMenu();
    Demo_IntExtOnOffConfig(ENABLE);
    f_mount(0, &fs);
    f_open (&F, (const char*)IconsAddr[0], FA_READ);
    return;
  }
  while (i < NumberOfFiles)
  {
    sprintf ((char*)str, "/USER/%-11.11s", DirectoryFiles[i]);/** Format the string */
    if (Storage_CheckBitmapFile((const char*)str, &bmplen) == 0)
    {
      LCD_SetDisplayWindow(239, 0x13F, 240, 320);
      Storage_OpenReadFile(239, 319, (const char*)str);
      pressedkey = Demo_DelayJoyStick(1000);
      if (pressedkey == LEFT)
      {
        if (i == 0)
        {
          i = NumberOfFiles;
        }
        i--;
      }
      else if (pressedkey == RIGHT)
      {
        if (i == (NumberOfFiles - 1))
        {
          i = -1;
        }
        i++;
      }
      else if (pressedkey == SEL)
      {
        i = NumberOfFiles;
      }
      else if (pressedkey == NOKEY)
      {
        i++;
      }
    }
    else
    {
      LCD_Clear(LCD_COLOR_WHITE);
      /* Set the Back Color */
      LCD_SetBackColor(LCD_COLOR_BLUE);
      /* Set the Text LCD_COLOR_WHITE */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      LCD_ClearLine(LCD_LINE_3);
      LCD_DisplayStringLine(LCD_LINE_3, (uint8_t *) str);
      LCD_DisplayStringLine(LCD_LINE_4, "Bitmap file is not  ");
      LCD_DisplayStringLine(LCD_LINE_5, "supported.          ");
      LCD_DisplayStringLine(LCD_LINE_6, "Press JoyStick to   ");
      LCD_DisplayStringLine(LCD_LINE_7, "continue.           ");
      pressedkey = Demo_DelayJoyStick(100);
      if (pressedkey == LEFT)
      {
        if (i == 0)
        {
          i = NumberOfFiles;
        }
        i--;
      }
      else if (pressedkey == RIGHT)
      {
        if (i == (NumberOfFiles - 1))
        {
          i = -1;
        }
        i++;
      }
      else if (pressedkey == SEL)
      {
        i = NumberOfFiles;
      }
      else if (pressedkey == NOKEY)
      {
        i++;
      }
    }
  }

  for (i = 0; i < MAX_FILES; i++)
  {
    free(DirectoryFiles[i]);
  }

  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  Demo_IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Play wave files stored on the SDcard.
  * @param  None
  * @retval None
  */
void Menu_WavePlayerFunc(void)
{
  EXTI_InitTypeDef     EXTI_InitStructure;
  uint8_t str[20], pressedkey = 0;
  int8_t index = 0;
  LCD_Clear(LCD_COLOR_WHITE);

  Demo_IntExtOnOffConfig(DISABLE);
  
  /* Disable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, DISABLE);
  
  while (Menu_ReadKey() != NOKEY)
  {}

  for (index = 0; index < MAX_FILES; index++)
  {
    DirectoryFiles[index] = malloc(13); /** Initialize the DirectoryFiles pointers (heap) */
  }

  NumberOfFiles = Storage_GetDirectoryWaveFiles ("/USER", DirectoryFiles); /** Get the WAV file names on root directory  Maximum 10 files */

  if (NumberOfFiles == 0)
  {
    for (index = 0; index < MAX_FILES; index++)
    {
      free(DirectoryFiles[index]);
    }
    LCD_Clear(LCD_COLOR_WHITE);
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLACK);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_0, "No wave files...    ");
    LCD_DisplayStringLine(LCD_LINE_1, "Exit:  Push JoyStick");
    while (Menu_ReadKey() == NOKEY)
    {}
    LCD_Clear(LCD_COLOR_WHITE);
    Menu_DisplayMenu();

    Demo_IntExtOnOffConfig(ENABLE);
    return;
  }

  /* Set the text and the background color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_BLACK);
  LCD_Clear(LCD_COLOR_WHITE);

  /* Display the Titles */
  LCD_DisplayStringLine(LCD_LINE_0, "STM32 DAC Audio Demo");
  LCD_DisplayStringLine(LCD_LINE_1, " Playing Wave files ");
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_3, "SEL  -> Play        ");
  LCD_DisplayStringLine(LCD_LINE_4, "DOWN -> Return      ");
  LCD_DisplayStringLine(LCD_LINE_5, "LEFT -> Next Wave   ");
  LCD_DisplayStringLine(LCD_LINE_6, "RIGHT->Previous Wave");
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_ClearLine(LCD_LINE_7);

  index = 0;
  LCD_SetBackColor(LCD_COLOR_GREEN);
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_9, "                    ");

  sprintf ((char*)str, "/USER/%-13.13s", DirectoryFiles[0]); /** Format the string */
  LCD_DisplayStringLine(LCD_LINE_9, (uint8_t *) str);

  while (index < NumberOfFiles)
  {
    pressedkey = Menu_ReadKey();
    if (pressedkey == LEFT)
    {
      if (index == 0)
      {
        index = NumberOfFiles;
      }
      index--;
      LCD_SetBackColor(LCD_COLOR_GREEN);
      LCD_DisplayStringLine(LCD_LINE_9, "                    ");
      sprintf ((char*)str, "/USER/%-13.13s", DirectoryFiles[index]); /** Format the string */
      LCD_DisplayStringLine(LCD_LINE_9, (uint8_t *) str);
    }
    else if (pressedkey == RIGHT)
    {
      if (index == (NumberOfFiles - 1))
      {
        index = -1;
      }
      index++;
      LCD_SetBackColor(LCD_COLOR_GREEN);
      LCD_DisplayStringLine(LCD_LINE_9, "                    ");
      sprintf ((char*)str, "/USER/%-13.13s", DirectoryFiles[index]); /** Format the string */
      LCD_DisplayStringLine(LCD_LINE_9, (uint8_t *) str);
    }
    else if (pressedkey == DOWN)
    {
      index = NumberOfFiles;
    }
    else if (pressedkey == SEL)
    {
      /* Configure and enable External interrupt */
      EXTI_InitStructure.EXTI_Line = EXTI_Line22;
      EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
      EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
      EXTI_InitStructure.EXTI_LineCmd = DISABLE;
      EXTI_Init(&EXTI_InitStructure);
      
      /* Set the text and the background color */
      LCD_SetBackColor(LCD_COLOR_BLUE);
      LCD_DisplayStringLine(LCD_LINE_3, "SEL  PAUSE LEFT  BWD");
      LCD_DisplayStringLine(LCD_LINE_4, "DOWN STOP  RIGHT FWD");
      LCD_SetBackColor(LCD_COLOR_WHITE);
      LCD_ClearLine(LCD_LINE_2);
      LCD_ClearLine(LCD_LINE_5);
      LCD_SetTextColor(LCD_COLOR_RED);
      LCD_SetBackColor(LCD_COLOR_BLUE);
      LCD_ClearLine(LCD_LINE_7);
      LCD_DisplayStringLine(LCD_LINE_6, "       Playing      ");
      LCD_SetBackColor(LCD_COLOR_GREEN);
      LCD_SetTextColor(LCD_COLOR_WHITE);
      LCD_DisplayStringLine(LCD_LINE_9, "                    ");
      sprintf ((char*)str, "/USER/%-13.13s", DirectoryFiles[index]); /** Format the string */
      LCD_DisplayStringLine(LCD_LINE_9, (uint8_t *) str);
      pressedkey = WavePlayerMenu_Start((const char*)str, &bmplen);
      
      /* Restart the COMP to adjust the LCD glass contrast with the luminosity range.*/
      Demo_COMPConfig();
      
      if (pressedkey == NOKEY)
      {
        if (Get_WaveFileStatus() != Valid_WAVE_File)
        {
          /* Set the Back Color */
          LCD_SetBackColor(LCD_COLOR_BLUE);
          /* Set the Text LCD_COLOR_WHITE */
          LCD_SetTextColor(LCD_COLOR_WHITE);
          LCD_ClearLine(LCD_LINE_3);
          LCD_DisplayStringLine(LCD_LINE_3, (uint8_t *) str);
          LCD_DisplayStringLine(LCD_LINE_4, "Wave file is not    ");
          LCD_DisplayStringLine(LCD_LINE_5, "supported.          ");
          LCD_DisplayStringLine(LCD_LINE_6, "Press JoyStick to   ");
          LCD_DisplayStringLine(LCD_LINE_7, "continue.           ");
          while (Menu_ReadKey() != NOKEY)
          {}
          pressedkey = Menu_ReadKey();
          while (pressedkey == NOKEY)
          {
            pressedkey = Menu_ReadKey();
          }
          if (pressedkey == LEFT)
          {
            if (index == 0)
            {
              index = NumberOfFiles;
            }
            index--;
            LCD_SetBackColor(LCD_COLOR_GREEN);
            LCD_DisplayStringLine(LCD_LINE_9, "                    ");
            sprintf ((char*)str, "/USER/%-13.13s", DirectoryFiles[index]); /** Format the string */
            LCD_DisplayStringLine(LCD_LINE_9, (uint8_t *) str);
          }
          else if (pressedkey == RIGHT)
          {
            if (index == (NumberOfFiles - 1))
            {
              index = -1;
            }
            index++;
            LCD_SetBackColor(LCD_COLOR_GREEN);
            LCD_DisplayStringLine(LCD_LINE_9, "                    ");
            sprintf ((char*)str, "/USER/%-13.13s", DirectoryFiles[index]); /** Format the string */
            LCD_DisplayStringLine(LCD_LINE_9, (uint8_t *) str);
          }
          else if (pressedkey == DOWN)
          {
            index = NumberOfFiles;
          }
          LCD_SetTextColor(LCD_COLOR_WHITE);
          LCD_SetBackColor(LCD_COLOR_BLUE);
          LCD_DisplayStringLine(LCD_LINE_3, "SEL  -> Play        ");
          LCD_DisplayStringLine(LCD_LINE_4, "DOWN -> Exit        ");
          LCD_DisplayStringLine(LCD_LINE_5, "LEFT -> Next Wave   ");
          LCD_DisplayStringLine(LCD_LINE_6, "RIGHT->Previous Wave");
          LCD_ClearLine(LCD_LINE_7);
        }
        else
        {
          index = 1;
        }
      }
      pressedkey = NOKEY;
    }
  }
  for (index = 0; index < MAX_FILES; index++)
  {
    free(DirectoryFiles[index]);
  }

  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  
  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
  
  Demo_IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Display the About menu.
  * @param  None
  * @retval None
  */
void Menu_AboutFunc(void)
{
  LCD_Clear(LCD_COLOR_WHITE);
  Demo_IntExtOnOffConfig(DISABLE);
  while (Menu_ReadKey() != NOKEY)
  {}
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_ClearLine(LCD_LINE_0);
  LCD_DisplayStringLine(LCD_LINE_1, "   STM32L152-EVAL   ");
  LCD_DisplayStringLine(LCD_LINE_2, "   Demonstration    ");
  LCD_DisplayStringLine(LCD_LINE_3, "   Version 1.2.0    ");
  LCD_DisplayStringLine(LCD_LINE_4, "   24-January-2012  ");
  LCD_ClearLine(LCD_LINE_5);
  LCD_DisplayStringLine(LCD_LINE_6, "MCD Application Team");
  LCD_DisplayStringLine(LCD_LINE_7, "   COPYRIGHT 2012   ");
  LCD_DisplayStringLine(LCD_LINE_8, " STMicroelectronics ");
  LCD_DisplayStringLine(LCD_LINE_9, "  www.st.com/stm32l ");
  while (Menu_ReadKey() == NOKEY)
  {}
  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  Demo_IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Display the About menu.
  * @param  None
  * @retval None
  */
void Menu_ProductPres(void)
{
  __IO uint32_t index = 0, pressedkey = 0;
  Demo_IntExtOnOffConfig(DISABLE);

  LCD_SetDisplayWindow(239, 0x13F, 240, 320);
  Storage_OpenReadFile(239, 319, "STFILES/STM32L.BMP");
  Demo_Delay(100);

  while (Menu_ReadKey() != NOKEY)
  {}

  LCD_Clear(LCD_COLOR_WHITE);
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  LCD_ClearLine(LCD_LINE_0);
  LCD_DisplayStringLine(LCD_LINE_1, " Press SEL to start ");
  LCD_ClearLine(LCD_LINE_2);
  LCD_ClearLine(LCD_LINE_3);
  LCD_DisplayStringLine(LCD_LINE_4, " When presentation  ");
  LCD_DisplayStringLine(LCD_LINE_5, "starts use RIGHT and");
  LCD_DisplayStringLine(LCD_LINE_6, "LEFT to go to the   ");
  LCD_DisplayStringLine(LCD_LINE_7, "next/previous slide ");
  LCD_DisplayStringLine(LCD_LINE_8, "and SEL to exit     ");
  LCD_ClearLine(LCD_LINE_9);
  while (Menu_ReadKey() != SEL)
  {}
  while (Menu_ReadKey() != NOKEY)
  {}
  /* Enable the SysTick Counter */
  SysTick->CTRL |= SysTick_CTRL_ENABLE;
  /* Start Wave audio playing */
  if (WavePlayer_Start() == 0)
  {
    while (index < 14)
    {
      WavePlayer_Pause();
      LCD_SetDisplayWindow(239, 0x13F, 240, 320);
      /* Slide n: index */
      Storage_OpenReadFile(239, 319, (const char*)SlideAddr[index]);
      WavePlayer_RePlay();

      /* Clear the SysTick Counter */
      SysTick->VAL = (uint32_t)0x0;

      pressedkey = Demo_DelayJoyStick(DelayTiming[index]);
      if (pressedkey == LEFT)
      {
        if (index == 0)
        {
          index = 1;
        }
        index--;
        WavePointerUpdate(SlidesSpeech[index]);
      }
      else if (pressedkey == RIGHT)
      {
        if (index != 13)
        {
          index++;
          WavePointerUpdate(SlidesSpeech[index]);
        }
        else
        {
          WavePlayer_Stop();
          index = 15;
        }

      }
      else if (pressedkey == SEL)
      {
        WavePlayer_Stop();
        index = 15;
      }
      else if (pressedkey == NOKEY)
      {
        index++;
      }
    }
  }
  /* Disable the SysTick Counter */
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
  /* Clear the SysTick Counter */
  SysTick->VAL = (uint32_t)0x0;

  LCD_Clear(LCD_COLOR_BLACK);
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLACK);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_0, "  End of slide show ");
  LCD_DisplayStringLine(LCD_LINE_1, "Exit:  Push JoyStick");
  while (Menu_ReadKey() == NOKEY)
  {}
  LCD_Clear(LCD_COLOR_WHITE);
  Menu_DisplayMenu();
  Demo_IntExtOnOffConfig(ENABLE);
}

/**
  * @brief  Checks if the bitmapfiles (slides + icons) are already loaded in the
  *         SD Card FLASH.
  * @param  None
  * @retval None
  */
uint32_t Menu_CheckBitmapFiles(void)
{
  uint32_t err = 0;
  uint32_t i = 0x300;
  uint8_t j = 0;

  LCD_Clear(LCD_COLOR_WHITE);
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  if (SD_Detect() == SD_NOT_PRESENT)
  {
    LCD_DisplayStringLine(LCD_LINE_1, "Please insert SDCard");
    while (SD_Detect() == SD_NOT_PRESENT)
    {}

    LCD_DisplayStringLine(LCD_LINE_1, "   SDCard detected  ");
    Demo_Delay(200);
  }

  /*------------------- FAT Initialization -------------------------------*/
  do
  {
    SD_LowLevel_DeInit();
    /** SDCARD Initialisation for FatFs                            */
    err = Storage_Init();
  }
  while ((err != 0) && (i-- != 0));

  /* SD Card not formatted */
  if (i == 0)
  {
    LCD_DisplayStringLine(LCD_LINE_1, "SDCard not formatted");
    LCD_DisplayStringLine(LCD_LINE_1, "Reprogram your card.");
    while (1)
    {}
  }
  /* Check icons if available on SD card under STFILES directory*/
  for (j = 0; j < NUMBER_OF_ICONS; j++)
  {
    if (f_open (&F, (const char*)IconsAddr[j], FA_READ))
    {
      return 1;
    }

    f_read (&F, Buffer1, _MAX_SS, &BytesRead);

    if (Menu_Buffercmp((uint8_t *)Icons64Check,  Buffer1, 6) != 0)
    {
      return 2;
    }
  }
  /* Check Slides if available on SD card under STFILES directory*/
  for (j = 0; j < NUMBER_OF_SLIDES; j++)
  {
    if (f_open (&F, (const char*)SlideAddr[j], FA_READ))
    {
      return 3;
    }

    f_read (&F, Buffer1, _MAX_SS, &BytesRead);

    if (Menu_Buffercmp((uint8_t *)SlidesCheck,  Buffer1, 6) != 0)
    {
      return 4;
    }
  }

  /* Check Alarm and Watch images if available on SD card under STFILES directory*/
  for (j = 0; j < 2; j++)
  {
    if (f_open (&F, (const char*)Images2Addr[j], FA_READ))
    {
      return 5;
    }

    f_read (&F, Buffer1, _MAX_SS, &BytesRead);

    if (Menu_Buffercmp((uint8_t *)Icons128Check,  Buffer1, 6) != 0)
    {
      return 6;
    }
  }
  /* Check STM32L, HELP and STLOGO images if available on SD card under STFILES directory */
  for (j = 0; j < 3; j++)
  {
    if (f_open (&F, (const char*)Images1Addr[j], FA_READ))
    {
      return 5;
    }

    f_read (&F, Buffer1, _MAX_SS, &BytesRead);

    if (Menu_Buffercmp((uint8_t *)SlidesCheck,  Buffer1, 6) != 0)
    {
      return 6;
    }
  }
  return 0;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length.
  * @retval  0: pBuffer1 identical to pBuffer2
  *          1: pBuffer1 differs from pBuffer2
  */
uint8_t Menu_Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return 1;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return 0;
}

/**
  * @brief  Displays the temperature in Celsius and fahrenheit degree.
  * @param  None
  * @retval None
  */
void Menu_ThermometerFun(void)
{
  uint32_t i = 0, pressedkey = 0;

  while (Menu_ReadKey() != NOKEY)
  {}

  /* Initialize the Temperature Sensor */
  LM75_Init();

  if (LM75_GetStatus() == SUCCESS)
  {
    /* Disable the JoyStick Interrupts */
    Demo_IntExtOnOffConfig(DISABLE);
    /* Clear the LCD */
    LCD_Clear(LCD_COLOR_WHITE);

    /* Set the Icon display window */
    LCD_SetDisplayWindow(170, 190, 64, 64);

    /* Display the Thermometer icon */
    Storage_OpenReadFile(170, 190, "STFILES/ICON6.BMP");
    /* Disable LCD Window mode */
    LCD_WindowModeDisable();
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    /* Set the Text Color */
    LCD_SetTextColor(LCD_COLOR_GREEN);
    LCD_DisplayStringLine(LCD_LINE_0, "     Temperature    ");

    /* Wait until no key is pressed */
    while (Menu_ReadKey() != NOKEY)
    {}

    /* Configure the Temperature sensor device STLM75:
          - Thermostat mode Interrupt
          - Fault tolerance: 00
        */
    LM75_WriteConfReg(0x02);

    /* Configure the THYS and TOS inorder to use the SMbus alert interrupt */
    LM75_WriteReg(LM75_REG_THYS, TEMPERATURE_THYS << 8);  /*31ÝC*/
    LM75_WriteReg(LM75_REG_TOS, TEMPERATURE_TOS << 8);   /*32ÝC*/

    I2C_ClearITPendingBit(I2C1, I2C_IT_SMBALERT);
    SMbusAlertOccurred = 0;

    /* Wait until a key is beiing pressed */
    while (pressedkey == NOKEY)
    {
      /* Get double of Temperature value */
      TempValue = LM75_ReadTemp();

      if (TempValue <= 256)
      {
        /* Positive temperature measured */
        TempCelsiusDisplay[7] = '+';
        /* Initialize the temperature sensor value*/
        TempValueCelsius = TempValue;
      }
      else
      {
        /* Negative temperature measured */
        TempCelsiusDisplay[7] = '-';
        /* Remove temperature value sign */
        TempValueCelsius = 0x200 - TempValue;
      }

      /* Calculate temperature digits in ÝC */
      if ((TempValueCelsius & 0x01) == 0x01)
      {
        TempCelsiusDisplay[12] = 0x05 + 0x30;
        TempFahrenheitDisplay[12] = 0x05 + 0x30;
      }
      else
      {
        TempCelsiusDisplay[12] = 0x00 + 0x30;
        TempFahrenheitDisplay[12] = 0x00 + 0x30;
      }

      TempValueCelsius >>= 1;

      TempCelsiusDisplay[8] = (TempValueCelsius / 100) + 0x30;
      TempCelsiusDisplay[9] = ((TempValueCelsius % 100) / 10) + 0x30;
      TempCelsiusDisplay[10] = ((TempValueCelsius % 100) % 10) + 0x30;

      if (TempValue > 256)
      {
        if (((9 * TempValueCelsius) / 5) <= 32)
        {
          /* Convert temperature ÝC to Fahrenheit */
          TempValueFahrenheit = abs (32 - ((9 * TempValueCelsius) / 5));

          /* Calculate temperature digits in ÝF */
          TempFahrenheitDisplay[8] = (TempValueFahrenheit / 100) + 0x30;
          TempFahrenheitDisplay[9] = ((TempValueFahrenheit % 100) / 10) + 0x30;
          TempFahrenheitDisplay[10] = ((TempValueFahrenheit % 100) % 10) + 0x30;
          /* Positive temperature measured */
          TempFahrenheitDisplay[7] = '+';
        }
        else
        {
          /* Convert temperature ÝC to Fahrenheit */
          TempValueFahrenheit = abs(((9 * TempValueCelsius) / 5) - 32);
          /* Calculate temperature digits in ÝF */
          TempFahrenheitDisplay[8] = (TempValueFahrenheit / 100) + 0x30;
          TempFahrenheitDisplay[9] = ((TempValueFahrenheit % 100) / 10) + 0x30;
          TempFahrenheitDisplay[10] = ((TempValueFahrenheit % 100) % 10) + 0x30;

          /* Negative temperature measured */
          TempFahrenheitDisplay[7] = '-';
        }
      }
      else
      {
        /* Convert temperature ÝC to Fahrenheit */
        TempValueFahrenheit = ((9 * TempValueCelsius) / 5) + 32;

        /* Calculate temperature digits in ÝF */
        TempFahrenheitDisplay[8] = (TempValueFahrenheit / 100) + 0x30;
        TempFahrenheitDisplay[9] = ((TempValueFahrenheit % 100) / 10) + 0x30;
        TempFahrenheitDisplay[10] = ((TempValueFahrenheit % 100) % 10) + 0x30;

        /* Positive temperature measured */
        TempFahrenheitDisplay[7] = '+';
      }

      /* Display Fahrenheit value on LCD */
      for (i = 0; i < 20; i++)
      {
        LCD_DisplayChar(LCD_LINE_7, (319 - (16 * i)), TempCelsiusDisplay[i]);

        LCD_DisplayChar(LCD_LINE_8, (319 - (16 * i)), TempFahrenheitDisplay[i]);
      }

      if (SMbusAlertOccurred == 1)
      {
        /* Set the Back Color */
        LCD_SetBackColor(LCD_COLOR_BLUE);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
        LCD_DisplayStringLine(LCD_LINE_2, "Warning: Temp exceed");
        LCD_DisplayStringLine(LCD_LINE_3, "        32 C        ");
      }
      if (SMbusAlertOccurred == 2)
      {
        /* Set the Back Color */
        LCD_SetBackColor(LCD_COLOR_WHITE);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_WHITE);
        LCD_ClearLine(LCD_LINE_2);
        LCD_ClearLine(LCD_LINE_3);
        SMbusAlertOccurred = 0;
        /* Set the Back Color */
        LCD_SetBackColor(LCD_COLOR_BLUE);
        /* Set the Text Color */
        LCD_SetTextColor(LCD_COLOR_GREEN);
      }
      pressedkey = Menu_ReadKey();
    }
  }
  else
  {
    LCD_Clear(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_2, " No TSENSOR Present ");
    LCD_DisplayStringLine(LCD_LINE_3, "Exit:  Push JoyStick");
    while (Menu_ReadKey() == NOKEY)
    {}
  }
  LCD_Clear(LCD_COLOR_WHITE);

  /* Display menu */
  Menu_DisplayMenu();

  /* Enable the JoyStick interrupt */
  Demo_IntExtOnOffConfig(ENABLE);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
