/*
*************************************************************************************
*                         			      Linux
*					           USB Host Controller Driver
*
*				        (c) Copyright 2006-2010, All winners Co,Ld.
*							       All Rights Reserved
*
* File Name 	: sw_hcd.h
*
* Author 		: javen
*
* Description 	: �弶����
*
* Notes         :
*
* History 		:
*      <author>    		<time>       	<version >    		<desc>
*       javen     	  2010-12-20           1.0          create this file
*
*************************************************************************************
*/
#ifndef  __SW_HCD_H__
#define  __SW_HCD_H__

/* The USB role is defined by the connector used on the board, so long as
 * standards are being followed.  (Developer boards sometimes won't.)
 */
enum sw_hcd_mode {
	SW_HCD_UNDEFINED = 0,
	SW_HCD_HOST,		/* A or Mini-A connector */
	SW_HCD_PERIPHERAL,	/* B or Mini-B connector */
	SW_HCD_OTG		    /* Mini-AB connector */
};

struct clk;

typedef struct sw_hcd_eps_bits {
	const char	name[16];
	u8		bits;
}sw_hcd_eps_bits_t;

typedef struct sw_hcd_config {
	/* sw_hcd configuration-specific details */
	unsigned	multipoint:1;	/* multipoint device */
	unsigned	dyn_fifo:1;	    /* supports dynamic fifo sizing */
	unsigned	soft_con:1;	    /* soft connect required */
	unsigned	utm_16:1;	    /* utm data witdh is 16 bits */
	unsigned	big_endian:1;	/* true if CPU uses big-endian */
	unsigned	mult_bulk_tx:1;	/* Tx ep required for multbulk pkts */
	unsigned	mult_bulk_rx:1;	/* Rx ep required for multbulk pkts */
	unsigned	high_iso_tx:1;	/* Tx ep required for HB iso */
	unsigned	high_iso_rx:1;	/* Rx ep required for HD iso */
	unsigned	dma:1;		    /* supports DMA */
	unsigned	vendor_req:1;	/* vendor registers required */

	u8		num_eps;	    /* number of endpoints _with_ ep0 */
	u8		dma_channels;	/* number of dma channels */
	u8		dyn_fifo_size;	/* dynamic size in bytes */
	u8		vendor_ctrl;	/* vendor control reg width */
	u8		vendor_stat;	/* vendor status reg witdh */
	u8		dma_req_chan;	/* bitmask for required dma channels */
	u32		ram_size;	    /* ram address size */

	struct sw_hcd_eps_bits *eps_bits;
	struct usb_port_info *port_info;
}sw_hcd_config_t;

typedef struct sw_hcd_platform_data {
	u32 usbc_base;

	/* sw_USB_HOST, SW_USB_PERIPHERAL, or SW_USB_OTG */
	u8		mode;

	/* (HOST or OTG) switch VBUS on/off */
	int		(*set_vbus)(struct device *dev, int is_on);

	/* (HOST or OTG) mA/2 power supplied on (default = 8mA) */
	u8		power;

	/* (PERIPHERAL) mA/2 max power consumed (default = 100mA) */
	u8		min_power;

	/* (HOST or OTG) msec/2 after VBUS on till power good */
	u8		potpgt;

	/* Power the device on or off */
	int		(*set_power)(int state);

	/* Turn device clock on or off */
	int		(*set_clock)(struct clk *clock, int is_on);

	/* sw_hcd configuration-specific details */
	struct sw_hcd_config	*config;
}sw_hcd_platform_data_t;



int sw_usb_host0_enable(void);
int sw_usb_host0_disable(void);

int sw_usb_disable_hcd0(void);
int sw_usb_enable_hcd0(void);

int sw_usb_disable_hcd1(void);
int sw_usb_enable_hcd1(void);

int sw_usb_disable_hcd2(void);
int sw_usb_enable_hcd2(void);


#endif   //__SW_HCD_H__

