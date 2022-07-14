//
// Created by ZouYu on 2022/7/10.
//
#include "usbd_ares.h"
#include "usbd_ctlreq.h"
#include "usbd_def.h"

static uint8_t USBD_ARES_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);

static uint8_t USBD_ARES_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);

static uint8_t USBD_ARES_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

static uint8_t USBD_ARES_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_ARES_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_ARES_EP0_RxReady(USBD_HandleTypeDef *pdev);

#ifndef USE_USBD_COMPOSITE

static uint8_t *USBD_ARES_GetFSCfgDesc(uint16_t *length);

static uint8_t *USBD_ARES_GetHSCfgDesc(uint16_t *length);

static uint8_t *USBD_ARES_GetOtherSpeedCfgDesc(uint16_t *length);

uint8_t *USBD_ARES_GetDeviceQualifierDescriptor(uint16_t *length);

#endif /* USE_USBD_COMPOSITE  */

#ifndef USE_USBD_COMPOSITE
/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_ARES_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
        {
                USB_LEN_DEV_QUALIFIER_DESC,
                USB_DESC_TYPE_DEVICE_QUALIFIER,
                0x00,
                0x02,
                0x00,
                0x00,
                0x00,
                0x40,
                0x01,
                0x00,
        };
#endif /* USE_USBD_COMPOSITE  */

USBD_ClassTypeDef USBD_ARES =
        {
                USBD_ARES_Init,
                USBD_ARES_DeInit,
                USBD_ARES_Setup,
                NULL,                 /* EP0_TxSent */
                USBD_ARES_EP0_RxReady,
                USBD_ARES_DataIn,
                USBD_ARES_DataOut,
                NULL,
                NULL,
                NULL,
#ifdef USE_USBD_COMPOSITE
                NULL,
  NULL,
  NULL,
  NULL,
#else
                USBD_ARES_GetHSCfgDesc,
                USBD_ARES_GetFSCfgDesc,
                USBD_ARES_GetOtherSpeedCfgDesc,
                USBD_ARES_GetDeviceQualifierDescriptor,
#endif /* USE_USBD_COMPOSITE  */
        };

#ifndef USE_USBD_COMPOSITE
/* USB ARES device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_ARES_CfgDesc[USB_ARES_CONFIG_DESC_SIZ] __ALIGN_END =
        {
                /* Configuration Descriptor */
                0x09,                                       /* bLength: Configuration Descriptor size */
                USB_DESC_TYPE_CONFIGURATION,                /* bDescriptorType: Configuration */
                LOBYTE(USB_ARES_CONFIG_DESC_SIZ),                    /* wTotalLength */
                HIBYTE(USB_ARES_CONFIG_DESC_SIZ),
                0x01,                                       /* bNumInterfaces: 2 interfaces */
                0x01,                                       /* bConfigurationValue: Configuration value */
                USBD_IDX_CONFIG_STR,                                       /* iConfiguration: Index of string descriptor
                                                 describing the configuration */
#if (USBD_SELF_POWERED == 1U)
                0xC0,                                       /* bmAttributes: Bus Powered according to user configuration */
#else
                0x80,                                       /* bmAttributes: Bus Powered according to user configuration */
#endif /* USBD_SELF_POWERED */
                USBD_MAX_POWER,                             /* MaxPower (mA) */

                /*---------------------------------------------------------------------------*/
                /* ARES Realtime Communication  Interface Descriptor - Zero Bandwidth Dummy  */
                /* Interface 0, Alternate Setting 0                                          */
                0x09,                                 /* bLength */
                USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
                0x00,                                 /* bInterfaceNumber */
                0x00,                                 /* bAlternateSetting */
                0x00,                                 /* bNumEndpoints */
                0xFF,                                 /* bInterfaceClass */
                0x00,                                 /* bInterfaceSubClass */
                0x00,                                 /* bInterfaceProtocol */
                USBD_IDX_INTERFACE_STR,               /* iInterface */
                /* 09 byte*/

                /* ARES Realtime Communication  Interface Descriptor - Operational          */
                /* Interface 0, Alternate Setting 1                                         */
                0x09,                                 /* bLength */
                USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
                0x00,                                 /* bInterfaceNumber */
                0x01,                                 /* bAlternateSetting */
                0x02,                                 /* bNumEndpoints */
                0xFF,                                 /* bInterfaceClass */
                0x00,                                 /* bInterfaceSubClass */
                0x00,                                 /* bInterfaceProtocol */
                USBD_IDX_INTERFACE_STR,               /* iInterface */
                /* 09 byte*/
                /* Endpoint 1 - Standard Descriptor */
                0x06,                                 /* bLength */
                USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
                ARES_IN_EP,                           /* bEndpointAddress 1 out endpoint */
                USBD_EP_TYPE_ISOC,                    /* bmAttributes */
                LOBYTE(500U), HIBYTE(500U),       /* wMaxPacketSize in Bytes */
                1,                                    /* bInterval */
                /* 06 byte*/
                /* Endpoint 2 - Standard Descriptor */
                0x06,                                 /* bLength */
                USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
                ARES_OUT_EP,                          /* bEndpointAddress 1 out endpoint */
                USBD_EP_TYPE_ISOC,                    /* bmAttributes */
                LOBYTE(100U), HIBYTE(100U),       /* wMaxPacketSize in Bytes */
                1,                                    /* bInterval */
                /* 06 byte*/
                /*---------------------------------------------------------------------------*/
        };
