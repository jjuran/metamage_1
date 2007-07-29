/*	=======
 *	Base.cc
 *	=======
 */

#include "Genie/IO/Base.hh"

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
			
			bool IsPipe()      const  { return true; }
			bool IsAnonymous() const  { return true; }
			
			std::string Name() const  { return itsName; }
			
			std::string Pathname() const  { return Name(); }
			
			FSTreePtr Parent() const
			{
				P7::ThrowErrno( ENOENT );
				
				return FSTreePtr();
			}
	};
	
	
	IOHandle* IOHandle::GetBaseForCast( TypeCode desiredType )
	{
		if ( ActualType() % desiredType == 0 )
		{
			return this;
		}
		
		if ( IOHandle* next = Next() )
		{
			return next->GetBaseForCast( desiredType );
		}
		
		return NULL;
	}
	
	FSTreePtr IOHandle::GetFile() const
	{
		return FSTreePtr( new FSTree_IOHandle( this ) );
	}
	
	void IOHandle::IOCtl( unsigned long request, int* argp )
	{
		if ( IOHandle* next = Next() )
		{
			return next->IOCtl( request, argp );
		}
		
		P7::ThrowErrno( EINVAL );
	}
	
	void Check_IOHandle_Cast_Result( IOHandle* cast )
	{
		if ( cast == NULL )
		{
			P7::ThrowErrno( EOPNOTSUPP );
		}
	}
	
}

