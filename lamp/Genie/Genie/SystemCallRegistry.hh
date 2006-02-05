/*	=====================
 *	SystemCallRegistry.hh
 *	=====================
 */

namespace Genie
{
	
	// Register system calls
	
	#define REGISTER_SYSTEM_CALL(call)  SystemCall call##_syscall_(#call, call)
	
	void RegisterSystemCall( const char* name, void* func );
	
	class SystemCall
	{
		public:
			SystemCall( const char* name, void* func )
			{
				RegisterSystemCall( name, func );
			}
	};
	
	// Iterate over registered system calls
	
	typedef std::map< const char*, void* > SystemCallRegistry;
	
	SystemCallRegistry::const_iterator SystemCallsBegin();
	SystemCallRegistry::const_iterator SystemCallsEnd();
	
}

