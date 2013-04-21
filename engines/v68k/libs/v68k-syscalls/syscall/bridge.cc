/*
	bridge.cc
	---------
*/

// Standard C
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

// POSIX
#include <unistd.h>
#include <sys/uio.h>

// v68k
#include "v68k/endian.hh"

// v68k-syscalls
#include "syscall/bridge.hh"


#pragma exceptions off


uint32_t errno_ptr_addr;

int32_t fake_pid = 12345;  // fake PID for getpid(), unless 0


static int32_t emulated_pid()
{
	return fake_pid > 0 ? fake_pid : getpid();
}

static bool get_stacked_args( const v68k::emulator& emu, uint32_t* out, int n )
{
	uint32_t sp = emu.regs.a[7];
	
	while ( n > 0 )
	{
		sp += 4;
		
		if ( !emu.mem.get_long( sp, *out, emu.data_space() ) )
		{
			return false;
		}
		
		++out;
		--n;
	}
	
	return true;
}

static void set_errno( v68k::emulator& emu )
{
	emu.regs.d[1] = errno;
	
	uint32_t errno_ptr;
	
	if ( emu.mem.get_long( errno_ptr_addr, errno_ptr, emu.data_space() )  &&  errno_ptr != 0 )
	{
		emu.mem.put_long( errno_ptr, errno, emu.data_space() );
	}
}

static inline bool set_result( v68k::emulator& emu, int result )
{
	emu.regs.d[0] = result;
	
	if ( result < 0 )
	{
		set_errno( emu );
	}
	
	return true;
}

static bool emu_exit( v68k::emulator& emu )
{
	uint32_t args[1];  // status
	
	if ( !get_stacked_args( emu, args, 1 ) )
	{
		return emu.bus_error();
	}
	
	const int status = int32_t( args[0] );
	
	exit( status );
	
	// Not reached
	return false;
}

static bool emu_read( v68k::emulator& emu )
{
	uint32_t args[3];  // fd, buffer, length
	
	if ( !get_stacked_args( emu, args, 3 ) )
	{
		return emu.bus_error();
	}
	
	const int fd = int32_t( args[0] );
	
	const uint32_t buffer = args[1];
	
	const size_t length = args[2];
	
	uint8_t* p = emu.mem.translate( buffer, length, emu.data_space(), v68k::mem_write );
	
	int result;
	
	if ( p == NULL )
	{
		result = -1;
		
		errno = EFAULT;
	}
	else
	{
		result = read( fd, p, length );
	}
	
	return set_result( emu, result );
}

static bool emu_write( v68k::emulator& emu )
{
	uint32_t args[3];  // fd, buffer, length
	
	if ( !get_stacked_args( emu, args, 3 ) )
	{
		return emu.bus_error();
	}
	
	const int fd = int32_t( args[0] );
	
	const uint32_t buffer = args[1];
	
	const size_t length = args[2];
	
	const uint8_t* p = emu.mem.translate( buffer, length, emu.data_space(), v68k::mem_read );
	
	int result;
	
	if ( p == NULL )
	{
		result = -1;
		
		errno = EFAULT;
	}
	else
	{
		result = write( fd, p, length );
	}
	
	return set_result( emu, result );
}

static bool emu_getpid( v68k::emulator& emu )
{
	int32_t result = emulated_pid();
	
	return set_result( emu, result );
}

static bool emu_kill( v68k::emulator& emu )
{
	uint32_t args[2];  // pid, sig
	
	if ( !get_stacked_args( emu, args, 2 ) )
	{
		return emu.bus_error();
	}
	
	pid_t     pid = int32_t( args[0] );
	const int sig = int32_t( args[1] );
	
	int result;
	
	if ( pid == emulated_pid() )
	{
		if ( fake_pid > 0  &&  pid == fake_pid )
		{
			pid = getpid();
		}
		
		result = kill( pid, sig );
	}
	else
	{
		result = -1;
		
		errno = EPERM;
	}
	
	return set_result( emu, result );
}

struct iovec_68k
{
	uint32_t ptr;
	uint32_t len;
};

static bool emu_writev( v68k::emulator& emu )
{
	uint32_t args[3];  // fd, iov, n
	
	if ( !get_stacked_args( emu, args, 3 ) )
	{
		return emu.bus_error();
	}
	
	int result = -1;
	
	const int fd = int32_t( args[0] );
	
	const uint32_t iov_addr = args[1];
	
	const size_t n = args[2];
	
	const size_t size = n * sizeof (iovec_68k);
	
	struct iovec* iov = (struct iovec*) malloc( sizeof (struct iovec) * n );
	
	const iovec_68k* iov_mem = (iovec_68k*) emu.mem.translate( iov_addr,
	                                                           size,
	                                                           emu.data_space(),
	                                                           v68k::mem_read );
	
	if ( iov == NULL )
	{
		errno = ENOMEM;
		
		goto end;
	}
	
	if ( iov_mem == NULL )
	{
		errno = EFAULT;
		
		goto end;
	}
	
	for ( int i = 0;  i < n;  ++i )
	{
		const uint32_t ptr = v68k::longword_from_big( iov_mem[i].ptr );
		const uint32_t len = v68k::longword_from_big( iov_mem[i].len );
		
		const uint8_t* p = emu.mem.translate( ptr, len, emu.data_space(), v68k::mem_read );
		
		if ( p == NULL )
		{
			errno = EFAULT;
			
			goto end;
		}
		
		iov[i].iov_base = (void*) p;
		iov[i].iov_len  = len;
	}
	
	result = writev( fd, iov, n );
	
end:
	
	free( iov );
	
	return set_result( emu, result );
}

bool bridge_call( v68k::emulator& emu )
{
	const uint16_t call_number = emu.regs.d[0];
	
	switch ( call_number )
	{
		case 1:  return emu_exit ( emu );
		case 3:  return emu_read ( emu );
		case 4:  return emu_write( emu );
		
		case 20:  return emu_getpid( emu );
		case 37:  return emu_kill  ( emu );
		
		case 146:  return emu_writev( emu );
		
		default:
			return false;
	}
}

