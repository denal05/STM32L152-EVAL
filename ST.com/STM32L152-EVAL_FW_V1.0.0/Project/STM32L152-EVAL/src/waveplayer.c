/**
  ******************************************************************************
  * @file    waveplayer.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    01-April-2011
  * @brief   This file includes the wave player driver for the STM32L152-EVAL demo.
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
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32L152_EVAL_Demo
  * @{
  */

/** @defgroup WAVEPLAYER
  * @brief    This file includes the wave player driver for the STM32L152-EVAL
  *           demo.
  * @{
  */

/** @defgroup WAVEPLAYER_Private_Types
  * @{
  */
typedef enum
{
  LittleEndian,
  BigEndian
}Endianness;
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Variables
  * @{
  */
WAVE_FormatTypeDef WAVE_Format;
static __IO ErrorCode WaveFileStatus = Unvalid_RIFF_ID;
uint16_t TIM6ARRValue = 4000;
__IO uint32_t WaveDataLength = 0;
extern __IO uint32_t WaveCounter;
static __IO uint32_t SpeechDataOffset = 0x00;
static uint32_t wavelen = 0;
extern FIL F;
extern UINT BytesWritten;
extern UINT BytesRead;
uint8_t Buffer1[_MAX_SS];
uint8_t Buffer2[_MAX_SS];
extern FATFS fs;
extern DIR dir;

/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_FunctionPrototypes
  * @{
  */
static uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat);
static ErrorCode WavePlayer_WaveParsing(const char* WavName, uint32_t *FileLen);
/**
  * @}
  */

/** @defgroup WAVEPLAYER_Private_Functions
  * @{
  */

/**
  * @brief  Wave player Initialization
  * @param  None
  * @retval None
  */
void WavePlayer_Init(void)
{
  DAC_InitTypeDef     DAC_InitStructure;
  GPIO_InitTypeDef    GPIO_InitStructure;

  /* TIM6 and DAC clocks enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM2 | RCC_APB1Periph_DAC, ENABLE);

  /* Configure DAC Channel1 and Channel2 as output */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  /* must be analogic (but works with other config which they have no effect) */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* TIM6 Configuration */
  TIM_DeInit(TIM6);

  TIM6ARRValue = (uint32_t)(SystemCoreClock / SAMPLE_RATE_8000);

  TIM_SetAutoreload(TIM6, TIM6ARRValue);

  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

  /* Enable TIM6 update interrupt */
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

  /* DAC deinitialize */
  DAC_DeInit();
  DAC_StructInit(&DAC_InitStructure);

  /* Fill DAC InitStructure */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;

  /* DAC Channel1: 8bit right---------------------------------------------------*/
  /* DAC Channel1 Init */
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* Enable DAC Channel1 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
}
/**
  * @brief  Returns the Wave file status.
  * @param  None
  * @retval Wave file status.
  */
ErrorCode Get_WaveFileStatus(void)
{
  return (WaveFileStatus);
}

/**
  * @brief  Start wave playing
  * @param  None
  * @retval None
  */
uint8_t WavePlayer_Start(void)
{
  WavePlayer_Init();

  /* Read the Speech wave file status */
  WaveFileStatus = WavePlayer_WaveParsing("STFILES/WAVE.WAV", &wavelen);

  if (WaveFileStatus == Valid_WAVE_File) /* the .WAV file is valid */
  {
    /* Set WaveDataLenght to the Speech wave length */
    WaveDataLength = WAVE_Format.DataSize;

    TIM_SetAutoreload(TIM6, TIM6ARRValue);

    /* Start TIM6 */
    TIM_Cmd(TIM6, ENABLE);
  }
  else
  {
    LCD_Clear(LCD_COLOR_WHITE);
    LCD_SetBackColor(LCD_COLOR_GREEN);
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_5, " ERROR: No Wave File");
    LCD_DisplayStringLine(LCD_LINE_6, " Press Joystick to  ");
    LCD_DisplayStringLine(LCD_LINE_7, " exit...            ");
    while (Menu_ReadKey() == NOKEY)
    {}
    LCD_Clear(LCD_COLOR_WHITE);
    Demo_IntExtOnOffConfig(ENABLE);
    return 1;
  }
  return 0;
}

/**
  * @brief  Stop wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_Stop(void)
{
  /* Disable TIM6 update interrupt */
  TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
  /* DAC deinitialize */
  DAC_DeInit();
  /* Disable TIM6 */
  TIM_Cmd(TIM6, DISABLE);
}

