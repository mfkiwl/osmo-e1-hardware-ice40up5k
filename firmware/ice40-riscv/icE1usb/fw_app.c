/*
 * fw_app.c
 *
 * Copyright (C) 2019-2020  Sylvain Munaut <tnt@246tNt.com>
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <no2usb/usb.h>
#include <no2usb/usb_dfu_rt.h>

#include "config.h"

#include "console.h"
#include "e1.h"
#include "led.h"
#include "misc.h"
#include "mini-printf.h"
#include "spi.h"
#include "utils.h"


extern const struct usb_stack_descriptors app_stack_desc;

static void
serial_no_init()
{
	uint8_t buf[8];
	char *id, *desc;
	int i;

	flash_manuf_id(buf);
	printf("Flash Manufacturer : %s\n", hexstr(buf, 3, true));

	flash_unique_id(buf);
	printf("Flash Unique ID    : %s\n", hexstr(buf, 8, true));

	/* Overwrite descriptor string */
		/* In theory in rodata ... but nothing is ro here */
	id = hexstr(buf, 8, false);
	desc = (char*)app_stack_desc.str[1];
	for (i=0; i<16; i++)
		desc[2 + (i << 1)] = id[i];
}

static void
boot_dfu(void)
{
	/* Force re-enumeration */
	usb_disconnect();

	/* Boot firmware */
	volatile uint32_t *boot = (void*)0x80000000;
	*boot = (1 << 2) | (1 << 0);
}

void
usb_dfu_rt_cb_reboot(void)
{
        boot_dfu();
}


static volatile uint32_t * const misc_regs = (void*)(MISC_BASE);

void main()
{
	bool e1_active = false;
	int cmd = 0;

	/* Init console IO */
	console_init();
	puts("Booting App image..\n");

	/* LED */
	led_init();

	/* SPI */
	spi_init();

	/* Setup E1 Vref */
	int d = 25;
	pdm_set(PDM_E1_CT, true, 128, false);
	pdm_set(PDM_E1_P,  true, 128 - d, false);
	pdm_set(PDM_E1_N,  true, 128 + d, false);

	/* Setup clock tuning */
	pdm_set(PDM_CLK_HI, true, 2048, false);
	pdm_set(PDM_CLK_LO, false,   0, false);

	/* Enable USB directly */
	serial_no_init();
	usb_init(&app_stack_desc);
	usb_dfu_rt_init();
	usb_e1_init();

	/* Start */
	e1_init(false);		// local tick
	e1_active = true;
	led_state(true);
	usb_connect();

	/* Main loop */
	while (1)
	{
		/* Prompt ? */
		if (cmd >= 0)
			printf("Command> ");

		/* Poll for command */
		cmd = getchar_nowait();

		if (cmd >= 0) {
			if (cmd > 32 && cmd < 127) {
				putchar(cmd);
				putchar('\r');
				putchar('\n');
			}

			switch (cmd)
			{
			case 'p':
				usb_debug_print();
				break;
			case 'b':
				boot_dfu();
				break;
			case 'o':
				e1_debug_print(false);
				break;
			case 'O':
				e1_debug_print(true);
				break;
			case 't':
				printf("%08x\n", misc_regs[0]);
			case 'e':
				e1_init(true);
				e1_active = true;
				led_state(true);
				break;
			case 'E':
				e1_init(false);
				e1_active = true;
				led_state(true);
				break;
			case 'c':
				usb_connect();
				break;
			case 'd':
				usb_disconnect();
				break;
			default:
				break;
			}
		}

		/* USB poll */
		usb_poll();

		/* E1 poll */
		if (e1_active) {
			e1_poll();
			usb_e1_run();
		}
	}
}