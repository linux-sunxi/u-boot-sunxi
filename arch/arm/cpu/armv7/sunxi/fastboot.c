/*
*************************************************************************************
*                         			      u-boot
*					                     Fast Boot
*
*				        (c) Copyright 2006-2011, All winners Co,Ld.
*							       All Rights Reserved
*
* File Name 	: fastboot.c
*
* Author 		: javen
*
* Description 	:
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	  2011-10-8            1.0          create this file
*
*************************************************************************************
*/

#include <common.h>
#include <asm/io.h>
#include <asm/types.h>
#include <environment.h>
#include <command.h>
#include <usbdevice.h>
#include <fastboot.h>

#ifdef CONFIG_FASTBOOT

#include  "usb/usb_bsp.h"

//-----------------------------------------------------------------------------
//   宏定义
//-----------------------------------------------------------------------------

/* debug */
#define  DMSG_PRINT(stuff...)		printf(stuff)
#define  DMSG_ERR(...)        		(DMSG_PRINT("WRN:L%d(%s):", __LINE__, __FILE__), DMSG_PRINT(__VA_ARGS__))

#if  0
    #define DMSG_DEBUG        			DMSG_PRINT
#else
    #define DMSG_DEBUG(...)
#endif

/* 普通信息打印 */
#if  0
    #define DMSG_INFO         			DMSG_PRINT
#else
    #define DMSG_INFO(...)
#endif

/* 严重警告 */
#if	0
    #define DMSG_PANIC        			DMSG_ERR
#else
    #define DMSG_PANIC(...)
#endif

/* usb & ccmu */
#define FASTBOOT_USB_BASE               0x01c13000
#define FASTBOOT_SRAM_BASE              0x01c00000
#define FASTBOOT_CCMU                   0x01c20000

/* vid & pid */
#define	DEVICE_VENDOR_ID  					0x0BB4  //0x1F3A
#define	DEVICE_PRODUCT_ID 					0x0FFF  //0x1010
#define	DEVICE_BCD        					0x0200

#define DEVICE_MANUFACTURER		            "USB Developer"  		/* 厂商信息 	*/
#define DEVICE_PRODUCT				        "Android Fastboot"  	/* 产品信息 	*/
#define DEVICE_SERIAL_NUMBER			    "20080411"  			/* 产品序列号 	*/
#define DEVICE_CONFIG                       "Android Fastboot"
#define DEVICE_INTERFACE                    "Android Bootloader Interface"

/* String 0 is the language id */
#define DEVICE_STRING_PRODUCT_INDEX       	1
#define DEVICE_STRING_SERIAL_NUMBER_INDEX 	2
#define DEVICE_STRING_CONFIG_INDEX        	3
#define DEVICE_STRING_INTERFACE_INDEX     	4
#define DEVICE_STRING_MANUFACTURER_INDEX  	5

#define DEVICE_STRING_MAX_INDEX           	DEVICE_STRING_MANUFACTURER_INDEX
#define DEVICE_STRING_LANGUAGE_ID         	0x0409 		/* English (United States) */

#define CONFIGURATION_NORMAL        1

/* ep packet size */
#define EP0_MAX_PACKET_SIZE					64
#define FULL_SPEED_EP_MAX_PACKET_SIZE		64
#define HIGH_SPEED_EP_MAX_PACKET_SIZE		512

#define CTRL_EP_INDEX		0
#define BULK_IN_EP_INDEX	1	/* tx */
#define BULK_OUT_EP_INDEX	2	/* rx */

#define FASTBOOT_EP_NUM     3

/* ep fifo size */
#define EP0_FIFOSIZE		64
#define BULK_FIFOSIZE		512

#define PRINT_REQ(r)                                                             \
  printf ("usb request \n");				                         \
  printf ("\tbmRequestType 0x%2.2x\n", r.bmRequestType);	                 \
  if (0 == (r.bmRequestType & USB_REQ_DIRECTION_MASK))                           \
    printf ("\t\tDirection : To device\n");				         \
  else                                                                           \
    printf ("\t\tDirection : To host\n");				         \
  if (USB_TYPE_STANDARD == (r.bmRequestType & USB_TYPE_STANDARD))                \
    printf ("\t\tType      : Standard\n");				         \
  if (USB_TYPE_CLASS == (r.bmRequestType & USB_TYPE_CLASS))                      \
    printf ("\t\tType      : Standard\n");				         \
  if (USB_TYPE_VENDOR == (r.bmRequestType & USB_TYPE_VENDOR))                    \
    printf ("\t\tType      : Standard\n");				         \
  if (USB_TYPE_RESERVED == (r.bmRequestType & USB_TYPE_RESERVED))                \
    printf ("\t\tType      : Standard\n");				         \
  if (USB_REQ_RECIPIENT_DEVICE == (r.bmRequestType & USB_REQ_RECIPIENT_MASK))    \
    printf ("\t\tRecipient : Device\n");				         \
  if (USB_REQ_RECIPIENT_INTERFACE == (r.bmRequestType & USB_REQ_RECIPIENT_MASK)) \
    printf ("\t\tRecipient : Interface\n");				         \
  if (USB_REQ_RECIPIENT_ENDPOINT == (r.bmRequestType & USB_REQ_RECIPIENT_MASK))  \
    printf ("\t\tRecipient : Endpoint\n");				         \
  if (USB_REQ_RECIPIENT_OTHER == (r.bmRequestType & USB_REQ_RECIPIENT_MASK))     \
    printf ("\t\tRecipient : Other\n");				                 \
  printf ("\tbRequest      0x%2.2x\n", r.bRequest);		                 \
  if (r.bRequest == USB_REQ_GET_STATUS)                                          \
    printf ("\t\tGET_STATUS\n");	                                         \
  else if (r.bRequest == USB_REQ_SET_ADDRESS)                                    \
    printf ("\t\tSET_ADDRESS\n");	                                         \
  else if (r.bRequest == USB_REQ_SET_FEATURE)                                    \
    printf ("\t\tSET_FEATURE\n");	                                         \
  else if (r.bRequest == USB_REQ_GET_DESCRIPTOR)                                 \
    printf ("\t\tGET_DESCRIPTOR\n");	                                         \
  else if (r.bRequest == USB_REQ_SET_CONFIGURATION)                              \
    printf ("\t\tSET_CONFIGURATION\n");	                                         \
  else if (r.bRequest == USB_REQ_SET_INTERFACE)                                  \
    printf ("\t\tUSB_REQ_SET_INTERFACE\n");	                                 \
  else                                                                           \
    printf ("\tUNKNOWN\n");	                                                 \
  printf ("\twValue        0x%4.4x\n", r.wValue);		                 \
  printf ("\twIndex        0x%4.4x\n", r.wIndex);		                 \
  printf ("\twLength       0x%4.4x\n", r.wLength);


