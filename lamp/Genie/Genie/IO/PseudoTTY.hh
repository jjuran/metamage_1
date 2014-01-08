/*	============
 *	PseudoTTY.hh
 *	============
 */

#ifndef GENIE_IO_PSEUDOTTY_HH
#define GENIE_IO_PSEUDOTTY_HH

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// plus
#include "plus/conduit.hh"

// vfs
#include "vfs/filehandle_ptr.hh"

// Genie
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	typedef std::size_t TerminalID;
	
	class PseudoTTYHandle : public StreamHandle
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
	
	void GetNewPseudoTTYPair( vfs::filehandle_ptr&  master,
	                          vfs::filehandle_ptr&  slave );
	
}

#endif

