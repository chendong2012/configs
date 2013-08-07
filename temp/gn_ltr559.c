/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/* drivers/hwmon/mt6516/amit/ltr559.c - LTR559 ALS/PS driver
 *
 * Author: MingHsien Hsieh <minghsien.hsieh@mediatek.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/kobject.h>
#include <linux/earlysuspend.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>

#include <linux/hwmsensor.h>
#include <linux/hwmsen_dev.h>
#include <linux/sensors_io.h>
#include <asm/io.h>
#include <cust_eint.h>
#include <cust_alsps_ltr559.h>
#include "gn_ltr559.h"
#include <linux/hwmsen_helper.h>

#include <linux/wakelock.h>

#include <mach/irqs.h>
#include <mach/eint.h>

#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#include <mach/mt_reg_base.h>

#define POWER_NONE_MACRO MT65XX_POWER_NONE
#define CALIBRATE_IN_PSENABLE

#define LTR559_I2C_ADDR_RAR	0 /*!< the index in obj->hw->i2c_addr: alert response address */
#define LTR559_I2C_ADDR_ALS	1 /*!< the index in obj->hw->i2c_addr: ALS address */
#define LTR559_I2C_ADDR_PS	2 /*!< the index in obj->hw->i2c_addr: PS address */
#define LTR559_DEV_NAME		"LTR559"

#define APS_TAG	"[ALS/PS] "
#define APS_FUN(f)      printk(KERN_ERR APS_TAG"%s\n", __FUNCTION__)
#define APS_ERR(fmt, args...)	printk(KERN_ERR APS_TAG"%s %d : "fmt, __FUNCTION__, __LINE__, ##args)
#define APS_DEBUG
#if defined(APS_DEBUG)
#define APS_LOG(fmt, args...)	printk(KERN_ERR APS_TAG "%s(%d):" fmt, __FUNCTION__, __LINE__, ##args)
#define APS_DBG(fmt, args...)	printk(KERN_ERR APS_TAG fmt, ##args)
#else
#define APS_LOG(fmt, args...)
#define APS_DBG(fmt, args...)
#endif


static struct i2c_client *ltr559_i2c_client = NULL;
static const struct i2c_device_id ltr559_i2c_id[] = {{LTR559_DEV_NAME,0},{}};
static struct i2c_board_info __initdata i2c_ltr559={ I2C_BOARD_INFO(LTR559_DEV_NAME, LTR559_I2C_SLAVE_ADDR)};

static int ltr559_enable_ps(struct i2c_client *client, bool enable);  

static struct ltr559_priv *g_ltr559_ptr = NULL;

static int ltr559_local_init(void);
static int ltr559_remove(void);
static int ltr559_init_flag = 0;

static struct sensor_init_info ltr559_init_info = {
	.name = "psensor_ltr559",
	.init = ltr559_local_init,
	.uninit = ltr559_remove,
};

#if defined(GN_PS_NEED_CALIBRATION)
struct PS_CALI_DATA_STRUCT
{
	int close;
	int far_away;
	int  valid;
};

static struct PS_CALI_DATA_STRUCT ps_cali={{0,0,0},};
#endif

typedef enum {
	CMC_TRC_APS_DATA	= 0x0002,
	CMC_TRC_EINT		= 0x0004,
	CMC_TRC_IOCTL		= 0x0008,
	CMC_TRC_I2C		= 0x0010,
	CMC_TRC_CVT_ALS		= 0x0020,
	CMC_TRC_CVT_PS		= 0x0040,
	CMC_TRC_DEBUG		= 0x8000,
} CMC_TRC;

typedef enum {
	CMC_BIT_ALS		= 1,
	CMC_BIT_PS		= 2,
} CMC_BIT;

struct ltr559_priv {
	struct alsps_hw *hw;
	struct i2c_client *client;
#ifdef GN_MTK_BSP_ALSPS_INTERRUPT_MODE
	struct delayed_work eint_work;
#endif
	/*misc*/
	atomic_t	trace;
	atomic_t	i2c_retry;
	atomic_t	als_suspend;
	atomic_t	als_debounce;	/*debounce time after enabling als*/
	atomic_t	als_deb_on;	/*indicates if the debounce is on*/
	atomic_t	als_deb_end;	/*the jiffies representing the end of debounce*/
	atomic_t	ps_mask;	/*mask ps: always return far away*/
	atomic_t	ps_debounce;	/*debounce time after enabling ps*/
	atomic_t	ps_deb_on;	/*indicates if the debounce is on*/
	atomic_t	ps_deb_end;	/*the jiffies representing the end of debounce*/
	atomic_t	ps_suspend;

	int		als;
	int		ps;
	u8		_align;
	u16		als_level_num;
	u16		als_value_num;
	u32		als_level[C_CUST_ALS_LEVEL-1];
	u32		als_value[C_CUST_ALS_LEVEL];

	bool		als_enable;	/*record current als status*/
	unsigned int	als_widow_loss;

	bool		ps_enable;	 /*record current ps status*/
	unsigned int	ps_thd_val;	 /*the cmd value can't be read, stored in ram*/
	ulong		enable;		 /*record HAL enalbe status*/
	ulong		pending_intr;	/*pending interrupt*/
	unsigned int	polling;
	/*early suspend*/
#if defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend	early_drv;
#endif
};


static struct ltr559_priv *ltr559_obj = NULL;
static struct platform_driver ltr559_alsps_driver;

static int als_gainrange;
static int final_lux_val;
static int proximity_high;
static int proximity_low;

static int ltr559_get_ps_value(struct ltr559_priv *obj, int ps);
static int ltr559_get_als_value(struct ltr559_priv *obj, int als);
static int ltr559_calibrate_inPsEnable(void);

static int hwmsen_read_byte_sr(struct i2c_client *client, u8 addr, u8 *data)
{
	u8 buf;
	int ret = 0;
	struct i2c_client client_read = *client;

	client_read.addr = (client->addr & I2C_MASK_FLAG) | I2C_WR_FLAG |I2C_RS_FLAG;
	buf = addr;
	ret = i2c_master_send(&client_read, (const char*)&buf, 1<<8 | 1);
	if (ret < 0) {
		APS_ERR("send command error!!\n");
		return -EFAULT;
	}

	*data = buf;
	client->addr = client->addr& I2C_MASK_FLAG;
	return 0;
}

int ltr559_get_timing(void)
{
	return 200;
}

