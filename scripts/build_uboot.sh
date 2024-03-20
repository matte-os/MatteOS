#!/usr/bin/env bash

cd toolchain/u-boot/
make qemu-riscv64_spl_defconfig CROSS_COMPILE=riscv64-unknown-linux-gnu-
make ARCH_C=c_zicsr_zifencei CROSS_COMPILE=riscv64-unknown-linux-gnu- OPENSBI=../opensbi/install/usr/share/opensbi/lp64/generic/firmware/fw_dynamic.bin
cd ../..
