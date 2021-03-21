riscv_cpp_source_files := $(shell find src/Kernel -name *.cpp)
riscv_cpp_object_files := $(patsubst src/Kernel/%.cpp, build/riscv/Kernel/%.o, $(riscv_cpp_source_files))

riscv_asm_source_files := $(shell find src/Kernel -name *.S)
riscv_asmobject_files := $(patsubst src/Kernel/%.S, build/riscv/Kernel/%.o, $(riscv_asm_source_files))

riscv_utils_cpp_source_files := $(shell find src/Utils -name *.cpp)
riscv_utils_cpp_object_files := $(patsubst src/Utils/%.cpp, build/riscv/Utils/%.o, $(riscv_utils_cpp_source_files))

riscv_object_files := $(riscv_asmobject_files) $(riscv_cpp_object_files) $(riscv_utils_cpp_object_files)

$(riscv_cpp_object_files): build/riscv/Kernel/%.o : src/Kernel/%.cpp
	mkdir -p $(dir $@) && \
	riscv64-unknown-linux-gnu-gcc -fpermissive -mcmodel=medany -c -I src -std=c++11 $(patsubst build/riscv/Kernel/%.o, src/Kernel/%.cpp, $@) -o $@

$(riscv_utils_cpp_object_files): build/riscv/Utils/%.o : src/Utils/%.cpp
	mkdir -p $(dir $@) && \
	riscv64-unknown-linux-gnu-gcc -mcmodel=medany -c -I src -std=c++11 $(patsubst build/riscv/Utils/%.o, src/Utils/%.cpp, $@) -o $@

$(riscv_asmobject_files): build/riscv/Kernel/%.o : src/Kernel/%.S
	mkdir -p $(dir $@) && \
	riscv64-unknown-linux-gnu-gcc  -c -I src -std=c++11 $(patsubst build/riscv/Kernel/%.o, src/Kernel/%.S, $@) -o $@


.PHONY: build-riscv
build-riscv: $(riscv_object_files)
	mkdir -p dist/riscv && \
	riscv64-unknown-linux-gnu-ld -o dist/riscv/kernel.elf -n -T src/linker.ld $(riscv_object_files)
.KOKOT: clear
clear:
	rm -rf build/riscv/*
