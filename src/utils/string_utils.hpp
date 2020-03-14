//
//  string_utils.hpp
//  c2
//
//  Created by Six on 1/7/2020.
//  Copyright 1/7/2020 Six. All rights reserved.
//
#ifndef _STRING_UTILS_HPP
#define _STRING_UTILS_HPP

#include <string>
#include <memory>

///
/// @brief Like sprintf
/// @param[out] result The result of sprintf
/// @param[in] fmt The format string, a-la sprintf
/// @param[in] ... Variable args a-la sprintf
/// @return Number of characters printed, excluding any null terminator
///
size_t util_sprintf(std::string& result, const char* fmt, ...);

///
/// @brief Like sprintf
/// @param[in] fmt The format string, a-la sprintf
/// @param[in] ... Variable args a-la sprintf
/// @return string formatted with sprintf
///
std::string util_sprintf(const char* fmt,...);

///
/// @brief A memory-safe strdup
/// @param[in] s The string to duplicate
/// @return A shared pointer to a null-terminated C string duplicated from s
///
std::shared_ptr<char> util_strdup(const std::string& s);


#endif // _STRING_UTILS_HPP