int ltr559_read_data_als(struct i2c_client *client, int *data)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int ret = 0;
	int alsval_ch1_lo = 0;
	int alsval_ch1_hi = 0;
	int alsval_ch0_lo = 0;
	int alsval_ch0_hi = 0;
	int luxdata_int;
	int luxdata_flt;
	int ratio;
	int alsval_ch0;
	int alsval_ch1;

	if (hwmsen_read_byte_sr(client, APS_RO_ALS_DATA_CH1_0, &alsval_ch1_lo)) {
		APS_ERR("reads als data (ch1 lo) = %d\n", alsval_ch1_lo);
		return -EFAULT;
	}
	if (hwmsen_read_byte_sr(client, APS_RO_ALS_DATA_CH1_1, &alsval_ch1_hi)) {
		APS_ERR("reads aps data (ch1 hi) = %d\n", alsval_ch1_hi);
		return -EFAULT;
	}
	alsval_ch1 = (alsval_ch1_hi * 256) + alsval_ch1_lo;
	APS_LOG("alsval_ch1_hi=%x alsval_ch1_lo=%x\n",alsval_ch1_hi,alsval_ch1_lo);


	if (hwmsen_read_byte_sr(client, APS_RO_ALS_DATA_CH0_0, &alsval_ch0_lo)) {
		APS_ERR("reads als data (ch0 lo) = %d\n", alsval_ch0_lo);
		return -EFAULT;
	}
	if (hwmsen_read_byte_sr(client, APS_RO_ALS_DATA_CH0_1, &alsval_ch0_hi)) {
		APS_ERR("reads als data (ch0 hi) = %d\n", alsval_ch0_hi);
		return -EFAULT;
	}
	alsval_ch0 = (alsval_ch0_hi * 256) + alsval_ch0_lo;
	APS_LOG("alsval_ch0_hi=%x alsval_ch0_lo=%x\n",alsval_ch0_hi,alsval_ch0_lo);

	// FIXME:
	// make sure we will not meet div0 error
	if ((alsval_ch0 + alsval_ch1) == 0) {
		APS_ERR("Both CH0 and CH1 are zero\n");
		ratio = 1000;
	} else {
		ratio = (alsval_ch1 * 1000) / (alsval_ch0 + alsval_ch1);
	}
	if (ratio < 450) {
		luxdata_flt = (17743 * alsval_ch0) + (11059 * alsval_ch1);
		luxdata_flt = luxdata_flt / 10000;
	} else if ((ratio >= 450) && (ratio < 640)) {
		luxdata_flt = (37725 * alsval_ch0) - (13363 * alsval_ch1);
		luxdata_flt = luxdata_flt / 10000;
	} else if ((ratio >= 640) && (ratio < 1000)) {
		luxdata_flt = (16900 * alsval_ch0) - (1690 * alsval_ch1);
		luxdata_flt = luxdata_flt / 10000;
	} else {
		luxdata_flt = 0;
	}

	printk("read_data_als ratio = %d \n",ratio);
	//Gionee: mali 2012-06-14 modify the CR00623845 from the old data to the new data for mmi test begin

	if (luxdata_flt > 65535)
		luxdata_flt = 65534;

	if (luxdata_flt <0)
		luxdata_flt = 65534;

	//Gionee: mali 2012-06-14 modify the CR00623845 from the old data to the new data for mmi test end
	// convert float to integer;
	luxdata_int = luxdata_flt;
	if ((luxdata_flt - luxdata_int) > 0.5) {
		luxdata_int = luxdata_int + 1;
	} else {
		luxdata_int = luxdata_flt;
	}

	if (atomic_read(&obj->trace) & CMC_TRC_APS_DATA) {
		APS_DBG("ALS (CH0): 0x%04X\n", alsval_ch0);
		APS_DBG("ALS (CH1): 0x%04X\n", alsval_ch1);
		APS_DBG("ALS (Ratio): %d\n", ratio);
		APS_DBG("ALS: %d\n", luxdata_int);
	}

	*data = luxdata_int;
	APS_LOG("luxdata_int=%x \n",luxdata_int);
	final_lux_val = luxdata_int;

	return 0;
}

int ltr559_read_data_ps(struct i2c_client *client, int *data)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int ret = 0;
	int psval_lo = 0;
	int psval_hi = 0;
	int psdata = 0;
	APS_FUN();
	if (hwmsen_read_byte_sr(client, APS_RO_PS_DATA_0, &psval_lo)) {
		APS_ERR("reads aps data = %d\n", psval_lo);
		return -EFAULT;
	}
	APS_LOG("ltr559_read_data_ps psval_lo = %x \n",psval_lo);

	if (hwmsen_read_byte_sr(client, APS_RO_PS_DATA_1, &psval_hi)) {
		APS_ERR("reads aps hi data = %d\n", psval_hi);
		return -EFAULT;
	}

	APS_LOG("ltr559_read_data_ps psval_hi = %x \n",psval_hi);
	psdata = ((psval_hi & 7) * 256) + psval_lo;
	*data = psdata;

	APS_LOG("ltr559_read_data_ps psdata = %x \n",psdata);
	return 0;

}

int ltr559_init_device(struct i2c_client *client)
{
	u8 buf =0;
	int i = 0;
	int ret = 0;
#ifdef GN_MTK_BSP_ALSPS_INTERRUPT_MODE
	if (hwmsen_write_byte(client, APS_RW_INTERRUPT, 0x01)) goto error_init_device;
	if (hwmsen_write_byte(client, APS_RW_INTERRUPT_PERSIST, 0x40)) goto error_init_device;
#endif 
	if (hwmsen_write_byte(client, APS_RW_PS_N_PULSES, 0x02)) goto error_init_device;
	if (hwmsen_write_byte(client, APS_RW_PS_LED, 0xff)) goto error_init_device;
#ifdef CALIBRATE_IN_PSENABLE	
#else
	if (hwmsen_write_byte(client, APS_RW_PS_THRES_UP_0, PS_THRES_UP_0_VALUE)) goto error_init_device;
	if (hwmsen_write_byte(client, APS_RW_PS_THRES_UP_1, PS_THRES_UP_1_VALUE)) goto error_init_device;
	if (hwmsen_write_byte(client, APS_RW_PS_THRES_LOW_0, PS_THRES_LOW_0_VALUE)) goto error_init_device;
	if (hwmsen_write_byte(client, APS_RW_PS_THRES_LOW_1, PS_THRES_LOW_1_VALUE)) goto error_init_device;
#endif	
	if (hwmsen_write_byte(client, APS_RW_PS_MEAS_RATE,0x00)) goto error_init_device;
	mdelay(WAKEUP_DELAY);
	return 0;
error_init_device:
	APS_ERR("init device reg error!\n");
	return -EFAULT;
}

/*----------------------------------------------------------------------------*/
static void ltr559_power(struct alsps_hw *hw, unsigned int on)
{
	static unsigned int power_on = 0;

	APS_LOG("power %s\n", on ? "on" : "off");
	APS_LOG("power id:%d POWER_NONE_MACRO:%d\n", hw->power_id, POWER_NONE_MACRO);

	if (hw->power_id != POWER_NONE_MACRO) {
		if (power_on == on)
			APS_LOG("ignore power control: %d\n", on);
		else if (on) {
			if (!hwPowerOn(hw->power_id, hw->power_vol, "LTR559"))
				APS_ERR("power on fails!!\n");
		} else {
			if (!hwPowerDown(hw->power_id, "LTR559"))
				APS_ERR("power off fail!!\n");
		}
	}
	power_on = on;
}

static int ltr559_enable_als(struct i2c_client *client, bool enable)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int err=0;
	u8 regdata=0;

	if (enable == obj->als_enable)
		return 0;

	if (hwmsen_read_byte_sr(client, APS_RW_ALS_CONTR, &regdata)) {
		APS_ERR("read APS_RW_ALS_CONTR register err!\n");
		return -1;
	}

	if (enable == TRUE) {
		APS_LOG("ALS(1): enable als only \n");
		regdata |= 0b00001101;
	} else {
		APS_LOG("ALS(1): disable als only \n");
		regdata &= 0b00000000;
	}

	if (hwmsen_write_byte(client, APS_RW_ALS_CONTR, regdata)) {
		APS_LOG("ltr559_enable_als failed!\n");
		return -1;
	}

	obj->als_enable = enable;
	return err;
}

static int ltr559_enable_ps(struct i2c_client *client, bool enable)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int err = 0;
	u8 regdata = 0;
	u8 regint = 0;
	int i;

	APS_LOG("enable:  %d, obj->ps_enable: %d\n",enable, obj->ps_enable);
	if (enable == obj->ps_enable)
		return 0;

	if (hwmsen_read_byte_sr(client, APS_RW_PS_CONTR, &regdata)) {
		APS_ERR("read APS_RW_PS_CONTR register err!\n");
		return -1;
	}

	if (enable == TRUE) {
		regdata |= 0b00000010;
		APS_LOG("PS(0): enable +++++++++++\n");
	} else {		
		APS_LOG("PS(0): disable -----------------\n");
		regdata &= 0b00000000;	// de-active the ps
	}

	APS_LOG("write ltr559_enable_ps regdata:%x \n",regdata);
	if (hwmsen_write_byte(client, APS_RW_PS_CONTR, regdata)) {
		APS_ERR("ltr559_enable_ps failed!\n");
		return -1;
	}

	if (hwmsen_read_byte_sr(client, APS_RW_PS_CONTR, &regdata)) {
		APS_ERR("ltr559_read failed!\n");
		return -1;
	}

	obj->ps_enable = enable;

#ifdef CALIBRATE_IN_PSENABLE
	ltr559_calibrate_inPsEnable();	
#endif

	return err;
}

#ifdef GN_MTK_BSP_ALSPS_INTERRUPT_MODE
static int ltr559_check_intr(struct i2c_client *client)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int err;
	u8 data=0;

	err = hwmsen_read_byte_sr(client,APS_RO_ALS_PS_STATUS,&data);

	if (err) {
		APS_ERR("WARNING: read int status: %d\n", err);
		return -1;
	}

	if (data & 0x02)
		set_bit(CMC_BIT_PS, &obj->pending_intr);
	else
		clear_bit(CMC_BIT_PS, &obj->pending_intr);

	if (atomic_read(&obj->trace) & CMC_TRC_DEBUG)
		APS_LOG("check intr: 0x%08X\n", obj->pending_intr);

	return 0;
}

