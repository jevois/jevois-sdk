/*
 * Driver for Digigram VX222 V2/Mic soundcards
 *
 * VX222-specific low-level routines
 *
 * Copyright (c) 2002 by Takashi Iwai <tiwai@suse.de>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/mutex.h>

#include <sound/core.h>
#include <sound/control.h>
#include <sound/tlv.h>
#include <asm/io.h>
#include "vx222.h"


static int vx2_reg_offset[VX_REG_MAX] = {
	[VX_ICR]    = 0x00,
	[VX_CVR]    = 0x04,
	[VX_ISR]    = 0x08,
	[VX_IVR]    = 0x0c,
	[VX_RXH]    = 0x14,
	[VX_RXM]    = 0x18,
	[VX_RXL]    = 0x1c,
	[VX_DMA]    = 0x10,
	[VX_CDSP]   = 0x20,
	[VX_CFG]    = 0x24,
	[VX_RUER]   = 0x28,
	[VX_DATA]   = 0x2c,
	[VX_STATUS] = 0x30,
	[VX_LOFREQ] = 0x34,
	[VX_HIFREQ] = 0x38,
	[VX_CSUER]  = 0x3c,
	[VX_SELMIC] = 0x40,
	[VX_COMPOT] = 0x44,
	[VX_SCOMPR] = 0x48,
	[VX_GLIMIT] = 0x4c,
	[VX_INTCSR] = 0x4c,
	[VX_CNTRL]  = 0x50,	
	[VX_GPIOC]  = 0x54,	
};

static int vx2_reg_index[VX_REG_MAX] = {
	[VX_ICR]	= 1,
	[VX_CVR]	= 1,
	[VX_ISR]	= 1,
	[VX_IVR]	= 1,
	[VX_RXH]	= 1,
	[VX_RXM]	= 1,
	[VX_RXL]	= 1,
	[VX_DMA]	= 1,
	[VX_CDSP]	= 1,
	[VX_CFG]	= 1,
	[VX_RUER]	= 1,
	[VX_DATA]	= 1,
	[VX_STATUS]	= 1,
	[VX_LOFREQ]	= 1,
	[VX_HIFREQ]	= 1,
	[VX_CSUER]	= 1,
	[VX_SELMIC]	= 1,
	[VX_COMPOT]	= 1,
	[VX_SCOMPR]	= 1,
	[VX_GLIMIT]	= 1,
	[VX_INTCSR]	= 0,	/* on the PLX */
	[VX_CNTRL]	= 0,	/* on the PLX */
	[VX_GPIOC]	= 0,	/* on the PLX */
};

static inline unsigned long vx2_reg_addr(struct vx_core *_chip, int reg)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;
	return chip->port[vx2_reg_index[reg]] + vx2_reg_offset[reg];
}

/**
 * snd_vx_inb - read a byte from the register
 * @offset: register enum
 */
static unsigned char vx2_inb(struct vx_core *chip, int offset)
{
	return inb(vx2_reg_addr(chip, offset));
}

/**
 * snd_vx_outb - write a byte on the register
 * @offset: the register offset
 * @val: the value to write
 */
static void vx2_outb(struct vx_core *chip, int offset, unsigned char val)
{
	outb(val, vx2_reg_addr(chip, offset));
	/*
	printk(KERN_DEBUG "outb: %x -> %x\n", val, vx2_reg_addr(chip, offset));
	*/
}

/**
 * snd_vx_inl - read a 32bit word from the register
 * @offset: register enum
 */
static unsigned int vx2_inl(struct vx_core *chip, int offset)
{
	return inl(vx2_reg_addr(chip, offset));
}

/**
 * snd_vx_outl - write a 32bit word on the register
 * @offset: the register enum
 * @val: the value to write
 */
static void vx2_outl(struct vx_core *chip, int offset, unsigned int val)
{
	/*
	printk(KERN_DEBUG "outl: %x -> %x\n", val, vx2_reg_addr(chip, offset));
	*/
	outl(val, vx2_reg_addr(chip, offset));
}

/*
 * redefine macros to call directly
 */
