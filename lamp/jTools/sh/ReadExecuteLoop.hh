// ==================
// ReadExecuteLoop.hh
// ==================

#ifndef READEXECUTELOOP_HH
#define READEXECUTELOOP_HH

// Standard C++
#include <vector>

// Io
#include "Io/Handle.hh"

extern bool gInteractive;

int ReadExecuteLoop( const Io::Handle& in, const std::vector< const char* >& params );

#endif

