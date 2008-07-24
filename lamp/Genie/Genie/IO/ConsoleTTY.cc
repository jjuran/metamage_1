/*	=============
 *	ConsoleTTY.cc
 *	=============
 */

#include "Genie/IO/ConsoleTTY.hh"

// Genie
#include "Genie/IO/ConsoleWindow.hh"


namespace Genie
{
	
	ConsoleTTYHandle::ConsoleTTYHandle( TerminalID          id,
	                                    const std::string&  name ) : itsID( id ),
	                                                                 itsWindow( new ConsoleWindow( id, name ) )
	{
	}
	
	ConsoleTTYHandle::~ConsoleTTYHandle()
	{
		GetDynamicGroup< ConsoleTTYHandle >().erase( itsID );
	}
	
	IOHandle* ConsoleTTYHandle::Next() const
	{
		return itsWindow.get();
	}
	
	bool ConsoleTTYHandle::IsDisconnected() const
	{
		return itsWindow.get() && static_cast< ConsoleWindow* >( itsWindow.get() )->IsDisconnected();
	}
	
	unsigned int ConsoleTTYHandle::SysPoll() const
	{
		bool readable = true;
		
		try
		{
			readable = !itsCurrentInput.empty()  ||  itsWindow.get() && static_cast< ConsoleWindow* >( itsWindow.get() )->IsReadyForInput();
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
					ConsoleWindow* window = static_cast< ConsoleWindow* >( itsWindow.get() );
					
					ASSERT( window != NULL );
					
					if ( window->IsReadyForInput() )
					{
						itsCurrentInput = window->ReadInput();
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
		return static_cast< ConsoleWindow* >( itsWindow.get() )->Write( data, byteCount );
	}
	
	void ConsoleTTYHandle::IOCtl( unsigned long request, int* argp )
	{
		switch ( request )
		{
			default:
				TTYHandle::IOCtl( request, argp );
				break;
		};
	}
	
}

