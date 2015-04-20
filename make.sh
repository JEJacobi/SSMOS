#!/bin/sh\

rm ssmos.bin
rm libssmos.a

mkdir obj
mkdir lib/obj

# Define the compile date:
echo -n "#define COMPILE_DATE \"Compiled on: " > kernel/date.h
echo -n `date` >> kernel/date.h
echo -n "\"" >> kernel/date.h

CC=i686-elf-gcc
CFLAGS="-std=gnu99 -ffreestanding -O2 -Wall"
LIB="-static -L../ -lssmos -I../lib"
# -Wextra -v

# Compile the system library first.
cd lib/obj
rm *
$CC -c ../*.c  $CFLAGS
ar rvs ../../libssmos.a *.o

cd ../../obj
rm *

# Next, the bootloader with nasm.
nasm -felf32 ../boot/boot.asm -o boot.bin

# Finally, the kernel.
$CC -c  ../kernel/*.c -I. $CFLAGS $LIB

# Including any assembly files.
nasm -felf32 ../kernel/*.asm
mv ../kernel/*.o .

# And glue it all together.
$CC -o ../ssmos.bin boot.bin *.o -ffreestanding -Ttext 0x7C00 -Wl,--oformat=binary -nostdlib -lgcc -O2 -I../kernel $LIB
