BUILD_DIR = build
BOOT_DIR = boot/bios
KERNEL_DIR = kernel

BOOTSECTOR_SRC = $(BOOT_DIR)/bootsector.asm
STAGE2_SRC = $(BOOT_DIR)/stage2.asm
KERNEL_SRC = $(KERNEL_DIR)/kernel.asm

BOOTSECTOR_BIN = $(BUILD_DIR)/bootsector.bin
STAGE2_BIN = $(BUILD_DIR)/stage2.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
IMAGE = $(BUILD_DIR)/sentinelOS.img

IMAGE_SECTORS = 2880
SECTOR_SIZE = 512

.PHONY: all bootsector stage2 kernel image size clean run

all: image

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

bootsector: $(BUILD_DIR)
	nasm -f bin $(BOOTSECTOR_SRC) -o $(BOOTSECTOR_BIN)

stage2: $(BUILD_DIR)
	nasm -f bin $(STAGE2_SRC) -o $(STAGE2_BIN)

kernel: $(BUILD_DIR)
	nasm -f elf64 kernel/kernel.asm -o build/kernel_asm.o
	x86_64-elf-g++ -ffreestanding -fno-exceptions -fno-rtti -mno-red-zone -nostdlib -c kernel/kernel.cpp -o build/kernel_cpp.o
	x86_64-elf-ld -T linker/kernel.ld -o build/kernel.elf build/kernel_asm.o build/kernel_cpp.o
	x86_64-elf-objcopy -O binary build/kernel.elf build/kernel.bin

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
	if [ "$$kernel_size" -gt 4096 ]; then \
		echo "ERROR: kernel is $$kernel_size bytes, but stage2 currently loads only 8 sectors"; \
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
	qemu-system-x86_64 -drive format=raw,file=$(IMAGE) -m 512M

clean:
	rm -rf $(BUILD_DIR)