//-----------------------------------------------------------------------------
//   数据结构
//-----------------------------------------------------------------------------
struct sw_udc{
    u32 usb_base;
    u32 sram_base;
    u32 ccmu_base;

	u32 address;		/* device address, that host distribute */
	u32 speed;		    /* flag. is high speed? 				*/

	u32 bulk_ep_size;	/* bulk ep max packet size 				*/
	u32 fifo_size;		/* fifo size 							*/

	__hdle bsp;
	struct usb_device_request req;
};

enum usb_device_speed {
	USB_SPEED_UNKNOWN = 0,			    /* enumerating */
	USB_SPEED_LOW, USB_SPEED_FULL,		/* usb 1.1 */
	USB_SPEED_HIGH,				        /* usb 2.0 */
	USB_SPEED_WIRELESS,			        /* wireless (usb 2.5) */
	USB_SPEED_SUPER,			        /* usb 3.0 */
};

//-----------------------------------------------------------------------------
//  预定义
//-----------------------------------------------------------------------------

static char *device_strings[DEVICE_STRING_MANUFACTURER_INDEX + 1];
static struct cmd_fastboot_interface *fastboot_interface = NULL;
static struct sw_udc udc;

static u8 fastboot_fifo_ep0[EP0_FIFOSIZE];
static u8 fastboot_fifo_bulk_ep[BULK_FIFOSIZE];

static unsigned int set_address = 0;
static unsigned int deferred_rx = 0;    /* rx 传输延时 */

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
static void print_sw_udc(struct sw_udc *sw_udc)
{
    DMSG_INFO("usb_base     = 0x%x\n", sw_udc->usb_base);
    DMSG_INFO("sram_base    = 0x%x\n", sw_udc->sram_base);
    DMSG_INFO("ccmu_base    = 0x%x\n", sw_udc->ccmu_base);

    DMSG_INFO("address      = 0x%x\n", sw_udc->address);
    DMSG_INFO("speed        = 0x%x\n", sw_udc->speed);

    DMSG_INFO("bulk_ep_size = 0x%x\n", sw_udc->bulk_ep_size);
    DMSG_INFO("fifo_size    = 0x%x\n", sw_udc->fifo_size);

    return;
}

/*
*******************************************************************************
*                     WriteDataStatusComplete
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static void WriteDataStatusComplete(__hdle hUSB, u32 ep_type, u32 complete)
{
	USBC_Dev_WriteDataStatus(hUSB, ep_type, complete);

    /* wait for tx packet sent out */
	while(USBC_Dev_IsWriteDataReady(hUSB, ep_type)){
        udelay(1);
    }

    if(ep_type == USBC_EP_TYPE_EP0){
        /* clear data end */
        if(complete){
            USBC_Dev_Ctrl_ClearSetupEnd(hUSB);
        }

        /* clear irq */
        USBC_INT_ClearEpPending(hUSB, USBC_EP_TYPE_TX, 0);
    }

	return;
}

/*
*******************************************************************************
*                     ReadDataStatusComplete
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static void ReadDataStatusComplete(__hdle hUSB, u32 ep_type, u32 complete)
{
	USBC_Dev_ReadDataStatus(hUSB, ep_type, complete);

    udelay(2);

    if(ep_type == USBC_EP_TYPE_EP0){
        /* clear data end */
        if(complete){
            USBC_Dev_Ctrl_ClearSetupEnd(hUSB);
        }

        /* clear irq */
        USBC_INT_ClearEpPending(hUSB, USBC_EP_TYPE_TX, 0);
    }

	return;
}

/*
*******************************************************************************
*                     fastboot_bulk_endpoint_reset
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static void fastboot_bulk_endpoint_reset (void)
{
	u8 old_ep_index = 0;

    DMSG_INFO("fastboot_bulk_endpoint_reset\n");

	old_ep_index = USBC_GetActiveEp(udc.bsp);

	print_sw_udc(&udc);

	/* tx */
	USBC_SelectActiveEp(udc.bsp, BULK_IN_EP_INDEX);
    USBC_Dev_ConfigEp(udc.bsp, USBC_TS_TYPE_BULK, USBC_EP_TYPE_TX, 1, udc.bulk_ep_size & 0x7ff);
	USBC_ConfigFifo(udc.bsp, USBC_EP_TYPE_TX, 1, udc.fifo_size, 1024);		//1k开始, 每个ep分配1K的空间
	USBC_INT_EnableEp(udc.bsp, USBC_EP_TYPE_TX, BULK_IN_EP_INDEX);

	/* rx */
	USBC_SelectActiveEp(udc.bsp, BULK_OUT_EP_INDEX);
	USBC_Dev_ConfigEp(udc.bsp, USBC_TS_TYPE_BULK, USBC_EP_TYPE_RX, 1, udc.bulk_ep_size & 0x7ff);
	USBC_ConfigFifo(udc.bsp, USBC_EP_TYPE_RX, 1, udc.fifo_size, 2048);		//2k开始, 每个ep分配1K的空间
	USBC_INT_EnableEp(udc.bsp, USBC_EP_TYPE_RX, BULK_OUT_EP_INDEX);

	USBC_SelectActiveEp(udc.bsp, old_ep_index);

	return;
}

