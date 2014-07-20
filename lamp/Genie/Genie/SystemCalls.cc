/*	==============
 *	SystemCalls.cc
 *	==============
 */

// relix-kernel
#include "relix/syscall/alarm.hh"
#include "relix/syscall/chdir.hh"
#include "relix/syscall/close.hh"
#include "relix/syscall/dup3.hh"
#include "relix/syscall/_exit.hh"
#include "relix/syscall/faccessat.hh"
#include "relix/syscall/fchmod.hh"
#include "relix/syscall/fchmodat.hh"
#include "relix/syscall/fcntl.hh"
#include "relix/syscall/fstat.hh"
#include "relix/syscall/fstatat.hh"
#include "relix/syscall/fsync.hh"
#include "relix/syscall/ftruncate.hh"
#include "relix/syscall/getdents.hh"
#include "relix/syscall/getpgid.hh"
#include "relix/syscall/getpid.hh"
#include "relix/syscall/getppid.hh"
#include "relix/syscall/getsid.hh"
#include "relix/syscall/gettid.hh"
#include "relix/syscall/ioctl.hh"
#include "relix/syscall/lseek.hh"
#include "relix/syscall/mkdirat.hh"
#include "relix/syscall/mknodat.hh"
#include "relix/syscall/mmap.hh"
#include "relix/syscall/msync.hh"
#include "relix/syscall/munmap.hh"
#include "relix/syscall/openat.hh"
#include "relix/syscall/pipe2.hh"
#include "relix/syscall/pread.hh"
#include "relix/syscall/pump.hh"
#include "relix/syscall/pwrite.hh"
#include "relix/syscall/read.hh"
#include "relix/syscall/readlinkat.hh"
#include "relix/syscall/registry.hh"
#include "relix/syscall/renameat.hh"
#include "relix/syscall/setpgid.hh"
#include "relix/syscall/setsid.hh"
#include "relix/syscall/sigsuspend.hh"
#include "relix/syscall/symlinkat.hh"
#include "relix/syscall/times.hh"
#include "relix/syscall/truncate.hh"
#include "relix/syscall/unlinkat.hh"
#include "relix/syscall/unshare.hh"
#include "relix/syscall/utimensat.hh"
#include "relix/syscall/write.hh"
#include "relix/syscall/writev.hh"


extern "C" void sync();


namespace Genie
{
	
	using relix::alarm;
	using relix::chdir;
	using relix::close;
	using relix::dup3;
	using relix::_exit;
	using relix::faccessat;
	using relix::fchmod;
	using relix::fchmodat;
	using relix::fcntl;
	using relix::fstat;
	using relix::fstatat;
	using relix::fsync;
	using relix::ftruncate;
	using relix::getdents;
	using relix::getpgid;
	using relix::getpid;
	using relix::getppid;
	using relix::getsid;
	using relix::gettid;
	using relix::ioctl;
	using relix::lseek;
	using relix::mkdirat;
	using relix::mknodat;
	using relix::msync;
	using relix::munmap;
	using relix::openat;
	using relix::pipe2;
	using relix::pread;
	using relix::pump;
	using relix::pwrite;
	using relix::read;
	using relix::_readlinkat;
	using relix::_relix_mmap;
	using relix::renameat;
	using relix::setpgid;
	using relix::setsid;
	using relix::symlinkat;
	using relix::times;
	using relix::truncate;
	using relix::unlinkat;
	using relix::unshare;
	using relix::utimensat;
	using relix::write;
	using relix::writev;
	
	
	static int fdatasync( int fd )
	{
		return fsync( fd );
	}
	
	static int pause()
	{
		return relix::sigsuspend( 0 );  // NULL
	}
	
	
	#pragma force_active on
	
	REGISTER_SYSTEM_CALL( alarm       );
	REGISTER_SYSTEM_CALL( chdir       );
	REGISTER_SYSTEM_CALL( close       );
	REGISTER_SYSTEM_CALL( dup3        );
	REGISTER_SYSTEM_CALL( _exit       );
	REGISTER_SYSTEM_CALL( faccessat   );
	REGISTER_SYSTEM_CALL( fchmod      );
	REGISTER_SYSTEM_CALL( fchmodat    );
	REGISTER_SYSTEM_CALL( fcntl       );
	REGISTER_SYSTEM_CALL( fdatasync   );
	REGISTER_SYSTEM_CALL( fstat       );
	REGISTER_SYSTEM_CALL( fstatat     );
	REGISTER_SYSTEM_CALL( fsync       );
	REGISTER_SYSTEM_CALL( ftruncate   );
	REGISTER_SYSTEM_CALL( getdents    );
	REGISTER_SYSTEM_CALL( getpgid     );
	REGISTER_SYSTEM_CALL( getpid      );
	REGISTER_SYSTEM_CALL( getppid     );
	REGISTER_SYSTEM_CALL( getsid      );
	REGISTER_SYSTEM_CALL( gettid      );
	REGISTER_SYSTEM_CALL( ioctl       );
	REGISTER_SYSTEM_CALL( lseek       );
	REGISTER_SYSTEM_CALL( mkdirat     );
	REGISTER_SYSTEM_CALL( mknodat     );
	REGISTER_SYSTEM_CALL( munmap      );
	REGISTER_SYSTEM_CALL( msync       );
	REGISTER_SYSTEM_CALL( openat      );
	REGISTER_SYSTEM_CALL( pause       );
	REGISTER_SYSTEM_CALL( pipe2       );
	REGISTER_SYSTEM_CALL( pread       );
	REGISTER_SYSTEM_CALL( pump        );
	REGISTER_SYSTEM_CALL( pwrite      );
	REGISTER_SYSTEM_CALL( read        );
	REGISTER_SYSTEM_CALL( _readlinkat );
	REGISTER_SYSTEM_CALL( _relix_mmap );
	REGISTER_SYSTEM_CALL( renameat    );
	REGISTER_SYSTEM_CALL( setpgid     );
	REGISTER_SYSTEM_CALL( setsid      );
	REGISTER_SYSTEM_CALL( symlinkat   );
	REGISTER_SYSTEM_CALL( sync        );
	REGISTER_SYSTEM_CALL( times       );
	REGISTER_SYSTEM_CALL( truncate    );
	REGISTER_SYSTEM_CALL( unlinkat    );
	REGISTER_SYSTEM_CALL( unshare     );
	REGISTER_SYSTEM_CALL( utimensat   );
	REGISTER_SYSTEM_CALL( write       );
	REGISTER_SYSTEM_CALL( writev      );
	
	#pragma force_active reset
	
}
