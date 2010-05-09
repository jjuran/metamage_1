/*	============
 *	PseudoTTY.cc
 *	============
 */

#include "Genie/IO/PseudoTTY.hh"

// Iota
#include "iota/decimal.hh"

// plus
#include "plus/var_string.hh"

// Genie
#include "Genie/IO/DynamicGroup.hh"
#include "Genie/IO/Terminal.hh"


namespace Genie
{
	
	static inline DynamicGroup& GetPseudoTTYMap()
	{
		return GetDynamicGroup< PseudoTTYHandle >();
	}
	
	static inline boost::shared_ptr< IOHandle >
	//
	NewPseudoTTY( TerminalID                           id,
	              const boost::shared_ptr< Conduit >&  input,
	              const boost::shared_ptr< Conduit >&  output )
	{
		return seize_ptr( new PseudoTTYHandle( id, input, output ) );
	}
	
	void GetNewPseudoTTYPair( boost::shared_ptr< IOHandle >& master,
	                          boost::shared_ptr< IOHandle >& slave )
	{
		static TerminalID index = 0;
		
		boost::shared_ptr< Conduit > incoming( new Conduit );
		boost::shared_ptr< Conduit > outgoing( new Conduit );
		
		boost::shared_ptr< IOHandle > master_handle( NewPseudoTTY( index, outgoing, incoming ) );
		boost::shared_ptr< IOHandle > slave_handle ( NewPseudoTTY( index, incoming, outgoing ) );
		
		GetPseudoTTYMap()[ index ] = slave_handle;
		
		++index;
		
		master.swap( master_handle );
		slave .swap( slave_handle  );
	}
	
	
	static inline boost::shared_ptr< IOHandle > NewTerminal( const plus::string& name )
	{
		return seize_ptr( new TerminalHandle( name ) );
	}
	
	static inline plus::string make_devpts( size_t id )
	{
		plus::var_string result = "/dev/pts/";
		
		result += iota::inscribe_decimal( id );
		
		return result;
	}
	
	PseudoTTYHandle::PseudoTTYHandle( std::size_t                   id,
			                          boost::shared_ptr< Conduit >  input,
			                          boost::shared_ptr< Conduit >  output )
	: TTYHandle( O_RDWR ),
	  itsID( id ),
	  itsTerminal( NewTerminal( make_devpts( id ) ) ),
	  itsInput( input ),
	  itsOutput( output )
	{
	}
	
	PseudoTTYHandle::~PseudoTTYHandle()
	{
		itsInput->CloseEgress();
		itsOutput->CloseIngress();
		
		GetPseudoTTYMap().erase( itsID );
	}
	
	IOHandle* PseudoTTYHandle::Next() const
	{
		return itsTerminal.get();
	}
	
	unsigned int PseudoTTYHandle::SysPoll()
	{
		return (itsInput->IsReadable() ? kPollRead : 0) | kPollWrite;
	}
	
	ssize_t PseudoTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		return itsInput->Read( data, byteCount, IsNonblocking() );
	}
	
	ssize_t PseudoTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return itsOutput->Write( data, byteCount, IsNonblocking() );
	}
	
}

