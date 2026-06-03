CC=arm-none-eabi-gcc
MACH=cortex-m4
CFLAGS= -c -mcpu=$(MACH) -mthumb -std=gnu11 -g3 -Wall
LDFLAGS= -mcpu=$(MACH) -mthumb -nostdlib -T linker.ld -Wl,-Map=final.map

all: main.o startup.o final.elf

main.o: main.c
	$(CC) $(CFLAGS) -o $@ $^

startup.o: startup.c
	$(CC) $(CFLAGS) -o $@ $^

final.elf: startup.o main.o
	$(CC) $(LDFLAGS) -o $@ $^

flash:
	openocd -f board/st_nucleo_f4.cfg -c "program final.elf verify reset exit"

clean:
	rm -rf *.o *.elf *.map
