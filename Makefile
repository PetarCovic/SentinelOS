BUILD_DIR = build
BOOT_DIR = boot/bios
KERNEL_DIR = kernel
KERNEL_CORE_DIR = kernel/core
INCLUDE_DIR = include
LINKER_DIR = linker

BOOTSECTOR_SRC = $(BOOT_DIR)/bootsector.asm
STAGE2_SRC = $(BOOT_DIR)/stage2.asm

KERNEL_ASM_SRC = $(KERNEL_DIR)/kernel.asm
KERNEL_MAIN_SRC = $(KERNEL_DIR)/main.cpp
TERMINAL_SRC = $(KERNEL_CORE_DIR)/terminal.cpp

LINKER_SCRIPT = $(LINKER_DIR)/kernel.ld

BOOTSECTOR_BIN = $(BUILD_DIR)/bootsector.bin
STAGE2_BIN = $(BUILD_DIR)/stage2.bin

KERNEL_ASM_OBJ = $(BUILD_DIR)/kernel_asm.o
KERNEL_MAIN_OBJ = $(BUILD_DIR)/main.o
TERMINAL_OBJ = $(BUILD_DIR)/terminal.o

KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin

IMAGE = $(BUILD_DIR)/sentinelOS.img

IMAGE_SECTORS = 2880
SECTOR_SIZE = 512

KERNEL_SECTORS = 8
KERNEL_MAX_SIZE = 4096

AS = nasm
CXX = x86_64-elf-g++
LD = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy

CXXFLAGS = -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -nostdlib -nostdinc++ -I$(INCLUDE_DIR) -Wall -Wextra -c

.PHONY: all bootsector stage2 kernel image size clean run

all: image

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

bootsector: $(BUILD_DIR)
	$(AS) -f bin $(BOOTSECTOR_SRC) -o $(BOOTSECTOR_BIN)

stage2: $(BUILD_DIR)
	$(AS) -f bin $(STAGE2_SRC) -o $(STAGE2_BIN)

$(KERNEL_ASM_OBJ): $(KERNEL_ASM_SRC) | $(BUILD_DIR)
	$(AS) -f elf64 $(KERNEL_ASM_SRC) -o $(KERNEL_ASM_OBJ)

$(KERNEL_MAIN_OBJ): $(KERNEL_MAIN_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(KERNEL_MAIN_SRC) -o $(KERNEL_MAIN_OBJ)

$(TERMINAL_OBJ): $(TERMINAL_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(TERMINAL_SRC) -o $(TERMINAL_OBJ)

kernel: $(KERNEL_ASM_OBJ) $(KERNEL_MAIN_OBJ) $(TERMINAL_OBJ)
	$(LD) -T $(LINKER_SCRIPT) -o $(KERNEL_ELF) $(KERNEL_ASM_OBJ) $(KERNEL_MAIN_OBJ) $(TERMINAL_OBJ)
	$(OBJCOPY) -O binary $(KERNEL_ELF) $(KERNEL_BIN)

size: bootsector stage2 kernel
	@boot_size=$$(stat -c%s $(BOOTSECTOR_BIN)); \
	if [ "$$boot_size" -ne 512 ]; then \
		echo "ERROR: bootsector is $$boot_size bytes, expected 512"; \
		exit 1; \
	else \
		echo "OK: bootsector is exactly 512 bytes"; \
	fi

	@stage2_size=$$(stat -c%s $(STAGE2_BIN)); \
	if [ "$$stage2_size" -gt 512 ]; then \
		echo "ERROR: stage2 is $$stage2_size bytes, but bootsector currently loads only 1 sector"; \
		exit 1; \
	else \
		echo "OK: stage2 is $$stage2_size bytes"; \
	fi

	@kernel_size=$$(stat -c%s $(KERNEL_BIN)); \
	if [ "$$kernel_size" -gt $(KERNEL_MAX_SIZE) ]; then \
		echo "ERROR: kernel is $$kernel_size bytes, but stage2 currently loads only $(KERNEL_SECTORS) sectors"; \
		exit 1; \
	else \
		echo "OK: kernel is $$kernel_size bytes"; \
	fi

image: size
	dd if=/dev/zero of=$(IMAGE) bs=$(SECTOR_SIZE) count=$(IMAGE_SECTORS)
	dd if=$(BOOTSECTOR_BIN) of=$(IMAGE) bs=$(SECTOR_SIZE) count=1 conv=notrunc
	dd if=$(STAGE2_BIN) of=$(IMAGE) bs=$(SECTOR_SIZE) seek=1 conv=notrunc
	dd if=$(KERNEL_BIN) of=$(IMAGE) bs=$(SECTOR_SIZE) seek=2 conv=notrunc
	@echo "Created $(IMAGE)"

run: image
	qemu-system-x86_64 -drive file=$(IMAGE),format=raw,if=floppy -m 512M

clean:
	rm -rf $(BUILD_DIR)