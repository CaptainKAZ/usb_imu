# ARES USB 通信类 下位机

通过对 Robomaster C型开发板进行编程，实现实时的单片机数据与电脑双向传输。主要基于CDC类进行改进。

## 描述符

### 设备描述符

包含于`usbd_desc.c`中
```c
__ALIGN_BEGIN uint8_t USBD_FS_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END =
{
0x12,                       /*bLength */
USB_DESC_TYPE_DEVICE,       /*bDescriptorType*/
#if (USBD_LPM_ENABLED == 1)
0x01,                       /*bcdUSB */ /* changed to USB version 2.01
in order to support LPM L1 suspend
resume test of USBCV3.0*/
#else
0x00,                       /*bcdUSB */
#endif /* (USBD_LPM_ENABLED == 1) */
0x02,
0x02,                       /*bDeviceClass*/
0x02,                       /*bDeviceSubClass*/
0x00,                       /*bDeviceProtocol*/
USB_MAX_EP0_SIZE,           /*bMaxPacketSize*/
LOBYTE(USBD_VID),           /*idVendor*/
HIBYTE(USBD_VID),           /*idVendor*/
LOBYTE(USBD_PID_FS),        /*idProduct*/
HIBYTE(USBD_PID_FS),        /*idProduct*/
0x00,                       /*bcdDevice rel. 2.00*/
0x02,
USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
USBD_IDX_PRODUCT_STR,       /*Index of product string*/
USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
USBD_MAX_NUM_CONFIGURATION  /*bNumConfigurations*/
};
```

| 描述符                | 值         | 备注                                         |
|--------------------|-----------|--------------------------------------------|
| bLength            | 0x12      | 设备描述符长度                                    |
| bDescriptorType    | 0x01      | 表示这个是个device而不是主机                          |
| bcdUSB             | 0x00 0x02 | 小端序 表示USB 2.0                              |
| bDeviceClass       | 0x00      | 设备类别 在接口描述符中实现                             |
| bDeviceSubClass    | 0x00      | 设备子类别                                      |
| bDeviceProtocol    | 0x00      | 必须为 0 因为设备类别未指定                            |
| bMaxPacketSize     | 0x40(64U) | 控制端点EP0最长包为64                              |
| idVendor           | 0x5241    | 小端序的AR  *(unsigned int *)(char[]){'A','R'} |
| idProduct          | 0x5345    | 小端序的ES  *(unsigned int *)(char[]){'E','S'} |
| bcdDevice          | 0x00 0x01 | 设备版本 1.0                                   |
| iManufacturer      | 0x01      | USBD_IDX_MFC_STR 设备制造商描述索引位置               |
| iProduct           | 0x02      | USBD_IDX_PRODUCT_STR 产品描述字符串索引位置           |
| iSerialNumber      | 0x03      | USBD_IDX_SERIAL_STR  产品序列号字符串索引位置          |
| bNumConfigurations | 0x01      | 就一个配置描述符                                   |

### 设备字符串描述符

包含于`usbd_desc.c`中
```
#define USBD_LANGID_STRING     1033
#define USBD_MANUFACTURER_STRING     "Association of Robotics Engineers at SUSTech"
#define USBD_PRODUCT_STRING_FS     "ARES realtime STM32 USB slave"
#define USBD_CONFIGURATION_STRING_FS     "Realtime Communication Config"
#define USBD_INTERFACE_STRING_FS     "Realtime Communication Interface"
```

| 描述符             | 值                                            | 备注                   |
|-----------------|----------------------------------------------|----------------------|
| LangIdStr       | 1033                                         | EN-US 设备             |
| ManufacturerStr | Association of Robotics Engineers at SUSTech | 制造者名字                |
| ProductStr      | ARES realtime STM32 USB slave                | 产品名                  |
| SerialNumberStr | Get_SerialNum()                              | 自动获得STM32的UID        |
| InterfaceStr    | Realtime Communication Interface             | 实际上index是没有在描述符里面指出的 |

所有设备字符串描述符都需要被转化成Unicode编码，所以在获取的时候使用的是函数，内部调用`USBD_GetString()`函数把普通ASCII字符串转化为Unicode放入 `USBD_StrDesc`然后再发送

### 配置描述符

以下所有描述符都包含在`usbd_ares.c`中

```c
__ALIGN_BEGIN static uint8_t USBD_ARES_CfgDesc[USB_ARES_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /* Configuration Descriptor */
  0x09,                                       /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,                /* bDescriptorType: Configuration */
  LOBYTE(USB_ARES_CONFIG_DESC_SIZ),           /* wTotalLength */
  HIBYTE(USB_ARES_CONFIG_DESC_SIZ),
  0x01,                                       /* bNumInterfaces: 1 interfaces */
  0x01,                                       /* bConfigurationValue: Configuration value */
  USBD_IDX_CONFIG_STR,                                       /* iConfiguration: Index of string descriptor
                                                 describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                       /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                       /* bmAttributes: Bus Powered according to user configuration */
#endif /* USBD_SELF_POWERED */
  USBD_MAX_POWER,                             /* MaxPower (mA) */
  /* 09 byte*/

```