/*
*******************************************************************************
*                     fastboot_reset
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static void fastboot_reset(void)
{
    DMSG_INFO("fastboot_reset\n");

	udc.address = 0;
	udc.speed = USB_SPEED_UNKNOWN;

	udc.bulk_ep_size = HIGH_SPEED_EP_MAX_PACKET_SIZE;
	udc.fifo_size = BULK_FIFOSIZE;

    USBC_ForceId(udc.bsp, USBC_ID_TYPE_DEVICE);
	USBC_ForceVbusValid(udc.bsp, USBC_VBUS_TYPE_HIGH);

    USBC_Dev_ConectSwitch(udc.bsp, USBC_DEVICE_SWITCH_OFF);

    USBC_EnableDpDmPullUp(udc.bsp);
    USBC_EnableIdPullUp(udc.bsp);
    USBC_SelectBus(udc.bsp, USBC_IO_TYPE_PIO, 0, 0);
    USBC_ConfigFIFO_Base(udc.bsp, udc.sram_base, USBC_FIFO_MODE_8K);
    USBC_EnhanceSignal(udc.bsp);

#ifdef	CONFIG_USB_1_1_DEVICE
	USBC_Dev_ConfigTransferMode(udc.bsp, USBC_TS_TYPE_BULK, USBC_TS_MODE_FS);
#else
	USBC_Dev_ConfigTransferMode(udc.bsp, USBC_TS_TYPE_BULK, USBC_TS_MODE_HS);
#endif

    /* disable all interrupt */
    USBC_INT_DisableUsbMiscAll(udc.bsp);
    USBC_INT_DisableEpAll(udc.bsp, USBC_EP_TYPE_RX);
    USBC_INT_DisableEpAll(udc.bsp, USBC_EP_TYPE_TX);

    /* 开启session end, reset、resume、suspend中断 */
	USBC_INT_EnableUsbMiscUint(udc.bsp, USBC_BP_INTUSB_SOF);
	USBC_INT_EnableUsbMiscUint(udc.bsp, USBC_BP_INTUSB_SUSPEND);
	USBC_INT_EnableUsbMiscUint(udc.bsp, USBC_BP_INTUSB_RESUME);
	USBC_INT_EnableUsbMiscUint(udc.bsp, USBC_BP_INTUSB_RESET);
	USBC_INT_EnableUsbMiscUint(udc.bsp, USBC_BP_INTUSB_DISCONNECT);

    /* enbale ep0_tx_irq */
	USBC_INT_EnableEp(udc.bsp, USBC_EP_TYPE_TX, 0);

    fastboot_bulk_endpoint_reset();

//    USBC_PrintAllReg(udc.usb_base, 0, 5, "fastboot_reset", printf);

    USBC_Dev_ConectSwitch(udc.bsp, USBC_DEVICE_SWITCH_ON);

    return;
}

/*
*******************************************************************************
*                     read_request
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int read_request(struct usb_device_request *req)
{
	u32 fifo_count  = 0;
	u32 fifo        = 0;
	int ret = FASTBOOT_OK;
	u8 old_ep_index = 0;

	old_ep_index = USBC_GetActiveEp(udc.bsp);
	USBC_SelectActiveEp(udc.bsp, CTRL_EP_INDEX);

 	fifo = USBC_SelectFIFO(udc.bsp, CTRL_EP_INDEX);
    fifo_count = USBC_ReadLenFromFifo(udc.bsp, USBC_EP_TYPE_EP0);
    if(fifo_count != 8 ){
    	DMSG_PANIC("err: ep0 fifo_count is not 8\n", fifo_count);
    	return FASTBOOT_ERROR;
    }

	USBC_ReadPacket(udc.bsp, fifo, fifo_count, (void *)req);
	ReadDataStatusComplete(udc.bsp, USBC_EP_TYPE_EP0, 0);

	USBC_SelectActiveEp(udc.bsp, old_ep_index);

	return ret;
}

/*
*******************************************************************************
*                     do_usb_req_set_interface
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int do_usb_req_set_interface(void)
{
	int ret = FASTBOOT_OK;

	/* Only support interface 0, alternate 0 */
	if((0 == udc.req.wIndex) && (0 == udc.req.wValue)){
		fastboot_bulk_endpoint_reset ();
	}else{
		DMSG_PANIC("err: invalid wIndex and wValue, (0, %d), (0, %d)\n", udc.req.wIndex, udc.req.wValue);
		ret = FASTBOOT_ERROR;
	}

	return ret;
}

/*
*******************************************************************************
*                     do_usb_req_set_address
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int do_usb_req_set_address(void)
{
	set_address = 1;

	udc.address         = (u8) (udc.req.wValue & 0x7f);
	udc.speed           = USBC_Dev_QueryTransferMode(udc.bsp);
	udc.bulk_ep_size    = fastboot_is_highspeed() ? HIGH_SPEED_EP_MAX_PACKET_SIZE : FULL_SPEED_EP_MAX_PACKET_SIZE;
    udc.fifo_size       = BULK_FIFOSIZE;

    DMSG_INFO("address:%d, speed:%d, bulk_ep_size:%d, fifo_size:%d\n",
               udc.address, udc.speed, udc.bulk_ep_size, udc.fifo_size);

    DMSG_INFO("usb enter %s\n", (udc.speed == USB_SPEED_HIGH) ? "high speed" : "full speed");

	return FASTBOOT_OK;
}

/*
*******************************************************************************
*                     do_usb_req_set_configuration
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int do_usb_req_set_configuration(void)
{
	int ret = FASTBOOT_OK;

	/* Only support 1 configuration so nak anything else */
	if (CONFIGURATION_NORMAL == udc.req.wValue) {
		fastboot_bulk_endpoint_reset();
	}else{
		DMSG_PANIC("err: invalid wValue, (0, %d)\n", udc.req.wValue);
		ret = FASTBOOT_ERROR;
	}

	return ret;
}

