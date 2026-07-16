BUILD_DIR = build
BOOT_DIR = boot/bios
KERNEL_DIR = kernel
KERNEL_CORE_DIR = kernel/core
KERNEL_BOOT_DIR = kernel/boot
KERNEL_MEMORY_DIR = kernel/memory
INCLUDE_DIR = include
LINKER_DIR = linker
DRIVERS_DIR = kernel/drivers

BOOTSECTOR_SRC = $(BOOT_DIR)/bootsector.asm
STAGE2_SRC = $(BOOT_DIR)/stage2.asm

KERNEL_ASM_SRC = $(KERNEL_DIR)/kernel.asm
KERNEL_MAIN_SRC = $(KERNEL_DIR)/main.cpp
TERMINAL_SRC = $(KERNEL_CORE_DIR)/terminal.cpp
MEMORY_SRC = $(KERNEL_CORE_DIR)/memory.cpp
PANIC_SRC = $(KERNEL_CORE_DIR)/panic.cpp
LOGGER_SRC = $(KERNEL_CORE_DIR)/logger.cpp
INTERRUPT_STUBS_SRC = $(KERNEL_DIR)/arch/x86_64/interrupt_stubs.asm
IDT_SRC = $(KERNEL_DIR)/arch/x86_64/idt.cpp
INTERRUPTS_SRC = $(KERNEL_DIR)/arch/x86_64/interrupts.cpp
EXCEPTIONS_SRC = $(KERNEL_DIR)/arch/x86_64/exceptions.cpp
IO_SRC = $(KERNEL_DIR)/arch/x86_64/io.cpp
PIC_SRC = $(KERNEL_DIR)/arch/x86_64/pic.cpp
KEYBOARD_SRC = $(DRIVERS_DIR)/keyboard.cpp
CPU_SRC = $(KERNEL_DIR)/arch/x86_64/cpu.cpp
STRING_SRC = $(KERNEL_CORE_DIR)/string.cpp
CONSOLE_SRC = $(KERNEL_CORE_DIR)/console.cpp
SHELL_SRC = $(KERNEL_CORE_DIR)/shell.cpp
MEMORY_MAP_SRC = $(KERNEL_BOOT_DIR)/memory_map.cpp
PHYSICAL_MEMORY_SRC = $(KERNEL_MEMORY_DIR)/physical_memory.cpp
PAGE_SRC = $(KERNEL_MEMORY_DIR)/page.cpp

LINKER_SCRIPT = $(LINKER_DIR)/kernel.ld

BOOTSECTOR_BIN = $(BUILD_DIR)/bootsector.bin
STAGE2_BIN = $(BUILD_DIR)/stage2.bin

KERNEL_ASM_OBJ = $(BUILD_DIR)/kernel_asm.o
KERNEL_MAIN_OBJ = $(BUILD_DIR)/main.o
TERMINAL_OBJ = $(BUILD_DIR)/terminal.o
MEMORY_OBJ=$(BUILD_DIR)/memory.o
PANIC_OBJ=$(BUILD_DIR)/panic.o
LOGGER_OBJ=$(BUILD_DIR)/logger.o
INTERRUPT_STUBS_OBJ = $(BUILD_DIR)/interrupt_stubs.o
IDT_OBJ = $(BUILD_DIR)/idt.o
INTERRUPTS_OBJ = $(BUILD_DIR)/interrupts.o
EXCEPTIONS_OBJ = $(BUILD_DIR)/exceptions.o
IO_OBJ = $(BUILD_DIR)/io.o
PIC_OBJ = $(BUILD_DIR)/pic.o
KEYBOARD_OBJ = $(BUILD_DIR)/keyboard.o
CPU_OBJ = $(BUILD_DIR)/cpu.o
STRING_OBJ = $(BUILD_DIR)/string.o
CONSOLE_OBJ = $(BUILD_DIR)/console.o
SHELL_OBJ = $(BUILD_DIR)/shell.o
MEMORY_MAP_OBJ = $(BUILD_DIR)/memory_map.o
PHYSICAL_MEMORY_OBJ = $(BUILD_DIR)/physical_memory.o
PAGE_OBJ = $(BUILD_DIR)/page.o

KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin

IMAGE = $(BUILD_DIR)/sentinelOS.img