/*----------------------------------------------------------------------------*/
void ltr559_eint_func(void)
{
	struct ltr559_priv *obj = g_ltr559_ptr;
	if (!obj)
		return;

	schedule_delayed_work(&obj->eint_work,0);
	if (atomic_read(&obj->trace) & CMC_TRC_EINT)
		APS_LOG("eint: als/ps intrs\n");
}
/*----------------------------------------------------------------------------*/
static void ltr559_eint_work(struct work_struct *work)
{
	struct ltr559_priv *obj = (struct ltr559_priv *)container_of(work, struct ltr559_priv, eint_work);
	int err;
	u8 buf;
	hwm_sensor_data sensor_data;

	APS_FUN();
	memset(&sensor_data, 0, sizeof(sensor_data));

	err = ltr559_check_intr(obj->client);
	if (err) {
		APS_ERR("check intrs: %d\n", err);
		return -1;
	}

	if ((1 << CMC_BIT_PS) & obj->pending_intr) {
		if (err = ltr559_read_data_ps(obj->client, &obj->ps)) {
			APS_ERR("ltr559 read ps data: %d\n", err);;
		}
		//map and store data to hwm_sensor_data
		sensor_data.values[0] = ltr559_get_ps_value(obj, obj->ps);

		if (sensor_data.values[0] == 0) {
			if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_0,0xff)) goto error_rw;
			if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_1,0x07)) goto error_rw;

			//Gionee mali add calibration for ltr559 2012-9-28 begin
#if defined(GN_PS_NEED_CALIBRATION)
			if (ps_cali.valid == 0) {
				#ifdef CALIBRATE_IN_PSENABLE
					if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_0,proximity_low & 0xff)) goto error_rw;
					if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_1,(proximity_low >> 8) & 0xff)) goto error_rw;				
				#else
					if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_0,PS_THRES_LOW_0_VALUE)) goto error_rw;
					if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_1,PS_THRES_LOW_1_VALUE)) goto error_rw;
				#endif
			} else if (ps_cali.valid == 1) {
				if (hwmsen_write_byte(obj->client, APS_RW_PS_THRES_LOW_0, (ps_cali.far_away) & 0x00ff)) goto error_rw;
				if (hwmsen_write_byte(obj->client, APS_RW_PS_THRES_LOW_1, ((ps_cali.far_away)>>8) & 0x07)) goto error_rw;
			}
#else
			#ifdef CALIBRATE_IN_PSENABLE
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_0,proximity_low & 0xff)) goto error_rw;
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_1,(proximity_low >> 8) & 0xff)) goto error_rw;
			#else
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_0,PS_THRES_LOW_0_VALUE)) goto error_rw;
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_1,PS_THRES_LOW_1_VALUE)) goto error_rw;
			#endif
#endif
		} else if (sensor_data.values[0] == 1) {
#if defined(GN_PS_NEED_CALIBRATION)
			if (ps_cali.valid == 0) {
			#ifdef CALIBRATE_IN_PSENABLE
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_0,proximity_high & 0xff)) goto error_rw;
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_1,(proximity_high>>8) & 0xff)) goto error_rw;		
			#else
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_0,PS_THRES_UP_0_VALUE)) goto error_rw;
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_1,PS_THRES_UP_1_VALUE)) goto error_rw;
			#endif
			} else if (ps_cali.valid == 1) {
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_0, ps_cali.close & 0x00ff)) goto error_rw;    
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_1, ((ps_cali.close)>> 8) & 0x07)) goto error_rw;
			}
#else
			#ifdef CALIBRATE_IN_PSENABLE
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_0,proximity_high & 0xff)) goto error_rw;
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_1,(proximity_high>>8) & 0xff)) goto error_rw;		
			#else
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_0,PS_THRES_UP_0_VALUE)) goto error_rw;
				if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_UP_1,PS_THRES_UP_1_VALUE)) goto error_rw;
			#endif
#endif
			if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_0,0x00)) goto error_rw;
			if (hwmsen_write_byte(obj->client,APS_RW_PS_THRES_LOW_1,0x00)) goto error_rw;
		}
		sensor_data.value_divide = 1;
		sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
		//let up layer to know
		APS_LOG("ltr559 read ps data = %d \n",sensor_data.values[0]);
		if (err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data))
			APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
	}

	mt_eint_unmask(CUST_EINT_ALS_NUM);
error_rw:
	APS_ERR("read or write alsps device reg error\n");
	return -EFAULT;
}

/*----------------------------------------------------------------------------*/
int ltr559_setup_eint(struct i2c_client *client)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);

	APS_FUN();
	g_ltr559_ptr = obj;

	/*configure to GPIO function, external interrupt*/
	mt_set_gpio_mode(GPIO_ALS_EINT_PIN, GPIO_ALS_EINT_PIN_M_EINT);
	mt_set_gpio_dir(GPIO_ALS_EINT_PIN, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_ALS_EINT_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_ALS_EINT_PIN, GPIO_PULL_UP);

	mt_eint_registration(CUST_EINT_ALS_NUM, CUST_EINT_ALS_TYPE, ltr559_eint_func, 1);
	mt_eint_unmask(CUST_EINT_ALS_NUM);
	return 0;
}
#endif 
//Gionee yanggy 2012-07-21 add for ps_interrupt mode end
/*----------------------------------------------------------------------------*/
static int ltr559_init_client(struct i2c_client *client)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int err=0;
	APS_FUN();	

	if ((err = ltr559_init_device(client))) {
		APS_ERR("ltr559_init_device init dev: %d\n", err);
		return err;
	}

#ifdef GN_MTK_BSP_ALSPS_INTERRUPT_MODE
	if ((err = ltr559_setup_eint(client))) {
		APS_ERR("setup eint: %d\n", err);
		return err;
	}
#endif

	return err;
}
/******************************************************************************
 * Sysfs attributes
 *******************************************************************************/
