/*	=====================
 *	SystemCallRegistry.hh
 *	=====================
 */

#ifndef GENIE_SYSTEMCALLREGISTRY_HH
#define GENIE_SYSTEMCALLREGISTRY_HH

// Standard C++
#include <vector>

// Lamp
#include "lamp/syscalls.h"


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
	
	#define REGISTER_SYSTEM_CALL(call)  SystemCallRegistration call##_syscall_( __NR_##call, #call, (void*) call )
	
	void RegisterSystemCall( unsigned index, const char* name, void* func );
	
	class SystemCallRegistration
	{
		public:
			SystemCallRegistration( unsigned index, const char* name, void* func )
			{
				RegisterSystemCall( index, name, func );
			}
	};
	
	struct SystemCall
	{
		const char*  name;
		void*        function;
		
		SystemCall() : name(), function()  {}
		
		SystemCall( const char* nom, void* f ) : name( nom ), function( f )  {}
	};
	
	typedef std::vector< SystemCall > SystemCallRegistry;
	
	const SystemCallRegistry& GetSystemCallRegistry();
	
	const SystemCall* GetSystemCall( unsigned index );
	
	const SystemCall* LookUpSystemCallByName( const char* name );
	
}

#endif