/*
*******************************************************************************
*                     do_usb_req_set_feature
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int do_usb_req_set_feature(void)
{
    DMSG_PANIC("wrn: do_usb_req_set_feature not support\n");

	return FASTBOOT_OK;
}

/*
*******************************************************************************
*                     do_usb_req_get_descriptor
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int do_usb_req_get_descriptor(void)
{
	int ret  = FASTBOOT_OK;
	u32 fifo = 0;

	if(0 == udc.req.wLength){
		DMSG_PANIC("wrn: req.wLength is zero\n");
		return FASTBOOT_OK;
	}

	if(udc.req.bmRequestType != (USB_DIR_IN | USB_TYPE_STANDARD | USB_RECIP_DEVICE)){
		DMSG_PANIC("err: invalid bmRequestType, (0, %d), (0, %d)\n", udc.req.bmRequestType);
		return FASTBOOT_ERROR;
	}

	switch(udc.req.wValue >> 8){
		case USB_DT_DEVICE:
		{
			struct usb_device_descriptor d;

			memset(&d, 0, sizeof(struct usb_device_descriptor));

			d.bLength = MIN(udc.req.wLength, sizeof (d));
			d.bDescriptorType    = USB_DT_DEVICE;
#ifdef CONFIG_USB_1_1_DEVICE
			d.bcdUSB             = 0x110;
#else
			d.bcdUSB             = 0x200;
#endif
			d.bDeviceClass       = 0xff;
			d.bDeviceSubClass    = 0xff;
			d.bDeviceProtocol    = 0xff;
			d.bMaxPacketSize0    = 0x40;
			d.idVendor           = DEVICE_VENDOR_ID;
			d.idProduct          = DEVICE_PRODUCT_ID;
			d.bcdDevice          = DEVICE_BCD;
			d.iManufacturer      = DEVICE_STRING_MANUFACTURER_INDEX;
			d.iProduct           = DEVICE_STRING_PRODUCT_INDEX;
			d.iSerialNumber      = DEVICE_STRING_SERIAL_NUMBER_INDEX;
			d.bNumConfigurations = 1;

			fifo = USBC_SelectFIFO(udc.bsp, CTRL_EP_INDEX);

            memcpy(fastboot_fifo_ep0, &d, d.bLength);
			USBC_WritePacket(udc.bsp, fifo, d.bLength, (void *)fastboot_fifo_ep0);

			WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_EP0, 1);
		}
		break;

		case USB_DT_CONFIG:
		{
			struct usb_configuration_descriptor c;
			struct usb_interface_descriptor i;
			struct usb_endpoint_descriptor ep_in, ep_out;
			unsigned char bytes_remaining = udc.req.wLength;
			unsigned char bytes_total = 0;

			memset(&c, 0, sizeof(struct usb_configuration_descriptor));
			memset(&i, 0, sizeof(struct usb_interface_descriptor));
			memset(&ep_in, 0, sizeof(struct usb_endpoint_descriptor));
			memset(&ep_out, 0, sizeof(struct usb_endpoint_descriptor));
			memset(fastboot_fifo_ep0, 0, sizeof(EP0_FIFOSIZE));

			/* configuration */
			c.bLength            	= MIN(bytes_remaining, sizeof (c));
			c.bDescriptorType    	= USB_DT_CONFIG;
			c.wTotalLength 			= sizeof (c) + sizeof (i) + sizeof (ep_in) + sizeof (ep_out);
			c.bNumInterfaces     	= 1;
			c.bConfigurationValue	= CONFIGURATION_NORMAL;
			c.iConfiguration     	= DEVICE_STRING_CONFIG_INDEX;
			c.bmAttributes       	= 0xc0;
			c.bMaxPower          	= 0x32;

			bytes_remaining -= c.bLength;
			memcpy (&fastboot_fifo_ep0[0], &c, c.bLength);
			bytes_total += c.bLength;

			/* interface */
			i.bLength             = MIN (bytes_remaining, sizeof(i));
			i.bDescriptorType     = USB_DT_INTERFACE;
			i.bInterfaceNumber    = 0x00;
			i.bAlternateSetting   = 0x00;
			i.bNumEndpoints       = 0x02;
			i.bInterfaceClass     = FASTBOOT_INTERFACE_CLASS;
			i.bInterfaceSubClass  = FASTBOOT_INTERFACE_SUB_CLASS;
			i.bInterfaceProtocol  = FASTBOOT_INTERFACE_PROTOCOL;
			i.iInterface          = DEVICE_STRING_INTERFACE_INDEX;

			bytes_remaining -= i.bLength;
			memcpy (&fastboot_fifo_ep0[bytes_total], &i, i.bLength);
			bytes_total += i.bLength;

			/* ep_in */
			ep_in.bLength            = MIN (bytes_remaining, sizeof (ep_in));
			ep_in.bDescriptorType    = USB_DT_ENDPOINT;
			ep_in.bEndpointAddress   = 0x80 | BULK_IN_EP_INDEX; /* IN */
			ep_in.bmAttributes       = USB_ENDPOINT_XFER_BULK;
			if(fastboot_is_highspeed()){
				ep_in.wMaxPacketSize = HIGH_SPEED_EP_MAX_PACKET_SIZE;
			}else{
				ep_in.wMaxPacketSize = FULL_SPEED_EP_MAX_PACKET_SIZE;
			}
			ep_in.bInterval          = 0x00;

			bytes_remaining -= ep_in.bLength;
			memcpy (&fastboot_fifo_ep0[bytes_total], &ep_in, ep_in.bLength);
			bytes_total += ep_in.bLength;

			/* ep_out */
			ep_out.bLength            = MIN (bytes_remaining, sizeof (ep_out));
			ep_out.bDescriptorType    = USB_DT_ENDPOINT;
			ep_out.bEndpointAddress   = BULK_OUT_EP_INDEX; /* OUT */
			ep_out.bmAttributes       = USB_ENDPOINT_XFER_BULK;
			if(fastboot_is_highspeed()){
				ep_out.wMaxPacketSize = HIGH_SPEED_EP_MAX_PACKET_SIZE;
			}else{
				ep_out.wMaxPacketSize = FULL_SPEED_EP_MAX_PACKET_SIZE;
			}
			ep_out.bInterval          = 0x00;

			bytes_remaining -= ep_out.bLength;
			memcpy (&fastboot_fifo_ep0[bytes_total], &ep_out, ep_out.bLength);
			bytes_total += ep_out.bLength;

			fifo = USBC_SelectFIFO(udc.bsp, CTRL_EP_INDEX);
			USBC_WritePacket(udc.bsp, fifo, bytes_total, (void *)fastboot_fifo_ep0);
			WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_EP0, 1);
		}
		break;

		case USB_DT_STRING:
		{
			unsigned char bLength = 0;
			unsigned char string_index = udc.req.wValue & 0xff;

			if(string_index > DEVICE_STRING_MAX_INDEX){
				/* Windows XP asks for an invalid string index.
				   Fail silently instead of doing

				   NAK_REQ();
				*/
			}else if(0 == string_index){
				/* Language ID */
				bLength = MIN(4, udc.req.wLength);

				memset(fastboot_fifo_ep0, 0, sizeof(EP0_FIFOSIZE));
				fastboot_fifo_ep0[0] = bLength;        /* length */
				fastboot_fifo_ep0[1] = USB_DT_STRING;  /* descriptor = string */
				fastboot_fifo_ep0[2] = DEVICE_STRING_LANGUAGE_ID & 0xff;
				fastboot_fifo_ep0[3] = DEVICE_STRING_LANGUAGE_ID >> 8;

				fifo = USBC_SelectFIFO(udc.bsp, CTRL_EP_INDEX);
				USBC_WritePacket(udc.bsp, fifo, bLength, (void *)fastboot_fifo_ep0);
				WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_EP0, 1);
			}else{
				/* Size of string in chars */
				unsigned char s = 0;
				unsigned char sl = strlen (&device_strings[string_index][0]);
				unsigned char bLength = 2 + (2 * sl);

				bLength = MIN(bLength, udc.req.wLength);

				memset(fastboot_fifo_ep0, 0, sizeof(EP0_FIFOSIZE));

				fastboot_fifo_ep0[0] = bLength;        /* length */
				fastboot_fifo_ep0[1] = USB_DT_STRING;  /* descriptor = string */

				/* Copy device string to fifo, expand to simple unicode */
				for(s = 0; s < sl; s++){
					fastboot_fifo_ep0[2+ 2*s + 0] = device_strings[string_index][s];
					fastboot_fifo_ep0[2+ 2*s + 1] = 0;
				}

				fifo = USBC_SelectFIFO(udc.bsp, CTRL_EP_INDEX);
				USBC_WritePacket(udc.bsp, fifo, bLength, (void *)fastboot_fifo_ep0);
				WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_EP0, 1);
			}
		}
		break;

		case USB_DT_DEVICE_QUALIFIER:
		{
#ifdef CONFIG_USB_1_1_DEVICE
			/* This is an invalid request for usb 1.1, nak it */
			USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_EP0);
