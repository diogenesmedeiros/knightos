ISO_NAME=knightos.iso
BUILD=build
SRC=src

CC=i686-elf-gcc
LD=i686-elf-ld

CFLAGS=-ffreestanding -O2 -Wall -Wextra -m32 -Iinclude
LDFLAGS=-T linker.ld -m elf_i386

COMMANDS_SOURCES = \
	$(SRC)/commands/command.c \
	$(SRC)/commands/clear.c \
	$(SRC)/commands/echo.c \
	$(SRC)/commands/help.c \
	$(SRC)/commands/login.c \
	$(SRC)/commands/reboot.c \
	$(SRC)/commands/shutdown.c \
	$(SRC)/commands/useradd.c \
	$(SRC)/commands/whoami.c \
	$(SRC)/commands/sysinfo.c \
	$(SRC)/commands/mkdir.c \
	$(SRC)/commands/ls.c \
	$(SRC)/commands/cd.c \
	$(SRC)/commands/format.c \
	$(SRC)/commands/touch.c \
	$(SRC)/commands/cat.c \
	$(SRC)/commands/rm.c \
	$(SRC)/commands/run_knt.c \
	$(SRC)/commands/edit.c \
	$(SRC)/commands/sudo.c


LIBS_SOURCES = \
	$(SRC)/lib/string.c \
	$(SRC)/lib/time.c \
	$(SRC)/lib/sha256.c \
	$(SRC)/lib/utils.c \
	$(SRC)/lib/stdlib.c \
	$(SRC)/lib/libc_compat.c
	
LANG_SOURCES = \
	$(SRC)/knt/interpreter.c \
	$(SRC)/knt/knightlang.c \
	$(SRC)/knt/lexer.c \
	$(SRC)/knt/parser.c

DRIVERS_SOURCES = \
	$(SRC)/drivers/keyboard.c \
	$(SRC)/drivers/mouse.c

KERNEL_SOURCES = \
	$(SRC)/kernel/kernel.c \
	$(SRC)/kernel/memory.c \
	$(SRC)/kernel/user.c \
	$(SRC)/kernel/shell.c \
	$(SRC)/kernel/terminal.c \
	$(SRC)/kernel/cpu.c \
	$(SRC)/kernel/disk.c \
	$(SRC)/kernel/editor.c

FS_SOURCES = \
	$(SRC)/fs/fs.c \
	$(SRC)/fs/file.c

KERNEL_BUILD = \
	$(KERNEL_SOURCES) \
	$(COMMANDS_SOURCES) \
	$(LIBS_SOURCES) \
	$(DRIVERS_SOURCES) \
	$(LANG_SOURCES) \
	$(FS_SOURCES)

ASM_SOURCES = $(SRC)/kernel/start.asm
ASM_OBJECTS = $(patsubst $(SRC)/%.asm, $(BUILD)/%.o, $(ASM_SOURCES))

KERNEL_OBJECTS = $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(KERNEL_BUILD)) $(ASM_OBJECTS)

KERNEL_BIN=$(BUILD)/kernel.bin

all: $(ISO_NAME)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: $(SRC)/%.asm | $(BUILD)
	mkdir -p $(dir $@)
	nasm -f elf32 $< -o $@

$(KERNEL_BIN): $(KERNEL_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(KERNEL_OBJECTS) -o $@

$(ISO_NAME): $(KERNEL_BIN) boot/grub/grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/kernel.bin
	cp boot/grub/grub.cfg isodir/boot/grub/
	grub-mkrescue -o $(ISO_NAME) isodir

run: $(ISO_NAME)
	qemu-system-i386 -enable-kvm -cpu host -bios \
		-m 2048 -netdev user,id=n1 -device e1000,netdev=n1 -hda disk.img -cdrom $(ISO_NAME)

clean:
	rm -rf isodir $(BUILD) $(ISO_NAME)