
CROSS_COMPILE = arm-none-linux-gnueabi

AOPS = --warn --fatal-warnings -mthumb
COPS = -Wall -Werror -O2 -nostdlib -nostartfiles -ffreestanding -mthumb -march=armv4t

# qemu-system-arm -L . -kernel test.bin

00000000.bin : test.elf rip test.bin
	./rip test.elf
	diff 00000000.bin test.bin

rip : rip.c
	gcc rip.c -o rip

test.bin : test.elf
	$(CROSS_COMPILE)-objcopy test.elf -O binary test.bin

test.elf : vectors.o putget.o uart.o memmap
	$(CROSS_COMPILE)-ld vectors.o putget.o uart.o -T memmap -o test.elf  
	$(CROSS_COMPILE)-objdump -D test.elf > test.list

vectors.o : vectors.s
	$(CROSS_COMPILE)-as $(AOPS) vectors.s -o vectors.o


putget.o : putget.s
	$(CROSS_COMPILE)-as $(AOPS) putget.s -o putget.o

uart.o : uart.c
	$(CROSS_COMPILE)-gcc -c $(COPS) uart.c -o uart.o

clean :
	rm -f *.o
	rm -f *.elf
	rm -f *.bin
	rm -f *.list
	rm -f rip