static ssize_t ltr559_show_config(struct device_driver *ddri, char *buf)
{
	ssize_t res;

	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	res = snprintf(buf, PAGE_SIZE, "(%d %d %d %d %d)\n",
			atomic_read(&ltr559_obj->i2c_retry), atomic_read(&ltr559_obj->als_debounce),
			atomic_read(&ltr559_obj->ps_mask), ltr559_obj->ps_thd_val, atomic_read(&ltr559_obj->ps_debounce));
	return res;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_store_config(struct device_driver *ddri, char *buf, size_t count)
{
	int retry, als_deb, ps_deb, mask, thres;
	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	if (5 == sscanf(buf, "%d %d %d %d %d", &retry, &als_deb, &mask, &thres, &ps_deb)) {
		atomic_set(&ltr559_obj->i2c_retry, retry);
		atomic_set(&ltr559_obj->als_debounce, als_deb);
		atomic_set(&ltr559_obj->ps_mask, mask);
		ltr559_obj->ps_thd_val= thres;
		atomic_set(&ltr559_obj->ps_debounce, ps_deb);
	} else {
		APS_ERR("invalid content: '%s', length = %d\n", buf, count);
	}
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_trace(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	res = snprintf(buf, PAGE_SIZE, "0x%04X\n", atomic_read(&ltr559_obj->trace));
	return res;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_store_trace(struct device_driver *ddri, char *buf, size_t count)
{
	int trace;
	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	if (1 == sscanf(buf, "0x%x", &trace))
		atomic_set(&ltr559_obj->trace, trace);
	else
		APS_ERR("invalid content: '%s', length = %d\n", buf, count);
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_als(struct device_driver *ddri, char *buf)
{
	int res;
	u8 dat = 0;

	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}
	if (res = ltr559_read_data_als(ltr559_obj->client, &ltr559_obj->als))
		return snprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	else {
		dat = ltr559_obj->als;
		return snprintf(buf, PAGE_SIZE, "0x%04X\n", dat);
	}
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_ps(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	u8 dat=0;
	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	if (res = ltr559_read_data_ps(ltr559_obj->client, &ltr559_obj->ps)) {
		return snprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	} else {
		dat = ltr559_get_ps_value(ltr559_obj, ltr559_obj->ps);
		return snprintf(buf, PAGE_SIZE, "0x%04X\n", dat);
	}
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_ps_raw(struct device_driver *ddri, char *buf)
{
	ssize_t res;
	int dat=0;
	int data = -1;
	int err = 0 ;

	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	if (err = ltr559_enable_ps(ltr559_obj->client, true)) {
		APS_ERR("enable ps fail: %d\n", err);
		return -1;
	}

	set_bit(CMC_BIT_PS, &ltr559_obj->enable);
	msleep(300);

	if (res = ltr559_read_data_ps(ltr559_obj->client, &ltr559_obj->ps)) {
		snprintf(buf, PAGE_SIZE, "ERROR: %d\n", res);
	} else {   
		dat = ltr559_obj->ps & 0x80;
		data = dat & 0x0000FFFF;
		dat = ltr559_get_ps_value(ltr559_obj, ltr559_obj->ps);
		data = ((dat<<16) & 0xFFFF0000) | data;
	}

	msleep(50);
	if (err = ltr559_enable_ps(ltr559_obj->client, false)) {
		APS_ERR("disable ps fail: %d\n", err);
		return -1;
	}
	clear_bit(CMC_BIT_PS, &ltr559_obj->enable);
	return snprintf(buf, PAGE_SIZE, "%08X\n", data);
}

/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_status(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;

	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	if (ltr559_obj->hw) {

		len += snprintf(buf+len, PAGE_SIZE-len, "CUST: %d, (%d %d)\n",
				ltr559_obj->hw->i2c_num, ltr559_obj->hw->power_id, ltr559_obj->hw->power_vol);

	} else {
		len += snprintf(buf+len, PAGE_SIZE-len, "CUST: NULL\n");
	}


	len += snprintf(buf+len, PAGE_SIZE-len, "MISC: %d %d\n", atomic_read(&ltr559_obj->als_suspend), atomic_read(&ltr559_obj->ps_suspend));

	return len;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_i2c(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	u32 base = I2C2_BASE;

	if (!ltr559_obj)
	{
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}
	return len;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_store_i2c(struct device_driver *ddri, char *buf, size_t count)
{
	int sample_div, step_div;
	unsigned long tmp;
	u32 base = I2C2_BASE;

	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	} else if (2 != sscanf(buf, "%d %d", &sample_div, &step_div)) {
		APS_ERR("invalid format: '%s'\n", buf);
		return 0;
	}

	return count;
}
/*----------------------------------------------------------------------------*/
#define IS_SPACE(CH) (((CH) == ' ') || ((CH) == '\n'))
/*----------------------------------------------------------------------------*/
static int read_int_from_buf(struct ltr559_priv *obj, const char* buf, size_t count,
		u32 data[], int len)
{
	int idx = 0;
	char *cur = (char*)buf, *end = (char*)(buf+count);

	while(idx < len)
	{
		while((cur < end) && IS_SPACE(*cur))
		{
			cur++;
		}

		if (1 != sscanf(cur, "%d", &data[idx]))
		{
			break;
		}

		idx++;
		while((cur < end) && !IS_SPACE(*cur))
		{
			cur++;
		}
	}
	return idx;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_reg(struct device_driver *ddri, char *buf)
{
	int i = 0;
	u8 bufdata;
	int count  = 0;
	
	if(!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	for(i = 0;i < 31 ;i++) {
		hwmsen_read_byte_sr(ltr559_obj->client,0x80+i,&bufdata);
		count+= sprintf(buf+count,"[%x] = (%x)\n",0x80+i,bufdata);
	}

	return count;
}

/*----------------------------------------------------------------------------*/
static ssize_t ltr559_store_reg(struct device_driver *ddri,char *buf,ssize_t count)
{

	u32 data[2];
	if(!ltr559_obj)
	{
		APS_ERR("ltr559_obj is null\n");
		return 0;
	}
	/*else if(2 != sscanf(buf,"%d %d",&addr,&data))*/
	else if(2 != read_int_from_buf(ltr559_obj,buf,count,data,2))
	{
		APS_ERR("invalid format:%s\n",buf);
		return 0;
	}

	hwmsen_write_byte(ltr559_obj->client,data[0],data[1]);

	return count;
}

/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_alslv(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	int idx;
	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	for(idx = 0; idx < ltr559_obj->als_level_num; idx++) {
		len += snprintf(buf+len, PAGE_SIZE-len, "%d ", ltr559_obj->hw->als_level[idx]);
	}
	len += snprintf(buf+len, PAGE_SIZE-len, "\n");
	return len;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_store_alslv(struct device_driver *ddri, char *buf, size_t count)
{
	struct ltr559_priv *obj;
	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	} else if (!strcmp(buf, "def")) {
		memcpy(ltr559_obj->als_level, ltr559_obj->hw->als_level, sizeof(ltr559_obj->als_level));
	} else if (ltr559_obj->als_level_num != read_int_from_buf(ltr559_obj, buf, count,
				ltr559_obj->hw->als_level, ltr559_obj->als_level_num)) {
		APS_ERR("invalid format: '%s'\n", buf);
	}
	
	return count;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_show_alsval(struct device_driver *ddri, char *buf)
{
	ssize_t len = 0;
	int idx;
	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	}

	for(idx = 0; idx < ltr559_obj->als_value_num; idx++) {
		len += snprintf(buf+len, PAGE_SIZE-len, "%d ", ltr559_obj->hw->als_value[idx]);
	}
	len += snprintf(buf+len, PAGE_SIZE-len, "\n");
	
	return len;
}
/*----------------------------------------------------------------------------*/
static ssize_t ltr559_store_alsval(struct device_driver *ddri, char *buf, size_t count)
{
	if (!ltr559_obj) {
		APS_ERR("ltr559_obj is null!!\n");
		return 0;
	} else if (!strcmp(buf, "def")) {
		memcpy(ltr559_obj->als_value, ltr559_obj->hw->als_value, sizeof(ltr559_obj->als_value));
	} else if (ltr559_obj->als_value_num != read_int_from_buf(ltr559_obj, buf, count,
				ltr559_obj->hw->als_value, ltr559_obj->als_value_num)) {
		APS_ERR("invalid format: '%s'\n", buf);
	}
	
	return count;
}

/*----------------------------------------------------------------------------*/
static DRIVER_ATTR(als,	 S_IWUSR | S_IRUGO, ltr559_show_als,	NULL);
static DRIVER_ATTR(ps,	 S_IWUSR | S_IRUGO, ltr559_show_ps,	NULL);
static DRIVER_ATTR(config, S_IWUSR | S_IRUGO, ltr559_show_config,ltr559_store_config);
static DRIVER_ATTR(alslv, S_IWUSR | S_IRUGO, ltr559_show_alslv, ltr559_store_alslv);
static DRIVER_ATTR(alsval, S_IWUSR | S_IRUGO, ltr559_show_alsval,ltr559_store_alsval);
static DRIVER_ATTR(trace, S_IWUSR | S_IRUGO, ltr559_show_trace, ltr559_store_trace);
static DRIVER_ATTR(status, S_IWUSR | S_IRUGO, ltr559_show_status, NULL);
static DRIVER_ATTR(reg,	 S_IWUSR | S_IRUGO, ltr559_show_reg, ltr559_store_reg);
static DRIVER_ATTR(i2c,	 S_IWUSR | S_IRUGO, ltr559_show_i2c, ltr559_store_i2c);
static DRIVER_ATTR(alsps,  S_IWUSR | S_IRUGO, ltr559_show_ps_raw, NULL);

/*----------------------------------------------------------------------------*/
static struct device_attribute *ltr559_attr_list[] = {
	&driver_attr_als,
	&driver_attr_ps,
	&driver_attr_trace,		/*trace log*/
	&driver_attr_config,
	&driver_attr_alslv,
	&driver_attr_alsval,
	&driver_attr_status,
	&driver_attr_i2c,
	&driver_attr_reg,
	&driver_attr_alsps
};
/*----------------------------------------------------------------------------*/
static int ltr559_create_attr(struct device_driver *driver)
{
	int idx, err = 0;
	int num = (int)(sizeof(ltr559_attr_list)/sizeof(ltr559_attr_list[0]));
	if (driver == NULL) {
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++) {
		if (err = driver_create_file(driver, ltr559_attr_list[idx])) {
			APS_ERR("driver_create_file (%s) = %d\n", ltr559_attr_list[idx]->attr.name, err);
			break;
		}
	}
	return err;
}
/*----------------------------------------------------------------------------*/
static int ltr559_delete_attr(struct device_driver *driver)
{
	int idx ,err = 0;
	int num = (int)(sizeof(ltr559_attr_list)/sizeof(ltr559_attr_list[0]));

	if (!driver)
		return -EINVAL;

	for (idx = 0; idx < num; idx++) {
		driver_remove_file(driver, ltr559_attr_list[idx]);
	}

	return err;
}
/******************************************************************************
 * Function Configuration
 ******************************************************************************/

static int ltr559_get_als_value(struct ltr559_priv *obj, int als)
{
	int idx;
	int invalid = 0;
	for(idx = 0; idx < obj->als_level_num; idx++) {
		if (als <= obj->hw->als_level[idx]) {
			break;
		}
	}

	if (idx >= obj->als_value_num) {
		APS_ERR("exceed range\n");
		idx = obj->als_value_num - 1;
	}

	if (1 == atomic_read(&obj->als_deb_on)) {
		unsigned long endt = atomic_read(&obj->als_deb_end);
		if (time_after(jiffies, endt)) {
			atomic_set(&obj->als_deb_on, 0);
		}

		if (1 == atomic_read(&obj->als_deb_on)) {
			invalid = 1;
		}
	}

	if (!invalid) {
		if (atomic_read(&obj->trace) & CMC_TRC_CVT_ALS) {
			APS_DBG("ALS: %05d => %05d\n", als, obj->hw->als_value[idx]);
		}
		return obj->hw->als_value[idx];
	} else {
		if (atomic_read(&obj->trace) & CMC_TRC_CVT_ALS) {
			APS_DBG("ALS: %05d => %05d (-1)\n", als, obj->hw->als_value[idx]);
		}
		return -1;
	}
}

/*----------------------------------------------------------------------------*/
static int ltr559_get_ps_value(struct ltr559_priv *obj, int ps)
{
	int val= -1;
	static int val_temp = 1;

	//Gionee mali add calibration for ltr559 2012-9-28 begin
#if defined(GN_PS_NEED_CALIBRATION)
	APS_LOG(" ps=%x,ps_cali.far_away = %x , ps_cali.close = %x ,ps_valid = %x \n", ps, ps_cali.far_away, ps_cali. close, ps_cali.valid);
	if (ps_cali.valid == 1) {   
		if (ps > ps_cali.close) {
			val = 0;
			val_temp = 0;
		} else if (ps < ps_cali.far_away) {
			val = 1;
			val_temp = 1;
		} else {
			val = val_temp;
		}
	} else {
		if (ps >= PS_THRES_UP_VALUE) {
			val = 0;
			val_temp = 0;
		} else if (ps <= PS_THRES_LOW_VALUE) {
			val = 1;
			val_temp = 1;
		} else {
			val = val_temp;
		}
		//Gionee mali modify for interrupt mode 2012-8-7 end
	}
#else
	if (ps>=PS_THRES_UP_VALUE) {
		val = 0;
		val_temp = 0;
	} else if (ps <= PS_THRES_LOW_VALUE) {
		val = 1;
		val_temp = 1;
	} else {
		val = val_temp;
	}
#endif
	//Gionee mali add calibration for ltr559 2012-9-28 end
	return val;
}

//add calibration in ps enable.

static int ltr559_calibrate_inPsEnable(void)
{
	int ret=0;
	int i=0;
	int data = 0;
	int data_total=0;
	int noise = 0;
	int len = 0;
	int err = 0;
	hwm_sensor_data sensor_data;

	if (!ltr559_obj) { 
		APS_ERR("ltr559_obj is null!!\n");
		//len = sprintf(buf, "ltr501_obj is null\n");
		//goto report_value;
		return -1;
	}

	for (i = 0; i < 10; i++) {
		// wait for ps value be stable

		mdelay(50);
		ret=ltr559_read_data_ps(ltr559_obj->client,&data);

		APS_ERR("the register of APS_RW_PS_CONTR data[i] is %x \n", data);

		if (ret < 0) {
			i--;
			continue;
		}

		data_total+= data;
	}
	noise = data_total/10;

	if(noise < 50) {
		proximity_high = noise + 30;
		proximity_low = noise + 20;
	} else if(noise < 100) {
		proximity_high = noise + 60;
		proximity_low = noise + 50;
	} else if(noise < 150) {
		proximity_high = noise + 80;
		proximity_low = noise + 70;
	} else if(noise < 200) {
		proximity_high = noise + 120;
		proximity_low = noise + 110; 
	} else if(noise < 1000) {
		proximity_high = noise + 300;
		proximity_low = noise + 280;	
	} else {
		proximity_high = noise + 200;
		proximity_low = noise + 180;	
	}

	hwmsen_write_byte(ltr559_obj->client, 0x90, proximity_high & 0xff);
	hwmsen_write_byte(ltr559_obj->client, 0x91, (proximity_high >> 8) & 0xff);
	hwmsen_write_byte(ltr559_obj->client, 0x92, proximity_low & 0xff);
	hwmsen_write_byte(ltr559_obj->client, 0x93, (proximity_low >> 8) & 0xff);

	sensor_data.values[0] = 1;
	sensor_data.value_divide = 1;
	sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
	if (err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)) {
		APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
	}	
	
	return 0;
}

//Gionee mali add calibration for ltr559 2012-9-28 begin
#if defined(GN_PS_NEED_CALIBRATION)
static ltr559_WriteCalibration(struct PS_CALI_DATA_STRUCT *data_cali)
{
	if (data_cali->valid ==1){
		ps_cali.close = data_cali->close; 
		ps_cali.far_away = data_cali->far_away;
		ps_cali.valid = 1;

		hwmsen_write_byte(ltr559_obj->client, 0x90, (ps_cali.close) & 0x00ff);
		hwmsen_write_byte(ltr559_obj->client, 0x91, ((ps_cali.close)>>8) & 0x07);
		hwmsen_write_byte(ltr559_obj->client, 0x92, (ps_cali.far_away)& 0x00ff);
		hwmsen_write_byte(ltr559_obj->client, 0x93, ((ps_cali.far_away)>>8)&0x07);
	} else {
		ps_cali.valid = 0; 
		#ifdef CALIBRATE_IN_PSENABLE
			hwmsen_write_byte(ltr559_obj->client, 0x90, proximity_high & 0xff);
			hwmsen_write_byte(ltr559_obj->client, 0x91, (proximity_high >> 8) & 0xff);
			hwmsen_write_byte(ltr559_obj->client, 0x92, proximity_low & 0xff);
			hwmsen_write_byte(ltr559_obj->client, 0x93, (proximity_low >> 8) & 0xff);
		#else
			hwmsen_write_byte(ltr559_obj->client, 0x90, PS_THRES_UP_0_VALUE);
			hwmsen_write_byte(ltr559_obj->client, 0x91, PS_THRES_UP_1_VALUE);
			hwmsen_write_byte(ltr559_obj->client, 0x92, PS_THRES_LOW_0_VALUE);
			hwmsen_write_byte(ltr559_obj->client, 0x93, PS_THRES_LOW_1_VALUE);
		#endif
	}
}

static int ltr559_read_data_for_cali(struct i2c_client *client,struct PS_CALI_DATA_STRUCT *ps_data_cali)
{

	int ret=0;
	int i=0;
	int data[COUNT];
	int data_total=0;
	ssize_t len = 0;
	int noise = 0;
	int max = 0;
	int buf;
	int err = 0;
	int j = 0;
	hwm_sensor_data sensor_data;

	if (!ltr559_obj) { 
		APS_ERR("ltr559_obj is null!!\n");
		len = sprintf(buf, "ltr501_obj is null\n");
		goto report_value;
	}

	// wait for register to be stable
	for (i = 0; i < COUNT; i++) {
		// wait for ps value be stable
		if (max++ > 50) {
			ps_cali.valid = 0;
			ps_data_cali->valid = 0;
			ps_data_cali->close = 0;
			ps_data_cali->far_away = 0;

			goto report_value;
		}

		mdelay(50);
		ret=ltr559_read_data_ps(ltr559_obj->client,&data[i]);

		APS_ERR("the register of APS_RW_PS_CONTR data[i] is %x \n", data[i]);

		if (ret < 0) {
			i--;
			continue;
		}

		data_total+= data[i];
		if (data[i] == 0) {
			j++;
		}
	}

	if (data_total == 0) {
		ps_data_cali->close = NOISE_HIGH;
		ps_data_cali->far_away = NOISE_LOW;
		ps_data_cali->valid = 1;

		ps_cali.close = NOISE_HIGH;
		ps_cali.far_away = NOISE_LOW;
		ps_cali.valid = 1;
	} else {
		noise=data_total/(COUNT-j);
		if (noise > NOISE_MAX) {
			ps_cali.far_away = 0;
			ps_cali.close = 0;
			ps_cali.valid = 0;

			ps_data_cali->valid = 0;
			ps_data_cali->close = 0;
			ps_data_cali->far_away = 0;
		} else {
			ps_data_cali->close = noise + NOISE_HIGH; 
			ps_data_cali->far_away = noise + NOISE_LOW;
			ps_data_cali->valid = 1;

			ps_cali.close = noise + NOISE_HIGH;
			ps_cali.far_away = noise + NOISE_LOW;
			ps_cali.valid = 1;
		}

	}

	if (ps_cali.valid ==1) {
		hwmsen_write_byte(ltr559_obj->client, 0x90, (ps_cali.close) & 0x00ff);
		hwmsen_write_byte(ltr559_obj->client, 0x91, ((ps_cali.close)>>8) & 0x07);
		hwmsen_write_byte(ltr559_obj->client, 0x92, (ps_cali.far_away)& 0x00ff);
		hwmsen_write_byte(ltr559_obj->client, 0x93, ((ps_cali.far_away)>>8)&0x07);
	} else {
		#ifdef CALIBRATE_IN_PSENABLE
			hwmsen_write_byte(ltr559_obj->client, 0x90, proximity_high & 0xff);
			hwmsen_write_byte(ltr559_obj->client, 0x91, (proximity_high >> 8) & 0xff);
			hwmsen_write_byte(ltr559_obj->client, 0x92, proximity_low & 0xff);
			hwmsen_write_byte(ltr559_obj->client, 0x93,  (proximity_low >> 8) & 0xff);		
		#else
			hwmsen_write_byte(ltr559_obj->client, 0x90, PS_THRES_UP_0_VALUE);
			hwmsen_write_byte(ltr559_obj->client, 0x91, PS_THRES_UP_1_VALUE);
			hwmsen_write_byte(ltr559_obj->client, 0x92, PS_THRES_LOW_0_VALUE);
			hwmsen_write_byte(ltr559_obj->client, 0x93, PS_THRES_LOW_1_VALUE);
		#endif
	}

report_value:
	sensor_data.values[0] = 1;
	sensor_data.value_divide = 1;
	sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
	if (err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)) {
		APS_ERR("call hwmsen_get_interrupt_data fail = %d\n", err);
	}

	return 0;
}
#endif
//Gionee mali add calibration for ltr559 2012-9-28 end

/******************************************************************************
 * Function Configuration
 ******************************************************************************/
static int ltr559_open(struct inode *inode, struct file *file)
{
	file->private_data = ltr559_i2c_client;

	if (!file->private_data)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}

	return nonseekable_open(inode, file);
}
/*----------------------------------------------------------------------------*/
static int ltr559_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}
/*----------------------------------------------------------------------------*/
static long ltr559_unlocked_ioctl(struct file *file, unsigned int cmd,unsigned long arg)
{
	struct i2c_client *client = (struct i2c_client*)file->private_data;
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int err = 0;
	void __user *ptr = (void __user*) arg;
	int dat;
	uint32_t enable;
#if defined(GN_PS_NEED_CALIBRATION)
	struct PS_CALI_DATA_STRUCT ps_cali_temp;
#endif
	APS_LOG("ltr559 cmd = %d \n", cmd);
	switch (cmd)
	{
		case ALSPS_SET_PS_MODE:
			if (copy_from_user(&enable, ptr, sizeof(enable))) {
				err = -EFAULT;
				goto err_out;
			}
			
			if (enable) {
				if (err = ltr559_enable_ps(obj->client, true)) {
					APS_ERR("enable ps fail: %d\n", err);
					goto err_out;
				}
				set_bit(CMC_BIT_PS, &obj->enable);
			} else {
				if (err = ltr559_enable_ps(obj->client, false)) {
					APS_ERR("disable ps fail: %d\n", err);
					goto err_out;
				}
				clear_bit(CMC_BIT_PS, &obj->enable);
			}
			break;

		case ALSPS_GET_PS_MODE:
			enable = test_bit(CMC_BIT_PS, &obj->enable) ? (1) : (0);
			if (copy_to_user(ptr, &enable, sizeof(enable))) {
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_PS_DATA:
			if (err = ltr559_read_data_ps(obj->client, &obj->ps)) {
				goto err_out;
			}
			dat = ltr559_get_ps_value(obj, obj->ps);
			if (copy_to_user(ptr, &dat, sizeof(dat))) {
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_PS_RAW_DATA:
			if (err = ltr559_read_data_ps(obj->client, &obj->ps)) {
				goto err_out;
			}

			dat = obj->ps & 0x80;
			if (copy_to_user(ptr, &dat, sizeof(dat))) {
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_SET_ALS_MODE:
			if (copy_from_user(&enable, ptr, sizeof(enable))) {
				err = -EFAULT;
				goto err_out;
			}
			
			if (enable) {
				if (err = ltr559_enable_als(obj->client, true)) {
					APS_ERR("enable als fail: %d\n", err);
					goto err_out;
				}
				set_bit(CMC_BIT_ALS, &obj->enable);
			} else {
				if (err = ltr559_enable_als(obj->client, false)) {
					APS_ERR("disable als fail: %d\n", err);
					goto err_out;
				}
				clear_bit(CMC_BIT_ALS, &obj->enable);
			}
			break;

		case ALSPS_GET_ALS_MODE:
			enable = test_bit(CMC_BIT_ALS, &obj->enable) ? (1) : (0);
			if (copy_to_user(ptr, &enable, sizeof(enable))) {
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_ALS_DATA:
			if (err = ltr559_read_data_als(obj->client, &obj->als)) {
				goto err_out;
			}

			dat = obj->als;
			if (copy_to_user(ptr, &dat, sizeof(dat))) {
				err = -EFAULT;
				goto err_out;
			}
			break;

		case ALSPS_GET_ALS_RAW_DATA:
			if (err = ltr559_read_data_als(obj->client, &obj->als)) {
				goto err_out;
			}

			dat = obj->als;	// & 0x3f;//modified by mayq
			if (copy_to_user(ptr, &dat, sizeof(dat))) {
				err = -EFAULT;
				goto err_out;
			}
			break;
			
			//Gionee mali add calibration for ltr559 2012-9-28 begin
#if defined(GN_PS_NEED_CALIBRATION)
		case ALSPS_SET_PS_CALI:
			/*case ALSPS_SET_PS_CALI:*/
			dat = (void __user*)arg;
			if (dat == NULL) {
				APS_LOG("dat == NULL\n");
				err = -EINVAL;
				break;    
			}

			if (copy_from_user(&ps_cali_temp, dat, sizeof(ps_cali_temp))) {
				APS_LOG("copy_from_user\n");
				err = -EFAULT;
				break;    
			}

			ltr559_WriteCalibration(&ps_cali_temp);
			APS_LOG("111111  ALSPS_SET_PS_CALI %d,%d,%d\t",ps_cali_temp.close, ps_cali_temp.far_away,ps_cali_temp.valid);

			break;

		case ALSPS_GET_PS_CALI:

			if (err = ltr559_enable_ps(obj->client, true)){
				APS_ERR("ltr559 ioctl enable ps fail: %d\n", err);
			}

			msleep(200);

			err = ltr559_read_data_for_cali(obj->client, &ps_cali_temp);
			if (err){
				goto err_out;
			}

			if (err = ltr559_enable_ps(client, false)) {
				APS_ERR("ltr559 ioctl disable ps fail: %d\n", err);
			}

			if (copy_to_user(ptr, &ps_cali_temp, sizeof(ps_cali_temp))) {
				err = -EFAULT;
				goto err_out;
			}              
			APS_LOG("ltr559 ALSPS_GET_PS_CALI %d,%d,%d\t",ps_cali_temp.close, ps_cali_temp.far_away,ps_cali_temp.valid);
			break;
#endif
			//Gionee mali add calibration for ltr559 2012-9-28 end
		default:
			APS_ERR("%s not supported = 0x%04x", __FUNCTION__, cmd);
			err = -ENOIOCTLCMD;
			break;
	}

err_out:
	return err;
}
/*----------------------------------------------------------------------------*/
static struct file_operations ltr559_fops = {
	.owner = THIS_MODULE,
	.open = ltr559_open,
	.release = ltr559_release,
	.unlocked_ioctl = ltr559_unlocked_ioctl,
};
/*----------------------------------------------------------------------------*/
static struct miscdevice ltr559_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "als_ps",
	.fops = &ltr559_fops,
};
/*----------------------------------------------------------------------------*/
static int ltr559_i2c_suspend(struct i2c_client *client, pm_message_t msg)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int err;
	APS_FUN();

	if (msg.event == PM_EVENT_SUSPEND)
	{
		if (!obj)
		{
			APS_ERR("null pointer!!\n");
			return -EINVAL;
		}
		atomic_set(&obj->als_suspend, 1);
		if (err = ltr559_enable_als(client, false))
		{
			APS_ERR("disable als: %d\n", err);
			return err;
		}

		//Gionee yanggy 2012-07-25 add for psensor interrupt mode begin
		/*==========this part is necessary when use polling mode==========*/
#ifndef GN_MTK_BSP_ALSPS_INTERRUPT_MODE
		atomic_set(&obj->ps_suspend, 1);
		if (err = ltr559_enable_ps(client, false))
		{
			APS_ERR("disable ps: %d\n", err);
			return err;
		}
#endif
		//Gionee yanggy 2012-07-25 add for psensor interrupt mode end
	}
	return 0;
}
/*----------------------------------------------------------------------------*/
static int ltr559_i2c_resume(struct i2c_client *client)
{
	struct ltr559_priv *obj = i2c_get_clientdata(client);
	int err;
	APS_FUN();

	if (!obj)
	{
		APS_ERR("null pointer!!\n");
		return -EINVAL;
	}

	atomic_set(&obj->als_suspend, 0);
	if (test_bit(CMC_BIT_ALS, &obj->enable))
	{
		if (err = ltr559_enable_als(client, true))
		{
			APS_ERR("enable als fail: %d\n", err);
		}
	}

	//Gionee yanggy 2012-07-25 add for psensor interrupt mode begin
	/*==========this part is necessary when use polling mode======*/
#ifndef GN_MTK_BSP_ALSPS_INTERRUPT_MODE
	atomic_set(&obj->ps_suspend, 0);
	if (test_bit(CMC_BIT_PS, &obj->enable))
	{
		if (err = ltr559_enable_ps(client, true))
		{
			APS_ERR("enable ps fail: %d\n", err);
		}
	}
#endif
	//Gionee yanggy 2012-07-25 add for psensor interrupt mode

	return 0;
}
/*----------------------------------------------------------------------------*/
static void ltr559_early_suspend(struct early_suspend *h)
{
	/*early_suspend is only applied for ALS*/
	struct ltr559_priv *obj = container_of(h, struct ltr559_priv, early_drv);
	int err;
	APS_FUN();

	if (!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}

	atomic_set(&obj->als_suspend, 1);
	if (err = ltr559_enable_als(obj->client, false))
	{
		APS_ERR("disable als fail: %d\n", err);
	}
}
/*----------------------------------------------------------------------------*/
static void ltr559_late_resume(struct early_suspend *h)
{
	/*early_suspend is only applied for ALS*/
	struct ltr559_priv *obj = container_of(h, struct ltr559_priv, early_drv);
	int err;
	APS_FUN();

	if (!obj)
	{
		APS_ERR("null pointer!!\n");
		return;
	}

	atomic_set(&obj->als_suspend, 0);
	if (test_bit(CMC_BIT_ALS, &obj->enable))
	{
		if (err = ltr559_enable_als(obj->client, true))
		{
			APS_ERR("enable als fail: %d\n", err);
		}
	}

}

int ltr559_ps_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct ltr559_priv *obj = (struct ltr559_priv *)self;

	//APS_FUN(f);
	APS_LOG("ltr559_ps_operate command:%d\n",command);
	switch (command)
	{
		case SENSOR_DELAY:
			if ((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if ((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;
				if (value)
				{
					if (err = ltr559_enable_ps(obj->client, true))
					{
						APS_ERR("enable ps fail: %d\n", err);
						return -1;
					}
					set_bit(CMC_BIT_PS, &obj->enable);
				}
				else
				{
					if (err = ltr559_enable_ps(obj->client, false))
					{
						APS_ERR("disable ps fail: %d\n", err);
						return -1;
					}
					clear_bit(CMC_BIT_PS, &obj->enable);
				}
			}
			break;

		case SENSOR_GET_DATA:
			if ((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data))) {
				APS_ERR("get sensor data parameter error!\n");
				err = -EINVAL;
			} else {
				sensor_data = (hwm_sensor_data *)buff_out;
				if (err = ltr559_read_data_ps(obj->client, &obj->ps)) {
					APS_ERR("SENSOR_GET_DATA^^^^^^^^^^!\n");
					err = -1;
					break;
				} else { 
					while(-1 == ltr559_get_ps_value(obj, obj->ps)) {
						ltr559_read_data_ps(obj->client, &obj->ps);
						msleep(50);
					}
					sensor_data->values[0] = ltr559_get_ps_value(obj, obj->ps);
					sensor_data->value_divide = 1;
					sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
					APS_LOG("fwq get ps data =%d\n",sensor_data->values[0]);
				}
			}
			break;
		default:
			APS_ERR("proxmy sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}

	return err;
}

int ltr559_als_operate(void* self, uint32_t command, void* buff_in, int size_in,
		void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	hwm_sensor_data* sensor_data;
	struct ltr559_priv *obj = (struct ltr559_priv *)self;

	switch (command)
	{
		case SENSOR_DELAY:
			if ((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Set delay parameter error!\n");
				err = -EINVAL;
			}
			// Do nothing
			break;

		case SENSOR_ENABLE:
			if ((buff_in == NULL) || (size_in < sizeof(int)))
			{
				APS_ERR("Enable sensor parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				value = *(int *)buff_in;
				if (value)
				{
					if (err = ltr559_enable_als(obj->client, true))
					{
						APS_ERR("enable als fail: %d\n", err);
						return -1;
					}
					set_bit(CMC_BIT_ALS, &obj->enable);
				}
				else
				{
					if (err = ltr559_enable_als(obj->client, false))
					{
						APS_ERR("disable als fail: %d\n", err);
						return -1;
					}
					clear_bit(CMC_BIT_ALS, &obj->enable);
				}
			}
			break;

		case SENSOR_GET_DATA:
			if ((buff_out == NULL) || (size_out< sizeof(hwm_sensor_data)))
			{
				APS_ERR("ltr559_als_operate get sensor data parameter error!\n");
				err = -EINVAL;
			}
			else
			{
				sensor_data = (hwm_sensor_data *)buff_out;

				if (err = ltr559_read_data_als(obj->client, &obj->als))
				{
					err = -1;;
				}
				else
				{
					while(-1 == ltr559_get_als_value(obj, obj->als))
					{
						ltr559_read_data_als(obj->client, &obj->als);
						msleep(50);
					}

					ltr559_read_data_ps(obj->client, &obj->ps);
					ltr559_get_ps_value(obj, obj->ps);
					sensor_data->values[0] = ltr559_get_als_value(obj, obj->als);
					sensor_data->value_divide = 1;
					sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
					APS_LOG("ltr559_als_operate get als data =%d\n",sensor_data->values[0]);
				}
			}
			break;
		default:
			APS_ERR("light sensor operate function no this parameter %d!\n", command);
			err = -1;
			break;
	}

	return err;
}


/*----------------------------------------------------------------------------*/
static int ltr559_i2c_detect(struct i2c_client *client, int kind, struct i2c_board_info *info)
{
	APS_FUN();
	strcpy(info->type, LTR559_DEV_NAME);
	return 0;
}

/*----------------------------------------------------------------------------*/
static int ltr559_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct ltr559_priv *obj;
	struct hwmsen_object obj_ps, obj_als;
	int err = 0;
	APS_FUN();

	if (!(obj = kzalloc(sizeof(*obj), GFP_KERNEL))) {
		err = -ENOMEM;
		goto exit;
	}
	memset(obj, 0, sizeof(*obj));
	ltr559_obj = obj;

	obj->hw = get_cust_alsps_hw_ltr559();
	ltr559_obj->polling = obj->hw->polling_mode;

#ifdef GN_MTK_BSP_ALSPS_INTERRUPT_MODE
	INIT_DELAYED_WORK(&obj->eint_work, ltr559_eint_work);
#endif
	obj->client = client;
	i2c_set_clientdata(client, obj);
	atomic_set(&obj->als_debounce, 1000);
	atomic_set(&obj->als_deb_on, 0);
	atomic_set(&obj->als_deb_end, 0);
	atomic_set(&obj->ps_debounce, 200);
	atomic_set(&obj->als_suspend, 0);

	obj->ps_enable = 0;
	obj->als_enable = 0;
	obj->enable = 0;
	obj->pending_intr = 0;
	obj->als_level_num = sizeof(obj->hw->als_level)/sizeof(obj->hw->als_level[0]);
	obj->als_value_num = sizeof(obj->hw->als_value)/sizeof(obj->hw->als_value[0]);
	BUG_ON(sizeof(obj->als_level) != sizeof(obj->hw->als_level));
	memcpy(obj->als_level, obj->hw->als_level, sizeof(obj->als_level));
	BUG_ON(sizeof(obj->als_value) != sizeof(obj->hw->als_value));
	memcpy(obj->als_value, obj->hw->als_value, sizeof(obj->als_value));
	atomic_set(&obj->i2c_retry, 3);
	//pre set ps threshold
	obj->ps_thd_val = obj->hw->ps_threshold;
	//pre set window loss
	obj->als_widow_loss = obj->hw->als_window_loss;

	ltr559_i2c_client = client;

	if (err = ltr559_init_client(client)) {
		goto exit_init_failed;
	}

	if (err = misc_register(&ltr559_device)) {
		APS_ERR("ltr559_device register failed\n");
		goto exit_misc_device_register_failed;
	}

#ifdef MTK_AUTO_DETECT_ALSPS
	if (err = ltr559_create_attr(&(ltr559_init_info.platform_diver_addr->driver))) {
		APS_ERR("create attribute err = %d\n", err);
		goto exit_create_attr_failed;
	}
#else
	if (err = ltr559_create_attr(&ltr559_alsps_driver.driver)) {
		APS_ERR("create attribute err = %d\n", err);
		goto exit_create_attr_failed;
	}
#endif
	//Gionee yanggy 2012-07-25 modify for psensor compability end
	/*---------------ps_obj init-----------------*/
	obj_ps.self = ltr559_obj;
	APS_LOG("obj->hw->polling_mode:%d\n",obj->hw->polling_mode);
	if (1 == obj->hw->polling_mode_ps) {
		obj_ps.polling = 1;
	} else {
		obj_ps.polling = 0;//interrupt mode
	}
	obj_ps.sensor_operate = ltr559_ps_operate;
	if (err = hwmsen_attach(ID_PROXIMITY, &obj_ps)) {
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}

	/*-------------als_obj init---------------- */
	obj_als.self = ltr559_obj;
	if (1 == obj->hw->polling_mode_als) {
		obj_als.polling = 1;
		APS_LOG("polling mode\n");
	} else {
		obj_als.polling = 0;//interrupt mode
		APS_LOG("interrupt mode\n");
	}
	obj_als.sensor_operate = ltr559_als_operate;
	if (err = hwmsen_attach(ID_LIGHT, &obj_als)) {
		APS_ERR("attach fail = %d\n", err);
		goto exit_create_attr_failed;
	}

#if defined(CONFIG_HAS_EARLYSUSPEND)
	obj->early_drv.level	= EARLY_SUSPEND_LEVEL_DISABLE_FB - 1,
		obj->early_drv.suspend = ltr559_early_suspend,
		obj->early_drv.resume = ltr559_late_resume,
		register_early_suspend(&obj->early_drv);
#endif

	APS_LOG("%s: OK\n", __func__);
	ltr559_init_flag = 0;
	return 0;

exit_create_attr_failed:
	misc_deregister(&ltr559_device);
exit_misc_device_register_failed:
exit_init_failed:
	//i2c_detach_client(client);
exit_kfree:
	kfree(obj);
exit:
	ltr559_i2c_client = NULL;
	APS_ERR("%s: err = %d\n", __func__, err);

	ltr559_init_flag = -1;
	return err;
}
/*----------------------------------------------------------------------------*/
static int ltr559_i2c_remove(struct i2c_client *client)
{
	int err;

	//Gionee yanggy 2012-07-25 modify for psensor compability begin
#ifdef MTK_AUTO_DETECT_ALSPS
	if (err = ltr559_delete_attr(&(ltr559_init_info.platform_diver_addr->driver)))
		APS_ERR("ltr559_delete_attr fail: %d\n", err);
#else
	if (err = ltr559_delete_attr(&ltr559_alsps_driver.driver))
		APS_ERR("ltr559_delete_attr fail: %d\n", err);
#endif
	//Gionee yanggy 2012-07-25 modify for psensor compability end
	if (err = misc_deregister(&ltr559_device))
		APS_ERR("misc_deregister fail: %d\n", err);

	ltr559_i2c_client = NULL;
	i2c_unregister_device(client);
	kfree(i2c_get_clientdata(client));

	return 0;
}

static struct i2c_driver ltr559_i2c_driver = {
	.probe		= ltr559_i2c_probe,
	.remove		= ltr559_i2c_remove,
	.detect		= ltr559_i2c_detect,
	.suspend	= ltr559_i2c_suspend,
	.resume		= ltr559_i2c_resume,
	.id_table	= ltr559_i2c_id,
	.driver = {
		.owner	= THIS_MODULE,
		.name	= LTR559_DEV_NAME,
	},
};

#ifdef MTK_AUTO_DETECT_ALSPS
static int ltr559_local_init(void)
{
	struct acc_hw *hw = get_cust_alsps_hw_ltr559();
	ltr559_power(hw, 1);
	if (i2c_add_driver(&ltr559_i2c_driver)) {
		APS_ERR("add driver error\n");
		return -1;
	}
	if (-1 == ltr559_init_flag)
		return -1;

	return 0;
}
#else
static int ltr559_probe(struct platform_device *pdev)
{
	struct alsps_hw *hw = get_cust_alsps_hw_ltr559();      
	ltr559_power(hw, 1);
	if (i2c_add_driver(&ltr559_i2c_driver)) {
		APS_ERR("i2c_add_driver add driver error %d\n",__LINE__);
		return -1;
	}
	return 0;
}
#endif
//Gionee yanggy 2012-07-23 modify for psensor compability end
/*----------------------------------------------------------------------------*/
static int ltr559_remove(void)
{
	struct alsps_hw *hw = get_cust_alsps_hw_ltr559();
	APS_FUN();
	ltr559_power(hw, 0);
	i2c_del_driver(&ltr559_i2c_driver);
	return 0;
}

#ifndef MTK_AUTO_DETECT_ALSPS
static struct platform_driver ltr559_alsps_driver = {
	.probe	= ltr559_probe,
	.remove	= ltr559_remove,
	.driver	= {
		.name	= "als_ps",
	}
};
#endif

static int __init ltr559_init(void)
{
	APS_FUN();
	struct alsps_hw *hw = get_cust_alsps_hw_ltr559();
	APS_LOG("%s: i2c_number=%d\n", __func__,hw->i2c_num);
	i2c_register_board_info(hw->i2c_num, &i2c_ltr559, 1);
#ifdef MTK_AUTO_DETECT_ALSPS
	if (hwmsen_psensor_add(&ltr559_init_info)) {
		APS_ERR("ltr559_init failed to register driver\n");
		return -ENODEV;
	}
#else
	if (platform_driver_register(&ltr559_alsps_driver)) {
		APS_ERR("ltr559_init failed to register driver\n");
		return -ENODEV;
	}
#endif
	return 0;
}

static void __exit ltr559_exit(void)
{
	APS_FUN();
#ifdef MTK_AUTO_DETECT_ALSPS
	hwmsen_psensor_del(&ltr559_init_info);
#else
	platform_driver_unregister(&ltr559_alsps_driver);
#endif
}

module_init(ltr559_init);
module_exit(ltr559_exit);

MODULE_AUTHOR("Chen Qiyan(chenqy@gionee.com");
MODULE_DESCRIPTION("LTR559 light sensor & p sensor driver");
MODULE_LICENSE("GPL");
