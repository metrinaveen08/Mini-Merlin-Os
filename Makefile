C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o}

# Change this if your cross-compiler is somewhere else
CC = $(shell command -v i386-elf-gcc 2>/dev/null || echo gcc)
LD = $(shell command -v i386-elf-ld 2>/dev/null || echo ld)
GDB = $(shell command -v i386-elf-gdb 2>/dev/null || echo gdb)
# -g: Use debugging symbols in gcc
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32 -fno-pie -fno-pic -fno-stack-protector -mno-sse -mno-sse2 -mno-mmx
LDFLAGS = -m elf_i386

IMAGE_SIZE = 5M

# First rule is run by default
os-image.bin: boot/boot.bin kernel.bin
	cat $^ > os-image.bin
	qemu-img resize -f raw os-image.bin ${IMAGE_SIZE}

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} ${LDFLAGS} -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} ${LDFLAGS} -o $@ -Ttext 0x1000 $^

run: os-image.bin
	@if [ -n "$$DISPLAY" ]; then \
		qemu-system-i386 -drive if=ide,format=raw,file=os-image.bin,readonly=off; \
	else \
		qemu-system-i386 -display curses -drive if=ide,format=raw,file=os-image.bin,readonly=off; \
	fi

run-headless: os-image.bin
	qemu-system-i386 -nographic -drive if=ide,format=raw,file=os-image.bin,readonly=off

# Open the connection to qemu and load our kernel-object file with symbols
debug: os-image.bin kernel.elf
	qemu-system-i386 -s -S -drive if=ide,format=raw,file=os-image.bin,readonly=off -d guest_errors,int &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o libc/*.o cpu/*.o