/**
  * @brief  Pause wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_RePlay(void)
{
  TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  /* Enable TIM6 update interrupt */
  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
}

/**
  * @brief  Pause wave playing
  * @param  None
  * @retval None
  */
void WavePlayer_Pause(void)
{
  TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
  /* Disable TIM6 update interrupt */
  TIM_ITConfig(TIM6, TIM_IT_Update, DISABLE);
}

/**
  * @brief  Decrements the played wave data length.
  * @param  None
  * @retval Current value of  WaveDataLength variable.
  */
void WavePointerUpdate(uint32_t value)
{
  f_lseek(&F, value);
}

/**
  * @brief  Decrements the played wave data length.
  * @param  None
  * @retval Current value of  WaveDataLength variable.
  */
uint32_t Decrement_WaveDataLength(void)
{
  if (WaveDataLength != 0x00)
  {
    WaveDataLength--;
  }
  return (WaveDataLength);
}


/**
  * @brief  Decrements the played wave data length.
  * @param  None
  * @retval Current value of  WaveDataLength variable.
  */
void Set_WaveDataLength(uint32_t value)
{
  WaveDataLength = value;
}

/**
  * @brief  Checks the format of the .WAV file and gets information about
  *   the audio format. This is done by reading the value of a
  *   number of parameters stored in the file header and comparing
  *   these to the values expected authenticates the format of a
  *   standard .WAV  file (44 bytes will be read). If  it is a valid
  *   .WAV file format, it continues reading the header to determine
  *   the  audio format such as the sample rate and the sampled data
  *   size. If the audio format is supported by this application,
  *   it retrieves the audio format in WAVE_Format structure and
  *   returns a zero value. Otherwise the function fails and the
  *   return value is nonzero.In this case, the return value specifies
  *   the cause of  the function fails. The error codes that can be
  *   returned by this function are declared in the header file.
  * @param  None
  * @retval Zero value if the function succeed, otherwise it return
  *         a nonzero value which specifies the error code.
  */
