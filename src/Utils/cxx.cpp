/**
 * @file cxx.cpp
 * @author Matěj Bucek (matejbucek)
 *
 * This file was inspired by Samy Pessé, How to make a Computer Operating System,
 * https://github.com/SamyPesse/How-to-Make-a-Computer-Operating-System/blob/master/src/kernel/runtime/cxx.cc
 */

extern "C" {

int __cxa_atexit(void (*Destructor)(void*), void* Parameter, void* HomeDSO);
void __cxa_finalize(void*);
void __cxa_pure_virtual();
void __stack_chk_guard_setup();
void __attribute__((noreturn)) __stack_chk_fail();
void _Unwind_Resume();
void __cxa_begin_catch();
}

void* __dso_handle;
void* __stack_chk_guard(0);


namespace __cxxabiv1 {
  __extension__ typedef int __guard __attribute__((mode(__DI__)));

  extern "C" {
  int __cxa_guard_acquire(__guard* Guard) { return !*(char*) (Guard); }
  void __cxa_guard_release(__guard* Guard) { *(char*) Guard = 1; }
  void __cxa_guard_abort(__guard*) {}
  }
}// namespace __cxxabiv1


int __cxa_atexit(void (*)(void*), void*, void*) {
  return 0;
}

void _Unwind_Resume() {
}

void __cxa_finalize(void*) {
}

void __cxa_pure_virtual() {
}

void __cxa_begin_catch() {
}

void __stack_chk_guard_setup() {
  unsigned char* Guard;
  Guard = (unsigned char*) &__stack_chk_guard;
  Guard[sizeof(__stack_chk_guard) - 1] = 255;
  Guard[sizeof(__stack_chk_guard) - 2] = '\n';
  Guard[0] = 0;
}


struct IntRegs;

void __attribute__((noreturn)) __stack_chk_fail() {
  for(;;);
}


void* __gxx_personality_v0 = (void*) 0xDEADBEAF;