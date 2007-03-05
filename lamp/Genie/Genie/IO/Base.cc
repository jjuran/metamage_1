/*	=======
 *	Base.cc
 *	=======
 */

#include "Genie/IO/Base.hh"

// POSIX
#include <sys/stat.h>

// Nucleus
#include "Nucleus/Convert.h"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	static std::string IOName( unsigned long id )
	{
		return "io:[" + NN::Convert< std::string >( id ) + "]";
	}
	
	class FSTree_IOHandle : public FSTree
	{
		private:
			std::string itsName;
		
		public:
			FSTree_IOHandle( const void* address ) : itsName( IOName( (unsigned long) address ) )  {}
			
			std::string Name() const  { return itsName; }
			
			std::string Pathname() const  { return Name(); }
			
			FSTreePtr Parent() const
			{
				P7::ThrowErrno( ENOENT );
				
				return FSTreePtr();
			}
			
			mode_t FileTypeMode() const  { return S_IFIFO; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
	};
	
	
	FSTreePtr IOHandle::GetFile() const
	{
		return FSTreePtr( new FSTree_IOHandle( this ) );
	}
	
	void Check_IOHandle_Cast( IOHandle& handle, TypeCode desiredType )
	{
		TypeCode actual = handle.ActualType();
		
		if ( actual % desiredType != 0 )
		{
			P7::ThrowErrno( EOPNOTSUPP );
		}
	}
	
}

