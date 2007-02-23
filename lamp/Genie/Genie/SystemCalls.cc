/*	==============
 *	SystemCalls.cc
 *	==============
 */

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include "sys/ioctl.h"
#include "unistd.h"

// Nucleus
#include "Nucleus/NAssert.h"

// Io
#include "io/io.hh"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / AEFramework
#include "AEFramework/AEFramework.h"

// POSeven
#include "POSeven/Errno.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Kerosene/Common
#include "KEnvironment.hh"
#include "SystemCalls.hh"

// Pedestal
#include "Pedestal/Application.hh"

// Genie
#include "Genie/FileDescriptors.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/IO/Pipe.hh"
#include "Genie/IO/RegularFile.hh"
#include "Genie/IO/TTY.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


extern "C"
{
	OSStatus Path2FSSpec( const char*  pathname,
	                      FSSpec*      outFSS );
}

namespace Nucleus
{
	
	template <>
	struct Converter< POSeven::Errno, io::end_of_input > : public std::unary_function< io::end_of_input, POSeven::Errno >
	{
		POSeven::Errno operator()( io::end_of_input ) const
		{
			return POSeven::Errno( 0 );
		}
	};
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	namespace P7 = POSeven;
	
	namespace Ped = Pedestal;
	
	
	int GetErrnoFromExceptionInSystemCall()
	{
		NN::RegisterExceptionConversion< P7::Errno, N::OSStatus          >();
		NN::RegisterExceptionConversion< P7::Errno, io::end_of_input     >();
		NN::RegisterExceptionConversion< P7::Errno, io::no_input_pending >();
		
		P7::Errno errnum = NN::Convert< P7::Errno >( NN::TheExceptionBeingHandled(), EINVAL );
		
		return CurrentProcess().SetErrno( errnum );
	}
	
	
	#pragma mark -
	#pragma mark ¥ Genie ¥
	
	/*
	static void ListOneProcess( const Process* proc, const Io::Stream< IORef >& Out )
	{
		int pid  = proc->ProcessID();
		int ppid = proc->ParentProcessID();
		int pgid = proc->GetPGID();
		int sid  = proc->GetSID();
		
		TTYHandle* tty = proc->ControllingTerminal();
		
		const char* dev = tty ? tty->TTYName() : "?";
		
		const std::string& name = proc->ProgramName();
		
		Out << ppid << "  "
		    << pid  << "  "
		    << pgid << "  "
		    << sid  << "  "
		    << dev  << "  "
		    << name << "\n";
	}
	*/
	
	static int* ErrnoPtr()
	{
		return CurrentProcess().ErrnoData();
	}
	
	REGISTER_SYSTEM_CALL( ErrnoPtr );
	
	static char*** EnvironPtr()
	{
		return CurrentProcess().EnvironData();
	}
	
	REGISTER_SYSTEM_CALL( EnvironPtr );
	
	static int pause();
	
	static OSStatus AESendBlocking( const AppleEvent* appleEvent, AppleEvent* reply )
	{
		try
		{
			(void) N::AESend( *appleEvent, N::kAEQueueReply | N::kAECanInteract );
			
			// Now that we've sent the event, retrieve the return ID
			N::AEReturnID returnID = N::AEGetAttributePtr< N::keyReturnIDAttr >( *appleEvent );
			
			// Subscribe to AEFramework's queued reply delivery and wake-up service
			N::ExpectReply( returnID, reply );
			
			// Sleep until the reply is delivered
			pause();
		}
		catch ( const N::OSStatus& err )
		{
			return err;
		}
		
		return noErr;
	}
	
	REGISTER_SYSTEM_CALL( AESendBlocking );
	
