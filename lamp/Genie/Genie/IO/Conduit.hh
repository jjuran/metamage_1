/*	==========
 *	Conduit.hh
 *	==========
 */

#ifndef GENIE_IO_CONDUIT_HH
#define GENIE_IO_CONDUIT_HH

// Standard C++
#include <list>
#include <string>


namespace Genie
{
	
	class Conduit
	{
		private:
			std::list< std::string > itsStrings;
			bool itsIngressHasClosed;
			bool itsEgressHasClosed;
		
		public:
			Conduit() : itsIngressHasClosed ( false ),
			            itsEgressHasClosed( false )
			{
			}
			
			bool IsReadable() const;
			bool IsWritable() const;
			
			bool IngressHasClosed() const  { return itsIngressHasClosed; }
			bool EgressHasClosed()  const  { return itsEgressHasClosed;  }
			
			bool CloseIngress()  { itsIngressHasClosed = true;  return itsEgressHasClosed;  }
			bool CloseEgress()   { itsEgressHasClosed  = true;  return itsIngressHasClosed; }
			
			int Read (       char* data, std::size_t byteCount, bool nonblocking );
			int Write( const char* data, std::size_t byteCount, bool nonblocking );
	};
	
}

#endif

