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
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/types/dynamic_group.hh"
#include "vfs/functions/resolve_pathname.hh"

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
	
	static inline vfs::filehandle_ptr
	//
	NewPseudoTTY( TerminalID                                    id,
	              const boost::intrusive_ptr< plus::conduit >&  input,
	              const boost::intrusive_ptr< plus::conduit >&  output )
	{
		return new PseudoTTYHandle( id, input, output );
	}
	
	static inline plus::string make_devpts( size_t id )
	{
		plus::var_string result = "/dev/pts/";
		
		result += gear::inscribe_decimal( id );
		
		return result;
	}
	
	void GetNewPseudoTTYPair( vfs::filehandle_ptr&  master,
	                          vfs::filehandle_ptr&  slave )
	{
		static TerminalID index = 0;
		
		boost::intrusive_ptr< plus::conduit > incoming( new plus::conduit );
		boost::intrusive_ptr< plus::conduit > outgoing( new plus::conduit );
		
		vfs::filehandle_ptr master_handle( NewPseudoTTY( index, outgoing, incoming ) );
		vfs::filehandle_ptr slave_handle ( NewPseudoTTY( index, incoming, outgoing ) );
		
		vfs::set_dynamic_element_by_id< PseudoTTYHandle >( index, slave_handle.get() );
		
		vfs::filehandle_ptr terminal = new TerminalHandle( *vfs::resolve_absolute_path( make_devpts( index ) ) );
		
		terminal->Attach( slave_handle.get() );
		
		++index;
		
		master.swap( master_handle );
		slave .swap( terminal      );
	}
	
	
	PseudoTTYHandle::PseudoTTYHandle( std::size_t                            id,
			                          boost::intrusive_ptr< plus::conduit >  input,
			                          boost::intrusive_ptr< plus::conduit >  output )
	: StreamHandle( O_RDWR ),
	  itsID( id ),
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
	
	unsigned int PseudoTTYHandle::SysPoll()
	{
		return (itsInput->is_readable() ? kPollRead : 0) | kPollWrite;
	}
	
	ssize_t PseudoTTYHandle::SysRead( char* data, std::size_t byteCount )
	{
		return itsInput->read( data, byteCount, is_nonblocking( *this ), &try_again );
	}
	
	ssize_t PseudoTTYHandle::SysWrite( const char* data, std::size_t byteCount )
	{
		return itsOutput->write( data, byteCount, is_nonblocking( *this ), &try_again, &broken_pipe );
	}
	
}

