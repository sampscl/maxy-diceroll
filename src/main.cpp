//
//  main.cpp
//  maxy-diceroll
//
//  Created by Clay Sammpson on 3/14/2020.
//  Copyright 3/14/2020 Clay Sammpson. All rights reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string>
#include "utils/debug.hpp"

struct Options {
  std::string ui_dir; //!< Path to where the UI lives
  std::string web_port; //!< Port for the web server, defaults to 80
  std::string web_addr; //!< Bind address for web server, defaults to 0.0.0.0 (any address)
};

////////////////////////////////////////////////////////////////////////////////
// parse_command_line
////////////////////////////////////////////////////////////////////////////////
static Options parse_command_line(int argc, char* argv[]) {
  Options o;
  o.ui_dir =
  o.web_port = "80";
  o.web_addr = "0.0.0.0";

  int c;
  while (1) {
    static struct option long_options[] = {
      {"ui-dir", required_argument, 0, 'u'},
      {"port",   required_argument, 0, 'p'},
      {"addr",   required_argument, 0, 'a'},
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;
    c = getopt_long (argc, argv, "u:p:a:",
                     long_options, &option_index);
    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c) {
      case 'a':
      o.web_addr = optarg;
      break;

      case 'p':
      o.web_port = optarg;
      break;

      case 'u':
      o.ui_dir = optarg;
      break;

      case '?':
      /* getopt_long already printed an error message. */
      break;

      default:
        abort();
      } // end switch option
  } // end while
  return o;
} // end parse_command_line

////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  Options o = parse_command_line(argc, argv);

  dpr("ui_dir = %s\n", o.ui_dir.c_str());
  dpr("web_port = %s\n", o.web_port.c_str());
  dpr("web_addr = %s\n", o.web_addr.c_str());

  return EXIT_SUCCESS;
} // end main
