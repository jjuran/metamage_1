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
			bool itsInputHasClosed;
			bool itsOutputHasClosed;
		
		public:
			Conduit() : itsInputHasClosed ( false ),
			            itsOutputHasClosed( false )
			{
			}
			
			bool IsReadable() const;
			bool IsWritable() const;
			
			bool InputHasClosed()  const  { return itsInputHasClosed;  }
			bool OutputHasClosed() const  { return itsOutputHasClosed; }
			
			bool CloseInput()   { itsInputHasClosed  = true;  return itsOutputHasClosed; }
			bool CloseOutput()  { itsOutputHasClosed = true;  return itsInputHasClosed;  }
			
			int Read (       char* data, std::size_t byteCount, bool blocking );
			int Write( const char* data, std::size_t byteCount, bool blocking );
	};
	
}

#endif

