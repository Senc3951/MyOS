SUB_DIRS := boot dev/display io lib sys

OBJ_DIR := ../../$(OBJ_DIR)/Kernel
OUTPUT_DIR := ../../$(OUTPUT_DIR)
INCLUDE_DIR := ../../$(INCLUDE_DIR)
KERNEL_BIN := ../../$(KERNEL_BIN)

LINKER_FILE := linker.ld
C_SRC := $(wildcard *.c $(foreach fd, $(SUB_DIRS), $(fd)/*.c))
C_OBJ := $(addprefix $(OBJ_DIR)/, $(C_SRC:c=o))
ASM_SRC := $(wildcard *.asm $(foreach fd, $(SUB_DIRS), $(fd)/*.asm))
ASM_OBJ := $(addprefix $(OBJ_DIR)/, $(ASM_SRC:asm=o))

CFLAGS += -I $(INCLUDE_DIR) -Wno-int-to-pointer-cast
LDFLAGS += -T $(LINKER_FILE)

.PHONY: build

build: $(KERNEL_BIN)
$(KERNEL_BIN): $(ASM_OBJ) $(C_OBJ)
	@echo "\n========== Linking Kernel Files =========="
	$(LD) $(LDFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: %.asm
	@mkdir -p $(@D)
	$(ASM) $(AFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@