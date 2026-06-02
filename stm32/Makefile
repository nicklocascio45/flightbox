CC=arm-none-eabi-gcc
MACH=cortex-m4
CFLAGS= -c -mcpu=$(MACH)
LDFLAGS= -T linker.ld -nostdlib

all: startup.o final.elf

startup.o: startup.c
	$(CC) $(CFLAGS) -o $@ $^

final.elf: startup.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o *.elf
