/*	=============
 *	ConsoleTTY.cc
 *	=============
 */

#include "Genie/IO/ConsoleTTY.hh"

// Standard C
#include <errno.h>

// Lamp
#include <lamp/winio.h>

// Nucleus
#include "Nucleus/Convert.h"

// POSeve
#include "POSeven/Errno.hh"

// Genie
#include "Genie/IO/ConsoleWindow.hh"
#include "Genie/IO/WindowLiberty.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static ConsoleMap gConsoleMap;
	
	const ConsoleMap& GetConsoleMap()
	{
		return gConsoleMap;
	}
	
	
	boost::shared_ptr< IOHandle > NewConsoleDevice()
	{
		static ConsoleID gLastID = 0;
		
		boost::shared_ptr< IOHandle > consoleDevice( new ConsoleTTYHandle( ++gLastID ) );
		
		gConsoleMap[ gLastID ] = consoleDevice;
		
		return consoleDevice;
	}
	
	TerminalHandle& GetConsoleByID( ConsoleID id )
	{
		ConsoleMap::const_iterator it = gConsoleMap.find( id );
		
		if ( it == gConsoleMap.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		ASSERT( !it->second.expired() );
		
		return IOHandle_Cast< TerminalHandle >( *it->second.lock() );
	}
	
	
	inline void LiberateConsole( const boost::shared_ptr< ConsoleWindow >& window )
	{
		ASSERT( window.get() != NULL );
		
		LiberateWindow( *window, window );
	}
	
	ConsoleTTYHandle::ConsoleTTYHandle( ConsoleID id ) : itsID( id )
	{
	}
	
	ConsoleTTYHandle::~ConsoleTTYHandle()
	{
		if ( itsWindow.get()  &&  itsWindow->ShouldBeSalvaged() )
		{
			std::string exCon = "(" + NN::Convert< std::string >( itsWindow->GetLeaderWaitStatus() ) + ")";
			
			itsWindow->SetTitle( N::Str255( exCon ) );
			
			LiberateConsole( itsWindow );
		}
		
		gConsoleMap.erase( itsID );
	}
	
	IOHandle* ConsoleTTYHandle::Next() const
	{
		return itsWindow.get();
	}
	
	FSTreePtr ConsoleTTYHandle::GetFile() const
	{
		return itsWindow->GetFile();
	}
	
	bool ConsoleTTYHandle::IsDisconnected() const
	{
		return itsWindow.get() && itsWindow->IsDisconnected();
	}
	
	unsigned int ConsoleTTYHandle::SysPoll() const
	{
		bool readable = true;
		
		try
		{
			readable = !itsCurrentInput.empty()  ||  itsWindow.get() && itsWindow->IsReadyForInput();
		}
		catch ( const io::end_of_input& )
		{
			// IsReadyForInput() throws on EOF, in which case readable is true
		}
		
		int readability = readable ? kPollRead : 0;
		
		return readability | kPollWrite;
	}
	
	int ConsoleTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		Open();
		
		// Zero byteCount always begets zero result
		if ( byteCount == 0 )
		{
			return 0;
		}
		
		while ( true )
		{
			if ( itsCurrentInput.empty() )
			{
				try
				{
					if ( itsWindow->IsReadyForInput() )
					{
						itsCurrentInput = itsWindow->ReadInput();
					}
				}
				catch ( const io::end_of_input& )
				{
					return 0;
				}
			}
			
			if ( !itsCurrentInput.empty() )
			{
				// Actual byteCount is lesser of requested size and available size
				std::size_t bytesCopied = std::min( byteCount, itsCurrentInput.size() );
				
				// Copy data from input to buffer
				std::copy( itsCurrentInput.begin(),
				           itsCurrentInput.begin() + bytesCopied,
				           data );
				
				// Slide remaining data to beginning
				std::copy( itsCurrentInput.begin() + bytesCopied,
				           itsCurrentInput.end(),
				           itsCurrentInput.begin() );
				
				// and take up the slack
				itsCurrentInput.resize( itsCurrentInput.size() - bytesCopied );
				
				return bytesCopied;
			}
			
			TryAgainLater();
		}
	}
	
	int ConsoleTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		Open();
		
		return itsWindow->Write( data, byteCount );
	}
	
	void ConsoleTTYHandle::IOCtl( unsigned long request, int* argp )
	{
		Open();
		
		switch ( request )
		{
			default:
				TTYHandle::IOCtl( request, argp );
				break;
		};
	}
	
	static std::string MakeConsoleName( ConsoleID id )
	{
		return "/dev/con/" + NN::Convert< std::string >( id );
	}
	
	static boost::shared_ptr< ConsoleWindow > NewConsole( ConsoleID id, const std::string& name )
	{
		boost::shared_ptr< ConsoleWindow > console( new ConsoleWindow( id, name ) );
		
		return console;
	}
	
	void ConsoleTTYHandle::Open()
	{
		if ( itsWindow.get() == NULL )
		{
			itsWindow = NewConsole( itsID, MakeConsoleName( itsID ) );
		}
	}
	
}

