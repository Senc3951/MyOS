SUB_DIRS := arch arch/acpi boot dev dev/display dev/keyboard io mem sys

OBJ_DIR := ../../$(OBJ_DIR)/kernel
OUTPUT_DIR := ../../$(OUTPUT_DIR)
INCLUDE_DIR := ../../$(INCLUDE_DIR)/kernel \
	-I ../../$(LIBC_DIR)
KERNEL_BIN := ../../$(KERNEL_BIN)
OBJ_LIBC_DIR := ../../$(LIBC_OBJ_DIR)

LINKER_FILE := linker.ld
SYMBOL_GENERATOR := gensymbols.sh
SYMBOL_FILE := _symbols.gen
OBJ_SYMBOL_FILE := $(OBJ_DIR)/_symbols.o

C_SRC := $(wildcard *.c $(foreach fd, $(SUB_DIRS), $(fd)/*.c))
C_OBJ := $(addprefix $(OBJ_DIR)/, $(C_SRC:c=o))
ASM_SRC := $(wildcard *.asm $(foreach fd, $(SUB_DIRS), $(fd)/*.asm))
ASM_OBJ := $(addprefix $(OBJ_DIR)/, $(ASM_SRC:asm=o))
LIBC_OBJ := $(wildcard *.o $(foreach fd, $(OBJ_LIBC_DIR), $(fd)/*.o))

CFLAGS += -I $(INCLUDE_DIR) -Wno-int-to-pointer-cast
LDFLAGS += -T $(LINKER_FILE)

.PHONY: build

build: $(KERNEL_BIN)
$(KERNEL_BIN): $(ASM_OBJ) $(C_OBJ) $(LIBC_OBJ)
	@echo "\n========== Generating Symbol file =========="
	@./$(SYMBOL_GENERATOR) $^
	$(CC) $(CFLAGS) -x c -c $(SYMBOL_FILE) -o $(OBJ_SYMBOL_FILE)
	$(LD) $(LDFLAGS) $^ -o $@ $(OBJ_SYMBOL_FILE)
	
	@echo "\n========== Re-Generating Symbol file =========="
	@./$(SYMBOL_GENERATOR) $(KERNEL_BIN)
	$(CC) $(CFLAGS) -x c -c $(SYMBOL_FILE) -o $(OBJ_SYMBOL_FILE)
	
	@echo "\n========== Re-Linking Kernel Files =========="
	$(LD) $(LDFLAGS) $^ -o $@ $(OBJ_SYMBOL_FILE)

$(OBJ_DIR)/%.o: %.asm
	@mkdir -p $(@D)
	$(ASM) $(AFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@