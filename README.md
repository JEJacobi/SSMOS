# SSMOS
####...or the *Shitty Self Made Operating System!*

SSMOS sounds a little bit more professional, though.

A just-for-fun 32-bit x86 OS, mostly made for learning purposes.

Features include:
- Overly commented code!
- Extremely inefficient and outdated drivers!
- No multi-core, or even multi-process support!
- A poorly self-implemented standard C library!
- Text mode only! That's right, a whole 80x25 resolution!
- A single kernel heap, capable of using a whopping 14 megabytes! Besides, who needs more than 640k?
- Being strongly tied to the x86 architecture, making it unlikely to ever be ported!

###Compiling:
If, for some bizarre reason, you have an urge to compile an operating system with more security holes than using *'password'* as a password, and technology that was already outdated in the late 80's, you'll need the following:
- **A Unix-like environment**, whether it's native *nix or Cygwin.
- **GCC**, [cross compiled](http://wiki.osdev.org/GCC_Cross-Compiler) for i686-elf. It's probably possible to compile SSMOS without it, but it's much much simpler with.
- **NASM**, to compile the bootloader and various kernel bits. No cross-compiling necessary.
- **mkdosfs** (at some point), to format disk images. If you're using Cygwin, there's a version [here](https://github.com/rhlee/dosfstools-cygwin/tree/cygwin).
- The **SSMOS source code**. If you're actually insane and planning to do something with the OS, I recommend you clone from the [stable branch](https://github.com/JEJacobi/SSMOS/tree/stable), as it's slightly less dangerous to run.

Once those two are set up, extract SSMOS into your choice of folder. If you haven't installed the cross compiler, open up make.sh, and modify CC as a path to it. After that, it should be as simple as going to the folder and running:

`./make.sh`

And for all that is good in this world, don't run SSMOS on any drive with any data you wouldn't be okay with losing.

### See the [SSMOS wiki](https://github.com/JEJacobi/SSMOS/wiki) for information on development or usage.