#undef vx_inb
#define vx_inb(chip,reg)	vx2_inb((struct vx_core*)(chip), VX_##reg)
#undef vx_outb
#define vx_outb(chip,reg,val)	vx2_outb((struct vx_core*)(chip), VX_##reg, val)
#undef vx_inl
#define vx_inl(chip,reg)	vx2_inl((struct vx_core*)(chip), VX_##reg)
#undef vx_outl
#define vx_outl(chip,reg,val)	vx2_outl((struct vx_core*)(chip), VX_##reg, val)


/*
 * vx_reset_dsp - reset the DSP
 */

#define XX_DSP_RESET_WAIT_TIME		2	/* ms */

static void vx2_reset_dsp(struct vx_core *_chip)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;

	/* set the reset dsp bit to 0 */
	vx_outl(chip, CDSP, chip->regCDSP & ~VX_CDSP_DSP_RESET_MASK);

	mdelay(XX_DSP_RESET_WAIT_TIME);

	chip->regCDSP |= VX_CDSP_DSP_RESET_MASK;
	/* set the reset dsp bit to 1 */
	vx_outl(chip, CDSP, chip->regCDSP);
}


static int vx2_test_xilinx(struct vx_core *_chip)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;
	unsigned int data;

	snd_printdd("testing xilinx...\n");
	/* This test uses several write/read sequences on TEST0 and TEST1 bits
	 * to figure out whever or not the xilinx was correctly loaded
	 */

	/* We write 1 on CDSP.TEST0. We should get 0 on STATUS.TEST0. */
	vx_outl(chip, CDSP, chip->regCDSP | VX_CDSP_TEST0_MASK);
	vx_inl(chip, ISR);
	data = vx_inl(chip, STATUS);
	if ((data & VX_STATUS_VAL_TEST0_MASK) == VX_STATUS_VAL_TEST0_MASK) {
		snd_printdd("bad!\n");
		return -ENODEV;
	}

	/* We write 0 on CDSP.TEST0. We should get 1 on STATUS.TEST0. */
	vx_outl(chip, CDSP, chip->regCDSP & ~VX_CDSP_TEST0_MASK);
	vx_inl(chip, ISR);
	data = vx_inl(chip, STATUS);
	if (! (data & VX_STATUS_VAL_TEST0_MASK)) {
		snd_printdd("bad! #2\n");
		return -ENODEV;
	}

	if (_chip->type == VX_TYPE_BOARD) {
		/* not implemented on VX_2_BOARDS */
		/* We write 1 on CDSP.TEST1. We should get 0 on STATUS.TEST1. */
		vx_outl(chip, CDSP, chip->regCDSP | VX_CDSP_TEST1_MASK);
		vx_inl(chip, ISR);
		data = vx_inl(chip, STATUS);
		if ((data & VX_STATUS_VAL_TEST1_MASK) == VX_STATUS_VAL_TEST1_MASK) {
			snd_printdd("bad! #3\n");
			return -ENODEV;
		}

		/* We write 0 on CDSP.TEST1. We should get 1 on STATUS.TEST1. */
		vx_outl(chip, CDSP, chip->regCDSP & ~VX_CDSP_TEST1_MASK);
		vx_inl(chip, ISR);
		data = vx_inl(chip, STATUS);
		if (! (data & VX_STATUS_VAL_TEST1_MASK)) {
			snd_printdd("bad! #4\n");
			return -ENODEV;
		}
	}
	snd_printdd("ok, xilinx fine.\n");
	return 0;
}


/**
 * vx_setup_pseudo_dma - set up the pseudo dma read/write mode.
 * @do_write: 0 = read, 1 = set up for DMA write
 */
static void vx2_setup_pseudo_dma(struct vx_core *chip, int do_write)
{
	/* Interrupt mode and HREQ pin enabled for host transmit data transfers
	 * (in case of the use of the pseudo-dma facility).
	 */
	vx_outl(chip, ICR, do_write ? ICR_TREQ : ICR_RREQ);

	/* Reset the pseudo-dma register (in case of the use of the
	 * pseudo-dma facility).
	 */
	vx_outl(chip, RESET_DMA, 0);
}

/*
 * vx_release_pseudo_dma - disable the pseudo-DMA mode
 */