#endif /* USE_USBD_COMPOSITE  */

static uint8_t ARESInEpAdd = ARES_IN_EP;
static uint8_t ARESOutEpAdd = ARES_OUT_EP;

static uint8_t USBD_ARES_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);
    USBD_ARES_HandleTypeDef *hares;

    hares = (USBD_ARES_HandleTypeDef *) USBD_malloc(sizeof(USBD_ARES_HandleTypeDef));

    if (hares == NULL) {
        pdev->pClassDataCmsit[pdev->classId] = NULL;
        return (uint8_t) USBD_EMEM;
    }

    (void) USBD_memset(hares, 0, sizeof(USBD_ARES_HandleTypeDef));

    pdev->pClassDataCmsit[pdev->classId] = (void *) hares;
    pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];


    pdev->ep_in[ARESInEpAdd & 0xFU].bInterval = ARES_FS_BINTERVAL;
    pdev->ep_out[ARESOutEpAdd & 0xFU].bInterval = ARES_FS_BINTERVAL;
    /* Open EP IN */
    (void) USBD_LL_OpenEP(pdev, ARESInEpAdd, USBD_EP_TYPE_ISOC,
                          ARES_DATA_FS_IN_PACKET_SIZE);
    pdev->ep_in[ARESInEpAdd & 0xFU].is_used = 1U;

    /* Open EP OUT */
    (void) USBD_LL_OpenEP(pdev, ARESOutEpAdd, USBD_EP_TYPE_ISOC,
                          ARES_DATA_FS_OUT_PACKET_SIZE);

    pdev->ep_out[ARESOutEpAdd & 0xFU].is_used = 1U;
    (void) USBD_LL_FlushEP(pdev, ARESInEpAdd);
    (void) USBD_LL_FlushEP(pdev, ARESOutEpAdd);


    hares->RxBuffer = NULL;

    /* Init  physical Interface components */
    ((USBD_ARES_ItfTypeDef *) pdev->pUserData[pdev->classId])->Init();

    /* Init Xfer states */
    hares->TxState = 0U;
    hares->RxState = 0U;

    if (hares->RxBuffer == NULL) {
        return (uint8_t) USBD_EMEM;
    }

    /* Prepare Out endpoint to receive next packet */
    (void) USBD_LL_PrepareReceive(pdev, ARESOutEpAdd, hares->RxBuffer,
                                  ARES_DATA_FS_OUT_PACKET_SIZE);
    return (uint8_t) USBD_OK;
}

static uint8_t USBD_ARES_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    UNUSED(cfgidx);

    /* Close EP IN */
    (void) USBD_LL_CloseEP(pdev, ARESInEpAdd);
    pdev->ep_in[ARESInEpAdd & 0xFU].is_used = 0U;
    pdev->ep_in[ARESInEpAdd & 0xFU].bInterval = 0U;
    /* Close EP OUT */
    (void) USBD_LL_CloseEP(pdev, ARESOutEpAdd);
    pdev->ep_out[ARESOutEpAdd & 0xFU].is_used = 0U;
    pdev->ep_out[ARESOutEpAdd & 0xFU].bInterval = 0U;

    /* DeInit  physical Interface components */
    if (pdev->pClassDataCmsit[pdev->classId] != NULL) {
        ((USBD_ARES_ItfTypeDef *) pdev->pUserData[pdev->classId])->DeInit();
        (void) USBD_free(pdev->pClassDataCmsit[pdev->classId]);
        pdev->pClassDataCmsit[pdev->classId] = NULL;
        pdev->pClassData = NULL;
    }

    return (uint8_t) USBD_OK;
}

