# RiscV OS

I am writing this operating system for own educational purposes.

I am really thankful to **Stephen Marz** whose **[osblog](https://osblog.stephenmarz.com/)** is great help and inspiration.

## Features
1. UART - :heavy_check_mark:
2. MMU - :heavy_check_mark:
3. Interrupts and traps - :heavy_check_mark:
4. External interrupts - :heavy_check_mark:
5. System calls
6. Processes
7. Block drivers
8. Filesystems
9. Userspace Processes

## Running and debugging

First of all, you have to have CMake, Clang, Ninja and QEMU with RISCV support.

### Prerequirements

**Ubuntu & Debian**

```bash
sudo apt install cmake clang ninja-build qemu-system-riscv64
```

_Or you can build QEMU your self_.

_Tested on QEMU v5.2.0. I had problems with running on v6.1.0 (Unreasonable interrupts: Illegal instruction)_

### Running

To build and run the OS just use script ```./build.sh```.

To build use ```ninja```.

### Debugging

You can add _-S_ into _build.sh_ for QEMU to wait until you connect and run with GDB.

**_Note: for GDB to work properly, you have to build it on your own from [RiscV-Toolchain](https://github.com/riscv-software-src/riscv-gnu-toolchain)._**

You can also use QEMU Monitor (CTRL+A C). It has some useful commands such as ```info mem``` (displays actual paging table) and ```info registers``` (displays all registers).