IMAGE_SECTORS = 2880
SECTOR_SIZE = 512

KERNEL_SECTORS = 64
KERNEL_MAX_SIZE = 32768

AS = nasm
CXX = x86_64-elf-g++
LD = x86_64-elf-ld
OBJCOPY = x86_64-elf-objcopy

CXXFLAGS = -std=c++17 -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -nostdlib -nostdinc++ -I$(INCLUDE_DIR) -Wall -Wextra -c
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

$(MEMORY_OBJ): $(MEMORY_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(MEMORY_SRC) -o $(MEMORY_OBJ)

$(PANIC_OBJ): $(PANIC_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(PANIC_SRC) -o $(PANIC_OBJ)

$(LOGGER_OBJ): $(LOGGER_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LOGGER_SRC) -o $(LOGGER_OBJ)

$(INTERRUPT_STUBS_OBJ): $(INTERRUPT_STUBS_SRC) | $(BUILD_DIR)
	$(AS) -f elf64 $(INTERRUPT_STUBS_SRC) -o $(INTERRUPT_STUBS_OBJ)

$(IDT_OBJ): $(IDT_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(IDT_SRC) -o $(IDT_OBJ)

$(INTERRUPTS_OBJ): $(INTERRUPTS_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INTERRUPTS_SRC) -o $(INTERRUPTS_OBJ)

$(EXCEPTIONS_OBJ): $(EXCEPTIONS_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(EXCEPTIONS_SRC) -o $(EXCEPTIONS_OBJ)

$(IO_OBJ): $(IO_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(IO_SRC) -o $(IO_OBJ)

$(PIC_OBJ): $(PIC_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(PIC_SRC) -o $(PIC_OBJ)

$(KEYBOARD_OBJ): $(KEYBOARD_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(KEYBOARD_SRC) -o $(KEYBOARD_OBJ)

$(CPU_OBJ): $(CPU_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CPU_SRC) -o $(CPU_OBJ)

$(STRING_OBJ): $(STRING_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(STRING_SRC) -o $(STRING_OBJ)

$(CONSOLE_OBJ): $(CONSOLE_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CONSOLE_SRC) -o $(CONSOLE_OBJ)

$(SHELL_OBJ): $(SHELL_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SHELL_SRC) -o $(SHELL_OBJ)

$(MEMORY_MAP_OBJ): $(MEMORY_MAP_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(MEMORY_MAP_SRC) -o $(MEMORY_MAP_OBJ)

$(PHYSICAL_MEMORY_OBJ): $(PHYSICAL_MEMORY_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(PHYSICAL_MEMORY_SRC) -o $(PHYSICAL_MEMORY_OBJ)

$(PAGE_OBJ): $(PAGE_SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(PAGE_SRC) -o $(PAGE_OBJ)

kernel: $(KERNEL_ASM_OBJ) $(KERNEL_MAIN_OBJ) $(TERMINAL_OBJ) $(MEMORY_OBJ) $(PANIC_OBJ) $(LOGGER_OBJ) $(INTERRUPT_STUBS_OBJ) $(IDT_OBJ) $(INTERRUPTS_OBJ) $(EXCEPTIONS_OBJ) $(IO_OBJ) $(PIC_OBJ) $(KEYBOARD_OBJ) $(CPU_OBJ) $(STRING_OBJ) $(CONSOLE_OBJ) $(SHELL_OBJ) $(MEMORY_MAP_OBJ) $(PHYSICAL_MEMORY_OBJ) $(PAGE_OBJ)
	$(LD) -T $(LINKER_SCRIPT) -o $(KERNEL_ELF) $(KERNEL_ASM_OBJ) $(KERNEL_MAIN_OBJ) $(TERMINAL_OBJ) $(MEMORY_OBJ) $(PANIC_OBJ) $(LOGGER_OBJ)  $(INTERRUPT_STUBS_OBJ) $(IDT_OBJ) $(INTERRUPTS_OBJ) $(EXCEPTIONS_OBJ) $(IO_OBJ) $(PIC_OBJ) $(KEYBOARD_OBJ) $(CPU_OBJ) $(STRING_OBJ) $(CONSOLE_OBJ) $(SHELL_OBJ) $(MEMORY_MAP_OBJ) $(PHYSICAL_MEMORY_OBJ) $(PAGE_OBJ)
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