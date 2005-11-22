/*	========
 *	Yield.hh
 *	========
 */

#ifndef GENIE_YIELD_HH
#define GENIE_YIELD_HH

#include "Nitrogen/Threads.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	class Process;
	
	Process& CurrentProcess();
	Process* CurrentProcessContext();
	
	void RegisterProcessContext( Process* process );
	
	void Yield();
	
	void StopThread( N::ThreadID thread );
	
	void ReadyThread( N::ThreadID thread );
	
}

#endif