static ErrorCode WavePlayer_WaveParsing(const char* WavName, uint32_t *FileLen)
{
  uint32_t temp = 0x00;
  uint32_t extraformatbytes = 0;

  f_mount(0, &fs);

  f_open (&F, WavName, FA_READ);

  f_read (&F, Buffer1, _MAX_SS, &BytesRead);

  /* Read chunkID, must be 'RIFF'  ----------------------------------------------*/
  temp = ReadUnit(Buffer1, 0, 4, BigEndian);
  if (temp != CHUNK_ID)
  {
    return(Unvalid_RIFF_ID);
  }

  /* Read the file length ----------------------------------------------------*/
  WAVE_Format.RIFFchunksize = ReadUnit(Buffer1, 4, 4, LittleEndian);

  /* Read the file format, must be 'WAVE' ------------------------------------*/
  temp = ReadUnit(Buffer1, 8, 4, BigEndian);
  if (temp != FILE_FORMAT)
  {
    return(Unvalid_WAVE_Format);
  }

  /* Read the format chunk, must be'fmt ' --------------------------------------*/
  temp = ReadUnit(Buffer1, 12, 4, BigEndian);
  if (temp != FORMAT_ID)
  {
    return(Unvalid_FormatChunk_ID);
  }
  /* Read the length of the 'fmt' data, must be 0x10 -------------------------*/
  temp = ReadUnit(Buffer1, 16, 4, LittleEndian);
  if (temp != 0x10)
  {
    extraformatbytes = 1;
  }
  /* Read the audio format, must be 0x01 (PCM) -------------------------------*/
  WAVE_Format.FormatTag = ReadUnit(Buffer1, 20, 2, LittleEndian);
  if (WAVE_Format.FormatTag != WAVE_FORMAT_PCM)
  {
    return(Unsupporetd_FormatTag);
  }

  /* Read the number of channels, must be 0x01 (Mono) ------------------------*/
  WAVE_Format.NumChannels = ReadUnit(Buffer1, 22, 2, LittleEndian);
  if (WAVE_Format.NumChannels != CHANNEL_MONO)
  {
    return(Unsupporetd_Number_Of_Channel);
  }

  /* Read the Sample Rate ----------------------------------------------------*/
  WAVE_Format.SampleRate = ReadUnit(Buffer1, 24, 4, LittleEndian);
  /* Update the OCA value according to the .WAV file Sample Rate */
  switch (WAVE_Format.SampleRate)
  {
    case SAMPLE_RATE_8000 :
      TIM6ARRValue = 4000;
      break; /* 8KHz = 32MHz / 4000 */
    case SAMPLE_RATE_11025:
      TIM6ARRValue = 2902;
      break; /* 11.025KHz = 32MHz / 2902 */
    case SAMPLE_RATE_22050:
      TIM6ARRValue = 1451;
      break; /* 22.05KHz = 32MHz / 1451 */
    case SAMPLE_RATE_44100:
      TIM6ARRValue = 725;
      break; /* 44.1KHz = 32MHz / 725 */
    default:
      return(Unsupporetd_Sample_Rate);
  }

  /* Read the Byte Rate ------------------------------------------------------*/
  WAVE_Format.ByteRate = ReadUnit(Buffer1, 28, 4, LittleEndian);

  /* Read the block alignment ------------------------------------------------*/
  WAVE_Format.BlockAlign = ReadUnit(Buffer1, 32, 2, LittleEndian);

  /* Read the number of bits per sample --------------------------------------*/
  WAVE_Format.BitsPerSample = ReadUnit(Buffer1, 34, 2, LittleEndian);
  if (WAVE_Format.BitsPerSample != BITS_PER_SAMPLE_8)
  {
    return(Unsupporetd_Bits_Per_Sample);
  }
  SpeechDataOffset = 36;
  /* If there is Extra format bytes, these bytes will be defined in "Fact Chunk" */
  if (extraformatbytes == 1)
  {
    /* Read th Extra format bytes, must be 0x00 ------------------------------*/
    temp = ReadUnit(Buffer1, 36, 2, LittleEndian);
    if (temp != 0x00)
    {
      return(Unsupporetd_ExtraFormatBytes);
    }
    /* Read the Fact chunk, must be 'fact' -----------------------------------*/
    temp = ReadUnit(Buffer1, 38, 4, BigEndian);
    if (temp != FACT_ID)
    {
      return(Unvalid_FactChunk_ID);
    }
    /* Read Fact chunk data Size ---------------------------------------------*/
    temp = ReadUnit(Buffer1, 42, 4, LittleEndian);

    SpeechDataOffset += 10 + temp;
  }
  /* Read the Data chunk, must be 'data' ---------------------------------------*/
  temp = ReadUnit(Buffer1, SpeechDataOffset, 4, BigEndian);
  SpeechDataOffset += 4;
  if (temp != DATA_ID)
  {
    return(Unvalid_DataChunk_ID);
  }

  /* Read the number of sample data ------------------------------------------*/
  WAVE_Format.DataSize = ReadUnit(Buffer1, SpeechDataOffset, 4, LittleEndian);
  SpeechDataOffset += 4;
  WaveCounter =  SpeechDataOffset;
  return(Valid_WAVE_File);
}

/**
  * @brief  Start wave playing
  * @param  None
  * @retval None
  */
