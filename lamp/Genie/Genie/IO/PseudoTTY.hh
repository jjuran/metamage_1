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

// Genie
#include "Genie/IO/Conduit.hh"
#include "Genie/IO/TTY.hh"


namespace Genie
{
	
	class PseudoTTYHandle : public TTYHandle
	{
		private:
			TerminalID                       itsID;
			IOPtr                            itsTerminal;
			boost::intrusive_ptr< conduit >  itsInput;
			boost::intrusive_ptr< conduit >  itsOutput;
			
			IOHandle* Next() const;
		
		public:
			PseudoTTYHandle( std::size_t                      id,
			                 boost::intrusive_ptr< conduit >  input,
			                 boost::intrusive_ptr< conduit >  output );
			
			~PseudoTTYHandle();
			
			FSTreePtr GetFile();
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
	};
	
	void GetNewPseudoTTYPair( IOPtr&  master,
	                          IOPtr&  slave );
	
}

#endif

