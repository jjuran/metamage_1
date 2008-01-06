/*	==============
 *	TraceTarget.hh
 *	==============
 */

#ifndef GENIE_PROCESS_TRACETARGET_HH
#define GENIE_PROCESS_TRACETARGET_HH

// POSIX
#include <sys/types.h>


namespace Genie
{
	
	class TraceTarget
	{
		private:
			pid_t itsTracingProcess;
		
		public:
			TraceTarget() : itsTracingProcess()  {}
			
			pid_t GetTracingProcess() const  { return itsTracingProcess; }
			bool IsBeingTraced() const  { return itsTracingProcess != 0; }
			
			void StartTracing( pid_t tracingProcess )  { itsTracingProcess = tracingProcess; }
			
			void StopTracing()  { itsTracingProcess = 0; }
	};
	
}

#endif

