//
// Created by ZouYu on 2022/7/10.
//

#ifndef USB_IMU_USBD_ARES_H
#define USB_IMU_USBD_ARES_H
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup usbd_cdc
  * @brief This file is the Header file for usbd_cdc.c
  * @{
  */


/** @defgroup usbd_cdc_Exported_Defines
  * @{
  */
#ifndef ARES_IN_EP
#define ARES_IN_EP                                   0x81U  /* EP1 for data IN */
#endif /* ARES_IN_EP */
#ifndef ARES_OUT_EP
#define ARES_OUT_EP                                  0x01U  /* EP1 for data OUT */
#endif /* ARES_OUT_EP */

#ifndef ARES_HS_BINTERVAL
#define ARES_HS_BINTERVAL                            0x01U
#endif /* ARES_HS_BINTERVAL */

#ifndef ARES_FS_BINTERVAL
#define ARES_FS_BINTERVAL                            0x01U
#endif /* ARES_FS_BINTERVAL */

/* ARES Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#define ARES_DATA_HS_MAX_PACKET_SIZE                 500U  /* Endpoint IN & OUT Packet size */
#define ARES_DATA_FS_MAX_PACKET_SIZE                 500U  /* Endpoint IN & OUT Packet size */

#define USB_ARES_CONFIG_DESC_SIZ                     41U
#define ARES_DATA_HS_IN_PACKET_SIZE                  ARES_DATA_HS_MAX_PACKET_SIZE
#define ARES_DATA_HS_OUT_PACKET_SIZE                 ARES_DATA_HS_MAX_PACKET_SIZE

#define ARES_DATA_FS_IN_PACKET_SIZE                  ARES_DATA_FS_MAX_PACKET_SIZE
#define ARES_DATA_FS_OUT_PACKET_SIZE                 ARES_DATA_FS_MAX_PACKET_SIZE

#define ARES_REQ_MAX_DATA_SIZE                       0x10

/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */



typedef struct USBD_ARES_Itf
{
    int8_t (* Init)(void);
    int8_t (* DeInit)(void);
    int8_t (* Control)(uint8_t cmd, uint8_t *pbuf, uint16_t length);
    int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);
    int8_t (* TransmitCplt)(uint8_t *Buf, uint32_t *Len, uint8_t epnum);
} USBD_ARES_ItfTypeDef;


typedef struct
{
    uint32_t alt_setting;
    uint32_t data[ARES_DATA_HS_MAX_PACKET_SIZE / 4U];      /* Force 32-bit alignment */
    uint8_t  CmdOpCode;
    uint8_t  CmdLength;
    uint8_t  *RxBuffer;
    uint8_t  *TxBuffer;
    uint32_t RxLength;
    uint32_t TxLength;

    __IO uint32_t TxState;
    __IO uint32_t RxState;
} USBD_ARES_HandleTypeDef;



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern USBD_ClassTypeDef USBD_ARES;
#define USBD_ARES_CLASS &USBD_ARES
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t USBD_ARES_RegisterInterface(USBD_HandleTypeDef *pdev,
                                   USBD_ARES_ItfTypeDef *fops);

uint8_t USBD_ARES_SetTxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff,
                             uint32_t length);

uint8_t USBD_ARES_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff);
uint8_t USBD_ARES_ReceivePacket(USBD_HandleTypeDef *pdev);
uint8_t USBD_ARES_TransmitPacket(USBD_HandleTypeDef *pdev);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif //USB_IMU_USBD_ARES_H
