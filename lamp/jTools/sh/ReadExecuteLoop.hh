// ==================
// ReadExecuteLoop.hh
// ==================

#ifndef READEXECUTELOOP_HH
#define READEXECUTELOOP_HH

// Standard C++
#include <vector>

// POSeven
#include "POSeven/FileDescriptor.hh"


extern int gLastResult;

enum PromptLevel
{
	kPS1 = 1,
	kPS2 = 2,
	kPS3 = 3,
	kPS4 = 4
};

void SetPromptLevel( PromptLevel level );

int ReadExecuteLoop( poseven::fd_t  fd,
                     bool           prompts );

#endif

