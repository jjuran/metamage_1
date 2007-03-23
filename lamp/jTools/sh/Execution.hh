// ============
// Execution.hh
// ============

#ifndef EXECUTION_HH
#define EXECUTION_HH

// Standard C++
#include <string>
#include <vector>


bool GetInteractiveness();
void SetInteractiveness( bool interactive );

bool GetWhetherToExitOnBatchError();
void SetWhetherToExitOnBatchError( bool toExit );

int ExecuteCmdLine( const std::string& cmd );

#endif

