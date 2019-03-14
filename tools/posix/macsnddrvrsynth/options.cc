/*
	options.cc
	----------
*/

// POSIX
#include <unistd.h>

// macsnddrvrsynth
#include "options.hh"


int in_fd = STDIN_FILENO;
int out_fd = STDOUT_FILENO;
bool stream = false;
int verbosity = 1;
