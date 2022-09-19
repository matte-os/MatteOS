#!/usr/bin/env bash

cd toolchain/opensbi/
make PLATFORM=generic CROSS_COMPILE=riscv64-unknown-linux-gnu- install
cd ../..