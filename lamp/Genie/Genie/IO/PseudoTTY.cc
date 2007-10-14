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
	
	
	PseudoTTYHandle::PseudoTTYHandle( std::size_t                   id,
			                          boost::shared_ptr< Conduit >  input,
			                          boost::shared_ptr< Conduit >  output )
	: TTYHandle( "/dev/pts/" + NN::Convert< std::string >( id ) ),
	  itsID( id ),
	  itsInput( input ),
	  itsOutput( output ),
	  itIsBlocking( true )
	{
	}
	
	PseudoTTYHandle::~PseudoTTYHandle()
	{
		itsInput->CloseEgress();
		itsOutput->CloseIngress();
	}
	
	unsigned int PseudoTTYHandle::SysPoll() const
	{
		return (itsInput->IsReadable() ? kPollRead : 0) | kPollWrite;
	}
	
	int PseudoTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		return itsInput->Read( data, byteCount, itIsBlocking );
	}
	
	int PseudoTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return itsOutput->Write( data, byteCount, itIsBlocking );
	}
	
}

