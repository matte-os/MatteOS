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

class TypeIDBase {
protected:
  static u64 m_counter;
};

template<typename T>
class TypeID : public TypeIDBase {
public:
  static u64 get() {
    static u64 id = m_counter++;
    return id;
  }
};

template<>
class TypeID<void> {
public:
  static u64 get() {
    return 0;
  }
};
