/*	============
 *	PseudoTTY.cc
 *	============
 */

#include "Genie/IO/PseudoTTY.hh"

// gear
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// vfs
#include "vfs/filehandle/types/dynamic_group.hh"

// Genie
#include "Genie/api/signals.hh"
#include "Genie/api/yield.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/IO/Terminal.hh"


namespace Genie
{
	
	static inline vfs::dynamic_group& GetPseudoTTYMap()
	{
		return vfs::get_dynamic_group< PseudoTTYHandle >();
	}
	
	static inline IOPtr
	//
	NewPseudoTTY( TerminalID                                    id,
	              const boost::intrusive_ptr< plus::conduit >&  input,
	              const boost::intrusive_ptr< plus::conduit >&  output )
	{
		return new PseudoTTYHandle( id, input, output );
	}
	
	void GetNewPseudoTTYPair( IOPtr&  master,
	                          IOPtr&  slave )
	{
		static TerminalID index = 0;
		
		boost::intrusive_ptr< plus::conduit > incoming( new plus::conduit );
		boost::intrusive_ptr< plus::conduit > outgoing( new plus::conduit );
		
		IOPtr master_handle( NewPseudoTTY( index, outgoing, incoming ) );
		IOPtr slave_handle ( NewPseudoTTY( index, incoming, outgoing ) );
		
		vfs::set_dynamic_element_by_id< PseudoTTYHandle >( index, slave_handle.get() );
		
		++index;
		
		master.swap( master_handle );
		slave .swap( slave_handle  );
	}
	
	
	static inline IOPtr NewTerminal( const plus::string& name )
	{
		return new TerminalHandle( name );
	}
	
	static inline plus::string make_devpts( size_t id )
	{
		plus::var_string result = "/dev/pts/";
		
		result += gear::inscribe_decimal( id );
		
		return result;
	}
	
	PseudoTTYHandle::PseudoTTYHandle( std::size_t                            id,
			                          boost::intrusive_ptr< plus::conduit >  input,
			                          boost::intrusive_ptr< plus::conduit >  output )
	: TTYHandle( O_RDWR ),
	  itsID( id ),
	  itsTerminal( NewTerminal( make_devpts( id ) ) ),
	  itsInput( input ),
	  itsOutput( output )
	{
	}
	
	PseudoTTYHandle::~PseudoTTYHandle()
	{
		itsInput->close_egress();
		itsOutput->close_ingress();
		
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
		return (itsInput->is_readable() ? kPollRead : 0) | kPollWrite;
	}
	
	ssize_t PseudoTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		return itsInput->read( data, byteCount, IsNonblocking(), &try_again );
	}
	
	ssize_t PseudoTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return itsOutput->write( data, byteCount, IsNonblocking(), &try_again, &broken_pipe );
	}
	
}

