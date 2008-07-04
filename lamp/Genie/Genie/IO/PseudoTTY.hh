/*	============
 *	PseudoTTY.hh
 *	============
 */

#ifndef GENIE_IO_PSEUDOTTY_HH
#define GENIE_IO_PSEUDOTTY_HH

// Standard C++
#include <map>

// Boost
#include <boost/shared_ptr.hpp>

// Genie
#include "Genie/IO/Conduit.hh"
#include "Genie/IO/InternallyNonblocking.hh"
#include "Genie/IO/TTY.hh"


namespace Genie
{
	
	class PseudoTTYHandle : public InternallyNonblocking< TTYHandle >
	{
		private:
			TerminalID                     itsID;
			boost::shared_ptr< IOHandle >  itsTerminal;
			boost::shared_ptr< Conduit >   itsInput;
			boost::shared_ptr< Conduit >   itsOutput;
			
			IOHandle* Next() const;
		
		public:
			PseudoTTYHandle( std::size_t                   id,
			                 boost::shared_ptr< Conduit >  input,
			                 boost::shared_ptr< Conduit >  output );
			
			~PseudoTTYHandle();
			
			FSTreePtr GetFile() const  { return itsTerminal->GetFile(); }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
	};
	
	void GetNewPseudoTTYPair( boost::shared_ptr< IOHandle >& master,
	                          boost::shared_ptr< IOHandle >& slave );
	
}

#endif

