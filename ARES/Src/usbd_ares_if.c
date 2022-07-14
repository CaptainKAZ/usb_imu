//
// Created by ZouYu on 2022/7/13.
//
/* USER CODE BEGIN Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_ares.h"
/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t ARESUserRxBufferFS[2048];
uint8_t ARESUserTxBufferFS[2048];

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t ARES_Init_FS(void);
static int8_t ARES_DeInit_FS(void);
static int8_t ARES_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t ARES_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t ARES_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_ARES_ItfTypeDef USBD_Interface_ARES_FS =
        {
                ARES_Init_FS,
                ARES_DeInit_FS,
                ARES_Control_FS,
                ARES_Receive_FS,
                ARES_TransmitCplt_FS
        };

/* Private functions ---------------------------------------------------------*/
static int8_t ARES_Init_FS(void)
{
    /* USER CODE BEGIN 3 */
    /* Set Application Buffers */
    USBD_ARES_SetTxBuffer(&hUsbDeviceFS, ARESUserTxBufferFS, 0);
    USBD_ARES_SetRxBuffer(&hUsbDeviceFS, ARESUserRxBufferFS);
    return (USBD_OK);
    /* USER CODE END 3 */
}

static int8_t ARES_DeInit_FS(void)
{
    /* USER CODE BEGIN 4 */
    return (USBD_OK);
    /* USER CODE END 4 */
}

static int8_t ARES_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
    switch(cmd)
    {

        default:
            break;
    }

    return (USBD_OK);
}
static int8_t ARES_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
    /* USER CODE BEGIN 6 */
    USBD_ARES_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
    USBD_ARES_ReceivePacket(&hUsbDeviceFS);
    return (USBD_OK);
    /* USER CODE END 6 */
}

uint8_t ARES_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
    uint8_t result = USBD_OK;
    /* USER CODE BEGIN 7 */
    USBD_ARES_HandleTypeDef *hares = (USBD_ARES_HandleTypeDef*)hUsbDeviceFS.pClassData;
    if (hares->TxState != 0){
        return USBD_BUSY;
    }
    USBD_ARES_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
    result = USBD_ARES_TransmitPacket(&hUsbDeviceFS);
    /* USER CODE END 7 */
    return result;
}

/**
  * @brief  ARES_TransmitCplt_FS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t ARES_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
    uint8_t result = USBD_OK;
    /* USER CODE BEGIN 13 */
    UNUSED(Buf);
    UNUSED(Len);
    UNUSED(epnum);
    /* USER CODE END 13 */
    return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
