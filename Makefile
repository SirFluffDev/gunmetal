CC=i686-elf-gcc
LD=i686-elf-ld

KERNEL_OFFSET=0x7e00

# File name for the OS image
OUT=os_image

# Generate list of *.c files in kernel folder
SOURCES = $(wildcard kernel/*.c)
HEADERS = $(wildcard kernel/*.h)

# Convert the *.c filenames to *.o to give a list of object files to build
OBJ = $(SOURCES:.c=.o)

# Default build target
all: $(OUT)

run: all
	qemu-system-x86_64 -drive file=$(OUT),format=raw -no-reboot

# Create the disk image by combining both the boot sector and the compiled kernel
$(OUT): bin/boot_sect.bin bin/kernel.bin
	cat $^ > $@
	truncate -s 1M $(OUT)

# Create kernel binary
bin/kernel.bin: kernel/kernel_entry.o $(OBJ)
	$(LD) -o $@ -Ttext $(KERNEL_OFFSET) $^ --oformat binary

# Create boot sector binary
bin/boot_sect.bin: boot/boot_sect.asm boot/*.asm
	nasm $< -f bin -i boot -o $@

# General rules
%.o: %.c $(HEADERS)
	$(CC) -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

# Clean up files
clean:
	rm -rf bin/*.bin kernel/*.o $(OUT)