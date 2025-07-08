ISO_NAME=knightos.iso
BUILD=build
SRC=src

CC=i686-elf-gcc
LD=i686-elf-ld

CFLAGS=-ffreestanding -O2 -Wall -Wextra -m32 -Iinclude
LDFLAGS=-T linker.ld -m elf_i386

KERNEL_SOURCES = $(SRC)/kernel.c $(SRC)/memory.c $(SRC)/keyboard.c \
				$(SRC)/terminal.c $(SRC)/shell.c $(SRC)/string.c \
				$(SRC)/user.c $(SRC)/command.c $(SRC)/commands/clear.c \
				$(SRC)/commands/echo.c $(SRC)/commands/help.c $(SRC)/commands/login.c \
				$(SRC)/commands/reboot.c $(SRC)/commands/shutdown.c $(SRC)/commands/useradd.c \
				$(SRC)/commands/whoami.c
KERNEL_OBJECTS = $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(KERNEL_SOURCES))
KERNEL_BIN=$(BUILD)/kernel.bin

all: $(ISO_NAME)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/%.o: $(SRC)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL_BIN): $(KERNEL_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) $(KERNEL_OBJECTS) -o $@

$(ISO_NAME): $(KERNEL_BIN) boot/grub/grub.cfg
	mkdir -p isodir/boot/grub
	cp $(KERNEL_BIN) isodir/boot/kernel.bin
	cp boot/grub/grub.cfg isodir/boot/grub/
	grub-mkrescue -o $(ISO_NAME) isodir

run: $(ISO_NAME)
	qemu-system-i386 -cdrom $(ISO_NAME)

clean:
	rm -rf isodir $(BUILD) $(ISO_NAME)