/*	============
 *	PseudoTTY.hh
 *	============
 */

#ifndef GENIE_IO_PSEUDOTTY_HH
#define GENIE_IO_PSEUDOTTY_HH

// Boost
#include <boost/shared_ptr.hpp>

// Io
#include "Io/StringBuffer.hh"

// Genie
#include "Genie/IO/Conduit.hh"
#include "Genie/IO/TTY.hh"


namespace Genie
{
	
	class PseudoTTYHandle : public TTYHandle
	{
		private:
			std::size_t                   itsID;
			boost::shared_ptr< Conduit >  itsInput;
			boost::shared_ptr< Conduit >  itsOutput;
			bool                          itIsBlocking;
		
		public:
			PseudoTTYHandle( std::size_t                   id,
			                 boost::shared_ptr< Conduit >  input,
			                 boost::shared_ptr< Conduit >  output );
			
			~PseudoTTYHandle();
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
	};
	
}

#endif

