/*	============
 *	PseudoTTY.cc
 *	============
 */

#include "Genie/IO/PseudoTTY.hh"

// Nucleus
#include "Nucleus/Convert.h"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	PseudoTTYHandle::PseudoTTYHandle( std::size_t                            id,
	                                  boost::shared_ptr< Io::StringBuffer >  input,
	                                  boost::shared_ptr< Io::StringBuffer >  output )
	: TTYHandle( "/dev/pts/" + NN::Convert< std::string >( id ) ),
	  id( id ),
	  input( input ),
	  output( output )
	{
	}
	
	PseudoTTYHandle::~PseudoTTYHandle()
	{
	}
	
	unsigned int PseudoTTYHandle::SysPoll() const
	{
		return (input->Empty() ? 0 : kPollRead) | kPollWrite;
	}
	
	int PseudoTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		return input->Read( data, byteCount );
	}
	
	int PseudoTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return output->Write( data, byteCount );
	}
	
}

