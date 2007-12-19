/*	============
 *	PseudoTTY.cc
 *	============
 */

#include "Genie/IO/PseudoTTY.hh"

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
	static PseudoTTYMap gPseudoTTYMap;
	
	const PseudoTTYMap& GetPseudoTTYMap()
	{
		return gPseudoTTYMap;
	}
	
	void GetNewPseudoTTYPair( boost::shared_ptr< IOHandle >& master,
	                          boost::shared_ptr< IOHandle >& slave )
	{
		static TerminalID index = 0;
		
		boost::shared_ptr< Conduit > incoming( new Conduit );
		boost::shared_ptr< Conduit > outgoing( new Conduit );
		
		boost::shared_ptr< IOHandle > master_handle( new PseudoTTYHandle( index, outgoing, incoming ) );
		boost::shared_ptr< IOHandle > slave_handle ( new PseudoTTYHandle( index, incoming, outgoing ) );
		
		gPseudoTTYMap[ index ] = slave_handle;
		
		++index;
		
		master.swap( master_handle );
		slave .swap( slave_handle  );
	}
	
	const boost::shared_ptr< IOHandle >& GetPseudoTTYByID( TerminalID id )
	{
		PseudoTTYMap::const_iterator it = gPseudoTTYMap.find( id );
		
		if ( it == gPseudoTTYMap.end() )
		{
			p7::throw_errno( ENOENT );
		}
		
		ASSERT( !it->second.expired() );
		
		return it->second.lock();
	}
	
	
	static boost::shared_ptr< IOHandle > NewTerminal( const std::string& name )
	{
		boost::shared_ptr< IOHandle > result( new TerminalHandle( name ) );
		
		return result;
	}
	
	PseudoTTYHandle::PseudoTTYHandle( std::size_t                   id,
			                          boost::shared_ptr< Conduit >  input,
			                          boost::shared_ptr< Conduit >  output )
	: itsID( id ),
	  itsTerminal( NewTerminal( "/dev/pts/" + NN::Convert< std::string >( id ) ) ),
	  itsInput( input ),
	  itsOutput( output )
	{
	}
	
	PseudoTTYHandle::~PseudoTTYHandle()
	{
		itsInput->CloseEgress();
		itsOutput->CloseIngress();
		
		gPseudoTTYMap.erase( itsID );
	}
	
	IOHandle* PseudoTTYHandle::Next() const
	{
		return itsTerminal.get();
	}
	
	unsigned int PseudoTTYHandle::SysPoll() const
	{
		return (itsInput->IsReadable() ? kPollRead : 0) | kPollWrite;
	}
	
	int PseudoTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		return itsInput->Read( data, byteCount, IsNonblocking() );
	}
	
	int PseudoTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return itsOutput->Write( data, byteCount, IsNonblocking() );
	}
	
}