#else
			struct usb_qualifier_descriptor d;

			memset(&d, 0, sizeof(struct usb_qualifier_descriptor));

			d.bLength = MIN(udc.req.wLength, sizeof(d));
			d.bDescriptorType    = USB_DT_DEVICE_QUALIFIER;
			d.bcdUSB             = 0x200;
			d.bDeviceClass       = 0xff;
			d.bDeviceSubClass    = 0xff;
			d.bDeviceProtocol    = 0xff;
			d.bMaxPacketSize0    = 0x40;
			d.bNumConfigurations = 1;
			d.bRESERVED          = 0;

			fifo = USBC_SelectFIFO(udc.bsp, CTRL_EP_INDEX);
			memcpy(fastboot_fifo_ep0, &d, d.bLength);
			USBC_WritePacket(udc.bsp, fifo, d.bLength, (void *)fastboot_fifo_ep0);
			WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_EP0, 1);
#endif
		}
		break;

		default:
			DMSG_PANIC("err: unkown wValue(%d)\n", udc.req.wValue);
			USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_EP0);
			ret = FASTBOOT_ERROR;
	}

	return ret;
}

/*
*******************************************************************************
*                     do_usb_req_get_status
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int do_usb_req_get_status(void)
{
	int ret = FASTBOOT_OK;
	unsigned char bLength = 0;
	u32 fifo = 0;

	if(0 == udc.req.wLength){
		/* sent zero packet */
		WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_EP0, 1);
		return FASTBOOT_ERROR;
	}

	bLength = MIN(udc.req.wValue, 2);

	memset(fastboot_fifo_ep0, 0, EP0_FIFOSIZE);
	fastboot_fifo_ep0[0] = USB_STATUS_SELFPOWERED;
	fastboot_fifo_ep0[1] = 0;

	fifo = USBC_SelectFIFO(udc.bsp, CTRL_EP_INDEX);
	USBC_WritePacket(udc.bsp, fifo, bLength, (void *)fastboot_fifo_ep0);
	WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_EP0, 1);

	return ret;
}

/*
*******************************************************************************
*                     fastboot_poll_h
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int fastboot_poll_h(void)
{
	int ret 	= FASTBOOT_INACTIVE;
	u32 status  = 0;
	u8 old_ep_index = 0;

	old_ep_index = USBC_GetActiveEp(udc.bsp);
	USBC_SelectActiveEp(udc.bsp, CTRL_EP_INDEX);

	if(USBC_Dev_IsReadDataReady(udc.bsp, USBC_EP_TYPE_EP0)){
		status = read_request(&udc.req);
		if(status!= 0){
			DMSG_PANIC("err: read_request failed\n");
			ret = FASTBOOT_ERROR;
			goto end;
		}

		/* Check data */
		if(USB_REQ_TYPE_STANDARD == (udc.req.bmRequestType & USB_REQ_TYPE_MASK)){
			/* standard */
			if (0 == (udc.req.bmRequestType & USB_REQ_DIRECTION_MASK)){
				/* host-to-device */
				if(USB_RECIP_DEVICE == (udc.req.bmRequestType & USB_REQ_RECIPIENT_MASK)){
					/* device */
					switch(udc.req.bRequest){
						case USB_REQ_SET_ADDRESS:
							ret = do_usb_req_set_address();
						break;

						case USB_REQ_SET_FEATURE:
							ret = do_usb_req_set_feature();
						break;

						case USB_REQ_SET_CONFIGURATION:
							ret = do_usb_req_set_configuration();
						break;

						default:
							USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_EP0);
							ret = FASTBOOT_ERROR;
							break;
					}
				}else if (USB_RECIP_INTERFACE == (udc.req.bmRequestType & USB_REQ_RECIPIENT_MASK)){
					switch (udc.req.bRequest)
					{
						case USB_REQ_SET_INTERFACE:
							ret = do_usb_req_set_interface();
						break;

						default:
							USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_EP0);
							ret = FASTBOOT_ERROR;
							break;
					}
				}else{
					DMSG_PANIC("err: unkown bmRequestType(%d)\n", udc.req.bmRequestType);
					USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_EP0);
					ret = FASTBOOT_ERROR;
				}
			}else{
				/* device-to-host */
				if(USB_RECIP_DEVICE == (udc.req.bmRequestType & USB_REQ_RECIPIENT_MASK)){
					switch(udc.req.bRequest){
						case USB_REQ_GET_DESCRIPTOR:
							ret = do_usb_req_get_descriptor();
						break;

						case USB_REQ_GET_STATUS:
							ret = do_usb_req_get_status();
						break;

						default:
							USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_EP0);
							ret = FASTBOOT_ERROR;
							break;
					}
				}else{
					DMSG_PANIC("err: unkown bmRequestType(%d)\n", udc.req.bmRequestType);
					USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_EP0);
					ret = FASTBOOT_ERROR;
				}
			}
		}else{/* if(USB_REQ_TYPE_STANDARD == (req.bmRequestType & USB_REQ_TYPE_MASK)) */
			/* Non-Standard Req */
			DMSG_PANIC("err: unkown bmRequestType(%d)\n", udc.req.bmRequestType);
			USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_EP0);
			ret = FASTBOOT_ERROR;
		}

		if(FASTBOOT_OK > ret){
			DMSG_PANIC("err: Unhandled req\n");
			PRINT_REQ(udc.req);
		}
	}

