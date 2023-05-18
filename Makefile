OS_NAME := MyOS
GRUB_DIR := iso/boot
GRUB_CONFIG_FILE := $(GRUB_DIR)/grub/grub.cfg
KERNEL_BIN_NAME := kernel.bin

export OBJ_DIR := obj
export OUTPUT_DIR := output
export INCLUDE_DIR := include

export ASM := nasm
export CC := x86_64-linux-gnu-gcc
export LD := x86_64-linux-gnu-ld
export CFLAGS := -ffreestanding -nostdlib -mno-red-zone -fpic
export AFLAGS := -f elf64
export LDFLAGS := -nostdlib

export KERNEL_BIN := $(OUTPUT_DIR)/$(KERNEL_BIN_NAME)
export OS_ISO := $(OUTPUT_DIR)/os.iso

QEMU := qemu-system-x86_64
QFLAGS := -m 2G -rtc base=localtime

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
$(OS_ISO): build_config_file kernel
	@if grub-file --is-x86-multiboot2 '$(KERNEL_BIN)'; then \
		echo "\n========== Compiling $@ =========="; \
		cp $(KERNEL_BIN) $(GRUB_DIR); \
		grub-mkrescue /usr/lib/grub/i386-pc -o $@ iso; \
	else \
		echo "Verification for $(KERNEL_BIN) failed, it is an invalid multiboot2 file."; \
	fi

kernel:
	@$(MAKE) -C src/kernel build

setup:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OUTPUT_DIR)

build_config_file:
	@mkdir -p iso
	@mkdir -p $(GRUB_DIR)
	@mkdir -p $(GRUB_DIR)/grub
	
	@echo "========== Generating GRUB Config file =========="
	@echo "set timeout=0" > $(GRUB_CONFIG_FILE)
	@echo "set default=0" >> $(GRUB_CONFIG_FILE)
	@echo "menuentry '$(OS_NAME)' {" >> $(GRUB_CONFIG_FILE)
	@echo "	multiboot2 /boot/$(KERNEL_BIN_NAME)" >> $(GRUB_CONFIG_FILE)
	@echo "	boot" >> $(GRUB_CONFIG_FILE)
	@echo "}" >> $(GRUB_CONFIG_FILE)

clean:
	@rm -rf $(OBJ_DIR)/*