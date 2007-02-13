/*	=============
 *	ConsoleTTY.cc
 *	=============
 */

#include "Genie/IO/ConsoleTTY.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Genie
#include "Genie/Console.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	void ConsoleTTYHandle::CheckConsole()
	{
		if ( console == NULL )
		{
			console = NewConsole( this );
		}
	}
	
	ConsoleTTYHandle::ConsoleTTYHandle( std::size_t id )
	: TTYHandle( "/dev/term/" + NN::Convert< std::string >( id ) ),
	  id( id ),
	  console( NULL )
	{
	}
	
	ConsoleTTYHandle::~ConsoleTTYHandle()
	{
		CloseConsole( console );
	}
	
	unsigned int ConsoleTTYHandle::SysPoll() const
	{
		return (console && console->IsReadable() ? kPollRead
		                                         : 0        ) | kPollWrite;
	}
	
	int ConsoleTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		CheckConsole();
		
		while ( true )
		{
			try
			{
				return console->Read( data, byteCount );
			}
			catch ( const io::no_input_pending& )
			{
				if ( IsBlocking() )
				{
					Yield();
				}
				else
				{
					throw;
				}
			}
		}
	}
	
	int ConsoleTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		CheckConsole();
		
		return console->Write( data, byteCount );
	}
	
}

