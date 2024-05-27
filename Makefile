include config.mk

GRUB_CFG_FILE := $(GRUB_DIR)/grub/grub.cfg

.PHONY: run debug build kernel setup clean

run: $(OS_FILE)
	$(QEMU) $(QFLAGS) -cdrom $<

debug: $(OS_FILE)
	$(QEMU) -s -S $(QFLAGS) -cdrom $<

build: $(OS_FILE)
$(OS_FILE): kernel
	@if grub-file --is-x86-multiboot2 '$(KERNEL_FILE)'; then \
		echo "\n========== Compiling $@ =========="; \
		cp $(KERNEL_FILE) $(GRUB_DIR); \
		grub-mkrescue /usr/lib/grub/i386-pc -o $@ iso; \
	else \
		echo "Verification for $(KERNEL_FILE) failed, it is an invalid multiboot2 file."; \
	fi	

kernel:
	@$(MAKE) -C $(KERNEL_DIR) build

setup:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OUTPUT_DIR)

	mkdir -p iso
	mkdir -p $(GRUB_DIR)
	mkdir -p $(GRUB_DIR)/grub
	cp $(RESOURCES_DIR)/* $(GRUB_DIR)

	@echo "set timeout=0" > $(GRUB_CFG_FILE)
	@echo "set default=0" >> $(GRUB_CFG_FILE)
	@echo "menuentry '$(OS_NAME)' {" >> $(GRUB_CFG_FILE)
	@echo "	multiboot2 /boot/kernel.bin $(OS_CFG)" >> $(GRUB_CFG_FILE)
	@echo "	module2 /boot/default8x16.psfu default_font" >> $(GRUB_CFG_FILE)
	@echo "	boot" >> $(GRUB_CFG_FILE)
	@echo "}" >> $(GRUB_CFG_FILE) 
	
clean:
	@$(MAKE) -C $(KERNEL_DIR) clean