end:
	USBC_SelectActiveEp(udc.bsp, old_ep_index);

	return ret;
}

/*
*******************************************************************************
*                     fastboot_resume
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int fastboot_resume(void)
{
	u8 old_ep_index = 0;

	old_ep_index = USBC_GetActiveEp(udc.bsp);
	USBC_SelectActiveEp(udc.bsp, CTRL_EP_INDEX);

	/* Here because of stall was sent */
	if(USBC_Dev_IsEpStall(udc.bsp, USBC_EP_TYPE_EP0)){
		DMSG_PANIC("ERR: ep0 stall\n");
		USBC_Dev_EpClearStall(udc.bsp, USBC_EP_TYPE_EP0);

		return FASTBOOT_OK;
	}

	/* Host stopped last transaction */
	if(USBC_Dev_Ctrl_IsSetupEnd(udc.bsp)){
		USBC_Dev_Ctrl_ClearSetupEnd(udc.bsp);
	}

	USBC_SelectActiveEp(udc.bsp, old_ep_index);

	/* Should we change the address ? */
	if(set_address){
		USBC_Dev_SetAddress(udc.bsp, udc.address);
		set_address = 0;

		DMSG_INFO("SetAddress: %d\n", udc.address);
	}

	return fastboot_poll_h();
}

/*
*******************************************************************************
*                     fastboot_rx_error
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static void fastboot_rx_error(void)
{
	u8 old_ep_index = 0;

	old_ep_index = USBC_GetActiveEp(udc.bsp);
	USBC_SelectActiveEp(udc.bsp, BULK_OUT_EP_INDEX);

	/* Clear the RXPKTRDY bit */
	ReadDataStatusComplete(udc.bsp, USBC_EP_TYPE_RX, 1);

	/* Send stall */
	USBC_Dev_EpSendStall(udc.bsp, USBC_EP_TYPE_RX);

	/* Wait till stall is sent.. */
	while(!USBC_Dev_IsEpStall(udc.bsp, USBC_EP_TYPE_RX)){
		udelay(1);
	}

	/* Clear stall */
	USBC_Dev_EpClearStall(udc.bsp, USBC_EP_TYPE_RX);

	USBC_SelectActiveEp(udc.bsp, old_ep_index);

	return;
}

/*
*******************************************************************************
*                     fastboot_rx
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int fastboot_rx(void)
{
	int ret = FASTBOOT_INACTIVE;
	u8 old_ep_index = 0;
	int err = 1;

	old_ep_index = USBC_GetActiveEp(udc.bsp);
	USBC_SelectActiveEp(udc.bsp, BULK_OUT_EP_INDEX);

	if(USBC_Dev_IsReadDataReady(udc.bsp, USBC_EP_TYPE_RX)){
		u16 count = 0;
		u32 fifo  = 0;
		int fifo_size = fastboot_fifo_size();

		ret = FASTBOOT_OK;

		count = USBC_ReadLenFromFifo(udc.bsp, USBC_EP_TYPE_RX);
		fifo = USBC_SelectFIFO(udc.bsp, BULK_OUT_EP_INDEX);
		USBC_ReadPacket(udc.bsp, fifo, count, (void *)fastboot_fifo_bulk_ep);

        if(count > fifo_size){
			fastboot_rx_error();
		}else{
			ReadDataStatusComplete(udc.bsp, USBC_EP_TYPE_RX, 1);
		}

		/* Pass this up to the interface's handler */
		if (fastboot_interface &&
		    fastboot_interface->rx_handler) {
			if(!fastboot_interface->rx_handler(fastboot_fifo_bulk_ep, count)){
				err = 0;
			}
		}

		if(err){
		    DMSG_PANIC("err: rx_handler failed\n");
		    ret = FASTBOOT_ERROR;
	    }
	}

	USBC_SelectActiveEp(udc.bsp, old_ep_index);

	return ret;
}

