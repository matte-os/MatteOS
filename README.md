# Matte OS 🛠️

This project is part of my bachelor's thesis and is a RISC-V-based operating system designed for educational purposes. It aims to provide a clear and approachable codebase, making it an excellent starting point for learning how operating systems work. The primary focus is on documentation, readability, and simplicity, ensuring that the concepts are easy to grasp without compromising functionality.

## Key Components 🌐

- **Virtual File System (VFS)**
- **Devices and Drivers**
- **FAT32 (partial support)**
- **Flattened Device Tree (FDT)**
- **Memory Management Unit (MMU)**
- **Processes and System Calls**
- **PLIC and Interrupts (Software & External)**
- **Utility Library (Utils)**
    - Arrays
    - Dynamic Arrays
    - HashMaps
    - ErrorOr
    - Locks
    - Smart Pointers
    - Strings
    - And more...

## Running and Debugging 🏋️‍♂️

### Toolchain

The toolchain is automatically downloaded and installed by the build script.

The project depends on:
- **RISC-V GNU Toolchain**
- **QEMU**
- **U-Boot**
- **OpenSBI**
- **Python3.10**
- **Doxygen**
- **Doxygen Awesome CSS**

### Prerequisites

You will need the following tools:
- **CMake**
- **Clang**
- **Ninja**

#### Ubuntu/Debian Installation:

```bash
sudo apt install cmake clang ninja-build
```

#### U-Boot Dependencies:

```bash
sudo apt-get install bc bison build-essential coccinelle \
  device-tree-compiler dfu-util efitools flex gdisk graphviz imagemagick \
  liblz4-tool libgnutls28-dev libguestfs-tools libncurses-dev \
  libpython3-dev libsdl2-dev libssl-dev lz4 lzma lzma-alone openssl \
  pkg-config python3 python3-asteval python3-coverage python3-filelock \
  python3-pkg-resources python3-pycryptodome python3-pyelftools \
  python3-pytest python3-pytest-xdist python3-sphinxcontrib.apidoc \
  python3-sphinx-rtd-theme python3-subunit python3-testtools \
  python3-virtualenv swig uuid-dev
```

#### QEMU Dependencies:

```bash
sudo apt install git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev ninja-build \
  git-email libaio-dev libbluetooth-dev libcapstone-dev libbrlapi-dev libbz2-dev libcap-ng-dev \
  libcurl4-gnutls-dev libgtk-3-dev libibverbs-dev libjpeg8-dev libncurses5-dev libnuma-dev librbd-dev \
  librdmacm-dev libsasl2-dev libsdl2-dev libseccomp-dev libsnappy-dev libssh-dev libvde-dev \
  libvdeplug-dev libvte-2.91-dev libxen-dev liblzo2-dev valgrind xfslibs-dev 
```

#### Python Dependencies:

```bash
sudo apt-get install -y make build-essential libssl-dev zlib1g-dev libbz2-dev libreadline-dev libsqlite3-dev \
  wget curl llvm libncurses5-dev libncursesw5-dev xz-utils tk-dev liblzma-dev tk-dev
```

### Build Process ⚙️

To simplify the build process, a build script `scripts/build.py` is provided. This script ensures all necessary dependencies, including the RISC-V toolchain and Clang, are installed.

#### Check Prerequisites:

```bash
scripts/build.py check
```

#### List Available Targets:

```bash
scripts/build.py -l
```

#### Initial Build:

```bash
scripts/build.py
```

#### Building Just the Kernel:

```bash
ninja
```

#### Building the Documentation:

```bash
scripts/build.py docs
```

### Running the OS 🚀

Use the following command to build and run the OS:

```bash
scripts/build.py run
```

After booting into U-Boot, execute the following command to load the flattened device tree and boot the kernel:

```txt
cp.l ${fdtcontroladdr} ${fdt_addr_r} 0x10000
bootm 0x82000000 - ${fdt_addr_r}
```

### Debugging the OS 🔧

Start QEMU with a GDB server:

```bash
scripts/build.py debug
```

Connect to the QEMU GDB server:

```bash
scripts/build.py debug-client
```

Once connected, press `c` to continue execution.

For additional debugging, use the QEMU Monitor (Ctrl+A C):
- **`info mem`**: Displays the current paging table.
- **`info registers`**: Shows the contents of all registers.

---

Feel free to explore, learn, and report any issues! 🙌

