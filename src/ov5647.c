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

#define ASSERT_KZMALLOC(SYMBOL) \
	if (!SYMBOL) \
	{ \
		dev_err(dev, "%s: kzalloc SYMBOL failed, no memory available.\n", \ 
				__func__); \
		return -ENOMEM; \
	}

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

static int ov5647_probe(struct i2c_client *client,
						const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct device_node *node = client->dev.of_node;
	struct ov5647 *dev;
	struct tegracam_device *tc_dev;
	int ret;

	if (IS_ENABLED(CONFIG_OF) || !node)
	{
		dev_err(dev, "%s: CONFIG_OF not set or device_node not available."
			    "Exiting.\n");
		return -EINVAL;
	} 

	dev_info(dev, "%s: device name: %s init_name: %s node: %p\n", __func__,
			 client->name, dev->init_name, node);

	dev = devm_kzalloc(dev, sizeof(struct ov5647), GFP_KERNEL);
	ASSERT_KZMALLOC(dev);

	dev_info(dev, "%s: kzalloc success\n", __func__);

	return 0;
}

static int ov5647_remove(struct i2c_client *client)
{
	return 0;
}

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
MODULE_DESCRIPTION("OmniVision OV5647 driver for Tegra");
MODULE_AUTHOR("Bartlomiej Nowak <bartlomiej.nwk@gmail.com>");
MODULE_LICENSE("GPL v3");
