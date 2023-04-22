if VIDEO_V4L2

menu "Camera device support for Jetson Nano"
	visible if !MEDIA_SUBDRV_AUTOSELECT || COMPILE_TEST

config VIDEO_OV5647
	tristate "OmniVision OV5647 Camera device support"
	depends on I2C && VIDEO_V4L2 && VIDEO_V4L2_SUBDEV_API
	---help---
	  V4L2 Driver for the OmniVision OV5647 camera.

endmenu

endif