static inline void vx2_release_pseudo_dma(struct vx_core *chip)
{
	/* HREQ pin disabled. */
	vx_outl(chip, ICR, 0);
}



/* pseudo-dma write */
static void vx2_dma_write(struct vx_core *chip, struct snd_pcm_runtime *runtime,
			  struct vx_pipe *pipe, int count)
{
	unsigned long port = vx2_reg_addr(chip, VX_DMA);
	int offset = pipe->hw_ptr;
	u32 *addr = (u32 *)(runtime->dma_area + offset);

	if (snd_BUG_ON(count % 4))
		return;

	vx2_setup_pseudo_dma(chip, 1);

	/* Transfer using pseudo-dma.
	 */
	if (offset + count > pipe->buffer_bytes) {
		int length = pipe->buffer_bytes - offset;
		count -= length;
		length >>= 2; /* in 32bit words */
		/* Transfer using pseudo-dma. */
		while (length-- > 0) {
			outl(cpu_to_le32(*addr), port);
			addr++;
		}
		addr = (u32 *)runtime->dma_area;
		pipe->hw_ptr = 0;
	}
	pipe->hw_ptr += count;
	count >>= 2; /* in 32bit words */
	/* Transfer using pseudo-dma. */
	while (count-- > 0) {
		outl(cpu_to_le32(*addr), port);
		addr++;
	}

	vx2_release_pseudo_dma(chip);
}


/* pseudo dma read */
static void vx2_dma_read(struct vx_core *chip, struct snd_pcm_runtime *runtime,
			 struct vx_pipe *pipe, int count)
{
	int offset = pipe->hw_ptr;
	u32 *addr = (u32 *)(runtime->dma_area + offset);
	unsigned long port = vx2_reg_addr(chip, VX_DMA);

	if (snd_BUG_ON(count % 4))
		return;

	vx2_setup_pseudo_dma(chip, 0);
	/* Transfer using pseudo-dma.
	 */
	if (offset + count > pipe->buffer_bytes) {
		int length = pipe->buffer_bytes - offset;
		count -= length;
		length >>= 2; /* in 32bit words */
		/* Transfer using pseudo-dma. */
		while (length-- > 0)
			*addr++ = le32_to_cpu(inl(port));
		addr = (u32 *)runtime->dma_area;
		pipe->hw_ptr = 0;
	}
	pipe->hw_ptr += count;
	count >>= 2; /* in 32bit words */
	/* Transfer using pseudo-dma. */
	while (count-- > 0)
		*addr++ = le32_to_cpu(inl(port));

	vx2_release_pseudo_dma(chip);
}

#define VX_XILINX_RESET_MASK        0x40000000
#define VX_USERBIT0_MASK            0x00000004
#define VX_USERBIT1_MASK            0x00000020
#define VX_CNTRL_REGISTER_VALUE     0x00172012

/*
 * transfer counts bits to PLX
 */
static int put_xilinx_data(struct vx_core *chip, unsigned int port, unsigned int counts, unsigned char data)
{
	unsigned int i;

	for (i = 0; i < counts; i++) {
		unsigned int val;

		/* set the clock bit to 0. */
		val = VX_CNTRL_REGISTER_VALUE & ~VX_USERBIT0_MASK;
		vx2_outl(chip, port, val);
		vx2_inl(chip, port);
		udelay(1);

		if (data & (1 << i))
			val |= VX_USERBIT1_MASK;
		else
			val &= ~VX_USERBIT1_MASK;
		vx2_outl(chip, port, val);
		vx2_inl(chip, port);

		/* set the clock bit to 1. */
		val |= VX_USERBIT0_MASK;
		vx2_outl(chip, port, val);
		vx2_inl(chip, port);
		udelay(1);
	}
	return 0;
}

/*
 * load the xilinx image
 */