/*
*******************************************************************************
*                     fastboot_suspend
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static int fastboot_suspend(void)
{
    return FASTBOOT_OK;
}

/*
*******************************************************************************
*                     filtrate_irq_misc
*
* Description:
*    过滤没用的中断, 保留 disconect, reset, resume, suspend
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static u32 filtrate_irq_misc(u32 irq_misc)
{
    u32 irq = irq_misc;

    irq &= ~(USBC_INTUSB_VBUS_ERROR | USBC_INTUSB_SESSION_REQ | USBC_INTUSB_CONNECT);
	USBC_INT_ClearMiscPending(udc.bsp, USBC_INTUSB_VBUS_ERROR);
	USBC_INT_ClearMiscPending(udc.bsp, USBC_INTUSB_SESSION_REQ);
	USBC_INT_ClearMiscPending(udc.bsp, USBC_INTUSB_CONNECT);

	return irq;
}

/*
*******************************************************************************
*                     clear_all_irq
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static void clear_all_irq(void)
{
    USBC_INT_ClearEpPendingAll(udc.bsp, USBC_EP_TYPE_TX);
    USBC_INT_ClearEpPendingAll(udc.bsp, USBC_EP_TYPE_RX);
    USBC_INT_ClearMiscPendingAll(udc.bsp);
}

/*
*******************************************************************************
*                     fastboot_poll
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int fastboot_poll(void)
{
	u8 usb_irq = 0;
	u16 tx_irq = 0;
	u16 rx_irq = 0;

	int ret = FASTBOOT_INACTIVE;
    int i = 0;
    u32 old_ep_idx  = 0;

    /* Save index */
	old_ep_idx = USBC_GetActiveEp(udc.bsp);

    /* Read status registers */
	usb_irq = USBC_INT_MiscPending(udc.bsp);
	tx_irq  = USBC_INT_EpPending(udc.bsp, USBC_EP_TYPE_TX);
	rx_irq  = USBC_INT_EpPending(udc.bsp, USBC_EP_TYPE_RX);

	usb_irq = filtrate_irq_misc(usb_irq);

    if(deferred_rx){
		ret = FASTBOOT_OK;
    }

    /* RESET */
	if(usb_irq & USBC_INTUSB_RESET){
	    DMSG_INFO("IRQ: reset\n");

	    USBC_INT_ClearMiscPending(udc.bsp, USBC_INTUSB_RESET);
        clear_all_irq();

        USBC_SelectActiveEp(udc.bsp, 0);
		USBC_Dev_SetAddress_default(udc.bsp);

		udc.address = 0;
		udc.speed = USB_SPEED_UNKNOWN;

		return FASTBOOT_OK;
    }

	/* RESUME */
	if (usb_irq & USBC_INTUSB_RESUME) {
		DMSG_INFO("IRQ: resume\n");

		/* clear interrupt */
		USBC_INT_ClearMiscPending(udc.bsp, USBC_INTUSB_RESUME);

		ret = fastboot_resume();
		if(FASTBOOT_OK > ret){
		    DMSG_PANIC("err: fastboot_resume failed\n");
			return ret;
		}

		return ret;
	}

	/* SUSPEND */
	if (usb_irq & USBC_INTUSB_SUSPEND) {
		DMSG_INFO("IRQ: suspend\n");

		/* clear interrupt */
		USBC_INT_ClearMiscPending(udc.bsp, USBC_INTUSB_SUSPEND);

		ret = fastboot_suspend ();
		if(FASTBOOT_OK > ret){
		    DMSG_PANIC("err: fastboot_suspend failed\n");
			return ret;
        }

		return ret;
	}

    /* DISCONNECT */
    if(usb_irq & USBC_INTUSB_DISCONNECT){
        DMSG_INFO("IRQ: disconnect\n");

		USBC_INT_ClearMiscPending(udc.bsp, USBC_INTUSB_DISCONNECT);

        return FASTBOOT_DISCONNECT;
	}

	/* SOF */
	if(usb_irq & USBC_INTUSB_SOF){
	    //DMSG_INFO("IRQ: SOF\n");

		USBC_INT_ClearMiscPending(udc.bsp, USBC_INTUSB_SOF);

		ret = fastboot_resume();
		if(FASTBOOT_OK > ret){
		    DMSG_PANIC("err: fastboot_resume failed\n");
			return ret;
		}

		/* The fastboot client blocks of read and
		   intrrx is not reliable.
		   Really poll */
		if(deferred_rx){
			ret = fastboot_rx ();
			if(FASTBOOT_OK > ret){
    			return ret;
    		}
		}

		deferred_rx = 0;
	}

	/* ep0 */
	if (tx_irq & USBC_INTTx_FLAG_EP0) {
	    USBC_INT_ClearEpPending(udc.bsp, USBC_EP_TYPE_TX, 0);
    }

	/* tx endpoint data transfers */
	for (i = 1; i < FASTBOOT_EP_NUM; i++) {
		u32 tmp = 1 << i;

		if (tx_irq & tmp) {
			/* Clear the interrupt bit by setting it to 1 */
			USBC_INT_ClearEpPending(udc.bsp, USBC_EP_TYPE_TX, i);

			/* TX interrupts happen when a packet has been sent
			   We already poll the csr register for this when
			   something is sent, so do not do it twice

			*/
		}
	}

	/* rx endpoint data transfers */
	for (i = 1; i < FASTBOOT_EP_NUM; i++) {
		u32 tmp = 1 << i;

		if (rx_irq & tmp) {
			/* Clear the interrupt bit by setting it to 1 */
			USBC_INT_ClearEpPending(udc.bsp, USBC_EP_TYPE_RX, i);

			/* Defer this to SOF */
			deferred_rx = 1;
		}
	}

    USBC_SelectActiveEp(udc.bsp, old_ep_idx);

    return ret;
}

/*
*******************************************************************************
*                     fastboot_shutdown
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
void fastboot_shutdown(void)
{
    DMSG_INFO("fastboot_shutdown\n");

    /* Let the cmd layer know that we are shutting down */
	if (fastboot_interface &&
	    fastboot_interface->reset_handler) {
		fastboot_interface->reset_handler();
	}

    USBC_Dev_ConectSwitch(udc.bsp, 0);

    udc.address         = 0;
    udc.speed           = USB_SPEED_UNKNOWN;
    udc.bulk_ep_size    = 0;
    udc.fifo_size       = 0;
    deferred_rx         = 0;
    fastboot_interface  = NULL;

    return;
}

