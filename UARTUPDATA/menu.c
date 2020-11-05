
#include "common.h"
#include "flash_if.h"
#include "menu.h"
#include "ymodem.h"

/** @addtogroup STM32F0xx_IAP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;
extern uint32_t UserMemoryMask;
uint32_t FlashProtection = 0;
uint8_t tab_1024[1024] =
  {
    0
  };
uint8_t FileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
uint32_t SerialDownload(void);
void SerialUpload(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
uint32_t SerialDownload(void)
{
  uint8_t Number[10] = {0};
  int32_t Size = 0;

	SerialPutString("SerialDownload entered!\n\r");

  SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
  Size = Ymodem_Receive(&tab_1024[0]);
	return Size;
//  if (Size > 0)
//  {
//    SerialPutString("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
//    SerialPutString(FileName);
//    Int2Str(Number, Size);
//    SerialPutString("\n\r Size: ");
//    SerialPutString(Number);
//    SerialPutString(" Bytes\r\n");
//    SerialPutString("-------------------\n");
//  }
//  else if (Size == -1)
//  {
//    SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
//  }
//  else if (Size == -2)
//  {
//    SerialPutString("\n\n\rVerification failed!\n\r");
//  }
//  else if (Size == -3)
//  {
//    SerialPutString("\r\n\nAborted by user.\n\r");
//  }
//  else
//  {
//    SerialPutString("\n\rFailed to receive the file!\n\r");
//  }
}

/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void SerialUpload(void)
{
  uint8_t status = 0 ; 

  SerialPutString("\n\n\rSelect Receive File\n\r");

  if (GetKey() == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    //status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"RS0102H4G3.bin.bin", USER_FLASH_SIZE);

    if (status != 0) 
    {
      SerialPutString("\n\rError Occurred while Transmitting File\n\r");
    }
    else
    {
      SerialPutString("\n\rFile uploaded successfully \n\r");
    }
  }
}

/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
void Main_Menu(void)
{
  uint8_t key = 0;
  uint8_t writeprotect = 0;

  SerialPutString("\r\n======================================================================");
  SerialPutString("\r\n=              M26 Module Update Tool                                =");
  SerialPutString("\r\n=                                                                    =");
  SerialPutString("\r\n=               (Version 1.0.0) =");
  SerialPutString("\r\n=                                                                    =");
  SerialPutString("\r\n=                By ZhiLangTeam                                      =");
  SerialPutString("\r\n======================================================================");
  SerialPutString("\r\n\r\n");
#if 1
  /* Test if any sector of Flash memory where user application will be loaded is write protected */
  if (FLASH_If_GetWriteProtectionStatus() != 0)  
  {
    FlashProtection = 1;
  }
  else
  {
    FlashProtection = 0;
  }
#endif
  while (1)
  {
    SerialPutString("\r\n============= Main Menu =====================\r\n\n");
//    SerialPutString("  Download Image To Internal Flash -------- 1\r\n\n");
//    SerialPutString("  Upload Image From  Internal Flash ------- 2\r\n\n");
//    SerialPutString("  Execute The New Program ----------------- 3\r\n\n");

    if(FlashProtection != 0)
    {
      SerialPutString("  Disable the write protection ------------------------- 4\r\n\n");
    }

    SerialPutString("==========================================================\r\n\n");
		SerialDownload();
		iap_load_app(APPLICATION_ADDRESS);


  }
}


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
