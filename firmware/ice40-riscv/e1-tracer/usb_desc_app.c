/*
 * usb_desc_app.c
 *
 * Copyright (C) 2019-2020  Sylvain Munaut <tnt@246tNt.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <no2usb/usb_proto.h>
#include <no2usb/usb.h>

#define NULL ((void*)0)
#define num_elem(a) (sizeof(a) / sizeof(a[0]))


static const struct {
	/* Configuration */
	struct usb_conf_desc conf;

	/* E1 */
	struct {
		struct {
			struct usb_intf_desc intf;
			struct usb_ep_desc ep_data_in0;
			struct usb_ep_desc ep_data_in1;
		} __attribute__ ((packed)) off;
		struct {
			struct usb_intf_desc intf;
			struct usb_ep_desc ep_data_in0;
			struct usb_ep_desc ep_data_in1;
		} __attribute__ ((packed)) on;
	} __attribute__ ((packed)) e1;

	/* CDC */
#if 0
	struct {
		struct usb_intf_desc intf_ctl;
		struct usb_cs_intf_hdr_desc cs_intf_hdr;
		struct usb_cs_intf_acm_desc cs_intf_acm;
		struct usb_cs_intf_union_desc cs_intf_union;
		uint8_t cs_intf_union_slave;
		struct usb_ep_desc ep_ctl;
		struct usb_intf_desc intf_data;
		struct usb_ep_desc ep_data_out;
		struct usb_ep_desc ep_data_in;
	} __attribute__ ((packed)) cdc;
