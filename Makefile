BUILD_DIR = build
BOOT_DIR = boot/bios

BOOTSECTOR = $(BUILD_DIR)/bootsector.bin
IMAGE = $(BUILD_DIR)/sentinelOS.img

.PHONY: all bootsector image size clean

all: image

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

bootsector: $(BUILD_DIR)
	nasm -f bin $(BOOT_DIR)/bootsector.asm -o $(BOOTSECTOR)

size: bootsector
	@actual=$$(stat -c%s $(BOOTSECTOR)); \
	if [ "$$actual" -ne 512 ]; then \
		echo "ERROR: bootsector is $$actual bytes, expected 512"; \
		exit 1; \
	else \
		echo "OK: bootsector is exactly 512 bytes"; \
	fi

image: size
	dd if=/dev/zero of=$(IMAGE) bs=512 count=2880
	dd if=$(BOOTSECTOR) of=$(IMAGE) conv=notrunc
	@echo "Created $(IMAGE)"

clean:
	rm -rf $(BUILD_DIR)