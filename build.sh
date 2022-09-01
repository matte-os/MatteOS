#!/usr/bin/env bash

./make_hdd.sh
echo "RISCVOS: Building OpenSBI, U-Boot SPL and the OS."
./scripts/build_opensbi.sh && ./scripts/build_uboot.sh && cmake -G Ninja . && ninja && echo "RISCVOS: Build successful!"