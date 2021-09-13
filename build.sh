#!/usr/bin/env bash

./make_hdd.sh

#make build-riscv && qemu-system-riscv64 -s -qmp tcp:localhost:4444,server,nowait -machine virt -cpu rv64 -smp 4 -m 128M -drive if=none,format=raw,file=hdd.dsk,id=foo -device virtio-blk-device,scsi=off,drive=foo -nographic -serial mon:stdio -bios none -device virtio-rng-device -device virtio-gpu-device -device virtio-net-device -device virtio-tablet-device -device virtio-keyboard-device -kernel dist/riscv/kernel.elf
ninja && qemu-system-riscv64 \
                            -s \
                            -machine virt \
                            -cpu rv64 \
                            -smp 4 \
                            -m 128M \
                            -nographic -serial mon:stdio \
                            -bios none \
                            -kernel dist/riscv/kernel.elf
                        #    -device virtio-rng-device \
                        #    -device virtio-gpu-device \
                        #    -device virtio-net-device \
                        #    -device virtio-tablet-device \
                        #    -device virtio-keyboard-device \
