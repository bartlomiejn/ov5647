/*
 * Copyright (C) 2023 Bartlomiej Nowak <bartlomiej.nwk@gmail.com>
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
#include <media/tegra-v4l2-camera.h>
#include <media/tegracam_core.h>

struct ov5647 
{
        struct i2c_client *i2c_client;
        struct v4l2_subdev *subdev;
};

static const struct of_device_id ov5647_of_match[] = {
        { .compatible = "nv,ov5647", },
        {},
};
MODULE_DEVICE_TABLE(of, ov5647_of_match);

static struct i2c_driver ov5647_i2c_driver = {
    .driver = {
        .name = "ov5647",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(ov5647_of_match),
    },
    .id_table = ov5647_id,
    .probe = ov5647_probe,
    .remove = ov5647_remove,
};

module_i2c_driver(ov5647_i2c_driver);

MODULE_VERSION("0.1");
MODULE_DESCRIPTION("OmniVision OV5647 driver for Jetson Nano");
MODULE_AUTHOR("Bartlomiej Nowak <bartlomiej.nwk@gmail.com>");
MODULE_LICENSE("GPL v3");
