/**
 * @file Basic.h
 * @author Matěj Bucek (matejbucek)
 *
 * This file was inspired by Samy Pessé, How to make a Computer Operating System,
 * https://github.com/SamyPesse/How-to-Make-a-Computer-Operating-System/blob/master/src/kernel/runtime/itoa.cc
 */
#pragma once
#include <Utils/Types.h>

void itoa(char* buf, unsigned long int n, int base);
size_t atoi(const char* str, size_t base = 10);