#endif

	/* DFU Runtime */
	struct {
		struct usb_intf_desc intf;
		struct usb_dfu_desc func;
	} __attribute__ ((packed)) dfu;
} __attribute__ ((packed)) _app_conf_desc = {
	.conf = {
		.bLength                = sizeof(struct usb_conf_desc),
		.bDescriptorType        = USB_DT_CONF,
		.wTotalLength           = sizeof(_app_conf_desc),
#if 0
		.bNumInterfaces         = 4,
#else
		.bNumInterfaces         = 2,
#endif
		.bConfigurationValue    = 1,
		.iConfiguration         = 4,
		.bmAttributes           = 0x80,
		.bMaxPower              = 0x32, /* 100 mA */
	},
	.e1 = {
		.off = {
			.intf = {
				.bLength		= sizeof(struct usb_intf_desc),
				.bDescriptorType	= USB_DT_INTF,
				.bInterfaceNumber	= 0,
				.bAlternateSetting	= 0,
				.bNumEndpoints		= 2,
				.bInterfaceClass	= 0xff,
				.bInterfaceSubClass	= 0xe1,
				.bInterfaceProtocol	= 0x00,
				.iInterface		= 5,
			},
			.ep_data_in0 = {
				.bLength		= sizeof(struct usb_ep_desc),
				.bDescriptorType	= USB_DT_EP,
				.bEndpointAddress	= 0x81,
				.bmAttributes		= 0x05,
				.wMaxPacketSize		= 0,
				.bInterval		= 1,
			},
			.ep_data_in1 = {
				.bLength		= sizeof(struct usb_ep_desc),
				.bDescriptorType	= USB_DT_EP,
				.bEndpointAddress	= 0x82,
				.bmAttributes		= 0x05,
				.wMaxPacketSize		= 0,
				.bInterval		= 1,
			},
		},
		.on = {
			.intf = {
				.bLength		= sizeof(struct usb_intf_desc),
				.bDescriptorType	= USB_DT_INTF,
				.bInterfaceNumber	= 0,
				.bAlternateSetting	= 1,
				.bNumEndpoints		= 2,
				.bInterfaceClass	= 0xff,
				.bInterfaceSubClass	= 0xe1,
				.bInterfaceProtocol	= 0x00,
				.iInterface		= 5,
			},
			.ep_data_in0 = {
				.bLength		= sizeof(struct usb_ep_desc),
				.bDescriptorType	= USB_DT_EP,
				.bEndpointAddress	= 0x81,
				.bmAttributes		= 0x05,
				.wMaxPacketSize		= 388,
				.bInterval		= 1,
			},
			.ep_data_in1 = {
				.bLength		= sizeof(struct usb_ep_desc),
				.bDescriptorType	= USB_DT_EP,
				.bEndpointAddress	= 0x82,
				.bmAttributes		= 0x05,
				.wMaxPacketSize		= 388,
				.bInterval		= 1,
			},
		},
	},
#if 0
	.cdc = {
		.intf_ctl = {
			.bLength		= sizeof(struct usb_intf_desc),
			.bDescriptorType	= USB_DT_INTF,
			.bInterfaceNumber	= 1,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 1,
			.bInterfaceClass	= 0x02,
			.bInterfaceSubClass	= 0x02,
			.bInterfaceProtocol	= 0x00,
			.iInterface		= 6,
		},
		.cs_intf_hdr = {
			.bLength		= sizeof(struct usb_cs_intf_hdr_desc),
			.bDescriptorType	= USB_DT_CS_INTF,
			.bDescriptorsubtype	= 0x00,
			.bcdCDC			= 0x0110,
		},
		.cs_intf_acm = {
			.bLength		= sizeof(struct usb_cs_intf_acm_desc),
			.bDescriptorType	= USB_DT_CS_INTF,
			.bDescriptorsubtype	= 0x02,
			.bmCapabilities		= 0x00,
		},
		.cs_intf_union = {
			.bLength		= sizeof(struct usb_cs_intf_union_desc) + 1,
			.bDescriptorType	= USB_DT_CS_INTF,
			.bDescriptorsubtype	= 0x06,
			.bMasterInterface	= 1,
		},
		.cs_intf_union_slave = 2,
		.ep_ctl = {
			.bLength		= sizeof(struct usb_ep_desc),
			.bDescriptorType	= USB_DT_EP,
			.bEndpointAddress	= 0x84,
			.bmAttributes		= 0x03,
			.wMaxPacketSize		= 64,
			.bInterval		= 0xff,
		},
		.intf_data = {
			.bLength		= sizeof(struct usb_intf_desc),
			.bDescriptorType	= USB_DT_INTF,
			.bInterfaceNumber	= 2,
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 2,
			.bInterfaceClass	= 0x0a,
			.bInterfaceSubClass	= 0x00,
			.bInterfaceProtocol	= 0x00,
			.iInterface		= 7,
		},
		.ep_data_out = {
			.bLength		= sizeof(struct usb_ep_desc),
			.bDescriptorType	= USB_DT_EP,
			.bEndpointAddress	= 0x05,
			.bmAttributes		= 0x02,
			.wMaxPacketSize		= 64,
			.bInterval		= 0x00,
		},
		.ep_data_in = {
			.bLength		= sizeof(struct usb_ep_desc),
			.bDescriptorType	= USB_DT_EP,
			.bEndpointAddress	= 0x85,
			.bmAttributes		= 0x02,
			.wMaxPacketSize		= 64,
			.bInterval		= 0x01,
		},
	},
#endif
	.dfu = {
		.intf = {
			.bLength		= sizeof(struct usb_intf_desc),
			.bDescriptorType	= USB_DT_INTF,
#if 0
			.bInterfaceNumber	= 3,
#else
			.bInterfaceNumber	= 1,
#endif
			.bAlternateSetting	= 0,
			.bNumEndpoints		= 0,
			.bInterfaceClass	= 0xfe,
			.bInterfaceSubClass	= 0x01,
			.bInterfaceProtocol	= 0x01,
			.iInterface		= 8,
		},
		.func = {
			.bLength		= sizeof(struct usb_dfu_desc),
			.bDescriptorType	= USB_DT_DFU,
			.bmAttributes		= 0x0d,
			.wDetachTimeOut		= 1000,
			.wTransferSize		= 4096,
			.bcdDFUVersion		= 0x0101,
		},
	},
};

static const struct usb_conf_desc * const _conf_desc_array[] = {
	&_app_conf_desc.conf,
};

static const struct usb_dev_desc _dev_desc = {
	.bLength		= sizeof(struct usb_dev_desc),
	.bDescriptorType	= USB_DT_DEV,
	.bcdUSB			= 0x0200,
	.bDeviceClass		= 0,
	.bDeviceSubClass	= 0,
	.bDeviceProtocol	= 0,
	.bMaxPacketSize0	= 64,
	.idVendor		= 0x1d50,
	.idProduct		= 0x6151,
	.bcdDevice		= 2,
	.iManufacturer		= 2,
	.iProduct		= 3,
	.iSerialNumber		= 1,
	.bNumConfigurations	= num_elem(_conf_desc_array),
};

#include "usb_str_app.gen.h"

const struct usb_stack_descriptors app_stack_desc = {
	.dev = &_dev_desc,
	.conf = _conf_desc_array,
	.n_conf = num_elem(_conf_desc_array),
	.str = _str_desc_array,
	.n_str = num_elem(_str_desc_array),
};
