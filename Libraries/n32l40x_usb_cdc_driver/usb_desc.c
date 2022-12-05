/******************** (C) COPYRIGHT 2020 Nations Technologies Inc ********************
* File Name          : usb_desc.c
* Author             : Nations
* Version            : v1.2.0
* Date               : 09/03/2020
* Description        : Descriptors for Mass Storage Device
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/
 
/**
 * @file usb_desc.c
 * @author Nations
 * @version v1.2.0
 *
 * @copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
 */
/* Includes ------------------------------------------------------------------*/
#include "usb_desc.h"

const uint8_t MASS_DeviceDescriptor[MASS_SIZ_DEVICE_DESC] =
{
    0x12,   /* bLength  */
    0x01,   /* bDescriptorType */
    0x00,   /* bcdUSB, version 2.00 */
    0x02,
 //   0x00,   /* bDeviceClass : each interface define the device class */
   // 0x00,   /* bDeviceSubClass */
   // 0x00,   /* bDeviceProtocol */
		0xEF,                       /*bDeviceClass*/
		0x02,                       /*bDeviceSubClass*/
		0x01,                       /*bDeviceProtocol*/
	
    0x40,   /* bMaxPacketSize0 0x40 = 64 */
    0xF5,   /*idVendor (0x19F5)*/
    0x19,
    0x20,   /* idProduct */
    0x57,
    0x00,   /* bcdDevice 2.00*/
    0x02,
    1,              /* index of string Manufacturer  */
    /**/
    2,              /* index of string descriptor of product*/
    /* */
    3,              /* */
    /* */
    /* */
    0x01    /*bNumConfigurations */
};
const uint8_t MASS_ConfigDescriptor[MASS_SIZ_CONFIG_DESC] =
{
    /*Configuration Descriptor*/
    0x09,   /* bLength: Configuration Descriptor size */
    0x02,      /* bDescriptorType: Configuration */
    MASS_SIZ_CONFIG_DESC,       /* wTotalLength:no of returned bytes */
    0x00,
    0x03,   /* bNumInterfaces:CDC 2 MSC 1 interface */
    0x01,   /* bConfigurationValue: Configuration value */
    0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
    0xC0,   /* bmAttributes: self powered */
    0x32,   /* MaxPower 0 mA */
	

		// +++lakun: IAD(Interface Association Descriptor)，用于指示CDC
		0X08,  // bLength: Interface Descriptor size，固定值
		0X0B,  // bDescriptorType: IAD，固定值
		0X00,  // bFirstInterface，第一个接口的起始序号，从0开始
		0X02,  // bInterfaceCount，本IAD下的接口数量
		0X02,  // bFunctionClass: CDC，表明该IAD是一个CDC类型的设备
		0X02,  // bFunctionSubClass：子类型，默认即可
		0X01,  // bFunctionProtocol：控制协议，默认即可
		0X00,  // iFunction
	
    /*Interface Descriptor*/
    0x09,   /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: Interface */
    /* Interface descriptor type */
    0x00,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x01,   /* bNumEndpoints: One endpoints used */
    0x02,   /* bInterfaceClass: Communication Interface Class */
    0x02,   /* bInterfaceSubClass: Abstract Control Model */
    0x01,   /* bInterfaceProtocol: Common AT commands */
    0x00,   /* iInterface: */
    /*Header Functional Descriptor*/
    0x05,   /* bLength: Endpoint Descriptor size */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x00,   /* bDescriptorSubtype: Header Func Desc */
    0x10,   /* bcdCDC: spec release number */
    0x01,
    /*Call Management Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x01,   /* bDescriptorSubtype: Call Management Func Desc */
    0x00,   /* bmCapabilities: D0+D1 */
    0x01,   /* bDataInterface: 1 */
    /*ACM Functional Descriptor*/
    0x04,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
    0x02,   /* bmCapabilities */
    /*Union Functional Descriptor*/
    0x05,   /* bFunctionLength */
    0x24,   /* bDescriptorType: CS_INTERFACE */
    0x06,   /* bDescriptorSubtype: Union func desc */
    0x00,   /* bMasterInterface: Communication class interface */
    0x01,   /* bSlaveInterface0: Data Class Interface */
    /*Endpoint 2 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x84,   /* bEndpointAddress: (IN2) */
    0x03,   /* bmAttributes: Interrupt */
    VIRTUAL_COM_PORT_INT_SIZE,      /* wMaxPacketSize: */
    0x00,
    0xFF,   /* bInterval: */
    /*Data class interface descriptor*/
    0x09,   /* bLength: Endpoint Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,  /* bDescriptorType: */
    0x01,   /* bInterfaceNumber: Number of Interface */
    0x00,   /* bAlternateSetting: Alternate setting */
    0x02,   /* bNumEndpoints: Two endpoints used */
    0x0A,   /* bInterfaceClass: CDC */
    0x00,   /* bInterfaceSubClass: */
    0x00,   /* bInterfaceProtocol: */
    0x00,   /* iInterface: */
    /*Endpoint 3 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x03,   /* bEndpointAddress: (OUT3) */
    0x02,   /* bmAttributes: Bulk */
    VIRTUAL_COM_PORT_DATA_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00,   /* bInterval: ignore for Bulk transfer */
    /*Endpoint 1 Descriptor*/
    0x07,   /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,   /* bDescriptorType: Endpoint */
    0x85,   /* bEndpointAddress: (IN1) */
    0x02,   /* bmAttributes: Bulk */
    VIRTUAL_COM_PORT_DATA_SIZE,             /* wMaxPacketSize: */
    0x00,
    0x00,    /* bInterval */
		
		 /*****************MSC Interface Descirptor*******************/
		0X08,  // bLength: Interface Descriptor size，固定值
		0X0B,  // bDescriptorType: IAD，固定值
		0X02,  // bFirstInterface，接口的起始序号（第0、1编号的接口用于CDC1，所以从2开始）
		0X01,  // bInterfaceCount，本IAD下的接口数量
		0X08,  // bFunctionClass: MSC，表明该IAD是一个MSC类型的设备
		0X06,  // bFunctionSubClass：子类型，默认即可
		0X50,  // bFunctionProtocol：控制协议，默认即可
		4,  // iFunction

    0x09, //bLength
    0x04, //bDescriptorType
    0x02, //bInterfaceNumber
    0x00, //bAlternateSetting
    0x02, //bNumEndpoints
    0x08, //bInterfaceClass
    0x06, //bInterfaceSubClass
    0x50, //bInterfaceProtocol
		4, //iConfiguration

/* iInterface: */
    /* 18 */
    0x07,   /*Endpoint descriptor length = 7*/
    0x05,   /*Endpoint descriptor type */
    0x81,   /*Endpoint address (IN, address 1) */
    0x02,   /*Bulk endpoint type */
    0x40,   /*Maximum packet size (64 bytes) */
    0x00,
    0x00,   /*Polling interval in milliseconds */

    /* 25 */
    0x07,   /*Endpoint descriptor length = 7 */
    0x05,   /*Endpoint descriptor type */
    0x02,   /*Endpoint address (OUT, address 2) */
    0x02,   /*Bulk endpoint type */
    0x40,   /*Maximum packet size (64 bytes) */
    0x00,
    0x00     /*Polling interval in milliseconds*/
    /*32*/
		
};
//const uint8_t MASS_ConfigDescriptor[MASS_SIZ_CONFIG_DESC] =
//{