static int vx2_load_xilinx_binary(struct vx_core *chip, const struct firmware *xilinx)
{
	unsigned int i;
	unsigned int port;
	const unsigned char *image;

	/* XILINX reset (wait at least 1 millisecond between reset on and off). */
	vx_outl(chip, CNTRL, VX_CNTRL_REGISTER_VALUE | VX_XILINX_RESET_MASK);
	vx_inl(chip, CNTRL);
	msleep(10);
	vx_outl(chip, CNTRL, VX_CNTRL_REGISTER_VALUE);
	vx_inl(chip, CNTRL);
	msleep(10);

	if (chip->type == VX_TYPE_BOARD)
		port = VX_CNTRL;
	else
		port = VX_GPIOC; /* VX222 V2 and VX222_MIC_BOARD with new PLX9030 use this register */

	image = xilinx->data;
	for (i = 0; i < xilinx->size; i++, image++) {
		if (put_xilinx_data(chip, port, 8, *image) < 0)
			return -EINVAL;
		/* don't take too much time in this loop... */
		cond_resched();
	}
	put_xilinx_data(chip, port, 4, 0xff); /* end signature */

	msleep(200);

	/* test after loading (is buggy with VX222) */
	if (chip->type != VX_TYPE_BOARD) {
		/* Test if load successful: test bit 8 of register GPIOC (VX222: use CNTRL) ! */
		i = vx_inl(chip, GPIOC);
		if (i & 0x0100)
			return 0;
		snd_printk(KERN_ERR "vx222: xilinx test failed after load, GPIOC=0x%x\n", i);
		return -EINVAL;
	}

	return 0;
}

	
/*
 * load the boot/dsp images
 */
static int vx2_load_dsp(struct vx_core *vx, int index, const struct firmware *dsp)
{
	int err;

	switch (index) {
	case 1:
		/* xilinx image */
		if ((err = vx2_load_xilinx_binary(vx, dsp)) < 0)
			return err;
		if ((err = vx2_test_xilinx(vx)) < 0)
			return err;
		return 0;
	case 2:
		/* DSP boot */
		return snd_vx_dsp_boot(vx, dsp);
	case 3:
		/* DSP image */
		return snd_vx_dsp_load(vx, dsp);
	default:
		snd_BUG();
		return -EINVAL;
	}
}


/*
 * vx_test_and_ack - test and acknowledge interrupt
 *
 * called from irq hander, too
 *
 * spinlock held!
 */
static int vx2_test_and_ack(struct vx_core *chip)
{
	/* not booted yet? */
	if (! (chip->chip_status & VX_STAT_XILINX_LOADED))
		return -ENXIO;

	if (! (vx_inl(chip, STATUS) & VX_STATUS_MEMIRQ_MASK))
		return -EIO;
	
	/* ok, interrupts generated, now ack it */
	/* set ACQUIT bit up and down */
	vx_outl(chip, STATUS, 0);
	/* useless read just to spend some time and maintain
	 * the ACQUIT signal up for a while ( a bus cycle )
	 */
	vx_inl(chip, STATUS);
	/* ack */
	vx_outl(chip, STATUS, VX_STATUS_MEMIRQ_MASK);
	/* useless read just to spend some time and maintain
	 * the ACQUIT signal up for a while ( a bus cycle ) */
	vx_inl(chip, STATUS);
	/* clear */
	vx_outl(chip, STATUS, 0);

	return 0;
}


/*
 * vx_validate_irq - enable/disable IRQ
 */
static void vx2_validate_irq(struct vx_core *_chip, int enable)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;

	/* Set the interrupt enable bit to 1 in CDSP register */
	if (enable) {
		/* Set the PCI interrupt enable bit to 1.*/
		vx_outl(chip, INTCSR, VX_INTCSR_VALUE|VX_PCI_INTERRUPT_MASK);
		chip->regCDSP |= VX_CDSP_VALID_IRQ_MASK;
	} else {
		/* Set the PCI interrupt enable bit to 0. */
		vx_outl(chip, INTCSR, VX_INTCSR_VALUE&~VX_PCI_INTERRUPT_MASK);
		chip->regCDSP &= ~VX_CDSP_VALID_IRQ_MASK;
	}
	vx_outl(chip, CDSP, chip->regCDSP);
}


/*
 * write an AKM codec data (24bit)
 */
