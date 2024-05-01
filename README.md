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

### Build

To make the building process of the OS easier, we have a build script `scripts/build.py` that will install
RISC-V toolchain, clang and all other dependencies. 

To check if you have all prerequisites installed, you can run the following command:
```bash
scripts/build.py check
```

You can list all targets with:
```bash
scripts/build.py -l
```

Initial build should be done using the build script like so:
```bash
scripts/build.py
```

For building just the kernel, you can use the following command:
```bash
ninja
```

### Running

To run the OS, you can use the following command:
```bash
scripts/build.py run
```
This command will ensure that the kernel image is built
and everything is prepared for running the OS.

After booting to U-Boot, you can run the kernel with the following command:
```txt
cp.l ${fdtcontroladdr} ${fdt_addr_r} 0x10000
bootm 0x82000000 - ${fdt_addr_r}
```
This command will load the flattened device tree to the address specified by the U-Boot
environment variable `fdt_addr_r` and then boot the kernel.

### Debugging

To debug the OS, you can use `scripts/build.py debug`, which will start QEMU with GDB server.
You can the use `scripts/build.py debug-client`, that will start GDB and connect to the QEMU GDB server.

Then just press `c` to continue the execution.

You can also use QEMU Monitor (CTRL+A C). It has some useful commands such as ```info mem```
(displays actual paging table) and ```info registers``` (displays all registers).