uint8_t WavePlayerMenu_Start(const char* FileName, uint32_t *FileLen)
{
  DAC_InitTypeDef            DAC_InitStructure;
  DMA_InitTypeDef            DMA_InitStructure;
  uint8_t tmp, KeyState = NOKEY;

  WavePlayer_Init();

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  WaveFileStatus = Unvalid_RIFF_ID;

  /* TIM6 Configuration */
  TIM_DeInit(TIM6);

  /* DMA1 channel2 configuration */
  DMA_DeInit(DMA1_Channel2);
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) & Buffer1;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
  DMA_InitStructure.DMA_BufferSize = 512;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel2, &DMA_InitStructure);

  /* Enable DMA1 Channel2 */
  DMA_Cmd(DMA1_Channel2, ENABLE);

  /* DAC deinitialize */
  DAC_DeInit();
  DAC_StructInit(&DAC_InitStructure);

  /* Fill DAC InitStructure */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

  /* DAC Channel1: 8bit right---------------------------------------------------*/
  /* DAC Channel1 Init */
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
     automatically connected to the DAC converter. */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  /* Enable DMA for DAC Channel1 */
  DAC_DMACmd(DAC_Channel_1, ENABLE);

  /* Read the Speech wave file status */
  WaveFileStatus = WavePlayer_WaveParsing((const char*)FileName, &wavelen);


  /* TIM6 TRGO selection */
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
  TIM_SetAutoreload(TIM6, TIM6ARRValue);


  if (WaveFileStatus == Valid_WAVE_File)  /* the .WAV file is valid */
  {
    /* Set WaveDataLenght to the Speech wave length */
    WaveDataLength = WAVE_Format.DataSize;
  }
  else
  {
    return NOKEY;
  }

  /* Start TIM6 */
  TIM_Cmd(TIM6, ENABLE);

  LCD_SetTextColor(LCD_COLOR_MAGENTA);
  /* Set the Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_DrawRect(LCD_LINE_7, 310, 16, 300);

  while (WaveDataLength)
  {
    f_read (&F, Buffer2, _MAX_SS, &BytesRead);
    if (WaveDataLength) WaveDataLength -= 512;
    if (WaveDataLength < 512) WaveDataLength = 0;

    while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET)
    {
      tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
      LCD_SetTextColor(LCD_COLOR_MAGENTA);
      LCD_DrawLine(LCD_LINE_7, 310 - ((tmp) * 3), 16, LCD_DIR_VERTICAL);
    }

    DMA1->IFCR = DMA1_FLAG_TC2;
    DMA1_Channel2->CCR = 0x0;

    KeyState = LCD_Update();
    if (KeyState == DOWN)
    {
      return DOWN;
    }
    DMA1_Channel2->CNDTR = 0x200;
    DMA1_Channel2->CPAR = DAC_DHR8R1_Address;
    DMA1_Channel2->CMAR = (uint32_t) & Buffer2;
    DMA1_Channel2->CCR = 0x2091;

    f_read (&F, Buffer1, _MAX_SS, &BytesRead);
    if (WaveDataLength) WaveDataLength -= 512;
    if (WaveDataLength < 512) WaveDataLength = 0;

    while (DMA_GetFlagStatus(DMA1_FLAG_TC2) == RESET)
    {
      tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
      LCD_SetTextColor(LCD_COLOR_MAGENTA);
      LCD_DrawLine(LCD_LINE_7, 310 - ((tmp) * 3), 16, LCD_DIR_VERTICAL);
    }

    DMA1->IFCR = DMA1_FLAG_TC2;
    DMA1_Channel2->CCR = 0x0;

    KeyState = LCD_Update();
    if (KeyState == DOWN)
    {
      return DOWN;
    }

    DMA1_Channel2->CNDTR = 0x200;
    DMA1_Channel2->CPAR = DAC_DHR8R1_Address;
    DMA1_Channel2->CMAR = (uint32_t) & Buffer1;
    DMA1_Channel2->CCR = 0x2091;
  }
  DMA1_Channel2->CCR = 0x0;

  /* Disable TIM6 */
  TIM_Cmd(TIM6, DISABLE);
  WaveDataLength = 0;

  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_3, "SEL  -> Play        ");
  LCD_DisplayStringLine(LCD_LINE_4, "DOWN -> Exit        ");
  LCD_DisplayStringLine(LCD_LINE_5, "LEFT -> Next Wave   ");
  LCD_DisplayStringLine(LCD_LINE_6, "RIGHT->Previous Wave");
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_ClearLine(LCD_LINE_7);

  return NOKEY;
}

/**
  * @brief  Reads a number of bytes from the SPI Flash and reorder them in Big
  *         or little endian.
  * @param  NbrOfBytes: number of bytes to read.
  *         This parameter must be a number between 1 and 4.
  * @param  ReadAddr: external memory address to read from.
  * @param  Endians: specifies the bytes endianness.
  *         This parameter can be one of the following values:
  *             - LittleEndian
  *             - BigEndian
  * @retval Bytes read from the SPI Flash.
  */
static uint32_t ReadUnit(uint8_t *buffer, uint8_t idx, uint8_t NbrOfBytes, Endianness BytesFormat)
{
  uint32_t index = 0;
  uint32_t temp = 0;

  for (index = 0; index < NbrOfBytes; index++)
  {
    temp |= buffer[idx + index] << (index * 8);
  }

  if (BytesFormat == BigEndian)
  {
    temp = __REV(temp);
  }
  return temp;
}

/**
  * @brief  Controls the wave player application LCD display messages
  * @param  None.
  * @retval None.
  */
