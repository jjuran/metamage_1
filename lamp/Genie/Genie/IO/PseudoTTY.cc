/*	============
 *	PseudoTTY.cc
 *	============
 */

#include "Genie/IO/PseudoTTY.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/IO/DynamicGroup.hh"
#include "Genie/IO/Terminal.hh"


namespace Genie
{
	
	static inline DynamicGroup& GetPseudoTTYMap()
	{
		return GetDynamicGroup< PseudoTTYHandle >();
	}
	
	static inline IOPtr
	//
	NewPseudoTTY( TerminalID                              id,
	              const boost::intrusive_ptr< Conduit >&  input,
	              const boost::intrusive_ptr< Conduit >&  output )
	{
		return seize_ptr( new PseudoTTYHandle( id, input, output ) );
	}
	
	void GetNewPseudoTTYPair( IOPtr&  master,
	                          IOPtr&  slave )
	{
		static TerminalID index = 0;
		
		boost::intrusive_ptr< Conduit > incoming( new Conduit );
		boost::intrusive_ptr< Conduit > outgoing( new Conduit );
		
		IOPtr master_handle( NewPseudoTTY( index, outgoing, incoming ) );
		IOPtr slave_handle ( NewPseudoTTY( index, incoming, outgoing ) );
		
		GetPseudoTTYMap()[ index ] = slave_handle;
		
		++index;
		
		master.swap( master_handle );
		slave .swap( slave_handle  );
	}
	
	
	static inline IOPtr NewTerminal( const plus::string& name )
	{
		return seize_ptr( new TerminalHandle( name ) );
	}
	
	static inline plus::string make_devpts( size_t id )
	{
		plus::var_string result = "/dev/pts/";
		
		result += gear::inscribe_decimal( id );
		
		return result;
	}
	
	PseudoTTYHandle::PseudoTTYHandle( std::size_t                      id,
			                          boost::intrusive_ptr< Conduit >  input,
			                          boost::intrusive_ptr< Conduit >  output )
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
	
	FSTreePtr PseudoTTYHandle::GetFile()
	{
		return itsTerminal->GetFile();
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

