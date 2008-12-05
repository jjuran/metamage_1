/*	=============
 *	PseudoFile.hh
 *	=============
 */

#ifndef GENIE_IO_PSEUDOFILE_HH
#define GENIE_IO_PSEUDOFILE_HH

// Standard C++
#include <string>

// Genie
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	template < class Property >
	class PseudoFileHandle : public VirtualFileHandle
	{
		private:
			Property itsProperty;
		
		public:
			PseudoFileHandle( const FSTreePtr&  file,
			                  const Property&   property ) : VirtualFileHandle( file ),
			                                                 itsProperty( property )
			{
			}
			
			int SysWrite( const char* data, std::size_t byteCount );
	};
	
	template < class Property >
	int PseudoFileHandle< Property >::SysWrite( const char* data, std::size_t byteCount )
	{
		if ( byteCount == 0 )
		{
			throw poseven::errno_t( EINVAL );
		}
		
		std::size_t length = byteCount - 1;
		
		if ( data[ length ] != '\n' )
		{
			throw poseven::errno_t( EINVAL );
		}
		
		std::string value( data, length );
		
		itsProperty.Set( value );
		
		return byteCount;
	}
	
}

#endif

