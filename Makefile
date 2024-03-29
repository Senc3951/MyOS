OS_NAME := MyOS
GRUB_DIR := iso/boot
GRUB_CONFIG_FILE := $(GRUB_DIR)/grub/grub.cfg
KERNEL_BIN_NAME := kernel.bin
LOG_FILE := serial.log

export OBJ_DIR := obj
export OUTPUT_DIR := output
export INCLUDE_DIR := include
export LIBC_DIR := libc
export LIBC_OBJ_DIR := $(OBJ_DIR)/$(LIBC_DIR)

export ASM := nasm
export CC := x86_64-linux-gnu-gcc
export LD := x86_64-linux-gnu-ld
export CFLAGS := -ffreestanding -nostdlib -mno-red-zone -fpic
export AFLAGS := -f elf64
export LDFLAGS := -nostdlib

export KERNEL_BIN := $(OUTPUT_DIR)/$(KERNEL_BIN_NAME)
export OS_ISO := $(OUTPUT_DIR)/os.iso

LIBC_SRC := $(wildcard *.c $(foreach fd, $(LIBC_DIR), $(fd)/*.c))
LIBC_OBJ := $(addprefix $(OBJ_DIR)/, $(LIBC_SRC:c=o))

QEMU := qemu-system-x86_64
QFLAGS := -m 2G -rtc base=localtime -chardev stdio,id=char0,logfile=$(LOG_FILE),signal=off \
  -serial chardev:char0

.PHONY: run debug build setup build_config_file clean

run: $(OS_ISO)
	@echo "========== Running $< =========="
	$(QEMU) $(QFLAGS) -cdrom $<

debug: QFLAGS += -s -S
debug: CFLAGS += -g
debug: AFLAGS += -g
debug: LDFLAGS += -g
debug: clean run

build: $(OS_ISO)
$(OS_ISO): build_config_file libc kernel
	@if grub-file --is-x86-multiboot2 '$(KERNEL_BIN)'; then \
		echo "\n========== Compiling $@ =========="; \
		cp $(KERNEL_BIN) $(GRUB_DIR); \
		grub-mkrescue /usr/lib/grub/i386-pc -o $@ iso; \
	else \
		echo "Verification for $(KERNEL_BIN) failed, it is an invalid multiboot2 file."; \
	fi

kernel:
	@$(MAKE) -C src/kernel build

libc: $(LIBC_OBJ)
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

setup:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OUTPUT_DIR)

build_config_file:
	@mkdir -p $(LIBC_OBJ_DIR)
	@mkdir -p iso
	@mkdir -p $(GRUB_DIR)
	@mkdir -p $(GRUB_DIR)/grub
	
	@echo "========== Generating GRUB Config file ==========\n"
	@echo "set timeout=0" > $(GRUB_CONFIG_FILE)
	@echo "set default=0" >> $(GRUB_CONFIG_FILE)
	@echo "menuentry '$(OS_NAME)' {" >> $(GRUB_CONFIG_FILE)
	@echo "	multiboot2 /boot/$(KERNEL_BIN_NAME)" >> $(GRUB_CONFIG_FILE)
	@echo "	boot" >> $(GRUB_CONFIG_FILE)
	@echo "}" >> $(GRUB_CONFIG_FILE)

clean:
	@rm -rf $(OBJ_DIR)/*