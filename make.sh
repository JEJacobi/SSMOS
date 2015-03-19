#!/bin/sh\

CC=i686-elf-gcc
CFLAGS="-std=gnu99 -ffreestanding -Wall -Wextra -O2"
LIB="-static -L../ -lssmos -I../lib"
#-v

# Clean up.
./clean.sh

# Compile the system library first.
cd lib/obj
$CC -c ../*.c  $CFLAGS
ar rvs ../../libssmos.a *.o

cd ../../obj

# Next, the bootloader with nasm.
nasm -felf32 ../boot/boot.asm -o boot.bin

# Finally, the kernel.
$CC -c  ../kernel/*.c -I. $CFLAGS $LIB

# And glue it all together.
$CC -o ../ssmos.bin boot.bin *.o -ffreestanding -Ttext 0x7C00 -Wl,--oformat=binary -nostdlib -lgcc -O2 -I../kernel $LIB