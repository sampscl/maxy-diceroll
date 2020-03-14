//
//  debug.hpp
//  utilities
//
//  Created by Six on 2/23/2018.
//  Copyright 2/23/2018 Six. All rights reserved.
//
#ifndef _DEBUG_HPP
#define _DEBUG_HPP

#include <stddef.h>

///
/// @brief Debug log with time, file, line, and function helper
/// @param[in] file The file name, will have directory delimiters removed
/// @param[in] line The line number
/// @param[in] func The function name
/// @param[in] fmt The format string a-la printf
/// @param[in] ... variable args for the format string
///
void formatted_print(const char *file, int line, const char *func, const char *fmt, ...);

///
/// @brief Debug print an io operation
/// @param[in] file The file name, will have directory delimiters removed
/// @param[in] line The line number
/// @param[in] func The function name
/// @param[in] in @li true for input @li false for output
/// @param[in] msg Pointer to the message to print
/// @param[in] len Length of msg, bytes
///
void debug_print_io(const char *file, int line, const char *func, bool in, const void *msg, size_t len);

///
/// Use dpr and iodpr (debug print and io debug print) and #define DEBUG to
/// control printing. These macros are called exactly like printf and prints to
/// stderr.
///

#ifdef DEBUG
#define dpr(fmt,...) formatted_print(__FILE__,__LINE__,__FUNCTION__,fmt,##__VA_ARGS__)
#define iodpr(in, msg, len) debug_print_io(__FILE__,__LINE__,__FUNCTION__, in, msg, len)
#else
inline void dpr(const char * fmt, ...) { (void)fmt; return; }
inline void iodpr(bool in, const void *msg, size_t len) { (void)msg; (void)len; return; }
#endif

/// @brief Always print
#define HARDPR(fmt,...) formatted_print(__FILE__,__LINE__,__FUNCTION__,fmt,##__VA_ARGS__)

/// @brief Always print
#define HARDIODPR(in, msg, len) debug_print_io(__FILE__,__LINE__,__FUNCTION__, in, msg, len)

#endif // _DEBUG_HPP
