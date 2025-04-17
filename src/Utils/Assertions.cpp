/**
 * @file Assertions.cpp
 * @author Matěj Bucek (matejbucek)
 */
#include <Kernel/Arch/riscv64/CPU.h>
#include <Utils/Assertions.h>
#include <Utils/DebugConsole.h>

using Kernel::CPU;
using Utils::DebugConsole;

void runtime_assert_impl(bool condition, const char* message, const char* file, int line) {
  if(!condition) {
    DebugConsole::print("Assetion in ");
    DebugConsole::print(file);
    DebugConsole::print(" at line ");
    DebugConsole::print_number(line, 10);
    DebugConsole::print(": ");
    DebugConsole::println(message);
    CPU::halt();
  }
}