| 描述符                | 值     | 备注                                           |
|--------------------|-------|----------------------------------------------|
| bLength            | 9     | 描述符长度                                        |
| bDescriptorType    | 0x02  | 描述符类型为 USB_DESC_TYPE_CONFIGURATION 配置描述符     |
| wTotalLength       | 67    | 配置所返回的所有数量的大小(包括所有interface和endpoint的大小 还没算） |
| bNumInterface      | 1     | 就一个接口（待扩展）                                   |
| bConfigurationVale | 1     | 就是config的序号 根据setConfig请求选择不同序号的配置           |
| iConfiguration     | 4     | Config描述字符串的索引值                              |
| bmAttribute        | 0xC0  | 需要总线供电                                       |
| MaxPower           | 0x32U | 最大消耗100mA                                    |

### 接口描述符

存在一个接口描述符，但是根据USB标准有两个备用设置，这主要考虑到USB标准中提到设备在默认配置下不应该包含同步传输这种占用固定带宽的端点。`libusb_set_interface_alt_setting()`可以主动设置备用配置序号，原理是发送USB_REQ_SET_INTERFACE控制请求，在`USBD_ARES_Setup()`函数中需要处理

```c
  /* ARES Realtime Communication  Interface Descriptor - Zero Bandwidth Dummy      */
  /* Interface 0, Alternate Setting 0                                              */
  ARES_INTERFACE_DESC_SIZE,             /* bLength */
  USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
  0x00,                                 /* bInterfaceNumber */
  0x00,                                 /* bAlternateSetting */
  0x00,                                 /* bNumEndpoints */
  0xFF,                                 /* bInterfaceClass */
  0x00,                                 /* bInterfaceSubClass */
  0x00,                                 /* bInterfaceProtocol */
  USBD_IDX_INTERFACE_STR,               /* iInterface */
  /* 09 byte*/

  /* ARES Realtime Communication  Interface Descriptor - Operational            */
  /* Interface 0, Alternate Setting 1                                           */
  ARES_INTERFACE_DESC_SIZE,             /* bLength */
  USB_DESC_TYPE_INTERFACE,              /* bDescriptorType */
  0x00,                                 /* bInterfaceNumber */
  0x01,                                 /* bAlternateSetting */
  0x02,                                 /* bNumEndpoints */
  0xFF,                                 /* bInterfaceClass */
  0x00,                                 /* bInterfaceSubClass */
  0x00,                                 /* bInterfaceProtocol */
  USBD_IDX_INTERFACE_STR,               /* iInterface */
  /* 09 byte*/
```

| 描述符                | 值    | 备注                             |
|--------------------|------|--------------------------------|
| bLength            | 0x09 | 描述符长度                          |
| bDescriptorType    | 0x04 | 为接口描述符 USB_DESC_TYPE_INTERFACE |
| bInterfaceNumber   | 0    | 接口序号                           |
| bAlternateSetting  | 0/1  | 有两个 分别为0和1                     |
| bNumEndpoints      | 0/2  | dummy没有端点 实际工作有两个端点            |
| bInterfaceClass    | 0xFF | Vendor Specific Class 自定义      |
| bInterfaceSubClass | 0x00 | 自定义 随便给                        |
| bInterfaceProtocol | 0x00 | 自定义 随便给                        |
| iInterface         | 0x05 | 描述该接口的字符串索引值                   |

### 端点描述符

两个端点 同步in和同步out 考虑到可能不能处理整64大小的包 设置同步in最大为500（理论带宽500kB/s） 同步out最大为100

```c
  /* Endpoint 1 - Standard Descriptor */
  0x06,                                 /* bLength */
  USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
  ARES_IN_EP,                           /* bEndpointAddress 1 out endpoint */
  USBD_EP_TYPE_ISOC,                    /* bmAttributes */
  LOBYTE(500U),HIBYTE(500U),            /* wMaxPacketSize in Bytes */
  1,                                    /* bInterval */
  /* 06 byte*/
  /* Endpoint 2 - Standard Descriptor */
  0x06,                                 /* bLength */
  USB_DESC_TYPE_ENDPOINT,               /* bDescriptorType */
  ARES_OUT_EP,                          /* bEndpointAddress 1 out endpoint */
  USBD_EP_TYPE_ISOC,                    /* bmAttributes */
  LOBYTE(100U),HIBYTE(100U),            /* wMaxPacketSize in Bytes */
  1,                                    /* bInterval */
  /* 06 byte*/
```

## 控制端点EP0实现

## 同步传输端点 EP01 EP81 实现

## 中断传输端点 EP02 EP82 实现