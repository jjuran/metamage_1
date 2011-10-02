/*
	syscall-bridge.cc
	-----------------
*/

// Standard C
#include <errno.h>
#include <stdlib.h>

// POSIX
#include <unistd.h>

// v68k-exec
#include "syscall-bridge.hh"


#pragma exceptions off


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
	
	const uint32_t errno_ptr_addr = params_addr + 2 * sizeof (uint32_t);
	
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

bool bridge_call( v68k::emulator& emu )
{
	const uint16_t call_number = emu.regs.d[0];
	
	switch ( call_number )
	{
		case 1:  return emu_exit ( emu );
		case 3:  return emu_read ( emu );
		case 4:  return emu_write( emu );
		
		default:
			return false;
	}
}

