include ../config.mk

SRC_DIR		:= src
INCLUDE_DIR	:= include
OBJ_DIR		:= ../$(OBJ_DIR)/kernel
KERNEL_FILE	:= ../$(KERNEL_FILE)

GEN_SYM_FILE	:= gensyms.sh
OUTPUT_SYM_FILE	:= ksyms.c
OBJ_SYM_FILE	:= $(OBJ_DIR)/_ksyms.o

rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

AS_FILES = $(call rwildcard,$(SRC_DIR),*.asm)
C_FILES = $(call rwildcard,$(SRC_DIR),*.c)
CPP_FILES = $(call rwildcard,$(SRC_DIR),*.cpp)
AS_OBJ = $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(AS_FILES))
C_OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_FILES))
CPP_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_FILES))
OBJ_FILES	:= $(AS_OBJ) $(C_OBJ) $(CPP_OBJ)
LINKER_FILE	:= linker.ld

AFLAGS += -Wall
CFLAGS += -I$(INCLUDE_DIR) -Wall -Wextra -fno-stack-protector -ffreestanding -nostdlib \
	-fno-builtin -mno-red-zone -Wno-int-to-pointer-cast -Wno-pointer-to-int-cast \
	-fno-omit-frame-pointer
CXXFLAGS += $(CFLAGS) -fno-exceptions -fno-rtti
LFLAGS += -nostdlib -z text -z noexecstack -T $(LINKER_FILE)

.PHONY: build clean

build: $(OBJ_FILES)
	#@echo "\nGenerating symbol file\n"
	#./$(GEN_SYM_FILE) $(KERNEL_FILE)
	#$(CC) $(CFLAGS) -x c -c $(OUTPUT_SYM_FILE) -o $(OBJ_SYM_FILE)
	#$(LD) $(LFLAGS) $^ $(OBJ_SYM_FILE) -o $(KERNEL_FILE)

	#@echo "\nRe-Generating symbol file\n"
	#./$(GEN_SYM_FILE) $(KERNEL_FILE)
	#$(CC) $(CFLAGS) -x c -c $(OUTPUT_SYM_FILE) -o $(OBJ_SYM_FILE)
	#$(LD) $(LFLAGS) $^ $(OBJ_SYM_FILE) -o $(KERNEL_FILE)
	$(LD) $(LFLAGS) $^ -o $(KERNEL_FILE)
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(@D)
	$(AS) $(AFLAGS) $^ -o $@

clean:
	rm -rf $(OBJ_DIR)/*