//
//  debug.cpp
//  utilities
//
//  Created by Six on 2/23/2018.
//  Copyright 2/23/2018 Six. All rights reserved.
//

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <memory>
#include <string>
#include <time.h>
#include <ctype.h>
#include <libgen.h>
#include "string_utils.hpp"
#include "buffer.hpp"

/// @brief A memory-safe pointer to a c string
typedef std::unique_ptr< std::string::value_type, std::default_delete<std::string::value_type[]> > safe_c_string;

////////////////////////////////////////////////////////////////////////////////
// make_time_str
////////////////////////////////////////////////////////////////////////////////
static std::string make_time_str(void) {

  time_t rawtime;
  struct tm *tme;
  time (&rawtime);
  tme = localtime(&rawtime);
  return util_sprintf("<%02d/%02d/%4d %02d:%02d:%02d>",tme->tm_mon+1,tme->tm_mday,tme->tm_year+1900,
                                                      tme->tm_hour,tme->tm_min,tme->tm_sec);
} // end time_str

////////////////////////////////////////////////////////////////////////////////
// make_unformatted_string - for when you have a message already formatted
////////////////////////////////////////////////////////////////////////////////
static std::string make_unformatted_string(const char *file, int line, const char *func, const char *msg) {
  const std::string time_str = make_time_str();
  std::shared_ptr<char> file_base = util_strdup(file);
  return util_sprintf("%s %s:%d:[%s]: %s", time_str.c_str(), basename(file_base.get()), line, func, msg);
} // end make_unformatted_string

////////////////////////////////////////////////////////////////////////////////
// vmake_formatted_string - for when you have sprintf-like formatting
// IMPORTANT NOTES:
// call va_start() first before calling this function
// call va_end() after calling this function
////////////////////////////////////////////////////////////////////////////////
static std::string vmake_formatted_string(const char *file, int line, const char *func, const char *fmt, int fmt_len, va_list args) {
  safe_c_string fmt_str(new std::string::value_type[fmt_len+1]);
  (void)vsnprintf(fmt_str.get(),fmt_len+1,fmt,args);
  return make_unformatted_string(file, line, func, fmt_str.get());
} // end vmake_formatted_string

////////////////////////////////////////////////////////////////////////////////
// formatted_print -- for when you need to log stuff a-la fprintf
////////////////////////////////////////////////////////////////////////////////
void formatted_print(const char *file, int line, const char *func, const char *fmt, ...) {
  va_list args;
  va_start(args,fmt);
  const int fmt_len = vsnprintf(NULL, 0, fmt, args);
  va_end(args);
  va_start(args,fmt);
  fprintf(stderr, "%s", vmake_formatted_string(file, line, func, fmt, fmt_len, args).c_str());
  va_end(args);
} // end formatted_print

////////////////////////////////////////////////////////////////////////////////
// debug_print_io
////////////////////////////////////////////////////////////////////////////////
void debug_print_io(const char *file, int line, const char *func, bool in, const void *msg, size_t len) {
  const char *dir = (in ? "<--" : "-->");
  Buffer<uint8_t> buf;
  buf.wrap(msg, len);

  size_t offset = 0;
  std::string chars;
  std::string hex;
  std::string tmp;
  std::string log_msg;
  const std::string time_string(make_time_str());

  // do 4 byte chunks while we can
  while(offset < len) {
    const uint8_t u = *buf.alias<uint8_t>(offset, &offset);
    hex += util_sprintf("%02X", u);
    if(isprint(u)) { chars.push_back(char(u)); }
    else           { chars.push_back('.'); }
  } // end while

  util_sprintf(log_msg, "%s %s %s %s\n", time_string.c_str(), dir, hex.c_str(), chars.c_str());
  fprintf(stderr, "%s", log_msg.c_str());
} // end debug_print_io