static void vx2_write_codec_reg(struct vx_core *chip, unsigned int data)
{
	unsigned int i;

	vx_inl(chip, HIFREQ);

	/* We have to send 24 bits (3 x 8 bits). Start with most signif. Bit */
	for (i = 0; i < 24; i++, data <<= 1)
		vx_outl(chip, DATA, ((data & 0x800000) ? VX_DATA_CODEC_MASK : 0));
	/* Terminate access to codec registers */
	vx_inl(chip, RUER);
}


#define AKM_CODEC_POWER_CONTROL_CMD 0xA007
#define AKM_CODEC_RESET_ON_CMD      0xA100
#define AKM_CODEC_RESET_OFF_CMD     0xA103
#define AKM_CODEC_CLOCK_FORMAT_CMD  0xA240
#define AKM_CODEC_MUTE_CMD          0xA38D
#define AKM_CODEC_UNMUTE_CMD        0xA30D
#define AKM_CODEC_LEFT_LEVEL_CMD    0xA400
#define AKM_CODEC_RIGHT_LEVEL_CMD   0xA500

static const u8 vx2_akm_gains_lut[VX2_AKM_LEVEL_MAX+1] = {
    0x7f,      
    0x7d,      
    0x7c,      
    0x7a,      
    0x79,      
    0x77,      
    0x76,      
    0x75,      
    0x73,      
    0x72,      
    0x71,      
    0x70,      
    0x6f,      
    0x6d,      
    0x6c,      
    0x6a,      
    0x69,      
    0x67,      
    0x66,      
    0x65,      
    0x64,      
    0x62,      
    0x61,      
    0x60,      
    0x5f,      
    0x5e,      
    0x5c,      
    0x5b,      
    0x59,      
    0x58,      
    0x56,      
    0x55,      
    0x54,      
    0x53,      
    0x52,      
    0x51,      
    0x50,      
    0x4e,      
    0x4d,      
    0x4b,      
    0x4a,      
    0x48,      
    0x47,      
    0x46,      
    0x44,      
    0x43,      
    0x42,      
    0x41,      
    0x40,      
    0x3f,      
    0x3e,      
    0x3c,      
    0x3b,      
    0x39,      
    0x38,      
    0x37,      
    0x36,      
    0x34,      
    0x33,      
    0x32,      
    0x31,      
    0x31,      
    0x30,      
    0x2e,      
    0x2d,      
    0x2b,      
    0x2a,      
    0x29,      
    0x28,      
    0x27,      
    0x25,      
    0x24,      
    0x24,      
    0x23,      
    0x22,      
    0x21,      
    0x20,      
    0x1f,      
    0x1e,      
    0x1d,      
    0x1c,      
    0x1b,      
    0x1a,      
    0x19,      
    0x18,      
    0x17,      
    0x16,      
    0x15,      
    0x14,      
    0x14,      
    0x13,      
    0x12,      
    0x12,      
    0x11,      
    0x11,      
    0x10,      
    0x10,      
    0x0f,      
    0x0e,      
    0x0d,      
    0x0d,      
    0x0c,      
    0x0b,      
    0x0b,      
    0x0a,      
    0x0a,      
    0x09,      
    0x09,      
    0x08,      
    0x08,      
    0x07,      
    0x07,      
    0x06,      
    0x06,      
    0x06,      
    0x05,      
    0x05,      
    0x05,      
    0x05,      
    0x04,      
    0x04,      
    0x04,      
    0x04,      
    0x03,      
    0x03,      
    0x03,      
    0x03,      
    0x03,      
    0x03,      
    0x02,      
    0x02,      
    0x02,      
    0x02,      
    0x02,      
    0x02,      
    0x02,      
    0x02,      
    0x02,      
    0x02,      
    0x01,      
    0x01,      
    0x01,      
    0x01,      
    0x01,      
    0x01,      
    0x01,      
    0x01,      
    0x00};     

/*
 * pseudo-codec write entry
 */