static uint8_t USBD_ARES_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req) {
    USBD_ARES_HandleTypeDef *hares = (USBD_ARES_HandleTypeDef *) pdev->pClassDataCmsit[pdev->classId];
    uint16_t len;
    uint8_t ifalt = 0U;
    uint16_t status_info = 0U;
    USBD_StatusTypeDef ret = USBD_OK;

    if (hares == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {
        case USB_REQ_TYPE_CLASS:
            if (req->wLength != 0U) {
                if ((req->bmRequest & 0x80U) != 0U) {
                    ((USBD_ARES_ItfTypeDef *) pdev->pUserData[pdev->classId])->Control(req->bRequest,
                                                                                       (uint8_t *) hares->data,
                                                                                       req->wLength);

                    len = MIN(ARES_REQ_MAX_DATA_SIZE, req->wLength);
                    (void) USBD_CtlSendData(pdev, (uint8_t *) hares->data, len);
                } else {
                    hares->CmdOpCode = req->bRequest;
                    hares->CmdLength = (uint8_t) MIN(req->wLength, USB_MAX_EP0_SIZE);

                    (void) USBD_CtlPrepareRx(pdev, (uint8_t *) hares->data, hares->CmdLength);
                }
            } else {
                ((USBD_ARES_ItfTypeDef *) pdev->pUserData[pdev->classId])->Control(req->bRequest,
                                                                                   (uint8_t *) req, 0U);
            }
            break;

        case USB_REQ_TYPE_STANDARD:
            switch (req->bRequest) {
                case USB_REQ_GET_STATUS:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        (void) USBD_CtlSendData(pdev, (uint8_t *) &status_info, 2U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_GET_INTERFACE:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        (void) USBD_CtlSendData(pdev, (uint8_t *) &hares->alt_setting, 1U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_SET_INTERFACE:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        if ((uint8_t) (req->wValue) <= USBD_MAX_NUM_INTERFACES) {
                            /* Do things only when alt_setting changes */
                            if (hares->alt_setting != (uint8_t) (req->wValue)) {
                                hares->alt_setting = (uint8_t) (req->wValue);
                            }
                            USBD_LL_FlushEP(pdev, ARESInEpAdd);
                            USBD_LL_FlushEP(pdev, ARESOutEpAdd);
                        } else {
                            /* Call the error management function (command will be nacked */
                            USBD_CtlError(pdev, req);
                            ret = USBD_FAIL;
                        }
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_CLEAR_FEATURE:
                    break;

                default:
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                    break;
            }
            break;

        default:
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
    }

    return (uint8_t) ret;
}

static uint8_t USBD_ARES_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    USBD_ARES_HandleTypeDef *hares;
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef *) pdev->pData;

    if (pdev->pClassDataCmsit[pdev->classId] == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    hares = (USBD_ARES_HandleTypeDef *) pdev->pClassDataCmsit[pdev->classId];

    if ((pdev->ep_in[epnum & 0xFU].total_length > 0U) &&
        ((pdev->ep_in[epnum & 0xFU].total_length % hpcd->IN_ep[epnum & 0xFU].maxpacket) == 0U)) {
        /* Update the packet total length */
        pdev->ep_in[epnum & 0xFU].total_length = 0U;

        /* Send ZLP */
        (void) USBD_LL_Transmit(pdev, epnum, NULL, 0U);
    } else {
        hares->TxState = 0U;

        if (((USBD_ARES_ItfTypeDef *) pdev->pUserData[pdev->classId])->TransmitCplt != NULL) {
            ((USBD_ARES_ItfTypeDef *) pdev->pUserData[pdev->classId])->TransmitCplt(hares->TxBuffer, &hares->TxLength,
                                                                                    epnum);
        }
    }

    return (uint8_t) USBD_OK;
}

static uint8_t USBD_ARES_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    USBD_ARES_HandleTypeDef *hares = (USBD_ARES_HandleTypeDef *) pdev->pClassDataCmsit[pdev->classId];

    if (pdev->pClassDataCmsit[pdev->classId] == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    /* Get the received data length */
    hares->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

    /* USB data will be immediately processed, this allow next USB traffic being
    NAKed till the end of the application Xfer */

    ((USBD_ARES_ItfTypeDef *) pdev->pUserData[pdev->classId])->Receive(hares->RxBuffer, &hares->RxLength);

    return (uint8_t) USBD_OK;
}

static uint8_t USBD_ARES_EP0_RxReady(USBD_HandleTypeDef *pdev) {
    USBD_ARES_HandleTypeDef *hares = (USBD_ARES_HandleTypeDef *) pdev->pClassDataCmsit[pdev->classId];

    if (hares == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    if ((pdev->pUserData[pdev->classId] != NULL) && (hares->CmdOpCode != 0xFFU)) {
        ((USBD_ARES_ItfTypeDef *) pdev->pUserData[pdev->classId])->Control(hares->CmdOpCode,
                                                                          (uint8_t *) hares->data,
                                                                          (uint16_t) hares->CmdLength);
        hares->CmdOpCode = 0xFFU;
    }

    return (uint8_t) USBD_OK;
}

#ifndef USE_USBD_COMPOSITE

static uint8_t *USBD_ARES_GetFSCfgDesc(uint16_t *length) {
    USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_ARES_CfgDesc, ARES_OUT_EP);
    USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_ARES_CfgDesc, ARES_IN_EP);

    if (pEpOutDesc != NULL) {
        pEpOutDesc->wMaxPacketSize = ARES_DATA_FS_MAX_PACKET_SIZE;
        pEpOutDesc->bInterval=ARES_FS_BINTERVAL;
    }

    if (pEpInDesc != NULL) {
        pEpInDesc->wMaxPacketSize = ARES_DATA_FS_MAX_PACKET_SIZE;
        pEpInDesc->bInterval=ARES_FS_BINTERVAL;
    }

    *length = (uint16_t) sizeof(USBD_ARES_CfgDesc);
    return USBD_ARES_CfgDesc;
}

static uint8_t *USBD_ARES_GetHSCfgDesc(uint16_t *length) {
    return USBD_ARES_GetFSCfgDesc(length);
}

static uint8_t *USBD_ARES_GetOtherSpeedCfgDesc(uint16_t *length) {
    return USBD_ARES_GetFSCfgDesc(length);
}

uint8_t *USBD_ARES_GetDeviceQualifierDescriptor(uint16_t *length) {
    *length = (uint16_t) sizeof(USBD_ARES_DeviceQualifierDesc);

    return USBD_ARES_DeviceQualifierDesc;
}

#endif /* USE_USBD_COMPOSITE  */

uint8_t USBD_ARES_RegisterInterface(USBD_HandleTypeDef *pdev,
                                   USBD_ARES_ItfTypeDef *fops) {
    if (fops == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    pdev->pUserData[pdev->classId] = fops;

    return (uint8_t) USBD_OK;
}

uint8_t USBD_ARES_SetTxBuffer(USBD_HandleTypeDef *pdev,
                             uint8_t *pbuff, uint32_t length) {
    USBD_ARES_HandleTypeDef *hares = (USBD_ARES_HandleTypeDef *) pdev->pClassDataCmsit[pdev->classId];

    if (hares == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    hares->TxBuffer = pbuff;
    hares->TxLength = length;

    return (uint8_t) USBD_OK;
}

uint8_t USBD_ARES_SetRxBuffer(USBD_HandleTypeDef *pdev, uint8_t *pbuff) {
    USBD_ARES_HandleTypeDef *hares = (USBD_ARES_HandleTypeDef *) pdev->pClassDataCmsit[pdev->classId];

    if (hares == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    hares->RxBuffer = pbuff;

    return (uint8_t) USBD_OK;
}

uint8_t USBD_ARES_TransmitPacket(USBD_HandleTypeDef *pdev) {
    USBD_ARES_HandleTypeDef *hares = (USBD_ARES_HandleTypeDef *) pdev->pClassDataCmsit[pdev->classId];
    USBD_StatusTypeDef ret = USBD_BUSY;

    if (pdev->pClassDataCmsit[pdev->classId] == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    if (hares->TxState == 0U) {
        /* Tx Transfer in progress */
        hares->TxState = 1U;

        /* Update the packet total length */
        pdev->ep_in[ARESInEpAdd & 0xFU].total_length = hares->TxLength;

        /* Transmit next packet */
        (void) USBD_LL_Transmit(pdev, ARESInEpAdd, hares->TxBuffer, hares->TxLength);

        ret = USBD_OK;
    }

    return (uint8_t) ret;
}

uint8_t USBD_ARES_ReceivePacket(USBD_HandleTypeDef *pdev) {
    USBD_ARES_HandleTypeDef *hares = (USBD_ARES_HandleTypeDef *) pdev->pClassDataCmsit[pdev->classId];

    if (pdev->pClassDataCmsit[pdev->classId] == NULL) {
        return (uint8_t) USBD_FAIL;
    }

    if (pdev->dev_speed == USBD_SPEED_HIGH) {
        /* Prepare Out endpoint to receive next packet */
        (void) USBD_LL_PrepareReceive(pdev, ARESOutEpAdd, hares->RxBuffer,
                                      ARES_DATA_HS_OUT_PACKET_SIZE);
    } else {
        /* Prepare Out endpoint to receive next packet */
        (void) USBD_LL_PrepareReceive(pdev, ARESOutEpAdd, hares->RxBuffer,
                                      ARES_DATA_FS_OUT_PACKET_SIZE);
    }

    return (uint8_t) USBD_OK;
}