/* 
 * OmniVision OV5647 V4L2 Tegra driver
 * Copyright (C) 2023 Bartlomiej Nowak <bartlomiej.nwk@gmail.com>
 *
 * Based on:
 * Copyright (c) 2015-2019, NVIDIA CORPORATION, All Rights Reserved.
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

#ifndef __OV5647_H__
#define __OV5647_H__

#include <media/nvc.h>
#include <uapi/media/nvc_image.h>

// TODO:
// #include <uapi/media/ov5647.h>

#define OV5647_OTP_LOAD_CTRL_ADDR 0x3D21
#define OV5647_OTP_PROGRAM_CTRL_ADDR 0x3D20
#define OV5647_OTP_BANK_START_ADDR 0x3D00
#define OV5647_OTP_BANK_END_ADDR 0x3D1F

static const char * const ov5647_regulators[] = {
	"avdd",		/* Analog power */
	"dovdd",	/* Digital I/O power */
	"dvdd",		/* Digital core power */
};
#define OV5647_REGULATORS_LEN ARRAY_SIZE(ov5647_regulators)

#endif  /* __OV5647_H__ */