static void vx2_write_akm(struct vx_core *chip, int reg, unsigned int data)
{
	unsigned int val;

	if (reg == XX_CODEC_DAC_CONTROL_REGISTER) {
		vx2_write_codec_reg(chip, data ? AKM_CODEC_MUTE_CMD : AKM_CODEC_UNMUTE_CMD);
		return;
	}

	/* `data' is a value between 0x0 and VX2_AKM_LEVEL_MAX = 0x093, in the case of the AKM codecs, we need
	   a look up table, as there is no linear matching between the driver codec values
	   and the real dBu value
	*/
	if (snd_BUG_ON(data >= sizeof(vx2_akm_gains_lut)))
		return;

	switch (reg) {
	case XX_CODEC_LEVEL_LEFT_REGISTER:
		val = AKM_CODEC_LEFT_LEVEL_CMD;
		break;
	case XX_CODEC_LEVEL_RIGHT_REGISTER:
		val = AKM_CODEC_RIGHT_LEVEL_CMD;
		break;
	default:
		snd_BUG();
		return;
	}
	val |= vx2_akm_gains_lut[data];

	vx2_write_codec_reg(chip, val);
}


/*
 * write codec bit for old VX222 board
 */
static void vx2_old_write_codec_bit(struct vx_core *chip, int codec, unsigned int data)
{
	int i;

	/* activate access to codec registers */
	vx_inl(chip, HIFREQ);

	for (i = 0; i < 24; i++, data <<= 1)
		vx_outl(chip, DATA, ((data & 0x800000) ? VX_DATA_CODEC_MASK : 0));

	/* Terminate access to codec registers */
	vx_inl(chip, RUER);
}


/*
 * reset codec bit
 */
static void vx2_reset_codec(struct vx_core *_chip)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;

	/* Set the reset CODEC bit to 0. */
	vx_outl(chip, CDSP, chip->regCDSP &~ VX_CDSP_CODEC_RESET_MASK);
	vx_inl(chip, CDSP);
	msleep(10);
	/* Set the reset CODEC bit to 1. */
	chip->regCDSP |= VX_CDSP_CODEC_RESET_MASK;
	vx_outl(chip, CDSP, chip->regCDSP);
	vx_inl(chip, CDSP);
	if (_chip->type == VX_TYPE_BOARD) {
		msleep(1);
		return;
	}

	msleep(5);  /* additionnel wait time for AKM's */

	vx2_write_codec_reg(_chip, AKM_CODEC_POWER_CONTROL_CMD); /* DAC power up, ADC power up, Vref power down */
	
	vx2_write_codec_reg(_chip, AKM_CODEC_CLOCK_FORMAT_CMD); /* default */
	vx2_write_codec_reg(_chip, AKM_CODEC_MUTE_CMD); /* Mute = ON ,Deemphasis = OFF */
	vx2_write_codec_reg(_chip, AKM_CODEC_RESET_OFF_CMD); /* DAC and ADC normal operation */

	if (_chip->type == VX_TYPE_MIC) {
		/* set up the micro input selector */
		chip->regSELMIC =  MICRO_SELECT_INPUT_NORM |
			MICRO_SELECT_PREAMPLI_G_0 |
			MICRO_SELECT_NOISE_T_52DB;

		/* reset phantom power supply */
		chip->regSELMIC &= ~MICRO_SELECT_PHANTOM_ALIM;

		vx_outl(_chip, SELMIC, chip->regSELMIC);
	}
}


/*
 * change the audio source
 */
static void vx2_change_audio_source(struct vx_core *_chip, int src)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;

	switch (src) {
	case VX_AUDIO_SRC_DIGITAL:
		chip->regCFG |= VX_CFG_DATAIN_SEL_MASK;
		break;
	default:
		chip->regCFG &= ~VX_CFG_DATAIN_SEL_MASK;
		break;
	}
	vx_outl(chip, CFG, chip->regCFG);
}


/*
 * set the clock source
 */
static void vx2_set_clock_source(struct vx_core *_chip, int source)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;

	if (source == INTERNAL_QUARTZ)
		chip->regCFG &= ~VX_CFG_CLOCKIN_SEL_MASK;
	else
		chip->regCFG |= VX_CFG_CLOCKIN_SEL_MASK;
	vx_outl(chip, CFG, chip->regCFG);
}

/*
 * reset the board
 */