//    0x09,   /* bLength: Configuation Descriptor size */
//    0x02,   /* bDescriptorType: Configuration */
//    MASS_SIZ_CONFIG_DESC,

//    0x00,
//    0x01,   /* bNumInterfaces: 1 interface */
//    0x01,   /* bConfigurationValue: */
//    /*      Configuration value */
//    0x00,   /* iConfiguration: */
//    /*      Index of string descriptor */
//    /*      describing the configuration */
//    0xC0,   /* bmAttributes: */
//    /*      bus powered */
//    100,   /* MaxPower 100 mA */

//    /*****************Interface Descirptor*******************/

//    0x09, //bLength
//    0x04, //bDescriptorType
//    0x00, //bInterfaceNumber
//    0x00, //bAlternateSetting
//    0x02, //bNumEndpoints
//    0x08, //bInterfaceClass
//    0x06, //bInterfaceSubClass
//    0x50, //bInterfaceProtocol
//    4, //iConfiguration

///* iInterface: */
//    /* 18 */
//    0x07,   /*Endpoint descriptor length = 7*/
//    0x05,   /*Endpoint descriptor type */
//    0x81,   /*Endpoint address (IN, address 1) */
//    0x02,   /*Bulk endpoint type */
//    0x40,   /*Maximum packet size (64 bytes) */
//    0x00,
//    0x00,   /*Polling interval in milliseconds */

//    /* 25 */
//    0x07,   /*Endpoint descriptor length = 7 */
//    0x05,   /*Endpoint descriptor type */
//    0x02,   /*Endpoint address (OUT, address 2) */
//    0x02,   /*Bulk endpoint type */
//    0x40,   /*Maximum packet size (64 bytes) */
//    0x00,
//    0x00     /*Polling interval in milliseconds*/
//    /*32*/
//};
const uint8_t MASS_StringLangID[MASS_SIZ_STRING_LANGID] =
{
    MASS_SIZ_STRING_LANGID,
    0x03,
    0x09,
    0x04
}
;      /* LangID = 0x0409: U.S. English */
const uint8_t MASS_StringVendor[MASS_SIZ_STRING_VENDOR] =
{
    MASS_SIZ_STRING_VENDOR, /* Size of manufaturer string */
    0x03,           /* bDescriptorType = String descriptor */
    /* Manufacturer: "NATIONS" */
    'N', 0, 'A', 0, 'T', 0, 'I', 0, 'O', 0, 'N', 0, 'S', 0
};
const uint8_t MASS_StringProduct[MASS_SIZ_STRING_PRODUCT] =
{
    MASS_SIZ_STRING_PRODUCT,
    0x03,
    /* Product name: "N32L40x:USB Mass Storage" */
    'N', 0, '3', 0, '2', 0, 'L', 0, '4', 0, '0', 0, 'x', 0, ' ', 0, 
    'S', 0, 't', 0, 'o', 0, 'r', 0, 'a', 0, 'g', 0, 'e', 0

};

uint8_t MASS_StringSerial[MASS_SIZ_STRING_SERIAL] =
{
    MASS_SIZ_STRING_SERIAL,
    0x03,
    /* N23L40x */
    'N', 0, '3', 0, '2', 0, 'L', 0, '4', 0, '0', 0, 'x', 0,
};
const uint8_t MASS_StringInterface[MASS_SIZ_STRING_INTERFACE] =
{
    MASS_SIZ_STRING_INTERFACE,
    0x03,
    /* Interface 0: "NATIONS Mass" */
    'N', 0, 'A', 0, 'T', 0, 'I', 0, 'O', 0, 'N', 'S', 0, ' ', 0, 'M', 0, 'a', 0, 's', 0, 's', 0
};

