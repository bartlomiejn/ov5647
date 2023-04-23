/* 
 * OmniVision OV5647 V4L2 Tegra driver
 * Copyright (C) 2023 Bartlomiej Nowak <bartlomiej.nwk@gmail.com>
 *
 * Based on: 
 * Raspberry PI OV5674 Camera Driver
 * Copyright (C) 2016, Synopsys, Inc
 *
 * Samsung S5K6AAFX SXGA 1/6" 1.3M CMOS Image Sensor driver
 * Copyright (C) 2011 Sylwester Nawrocki <s.nawrocki@samsung.com>
 *
 * Omnivision OV7670 Camera Driver
 * Copyright (C) 2006-7 Jonathan Corbet <corbet@lwn.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */ 

#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/clk.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <media/tegra-v4l2-camera.h>
#include <media/tegracam_core.h>
#include <media/ov5647.h>
#include <media/ov5647_mode_tbls.h>

#define OV5647_VERSION "0.1"
#define OV5647_NAME "ov5647"
#define OV5647_OF_COMPAT "nv,ov5647"

struct ov5647 
{
	struct v4l2_subdev *subdev;

	struct camera_common_i2c i2c_dev;
	struct camera_common_data *s_data;
	struct tegracam_device *tc_dev;

	struct gpio_desc *pwdn;
	struct regulator_bulk_data supplies[OV5647_NUM_SUPPLIES];

	bool streaming;
};

static inline struct ov5647 *to_ov5647(struct v4l2_subdev *sd)
{
	return container_of(sd, struct ov5647, sd);
}

//
// Write/read 
//

static int ov5647_write16(struct v4l2_subdev *sd, u16 reg, u16 val)
{
	int ret;
	unsigned char data[4] = { reg >> 8, reg & 0xff, val >> 8, val & 0xff};
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	ret = i2c_master_send(client, data, 4);
	/*
	 * Writing the wrong number of bytes also needs to be flagged as an
	 * error. Success needs to produce a 0 return code.
	 */
	if (ret == 4) {
		ret = 0;
	} else {
		dev_dbg(&client->dev, "%s: i2c write error, reg: %x\n",
			__func__, reg);
		if (ret >= 0)
			ret = -EINVAL;
	}

	return ret;
}

static int ov5647_write(struct v4l2_subdev *sd, u16 reg, u8 val)
{
	int ret;
	unsigned char data[3] = { reg >> 8, reg & 0xff, val};
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	ret = i2c_master_send(client, data, 3);
	/*
	 * Writing the wrong number of bytes also needs to be flagged as an
	 * error. Success needs to produce a 0 return code.
	 */
	if (ret == 3) {
		ret = 0;
	} else {
		dev_dbg(&client->dev, "%s: i2c write error, reg: %x\n",
				__func__, reg);
		if (ret >= 0)
			ret = -EINVAL;
	}

	return ret;
}

static int ov5647_read(struct v4l2_subdev *sd, u16 reg, u8 *val)
{
	int ret;
	unsigned char data_w[2] = { reg >> 8, reg & 0xff };
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	ret = i2c_master_send(client, data_w, 2);
	/*
	 * A negative return code, or sending the wrong number of bytes, both
	 * count as an error.
	 */
	if (ret != 2) {
		dev_dbg(&client->dev, "%s: i2c write error, reg: %x\n",
			__func__, reg);
		if (ret >= 0)
			ret = -EINVAL;
		return ret;
	}

	ret = i2c_master_recv(client, val, 1);
	/*
	 * The only return value indicating success is 1. Anything else, even
	 * a non-negative value, indicates something went wrong.
	 */
	if (ret == 1) {
		ret = 0;
	} else {
		dev_dbg(&client->dev, "%s: i2c read error, reg: %x\n",
				__func__, reg);
		if (ret >= 0)
			ret = -EINVAL;
	}

	return ret;
}

static int ov5647_write_array(struct v4l2_subdev *sd,
				struct regval_list *regs, int array_size)
{
	int i, ret;

	for (i = 0; i < array_size; i++) {
		ret = ov5647_write(sd, regs[i].addr, regs[i].data);
		if (ret < 0)
			return ret;
	}

	return 0;
}

//
// Device
//

static int ov5647_probe(struct i2c_client *client,
						const struct i2c_device_id *id)
{
	return 0;
}

static int ov5647_remove(struct i2c_client *client)
{
	struct v4l2_subdev *sd = i2c_get_clientdata(client);
	struct ov5647 *ov5647 = to_ov5647(sd);

	v4l2_async_unregister_subdev(&ov5647->sd);
	media_entity_cleanup(&ov5647->sd.entity);
	v4l2_ctrl_handler_free(&ov5647->ctrls);
	v4l2_device_unregister_subdev(sd);
	mutex_destroy(&ov5647->lock);

	return 0;
}

#ifdef CONFIG_VIDEO_OV5647_DEBUG
static int ov5647_sensor_get_register(struct v4l2_subdev *sd,
									  struct v4l2_dbg_register *reg)
{
	u8 val;
	int ret;

	ret = ov5647_read(sd, reg->reg & 0xff, &val);
	if (ret < 0)
		return ret;

	reg->val = val;
	reg->size = 1;

	return 0;
}

static int ov5647_sensor_set_register(struct v4l2_subdev *sd,
									  const struct v4l2_dbg_register *reg)
{
	return ov5647_write(sd, reg->reg & 0xff, reg->val & 0xff);
}
#endif

static int ov5647_s_stream(struct v4l2_subdev *sd, int enable)
{
	struct ov5647 *state = to_ov5647(sd);
	int ret = 0;

	mutex_lock(&state->lock);

	if (enable)
		ret = ov5647_stream_on(sd);
	else
		ret = ov5647_stream_off(sd);

	mutex_unlock(&state->lock);
	return ret;
}

// 
// Operations
//

static const struct v4l2_subdev_core_ops ov5647_subdev_core_ops = {
	.s_power = ov5647_sensor_power,
#ifdef CONFIG_VIDEO_OV5647_DEBUG
	.g_register	= ov5647_sensor_get_register,
	.s_register	= ov5647_sensor_set_register,
#endif
	.subscribe_event = v4l2_ctrl_subdev_subscribe_event,
	.unsubscribe_event = v4l2_event_subdev_unsubscribe,
};

static const struct v4l2_subdev_video_ops ov5647_subdev_video_ops = {
	.s_stream =	ov5647_s_stream,
};

static const struct i2c_device_id ov5647_id[] = {
    { OV5647_NAME, 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, ov5647_id);

static const struct of_device_id ov5647_of_match[] = {
	{ .compatible = OV5647_OF_COMPAT, },
	{},
};
MODULE_DEVICE_TABLE(of, ov5647_of_match);

static struct i2c_driver ov5647_driver = {
	.driver = {
		.name = OV5647_NAME,
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(ov5647_of_match),
    },
	.probe = ov5647_probe,
	.remove = ov5647_remove,
	.id_table = ov5647_id,
};

module_i2c_driver(ov5647_driver);

MODULE_VERSION(OV5647_VERSION);
MODULE_DESCRIPTION("OmniVision OV5647 driver for Jetson Nano");
MODULE_AUTHOR("Bartlomiej Nowak <bartlomiej.nwk@gmail.com>");
MODULE_LICENSE("GPL v3");