static void vx2_reset_board(struct vx_core *_chip, int cold_reset)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;

	/* initialize the register values */
	chip->regCDSP = VX_CDSP_CODEC_RESET_MASK | VX_CDSP_DSP_RESET_MASK ;
	chip->regCFG = 0;
}



/*
 * input level controls for VX222 Mic
 */

/* Micro level is specified to be adjustable from -96dB to 63 dB (board coded 0x00 ... 318),
 * 318 = 210 + 36 + 36 + 36   (210 = +9dB variable) (3 * 36 = 3 steps of 18dB pre ampli)
 * as we will mute if less than -110dB, so let's simply use line input coded levels and add constant offset !
 */
#define V2_MICRO_LEVEL_RANGE        (318 - 255)

static void vx2_set_input_level(struct snd_vx222 *chip)
{
	int i, miclevel, preamp;
	unsigned int data;

	miclevel = chip->mic_level;
	miclevel += V2_MICRO_LEVEL_RANGE; /* add 318 - 0xff */
	preamp = 0;
        while (miclevel > 210) { /* limitation to +9dB of 3310 real gain */
		preamp++;	/* raise pre ampli + 18dB */
		miclevel -= (18 * 2);   /* lower level 18 dB (*2 because of 0.5 dB steps !) */
        }
	if (snd_BUG_ON(preamp >= 4))
		return;

	/* set pre-amp level */
	chip->regSELMIC &= ~MICRO_SELECT_PREAMPLI_MASK;
	chip->regSELMIC |= (preamp << MICRO_SELECT_PREAMPLI_OFFSET) & MICRO_SELECT_PREAMPLI_MASK;
	vx_outl(chip, SELMIC, chip->regSELMIC);

	data = (unsigned int)miclevel << 16 |
		(unsigned int)chip->input_level[1] << 8 |
		(unsigned int)chip->input_level[0];
	vx_inl(chip, DATA); /* Activate input level programming */

	/* We have to send 32 bits (4 x 8 bits) */
	for (i = 0; i < 32; i++, data <<= 1)
		vx_outl(chip, DATA, ((data & 0x80000000) ? VX_DATA_CODEC_MASK : 0));

	vx_inl(chip, RUER); /* Terminate input level programming */
}


#define MIC_LEVEL_MAX	0xff

static const DECLARE_TLV_DB_SCALE(db_scale_mic, -6450, 50, 0);

/*
 * controls API for input levels
 */

/* input levels */
static int vx_input_level_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = MIC_LEVEL_MAX;
	return 0;
}

static int vx_input_level_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct vx_core *_chip = snd_kcontrol_chip(kcontrol);
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;
	mutex_lock(&_chip->mixer_mutex);
	ucontrol->value.integer.value[0] = chip->input_level[0];
	ucontrol->value.integer.value[1] = chip->input_level[1];
	mutex_unlock(&_chip->mixer_mutex);
	return 0;
}

static int vx_input_level_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct vx_core *_chip = snd_kcontrol_chip(kcontrol);
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;
	if (ucontrol->value.integer.value[0] < 0 ||
	    ucontrol->value.integer.value[0] > MIC_LEVEL_MAX)
		return -EINVAL;
	if (ucontrol->value.integer.value[1] < 0 ||
	    ucontrol->value.integer.value[1] > MIC_LEVEL_MAX)
		return -EINVAL;
	mutex_lock(&_chip->mixer_mutex);
	if (chip->input_level[0] != ucontrol->value.integer.value[0] ||
	    chip->input_level[1] != ucontrol->value.integer.value[1]) {
		chip->input_level[0] = ucontrol->value.integer.value[0];
		chip->input_level[1] = ucontrol->value.integer.value[1];
		vx2_set_input_level(chip);
		mutex_unlock(&_chip->mixer_mutex);
		return 1;
	}
	mutex_unlock(&_chip->mixer_mutex);
	return 0;
}

/* mic level */
static int vx_mic_level_info(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_info *uinfo)
{
	uinfo->type = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = MIC_LEVEL_MAX;
	return 0;
}

static int vx_mic_level_get(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct vx_core *_chip = snd_kcontrol_chip(kcontrol);
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;
	ucontrol->value.integer.value[0] = chip->mic_level;
	return 0;
}

