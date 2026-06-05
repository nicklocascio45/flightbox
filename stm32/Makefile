CC=arm-none-eabi-gcc
MACH=cortex-m4
CFLAGS= -c -mcpu=$(MACH) -mthumb -std=gnu11 -g3 -Wall
LDFLAGS= -mcpu=$(MACH) -mthumb -nostdlib -T linker.ld -Wl,-Map=final.map

all: main.o startup.o final.elf final.bin

main.o: main.c
	$(CC) $(CFLAGS) -o $@ $^

startup.o: startup.c
	$(CC) $(CFLAGS) -o $@ $^

final.elf: startup.o main.o
	$(CC) $(LDFLAGS) -o $@ $^

final.bin: final.elf
	arm-none-eabi-objcopy -O binary $< $@

flash: final.bin
	st-flash --reset write $< 0x8000000

clean:
	rm -rf *.o *.elf *.map *.bin
