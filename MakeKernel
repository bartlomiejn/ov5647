CROSS_COMPILE=$(HOME)/Projects/toolchains/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu
MAKE_INV=make -C kernel/kernel-4.9/ ARCH=arm64 O=build LOCALVERSION=-tegra CROSS_COMPILE=$(CROSS_COMPILE) --output-sync=target 
DTB=tegra210-p3448-0000-p3449-0000-a02.dtb

kernel:
	$(MAKE_INV) zImage
	$(MAKE_INV) modules
	$(MAKE_INV) dtbs
