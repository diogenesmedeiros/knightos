ISO_NAME=knightos.iso
BUILD=build
SRC=src

CC=i686-elf-gcc
LD=i686-elf-ld
AS=i686-elf-as
NASM=nasm

CFLAGS=-ffreestanding -O2 -Wall -Wextra -m32 -Iinclude
LDFLAGS=-T linker.ld -m elf_i386

C_SOURCES := $(shell find $(SRC) -name '*.c')
ASM_SOURCES_ASM := $(shell find $(SRC) -name '*.asm')
ASM_SOURCES_S := $(shell find $(SRC) -name '*.s')

C_OBJECTS := $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(C_SOURCES))
ASM_OBJECTS_ASM := $(patsubst $(SRC)/%.asm, $(BUILD)/%.o, $(ASM_SOURCES_ASM))
ASM_OBJECTS_S := $(patsubst $(SRC)/%.s, $(BUILD)/%.o, $(ASM_SOURCES_S))

KERNEL_OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS_ASM) $(ASM_OBJECTS_S)

KERNEL_BIN := $(BUILD)/kernel.bin

all: $(ISO_NAME)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC)/%.asm | $(BUILD)
	mkdir -p $(dir $@)
	$(NASM) -f elf32 $< -o $@

$(BUILD)/%.o: $(SRC)/%.s | $(BUILD)
	mkdir -p $(dir $@)
	$(AS) -o $@ $<

$(KERNEL_BIN): $(KERNEL_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(KERNEL_OBJECTS) -o $@

$(ISO_NAME): $(KERNEL_BIN) boot/grub/grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/kernel.bin
	cp boot/grub/grub.cfg isodir/boot/grub/
	grub-mkrescue -o $(ISO_NAME) isodir

run: $(ISO_NAME)
	qemu-system-i386 -enable-kvm -cpu host \
		-m 2048 -netdev user,id=n1 -device e1000,netdev=n1 -hda disk.img -cdrom $(ISO_NAME)

clean:
	rm -rf isodir $(BUILD) $(ISO_NAME)