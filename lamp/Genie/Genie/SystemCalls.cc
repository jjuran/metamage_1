/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Debug
#include "debug/assert.hh"

// relix-kernel
#include "relix/syscall/alarm.hh"
#include "relix/syscall/chdir.hh"
#include "relix/syscall/close.hh"
#include "relix/syscall/dup3.hh"
#include "relix/syscall/ftruncate.hh"
#include "relix/syscall/getpgid.hh"
#include "relix/syscall/getpid.hh"
#include "relix/syscall/getppid.hh"
#include "relix/syscall/getsid.hh"
#include "relix/syscall/gettid.hh"
#include "relix/syscall/lseek.hh"
#include "relix/syscall/pipe2.hh"
#include "relix/syscall/pread.hh"
#include "relix/syscall/pwrite.hh"
#include "relix/syscall/read.hh"
#include "relix/syscall/setpgid.hh"
#include "relix/syscall/setsid.hh"
#include "relix/syscall/sigsuspend.hh"
#include "relix/syscall/truncate.hh"
#include "relix/syscall/write.hh"
#include "relix/syscall/writev.hh"

// Genie
#include "Genie/current_process.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"


namespace Genie
{
	
	using relix::alarm;
	using relix::chdir;
	using relix::close;
	using relix::dup3;
	
	
	static void _exit( int status )
	{
		// We don't return to the dispatcher, but Terminate() compensates.
		
		Process& current = current_process();
		
		current.Exit( status );  // doesn't return
		
		ASSERT( false && "Process::Exit() doesn't return" );
	}
	
	
	using relix::getpgid;
	using relix::getpid;
	using relix::getppid;
	using relix::getsid;
	using relix::gettid;
	using relix::lseek;
	
	
	static int pause()
	{
		return relix::sigsuspend( NULL );
	}
	
	
	using relix::pipe2;
	using relix::pread;
	using relix::read;
	using relix::setpgid;
	using relix::setsid;
	using relix::truncate;
	using relix::ftruncate;
	using relix::pwrite;
	using relix::write;
	using relix::writev;
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( alarm     );
	REGISTER_SYSTEM_CALL( chdir     );
	REGISTER_SYSTEM_CALL( close     );
	REGISTER_SYSTEM_CALL( dup3      );
	REGISTER_SYSTEM_CALL( _exit     );
	REGISTER_SYSTEM_CALL( getpgid   );
	REGISTER_SYSTEM_CALL( getpid    );
	REGISTER_SYSTEM_CALL( getppid   );
	REGISTER_SYSTEM_CALL( getsid    );
	REGISTER_SYSTEM_CALL( gettid    );
	REGISTER_SYSTEM_CALL( lseek     );
	REGISTER_SYSTEM_CALL( pause     );
	REGISTER_SYSTEM_CALL( pipe2     );
	REGISTER_SYSTEM_CALL( pread     );
	REGISTER_SYSTEM_CALL( read      );
	REGISTER_SYSTEM_CALL( setpgid   );
	REGISTER_SYSTEM_CALL( setsid    );
	REGISTER_SYSTEM_CALL( truncate  );
	REGISTER_SYSTEM_CALL( ftruncate );
	REGISTER_SYSTEM_CALL( write     );
	REGISTER_SYSTEM_CALL( writev    );
	REGISTER_SYSTEM_CALL( pwrite    );
	
	#pragma force_active reset
	
}
