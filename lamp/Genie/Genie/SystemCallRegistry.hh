/*	=====================
 *	SystemCallRegistry.hh
 *	=====================
 */

#ifndef GENIE_SYSTEMCALLREGISTRY_HH
#define GENIE_SYSTEMCALLREGISTRY_HH

namespace Genie
{
	
#if TARGET_CPU_68K && defined(__MWERKS__)
	
	#define DECLARE_MODULE_INIT(Module)  void Init_##Module##_();
	#define DEFINE_MODULE_INIT(Module)   void Init_##Module##_()  {}
	#define CALL_MODULE_INIT(Module)          Init_##Module##_()
	
	void InitKernelModules();
	
#else
	
	#define DECLARE_MODULE_INIT(Module)
	#define DEFINE_MODULE_INIT(Module)
	#define CALL_MODULE_INIT(Module)
	
	inline void InitKernelModules()  {}
	
#endif
	
	// Register system calls
	
	#define REGISTER_SYSTEM_CALL(call)  SystemCall call##_syscall_(#call, (void*) call)
	
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

#endif