/*
*******************************************************************************
*                     fastboot_is_highspeed
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int fastboot_is_highspeed(void)
{
	return ((udc.speed == USB_SPEED_HIGH) ? 1 : 0);
}

/*
*******************************************************************************
*                     fastboot_is_highspeed
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int fastboot_fifo_size(void)
{
	return ((udc.speed == USB_SPEED_HIGH) ? HIGH_SPEED_EP_MAX_PACKET_SIZE : FULL_SPEED_EP_MAX_PACKET_SIZE);
}

/*
*******************************************************************************
*                     fastboot_tx_status
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int fastboot_tx_status(const char *buffer, unsigned int buffer_size)
{
	u32 transfer_size = 0;
    u32 old_ep_idx = 0;
    u32 fifo = 0;

    u32 left = 0;
    u32 offset = 0;
    u32 this_len = 0;
    u32 zero = 0;

    /* Save index */
	old_ep_idx = USBC_GetActiveEp(udc.bsp);
	USBC_SelectActiveEp(udc.bsp, BULK_IN_EP_INDEX);

    /* wait for tx packet sent out */
    while(USBC_Dev_IsWriteDataReady(udc.bsp, USBC_EP_TYPE_TX)){
        udelay(1);
    }

    left = buffer_size;
    offset = 0;

    while(left){
    	/* fastboot client only reads back at most 64 */
    	transfer_size = MIN(64, buffer_size);

        fifo = USBC_SelectFIFO(udc.bsp, BULK_IN_EP_INDEX);

        memcpy(fastboot_fifo_bulk_ep, (buffer + offset), transfer_size);
        this_len = USBC_WritePacket(udc.bsp, fifo, transfer_size, (void *)fastboot_fifo_bulk_ep);

        left -= this_len;
        offset += this_len;

        WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_TX, 1);

        if((transfer_size == udc.bulk_ep_size) && left){
            zero= 1;
        }
    }

    if(zero){
        DMSG_INFO("sent zero packet\n");

        WriteDataStatusComplete(udc.bsp, USBC_EP_TYPE_TX, 1);
    }

    USBC_SelectActiveEp(udc.bsp, old_ep_idx);

    return 0;
}

/*
*******************************************************************************
*                     fastboot_tx
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int fastboot_tx(unsigned char *buffer, unsigned int buffer_size)
{
    DMSG_DEBUG("fastboot_tx, ep(%d)\n", USBC_GetActiveEp(udc.bsp));

    return fastboot_tx_status((const char *)buffer, buffer_size);
}

/*
*******************************************************************************
*                     fastboot_getvar
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int fastboot_getvar(const char *rx_buffer, char *tx_buffer)
{
    DMSG_PANIC("wrn: fastboot_getvar not support\n");

	return 0;
}

/*
*******************************************************************************
*                     fastboot_preboot
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int fastboot_preboot(void)
{
    DMSG_PANIC("wrn: fastboot_preboot not support\n");

    return 0;
}

/*
*******************************************************************************
*                     open_usb_clock
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
static u32 open_usb_clock(u32 ccmu_base)
{
    u32 reg_value = 0;

    DMSG_INFO("open_usb_clock\n");

    //Gating AHB clock for USB_phy0
	reg_value = readl(ccmu_base + 0x60);
	x_set_bit(reg_value, 0);	/* AHB clock gate usb0 */
	writel(reg_value, (ccmu_base + 0x60));

    //delay to wati SIE stable
	reg_value = 10000;
	while(reg_value--);

	//Enable module clock for USB phy0
	reg_value = readl(ccmu_base + 0xcc);
	x_set_bit(reg_value, 8);
	x_set_bit(reg_value, 0);//disable reset
	writel(reg_value, (ccmu_base + 0xcc));

	//delay some time
	reg_value = 10000;
	while(reg_value--);

	return 0;
}

/*
*******************************************************************************
*                     close_usb_clock
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
u32 close_usb_clock(u32 ccmu_base)
{
    u32 reg_value = 0;

    DMSG_INFO("close_usb_clock\n");

    //开usb ahb时钟
	reg_value = readl(ccmu_base + 0x60);
	x_clear_bit(reg_value, 0);	/* AHB clock gate usb0 */
	writel(reg_value, (ccmu_base + 0x60));

    //等sie的时钟变稳
	reg_value = 10000;
	while(reg_value--);

	//开USB phy时钟
	reg_value = readl((ccmu_base + 0xcc));
	x_clear_bit(reg_value, 0);
	x_clear_bit(reg_value, 8);
	writel(reg_value, (ccmu_base + 0xcc));

	//延时
	reg_value = 10000;
	while(reg_value--);

	return 0;
}

/*
*******************************************************************************
*                     fastboot_getvar
*
* Description:
*    void
*
* Parameters:
*    void
*
* Return value:
*    void
*
* note:
*    void
*
*******************************************************************************
*/
int fastboot_init(struct cmd_fastboot_interface *interface)
{
	bsp_usbc_t usbc;
#if DEBUG
    printf("fastboot_init\n");
#endif
    device_strings[DEVICE_STRING_MANUFACTURER_INDEX]    = DEVICE_MANUFACTURER;
    device_strings[DEVICE_STRING_PRODUCT_INDEX]         = DEVICE_PRODUCT;
    device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX]   = DEVICE_SERIAL_NUMBER;
    device_strings[DEVICE_STRING_CONFIG_INDEX]          = DEVICE_CONFIG;
	device_strings[DEVICE_STRING_INTERFACE_INDEX]       = DEVICE_INTERFACE;

	/* The interface structure */
	fastboot_interface                          = interface;
	fastboot_interface->product_name            = device_strings[DEVICE_STRING_PRODUCT_INDEX];
	fastboot_interface->serial_no               = device_strings[DEVICE_STRING_SERIAL_NUMBER_INDEX];
	fastboot_interface->nand_block_size         = 512;
	fastboot_interface->nand_oob_size           = 64;
	fastboot_interface->transfer_buffer         = (unsigned char *) FASTBOOT_TRANSFER_BUFFER;
	fastboot_interface->transfer_buffer_size    = FASTBOOT_TRANSFER_BUFFER_SIZE;

    udc.usb_base    = FASTBOOT_USB_BASE;
    udc.sram_base   = FASTBOOT_SRAM_BASE;
    udc.ccmu_base   = FASTBOOT_CCMU;

	/* open usb clock */
	close_usb_clock(udc.ccmu_base);
	open_usb_clock(udc.ccmu_base);

	/* init usb bsp */
    memset(&usbc, 0, sizeof(bsp_usbc_t));
    usbc.usbc_info[0].num = 0;
    usbc.usbc_info[0].base = udc.usb_base;
	usbc.sram_base = udc.sram_base;
	USBC_init(&usbc);

	udc.bsp = USBC_open_otg(0);
	if(udc.bsp == 0){
		printf("ERR: USBC_open_otg failed\n");
		return -1;
	}

    fastboot_reset();

    return 0;
}

#endif /* CONFIG_FASTBOOT */


