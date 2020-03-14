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
#include <signal.h>
#include <libgen.h>
#include <string>
#include "utils/debug.hpp"
#include "utils/string_utils.hpp"
#include "mongoose.h"

struct Options {
  std::string ui_dir; //!< Path to where the UI lives
  std::string web_port; //!< Port for the web server, defaults to 80
  std::string web_addr; //!< Bind address for web server, defaults to 0.0.0.0 (any address)
  bool error;
};

////////////////////////////////////////////////////////////////////////////////
// Stop when this is false
////////////////////////////////////////////////////////////////////////////////
static bool run = true;

////////////////////////////////////////////////////////////////////////////////
// on_sigint
////////////////////////////////////////////////////////////////////////////////
static void on_sigint(int signal) {
  (void)signal; // not used
  if(!run) { exit(-1); }
  run = false;
} // end on_sigint

////////////////////////////////////////////////////////////////////////////////
// on_sigterm
////////////////////////////////////////////////////////////////////////////////
static void on_sigterm(int signal) {
  (void)signal; // not used
  if(!run) { exit(-1); }
  run = false;
} // end on_sigterm


////////////////////////////////////////////////////////////////////////////////
// setup_signals
////////////////////////////////////////////////////////////////////////////////
static void setup_signals(void) {
  struct sigaction action;

  memset(&action, 0, sizeof(action));
  action.sa_handler = on_sigint;
  sigaction(SIGINT, &action, nullptr);

  memset(&action, 0, sizeof(action));
  action.sa_handler = on_sigterm;
  sigaction(SIGTERM, &action, nullptr);

  memset(&action, 0, sizeof(action));
  action.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &action, nullptr); // retarded bsd behavior from way back is still inconsistent today

} // end setup_signals

////////////////////////////////////////////////////////////////////////////////
// parse_command_line
////////////////////////////////////////////////////////////////////////////////
static Options parse_command_line(int argc, char* argv[]) {
  std::shared_ptr<char> prog = util_strdup(argv[0]);
  Options o;
  o.error = false;
  o.ui_dir = dirname(prog.get());
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
      o.error = true;
      break;

      default:
      o.error = true;
      break;
      } // end switch option
  } // end while
  return o;
} // end parse_command_line

////////////////////////////////////////////////////////////////////////////////
// ev_handler
////////////////////////////////////////////////////////////////////////////////
static void ev_handler(struct mg_connection *c, int ev, void *p) {
  if(ev != MG_EV_HTTP_REQUEST) {
    return;
  }

  const Options* o = reinterpret_cast<const Options*>(c->mgr->user_data);
  struct http_message *hm = (struct http_message *) p;
  struct mg_serve_http_opts opts = {0};
  opts.document_root = o->ui_dir.c_str();

  iodpr(true, hm->message.p, hm->message.len);
  dpr("method        : %.*s\n", int(hm->method.len), hm->method.p);
  dpr("uri           : %.*s\n", int(hm->uri.len), hm->uri.p);
  dpr("proto         : %.*s\n", int(hm->proto.len), hm->proto.p);
  dpr("query_string  : %.*s\n", int(hm->query_string.len), hm->query_string.p);

  mg_serve_http(c, hm, opts);
} // end ev_handler
////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
  dpr("Startup\n");
  setup_signals();

  Options o = parse_command_line(argc, argv);
  if(o.error) {
    fprintf(stderr, "Error parsing options\n");
    return EXIT_FAILURE;
  }
  dpr("ui_dir   = %s\n", o.ui_dir.c_str());
  dpr("web_port = %s\n", o.web_port.c_str());
  dpr("web_addr = %s\n", o.web_addr.c_str());

  struct mg_mgr mgr;
  struct mg_connection *c;

  std::string addr = util_sprintf("%s:%s", o.web_addr.c_str(), o.web_port.c_str());

  mg_mgr_init(&mgr, &o);
  c = mg_bind(&mgr, addr.c_str(), ev_handler);
  mg_set_protocol_http_websocket(c);

  while(run) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  dpr("Cleanup\n");
  return EXIT_SUCCESS;
} // end main
