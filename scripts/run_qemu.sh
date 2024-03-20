#!/usr/bin/env bash

qemu-system-riscv64 -cpu rv64 -nographic -machine virt -smp 1 -m 2G -bios toolchain/u-boot/spl/u-boot-spl.bin -kernel toolchain/u-boot/u-boot.itb -device loader,file=dist/riscv/uImage,addr=0x82000000,force-raw=on -s -S -d guest_errors
