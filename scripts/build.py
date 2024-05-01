#!/usr/bin/env python3
import os
import sys
import argparse


class Target:
    __slots__ = ("name", "dependencies", "action", "description")

    def __init__(self, name: str, dependencies: [str], action: str = None, description: str = ""):
        self.name = name
        self.dependencies = dependencies
        self.action = action
        self.description = description

    def run(self) -> int:
        if self.action is None:
            return 0
        return os.system(self.action)

    def __str__(self):
        return f"Target(name: {self.name}, description: {self.description}, dependencies: [{', '.join(self.dependencies)}])"


class Build:
    __slots__ = "targets"

    def __init__(self, target_list: [Target]):
        self.targets = target_list

    def get_target(self, target: str) -> Target:
        for t in self.targets:
            if t.name == target:
                return t

    def build_dependency_graph(self, target: Target) -> [Target]:
        graph = []

        for dep in target.dependencies:
            graph.extend(self.build_dependency_graph(self.get_target(dep)))

        graph.append(target)
        return list(dict.fromkeys(graph))

    def run(self, cli_targets: [str], debug: bool = False):
        targets_to_run = []
        for t in cli_targets:
            target = self.get_target(t)
            if target is None:
                print("Target %s not found" % t)
                sys.exit(1)
            targets_to_run.extend(self.build_dependency_graph(target))

        if debug:
            print('Debug: Targets to run:', [str(t) for t in targets_to_run])

        for t in list(dict.fromkeys(targets_to_run)):
            if debug:
                print('Debug: Running target', str(t))
            t.run()


targets = [
    Target("riscv-gnu-toolchain", [], """
    if [ ! -f 'toolchain/riscv.tar.gz' ]; then
    wget -c https://github.com/riscv-collab/riscv-gnu-toolchain/releases/download/2024.04.12/riscv64-glibc-ubuntu-22.04-gcc-nightly-2024.04.12-nightly.tar.gz -O toolchain/riscv.tar.gz
    fi
    if [ ! -d 'toolchain/riscv' ]; then
    tar -xvf toolchain/riscv.tar.gz -C toolchain
    fi
    """, "Download and extract RISC-V GNU toolchain"),
    Target("clang", [], """
    if [ ! -f 'toolchain/clang.tar.xz' ]; then
    wget -c https://github.com/llvm/llvm-project/releases/download/llvmorg-17.0.1/clang+llvm-17.0.1-aarch64-linux-gnu.tar.xz -O toolchain/clang.tar.xz
    fi
    if [ ! -d 'toolchain/clang' ]; then
    mkdir -p toolchain/clang
    tar -xvf toolchain/clang.tar.xz -C toolchain/clang/ --strip-components=1
    fi
    """, "Download and extract Clang"),
    Target("python3.10", [], """
    if [ ! -f 'toolchain/python3.10.tar.xz' ]; then
    wget -c https://www.python.org/ftp/python/3.10.0/Python-3.10.0.tar.xz -O toolchain/python3.10.tar.xz
    fi
    if [ ! -d 'toolchain/python3.10-build' ]; then
    mkdir -p toolchain/python3.10-build
    tar -xvf toolchain/python3.10.tar.xz -C toolchain/python3.10-build/ --strip-components=1
    fi
    cd toolchain/python3.10-build
    if [ ! -d '../python3.10' ]; then
    ./configure --prefix=$(realpath "$(pwd)/../python3.10") --enable-optimizations --enable-shared
    fi
    if [ ! -f '../python3.10/lib/libpython3.10.so' ]; then
    make -j$(nproc) all
    make install
    fi
    """, "Download and extract Python 3.10, that is needed by GDB"),
    Target("toolchain", ["riscv-gnu-toolchain", "clang", "python3.10"], description="Download and extract toolchains"),
    Target("clean", [], """
    ninja clean
    cd toolchain/opensbi && make clean
    cd toolchain/u-boot && make clean
    """, "Clean the build directory"),
    Target("fresh", ["clean"], """
    rm -rf toolchain/clang
    rm -rf toolchain/riscv
    """, "Clean the build directory and remove toolchains"),
    Target("opensbi", ["toolchain"], """
    scripts/build_opensbi.sh
    """, "Build OpenSBI"),
    Target("uboot", ["toolchain"], """
    scripts/build_uboot.sh
    """, "Build U-Boot"),
    Target("kernel", ["toolchain", "opensbi", "uboot"], """
    cmake -GNinja .
    ninja
    """, "Build the kernel"),
    Target("image", ["kernel"], """
    scripts/build_image.sh
    """, "Build the image"),
    Target("run", ["image"], """
    qemu-system-riscv64 -cpu rv64 -nographic -machine virt -smp 1 -m 2G -bios toolchain/u-boot/spl/u-boot-spl.bin -kernel toolchain/u-boot/u-boot.itb -device loader,file=dist/riscv/uImage,addr=0x82000000,force-raw=on -s -d guest_errors
    """, "Run the image"),
    Target("dir", [], "ls", "List files in the current directory"),
    Target("all", ["image"], description="Build everything"),
    Target("check", [], """
    echo 'This script is going to check for needed libraries and tools: '
    """, "Check for needed libraries and tools"),
    Target("debug", [], """
    qemu-system-riscv64 -cpu rv64 -nographic -machine virt -smp 1 -m 2G -bios toolchain/u-boot/spl/u-boot-spl.bin -kernel toolchain/u-boot/u-boot.itb -device loader,file=dist/riscv/uImage,addr=0x82000000,force-raw=on -s -S -d guest_errors
    """, "Run qemu in debug mode"),
    Target("debug-client", ["riscv-gnu-toolchain", "python3.10"], """
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:toolchain/python3.10/lib/
    export PYTHONHOME=$(realpath "toolchain/python3.10")
    export PYTHONPATH=$PYTHONHOME/lib/python3.10
    toolchain/riscv/bin/riscv64-unknown-linux-gnu-gdb -ex 'target remote localhost:1234' dist/riscv/kernel.elf
    """, "Run gdb in debug mode")
]

if __name__ == "__main__":
    build = Build(targets)

    parser = argparse.ArgumentParser(description="RISC-V OS Build system")
    parser.add_argument("-d", "--debug", help="Enable debug mode", action="store_true")
    parser.add_argument("-l", "--list", help="List all available targets", action="store_true")
    parser.add_argument("targets", type=str, nargs="*", help="Targets to build")

    args = parser.parse_args()

    if args.list:
        print("Available targets:")
        for t in targets:
            print(t)
        sys.exit(0)

    if len(args.targets) > 0:
        build.run(args.targets, args.debug)
    else:
        build.run(["all"], args.debug)
