OPT			:= -O1
DEBUG		:= true
OS_NAME		:= EpsilonOS
OS_CFG		:= name=$(OS_NAME)

CC      	:= gcc
CXX     	:= g++
AS      	:= nasm
LD      	:= ld
QEMU    	:= qemu-system-i386

CFLAGS      := -m32 $(OPT)
CXXFLAGS    := 
AFLAGS      := -felf32 $(OPT)
LFLAGS		:= $(OPT)
QFLAGS		:= -machine q35 -cpu max -m 1G -d cpu_reset -rtc base=localtime -net none \
	-serial stdio

ifeq ($(DEBUG), true)
	CFLAGS += -ggdb -D DEBUG
	AFLAGS += -g
	LFLAGS += -g
endif

KERNEL_DIR		:= kernel
OBJ_DIR			:= obj
OUTPUT_DIR		:= output
GRUB_DIR		:= iso/boot
RESOURCES_DIR	:= resources

OS_FILE		:= $(OUTPUT_DIR)/$(OS_NAME).iso
KERNEL_FILE	:= $(OUTPUT_DIR)/kernel.bin