uint8_t LCD_Update(void)
{
  uint8_t KeyState = NOKEY;
  uint32_t tmp = 0, index = 0;

  KeyState = Menu_ReadKey();

  if (KeyState == SEL)
  {
    DMA1_Channel3->CCR = 0x0;

    /* Disable TIM6 */
    TIM_Cmd(TIM6, DISABLE);

    /* Set the text and the background color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_3, "SEL  -> Play        ");
    LCD_DisplayStringLine(LCD_LINE_4, "DOWN -> Exit        ");
    LCD_SetBackColor(LCD_COLOR_WHITE);
    LCD_ClearLine(LCD_LINE_2);
    LCD_ClearLine(LCD_LINE_5);
    LCD_SetTextColor(LCD_COLOR_RED);
    LCD_SetBackColor(LCD_COLOR_BLUE);
    LCD_DisplayStringLine(LCD_LINE_6, "       Paused       ");
    KeyState = Menu_ReadKey();

    while ((KeyState != SEL) && (KeyState != DOWN))
    {
      KeyState = Menu_ReadKey();
    }
    if (KeyState == SEL)
    {
      LCD_SetBackColor(LCD_COLOR_BLUE);
      LCD_SetTextColor(LCD_COLOR_WHITE);
      LCD_DisplayStringLine(LCD_LINE_3, "SEL  PAUSE LEFT  BWD");
      LCD_DisplayStringLine(LCD_LINE_4, "DOWN STOP  RIGHT FWD");
      LCD_SetTextColor(LCD_COLOR_RED);
      LCD_SetBackColor(LCD_COLOR_BLUE);
      LCD_DisplayStringLine(LCD_LINE_6, "       Playing      ");
      DMA1_Channel3->CCR = 0x0;
      /* Enable TIM6 */
      TIM_Cmd(TIM6, ENABLE);
      return KeyState;
    }
  }
  if (KeyState == DOWN)
  {
    LCD_SetTextColor(LCD_COLOR_WHITE);
    LCD_DisplayStringLine(LCD_LINE_3, "SEL  -> Play        ");
    LCD_DisplayStringLine(LCD_LINE_4, "DOWN -> Exit        ");
    LCD_DisplayStringLine(LCD_LINE_5, "LEFT -> Next Wave   ");
    LCD_DisplayStringLine(LCD_LINE_6, "RIGHT->Previous Wave");
    LCD_SetBackColor(LCD_COLOR_BLUE);
    LCD_ClearLine(LCD_LINE_7);
    return KeyState;
  }
  if (KeyState == LEFT)
  {
    tmp = F.fptr;
    if (tmp > (F.fsize / 20))
    {
      tmp -= F.fsize / 20;
    }
    else
    {
      tmp = 0;
    }
    f_lseek(&F, tmp);

    WaveDataLength += (WAVE_Format.DataSize / 20);
    if (WaveDataLength > WAVE_Format.DataSize)
    {
      WaveDataLength = WAVE_Format.DataSize;
    }
    LCD_SetBackColor(LCD_COLOR_BLUE);
    LCD_ClearLine(LCD_LINE_7);
    LCD_SetTextColor(LCD_COLOR_MAGENTA);
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    LCD_DrawRect(LCD_LINE_7, 310, 16, 300);
    tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
    LCD_SetTextColor(LCD_COLOR_MAGENTA);
    for (index = 0; index < tmp; index++)
    {
      LCD_DrawLine(LCD_LINE_7, 310 - ((index) * 3), 16, LCD_DIR_VERTICAL);
    }
  }
  if (KeyState == RIGHT)
  {
    tmp = F.fptr;
    tmp += F.fsize / 20;
    if (tmp >  F.fsize)
    {
      tmp = F.fsize;
    }

    f_lseek(&F, tmp);

    if (WaveDataLength < (WAVE_Format.DataSize / 20))
    {
      WaveDataLength = 0;
    }
    else
    {
      WaveDataLength -= (WAVE_Format.DataSize / 20);
    }
    LCD_SetBackColor(LCD_COLOR_BLUE);
    LCD_ClearLine(LCD_LINE_7);
    LCD_SetTextColor(LCD_COLOR_MAGENTA);
    /* Set the Back Color */
    LCD_SetBackColor(LCD_COLOR_BLUE);
    LCD_DrawRect(LCD_LINE_7, 310, 16, 300);
    tmp = (uint8_t) ((uint32_t)((WAVE_Format.DataSize - WaveDataLength) * 100) / WAVE_Format.DataSize);
    LCD_SetTextColor(LCD_COLOR_MAGENTA);
    for (index = 0; index < tmp; index++)
    {
      LCD_DrawLine(LCD_LINE_7, 310 - ((index) * 3), 16, LCD_DIR_VERTICAL);
    }
  }
  return NOKEY ;
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

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
