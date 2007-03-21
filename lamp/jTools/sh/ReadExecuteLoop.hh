// ==================
// ReadExecuteLoop.hh
// ==================

#ifndef READEXECUTELOOP_HH
#define READEXECUTELOOP_HH

// Standard C++
#include <vector>

// POSeven
#include "POSeven/FileDescriptor.hh"


namespace P7 = POSeven;

extern int gLastResult;

enum PromptLevel
{
	kPS1 = 1,
	kPS2 = 2,
	kPS3 = 3,
	kPS4 = 4
};

void SetPromptLevel( PromptLevel level );

void SetWhetherToExitOnBatchError( bool exit );

int ReadExecuteLoop( P7::FileDescriptor  fd,
                     bool                prompts );

#endif

