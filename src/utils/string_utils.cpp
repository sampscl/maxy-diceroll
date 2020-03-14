//
//  string_utils.cpp
//  c2
//
//  Created by Six on 4/23/11.
//  Copyright 2011 Six. All rights reserved.
//
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "string_utils.hpp"

typedef std::unique_ptr< std::string::value_type, std::default_delete<std::string::value_type[]> > safe_c_string;

size_t util_sprintf(std::string& result, const char* fmt, ...) {

  va_list args;

  va_start(args,fmt);
  const int len = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  safe_c_string tmp(new std::string::value_type[len+1]);

  va_start(args,fmt);
  const size_t resultLen = vsnprintf(tmp.get(),len+1,fmt,args);
  va_end(args);

  result.assign(tmp.get());
  return resultLen;
}

std::string util_sprintf(const char* fmt,...) {
  std::string result;
  va_list args;

  va_start(args,fmt);
  const int len = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  safe_c_string tmp(new std::string::value_type[len+1]);

  va_start(args,fmt);
  (void)vsnprintf(tmp.get(),len+1,fmt,args);
  va_end(args);

  result.assign(tmp.get());
  return result;
}

std::shared_ptr<char> util_strdup(const std::string& s) {
  std::shared_ptr<char> result(new char[s.size() + 1], std::default_delete<char[]>());
  strcpy(result.get(), s.c_str());
  return result;
} // end strdup
