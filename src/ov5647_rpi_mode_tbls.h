

struct ov5647_mode {
	struct v4l2_mbus_framefmt format;
	struct v4l2_rect crop;
	u64 pixel_rate;
	int	hts;
	int	vts;
	const struct regval_list *reg_list;
	unsigned int num_regs;
};

static const struct ov5647_mode ov5647_modes[] = {
	/* 2592x1944 full resolution full FOV 10-bit mode. */
	{
		.format = {
			.code		= MEDIA_BUS_FMT_SBGGR10_1X10,
			.colorspace	= V4L2_COLORSPACE_RAW,
			.field		= V4L2_FIELD_NONE,
			.width		= 2592,
			.height		= 1944
		},
		.crop = {
			.left		= OV5647_PIXEL_ARRAY_LEFT,
			.top		= OV5647_PIXEL_ARRAY_TOP,
			.width		= 2592,
			.height		= 1944
		},
		.pixel_rate	= 87500000,
		.hts		= 2844,
		.vts		= 0x7b0,
		.reg_list	= ov5647_2592x1944_10bpp,
		.num_regs	= ARRAY_SIZE(ov5647_2592x1944_10bpp)
	},
	/* 1080p30 10-bit mode. Full resolution centre-cropped down to 1080p. */
	{
		.format = {
			.code		= MEDIA_BUS_FMT_SBGGR10_1X10,
			.colorspace	= V4L2_COLORSPACE_RAW,
			.field		= V4L2_FIELD_NONE,
			.width		= 1920,
			.height		= 1080
		},
		.crop = {
			.left		= 348 + OV5647_PIXEL_ARRAY_LEFT,
			.top		= 434 + OV5647_PIXEL_ARRAY_TOP,
			.width		= 1928,
			.height		= 1080,
		},
		.pixel_rate	= 81666700,
		.hts		= 2416,
		.vts		= 0x450,
		.reg_list	= ov5647_1080p30_10bpp,
		.num_regs	= ARRAY_SIZE(ov5647_1080p30_10bpp)
	},
	/* 2x2 binned full FOV 10-bit mode. */
	{
		.format = {
			.code		= MEDIA_BUS_FMT_SBGGR10_1X10,
			.colorspace	= V4L2_COLORSPACE_RAW,
			.field		= V4L2_FIELD_NONE,
			.width		= 1296,
			.height		= 972
		},
		.crop = {
			.left		= OV5647_PIXEL_ARRAY_LEFT,
			.top		= OV5647_PIXEL_ARRAY_TOP,
			.width		= 2592,
			.height		= 1944,
		},
		.pixel_rate	= 81666700,
		.hts		= 1896,
		.vts		= 0x59b,
		.reg_list	= ov5647_2x2binned_10bpp,
		.num_regs	= ARRAY_SIZE(ov5647_2x2binned_10bpp)
	},
	/* 10-bit VGA full FOV 60fps. 2x2 binned and subsampled down to VGA. */
	{
		.format = {
			.code		= MEDIA_BUS_FMT_SBGGR10_1X10,
			.colorspace	= V4L2_COLORSPACE_RAW,
			.field		= V4L2_FIELD_NONE,
			.width		= 640,
			.height		= 480
		},
		.crop = {
			.left		= 16 + OV5647_PIXEL_ARRAY_LEFT,
			.top		= OV5647_PIXEL_ARRAY_TOP,
			.width		= 2560,
			.height		= 1920,
		},
		.pixel_rate	= 55000000,
		.hts		= 1852,
		.vts		= 0x1f8,
		.reg_list	= ov5647_640x480_10bpp,
		.num_regs	= ARRAY_SIZE(ov5647_640x480_10bpp)
	},
};

/* Default sensor mode is 1080p30fps */
#define OV5647_DEFAULT_MODE		(&ov5647_modes[1])
#define OV5647_DEFAULT_FORMAT 	(ov5647_modes[1].format)
