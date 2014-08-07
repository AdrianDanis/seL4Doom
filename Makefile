
lib-dirs:=libs

# The main target we want to generate
all: app-images

simulate-qemu:
	qemu-system-i386 -m 512 -kernel images/kernel-ia32-pc99 -initrd images/doom-image-ia32-pc99 -serial stdio -vga std

-include .config

include tools/common/project.mk