static int vx_mic_level_put(struct snd_kcontrol *kcontrol, struct snd_ctl_elem_value *ucontrol)
{
	struct vx_core *_chip = snd_kcontrol_chip(kcontrol);
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;
	if (ucontrol->value.integer.value[0] < 0 ||
	    ucontrol->value.integer.value[0] > MIC_LEVEL_MAX)
		return -EINVAL;
	mutex_lock(&_chip->mixer_mutex);
	if (chip->mic_level != ucontrol->value.integer.value[0]) {
		chip->mic_level = ucontrol->value.integer.value[0];
		vx2_set_input_level(chip);
		mutex_unlock(&_chip->mixer_mutex);
		return 1;
	}
	mutex_unlock(&_chip->mixer_mutex);
	return 0;
}

static struct snd_kcontrol_new vx_control_input_level = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	.access =	(SNDRV_CTL_ELEM_ACCESS_READWRITE |
			 SNDRV_CTL_ELEM_ACCESS_TLV_READ),
	.name =		"Capture Volume",
	.info =		vx_input_level_info,
	.get =		vx_input_level_get,
	.put =		vx_input_level_put,
	.tlv = { .p = db_scale_mic },
};

static struct snd_kcontrol_new vx_control_mic_level = {
	.iface =	SNDRV_CTL_ELEM_IFACE_MIXER,
	.access =	(SNDRV_CTL_ELEM_ACCESS_READWRITE |
			 SNDRV_CTL_ELEM_ACCESS_TLV_READ),
	.name =		"Mic Capture Volume",
	.info =		vx_mic_level_info,
	.get =		vx_mic_level_get,
	.put =		vx_mic_level_put,
	.tlv = { .p = db_scale_mic },
};

/*
 * FIXME: compressor/limiter implementation is missing yet...
 */

static int vx2_add_mic_controls(struct vx_core *_chip)
{
	struct snd_vx222 *chip = (struct snd_vx222 *)_chip;
	int err;

	if (_chip->type != VX_TYPE_MIC)
		return 0;

	/* mute input levels */
	chip->input_level[0] = chip->input_level[1] = 0;
	chip->mic_level = 0;
	vx2_set_input_level(chip);

	/* controls */
	if ((err = snd_ctl_add(_chip->card, snd_ctl_new1(&vx_control_input_level, chip))) < 0)
		return err;
	if ((err = snd_ctl_add(_chip->card, snd_ctl_new1(&vx_control_mic_level, chip))) < 0)
		return err;

	return 0;
}


/*
 * callbacks
 */
struct snd_vx_ops vx222_ops = {
	.in8 = vx2_inb,
	.in32 = vx2_inl,
	.out8 = vx2_outb,
	.out32 = vx2_outl,
	.test_and_ack = vx2_test_and_ack,
	.validate_irq = vx2_validate_irq,
	.akm_write = vx2_write_akm,
	.reset_codec = vx2_reset_codec,
	.change_audio_source = vx2_change_audio_source,
	.set_clock_source = vx2_set_clock_source,
	.load_dsp = vx2_load_dsp,
	.reset_dsp = vx2_reset_dsp,
	.reset_board = vx2_reset_board,
	.dma_write = vx2_dma_write,
	.dma_read = vx2_dma_read,
	.add_controls = vx2_add_mic_controls,
};

/* for old VX222 board */
struct snd_vx_ops vx222_old_ops = {
	.in8 = vx2_inb,
	.in32 = vx2_inl,
	.out8 = vx2_outb,
	.out32 = vx2_outl,
	.test_and_ack = vx2_test_and_ack,
	.validate_irq = vx2_validate_irq,
	.write_codec = vx2_old_write_codec_bit,
	.reset_codec = vx2_reset_codec,
	.change_audio_source = vx2_change_audio_source,
	.set_clock_source = vx2_set_clock_source,
	.load_dsp = vx2_load_dsp,
	.reset_dsp = vx2_reset_dsp,
	.reset_board = vx2_reset_board,
	.dma_write = vx2_dma_write,
	.dma_read = vx2_dma_read,
};

