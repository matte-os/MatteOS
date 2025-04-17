/**
 * @file Types.h
 * @author Matěj Bucek (matejbucek)
 *
* This file contains type definitions from Samy Passém, How to make a Computer Operating System,
 * https://github.com/SamyPesse/How-to-Make-a-Computer-Operating-System/blob/master/src/kernel/runtime/types.h
 */
#pragma once
#define NULL 0
typedef unsigned long size_t;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef signed long long __int64;
typedef unsigned long uintptr_t;

template<typename T>
struct TypeID {
  static size_t get() {
    static char unique;
    return reinterpret_cast<size_t>(&unique);
  }
};