	static OSStatus Path2FSSpec( const char*  pathname,
	                             FSSpec*      outFSS )
	{
		try
		{
			*outFSS = ResolvePathname( pathname, CurrentProcess().CurrentWorkingDirectory() )->GetFSSpec();
		}
		catch ( const N::OSStatus& err )
		{
			return CurrentProcess().SetErrno( err );
		}
		catch ( ... )
		{
			return extFSErr;  // Just a guess
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( Path2FSSpec );
	
	
	#pragma mark -
	#pragma mark ¥ unistd ¥
	
	static int chdir( const char* pathname )
	{
		try
		{
			if ( pathname == NULL )
			{
				return CurrentProcess().SetErrno( EINVAL );
			}
			
			FSTreePtr newCWD = ResolvePathname( pathname, CurrentProcess().CurrentWorkingDirectory() );
			
			if ( newCWD->IsLink() )
			{
				newCWD = newCWD->ResolveLink();
			}
			
			CurrentProcess().ChangeDirectory( newCWD );
			
			return 0;
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
	}
	
	REGISTER_SYSTEM_CALL( chdir );
	
	
	static int close( int fd )
	{
		try
		{
			CloseFileDescriptor( fd );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( close );
	
	static int DupFD( int oldfd, int newfd )
	{
		try
		{
			AssignFileDescriptor( newfd, GetFileHandle( oldfd ) );  // Clears the closeOnExec flag
			
			return newfd;
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
	}
	
	static int dup( int oldfd )
	{
		return DupFD( oldfd, LowestUnusedFileDescriptor() );
	}
	
	REGISTER_SYSTEM_CALL( dup );
	
	static int dup2( int oldfd, int newfd )
	{
		return DupFD( oldfd, newfd );
	}
	
	REGISTER_SYSTEM_CALL( dup2 );
	
	static int execve( const char* path, const char* const argv[], const char* const envp[] )
	{
		try
		{
			Process& current( CurrentProcess() );
			
			bool forked = current.Forked();
			
			NN::Owned< N::ThreadID > savedThread;
			
			// Local scope to make sure progFile gets destroyed
			{
				FSTreePtr progFile = ResolvePathname( path, current.CurrentWorkingDirectory() );
				
				// Start a new thread with the child's process context
				
				//progFile->Exec( argv, envp );
				
				savedThread = current.Exec( progFile->GetFSSpec(), argv, envp );
			}
			
			// If this thread is us (i.e. we didn't fork), we're now toast.
			if ( savedThread.Get() != N::kNoThreadID )
			{
				N::DisposeThread( savedThread, NULL, false );
			}
			
			// A non-forked exec kills its own thread and doesn't return
			ASSERT( forked );
			
			// Hope nothing bad happened while we thought we were still the child
			
			// Set this thread's process context back to the forker
			Process* parent = &gProcessTable[ current.ParentProcessID() ];
			
			RegisterProcessContext( parent );
			
			// Yes, in Genie a forked exec() DOES return on success.
			return 0;
		}
		catch ( const N::OSStatus& err )
		{
			std::string errMsg = "\n";
			
			try
			{
				throw;
			}
			catch ( const N::ErrMessage& msg )
			{
				errMsg = ", errMessage: " + NN::Convert< std::string >( msg.errMessage ) + "\n";
			}
			catch ( ... )  {}
			
			if ( err.Get() != fnfErr )
			{
				std::printf( "OSStatus %d%s", int( err.Get() ), errMsg.c_str() );
			}
		}
		catch ( ... )  {}
		
		return GetErrnoFromExceptionInSystemCall();
	}
	
	REGISTER_SYSTEM_CALL( execve );
	
	static void _exit( int status )
	{
		Process& current( CurrentProcess() );
		
		current.Terminate( (status & 0xFF) << 8 );  // doesn't reap, won't terminate thread
		
		if ( current.Forked() )
		{
			RegisterProcessContext( &gProcessTable[ current.ParentProcessID() ] );
			
			return;
		}
		
		while ( true )
		{
			// Stay awhile...  Stay forever!
			Yield();
		}
	}
	
	REGISTER_SYSTEM_CALL( _exit );
	
	static char* getcwd( char* buf, std::size_t size )
	{
		try
		{
			FSTreePtr cwd = CurrentProcess().CurrentWorkingDirectory();
			
			std::string result = cwd->Pathname();
			
			if ( result.size() + 1 > size )
			{
				return NULL;
			}
			
			std::copy( result.c_str(),
			           result.c_str() + result.size() + 1,
			           buf );
			
			return buf;
		}
		catch ( ... )
		{
			return NULL;
		}
	}
	
	REGISTER_SYSTEM_CALL( getcwd );
	
	static pid_t getpgrp();
	
	static pid_t getpgid( pid_t pid )
	{
		if ( pid == 0 )
		{
			return getpgrp();
		}
		
		return gProcessTable[ pid ].GetPGID();
	}
	
	REGISTER_SYSTEM_CALL( getpgid );
	
	static pid_t getpgrp()
	{
		return CurrentProcess().GetPGID();
	}
	
	REGISTER_SYSTEM_CALL( getpgrp );
	
	static pid_t getpid()
	{
		return CurrentProcess().ProcessID();
	}
	
	REGISTER_SYSTEM_CALL( getpid );
	
	static pid_t getppid()
	{
		return CurrentProcess().ParentProcessID();
	}
	
	REGISTER_SYSTEM_CALL( getppid );
	
	static off_t lseek( int fd, off_t offset, int whence )
	{
		try
		{
			RegularFileHandle& fh = GetFileHandleWithCast< RegularFileHandle >( fd );
			
			return fh.Seek( offset, whence );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return CurrentProcess().SetErrno( ESPIPE );
	}
	
	REGISTER_SYSTEM_CALL( lseek );
	
	static int pause()
	{
		CurrentProcess().Stop();  // Sleep, until...
		
		return CurrentProcess().SetErrno( EINTR );
	}
	
	REGISTER_SYSTEM_CALL( pause );
	
	static int pipe( int filedes[ 2 ] )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		int reader = LowestUnusedFileDescriptor( 3 );
		int writer = LowestUnusedFileDescriptor( reader + 1 );
		
		boost::shared_ptr< PipeState > pipeState( new PipeState );
		
		boost::shared_ptr< IOHandle > pipeIn ( new PipeInHandle ( pipeState ) );
		boost::shared_ptr< IOHandle > pipeOut( new PipeOutHandle( pipeState ) );
		
		AssignFileDescriptor( reader, pipeOut );
		AssignFileDescriptor( writer, pipeIn );
		
		filedes[ 0 ] = reader;
		filedes[ 1 ] = writer;
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( pipe );
	
	static int peek( int fd, const char** buffer, size_t minBytes )
	{
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			const std::string& peekBuffer = stream.Peek( minBytes );
			
			if ( buffer != NULL )
			{
				*buffer = peekBuffer.c_str();
			}
			
			return peekBuffer.size();
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
	}
	
	REGISTER_SYSTEM_CALL( peek );
	
	static ssize_t read( int fd, void* buf, size_t count )
	{
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			int get = stream.Read( reinterpret_cast< char* >( buf ),
			                       count );
			
			return get;
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
	}
	
	REGISTER_SYSTEM_CALL( read );
	
	static int setpgid( pid_t pid, pid_t pgid )
	{
		try
		{
			Process& target( pid != 0 ? gProcessTable[ pid ]
			                          : CurrentProcess() );
			
			pid = target.ProcessID();
			
			target.SetPGID( pgid != 0 ? pgid : pid );
			
			return pid;
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
	}
	
	REGISTER_SYSTEM_CALL( setpgid );
	
	static pid_t setsid()
	{
		Process& current( CurrentProcess() );
		
		current.SetControllingTerminal( NULL );
		
		int pid = current.ProcessID();
		
		current.SetPGID( pid );
		current.SetSID ( pid );
		
		return pid;
	}
	
	REGISTER_SYSTEM_CALL( setsid );
	
	static unsigned int sleep( unsigned int seconds )
	{
		SInt32 remaining = seconds * 60;  // Close enough
		
		UInt32 startTicks = ::TickCount();
		
		UInt32 endTicks = startTicks + remaining;
		
		try
		{
			// Yield at least once, even for 0 seconds
			do
			{
				Ped::AdjustSleepForTimer( remaining );
				
				Yield();
				
				remaining = endTicks - ::TickCount();
			}
			while ( remaining > 0 );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( sleep );
	
	static const char* ttyname( int fd )
	{
		try
		{
			TTYHandle& tty = GetFileHandleWithCast< TTYHandle >( fd );
			
			return tty.TTYName().c_str();
		}
		catch ( ... ) {}
		
		return NULL;
	}
	
	REGISTER_SYSTEM_CALL( ttyname );
	
	/*
	// ttypair() is analogous to socketpair(), and creates a pseudo-tty device.
	// File descriptors refering to the master and slave respectively are returned
	// in filedes.
	// I made this up too.
	static int ttypair( int filedes[ 2 ] )
	{
		FileDescriptorMap& files = CurrentProcess().FileDescriptors();
		
		NewPseudoTerminal_Result fds = NewPseudoTerminal();
		
		int fd0 = LowestUnusedFrom( files, 0 );
		int fd1 = LowestUnusedFrom( files, fd0 + 1 );
		
		files[ fd0 ] = fds.first;
		files[ fd1 ] = fds.second;
		
		filedes[ 0 ] = fd0;
		filedes[ 1 ] = fd1;
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( ttypair );
	*/
	
	static ssize_t write( int fd, const void* buf, size_t count )
	{
		try
		{
			StreamHandle& stream = GetFileHandleWithCast< StreamHandle >( fd );
			
			const char* data = reinterpret_cast< const char* >( buf );
			
			int put = stream.Write( data, count );
			
			return put;
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
	}
	
	REGISTER_SYSTEM_CALL( write );
	
}

