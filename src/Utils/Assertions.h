/**
 * @file Assertions.h
 * @author Matěj Bucek (matejbucek)
 */
#pragma once

#ifdef __GNUC__
#define GET_CLASS_NAME (__PRETTY_FUNCTION__)
#else
#define GET_CLASS_NAME (__FUNCTION__)
#endif

#define runtime_assert(condition, message) runtime_assert_impl(condition, message, GET_CLASS_NAME, __LINE__)
void runtime_assert_impl(bool condition, const char* message, const char* file, int line);