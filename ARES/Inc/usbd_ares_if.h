//
// Created by ZouYu on 2022/7/13.
//

#ifndef USB_IMU_USBD_ARES_IF_H
#define USB_IMU_USBD_ARES_IF_H
#include "usbd_ares.h"
#ifdef __cplusplus
extern "C" {
#endif
uint8_t ARES_Transmit_FS(uint8_t* Buf, uint16_t Len);
extern USBD_ARES_ItfTypeDef USBD_Interface_ARES_FS;
#ifdef __cplusplus
}
#endif

#endif //USB_IMU_USBD_ARES_IF_H
