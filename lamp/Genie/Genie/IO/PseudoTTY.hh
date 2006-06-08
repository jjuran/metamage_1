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
#include "Genie/IO/TTY.hh"


namespace Genie
{
	
	class PseudoTTYHandle : public TTYHandle
	{
		private:
			std::size_t id;
			boost::shared_ptr< Io::StringBuffer > input;
			boost::shared_ptr< Io::StringBuffer > output;
		
		public:
			PseudoTTYHandle( std::size_t                            id,
			                 boost::shared_ptr< Io::StringBuffer >  input,
			                 boost::shared_ptr< Io::StringBuffer >  output );
			
			static TypeCode Type()  { return kPseudoTTYType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
			
	};
	
}

#endif

