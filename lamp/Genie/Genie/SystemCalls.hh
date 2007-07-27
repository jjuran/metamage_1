/*	==============
 *	SystemCalls.hh
 *	==============
 */

#ifndef GENIE_SYSTEMCALLS_HH
#define GENIE_SYSTEMCALLS_HH

namespace Genie
{
	
	int GetErrnoFromExceptionInSystemCall();
	
	class Process;
	
	class SystemCallFrame
	{
		private:
			Process& itsCaller;
			
			// Non-copyable
			SystemCallFrame           ( const SystemCallFrame& );
			SystemCallFrame& operator=( const SystemCallFrame& );
		
		public:
			SystemCallFrame( const char* name );
			~SystemCallFrame();
			
			Process& Caller() const  { return itsCaller; }
			
			int SetErrno( int errorNumber ) const;
	};
	
}

#endif

