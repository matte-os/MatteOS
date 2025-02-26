#!/usr/bin/env bash

ninja
cd build/dist/riscv
riscv64-unknown-linux-gnu-objcopy -O binary -R .note -R .comment -S kernel.elf kernel.bin
gzip -f -9 kernel.bin
mkimage -A riscv -O linux -T kernel -C gzip -a 0x86000000 -e 0x86000000 -n "Matte OS" -d kernel.bin.gz uImage
cd ../..