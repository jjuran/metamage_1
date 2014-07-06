/*	============
 *	PseudoTTY.cc
 *	============
 */

#include "Genie/IO/PseudoTTY.hh"

// gear
#include "gear/inscribe_decimal.hh"

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/conduit.hh"
#include "plus/var_string.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/node.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/conjoin.hh"
#include "vfs/filehandle/types/dynamic_group.hh"
#include "vfs/functions/resolve_pathname.hh"

// relix-kernel
#include "relix/api/root.hh"
#include "relix/api/try_again.hh"
#include "relix/fs/pts_tag.hh"
#include "relix/fs/terminal.hh"
#include "relix/signal/broken_pipe.hh"


namespace Genie
{
	
	using relix::broken_pipe;
	using relix::try_again;
	
	
	typedef std::size_t TerminalID;
	
	class PseudoTTYHandle : public vfs::filehandle
	{
		private:
			TerminalID                             itsID;
			boost::intrusive_ptr< plus::conduit >  itsInput;
			boost::intrusive_ptr< plus::conduit >  itsOutput;
		
		public:
			PseudoTTYHandle( std::size_t                            id,
			                 boost::intrusive_ptr< plus::conduit >  input,
			                 boost::intrusive_ptr< plus::conduit >  output );
			
			~PseudoTTYHandle();
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
	};
	
	static unsigned pseudotty_poll( vfs::filehandle* that )
	{
		return static_cast< PseudoTTYHandle& >( *that ).SysPoll();
	}
	
	static ssize_t pseudotty_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		return static_cast< PseudoTTYHandle& >( *that ).SysRead( buffer, n );
	}
	
	static ssize_t pseudotty_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		return static_cast< PseudoTTYHandle& >( *that ).SysWrite( buffer, n );
	}
	
	static const vfs::stream_method_set pseudotty_stream_methods =
	{
		&pseudotty_poll,
		&pseudotty_read,
		&pseudotty_write,
	};
	
	static const vfs::filehandle_method_set pseudotty_methods =
	{
		NULL,
		NULL,
		&pseudotty_stream_methods,
	};
	
	
	static inline vfs::dynamic_group& GetPseudoTTYMap()
	{
		return vfs::get_dynamic_group< relix::pts_tag >();
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
		
		vfs::set_dynamic_element_by_id< relix::pts_tag >( index, slave_handle.get() );
		
		vfs::filehandle_ptr terminal = relix::new_terminal( *vfs::resolve_absolute_path( *relix::root(), make_devpts( index ) ) );
		
		conjoin( *terminal, *slave_handle );
		
		++index;
		
		master.swap( master_handle );
		slave .swap( terminal      );
	}
	
	
	PseudoTTYHandle::PseudoTTYHandle( std::size_t                            id,
			                          boost::intrusive_ptr< plus::conduit >  input,
			                          boost::intrusive_ptr< plus::conduit >  output )
	: vfs::filehandle( O_RDWR, &pseudotty_methods ),
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
		return (itsInput->is_readable() ? vfs::Poll_read : 0) | vfs::Poll_write;
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
