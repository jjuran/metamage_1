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
	namespace p7 = poseven;
	
	static std::string IOName( const void* address, bool is_pipe )
	{
		std::string prefix = is_pipe ? "pipe" : "socket";
		
		return prefix + ":[" + NN::Convert< std::string >( (unsigned long) address ) + "]";
	}
	
	class FSTree_IOHandle : public FSTree
	{
		public:
			FSTree_IOHandle( const void* address ) : FSTree( FSTreePtr(), IOName( address, true ) )
			{
			}
			
			bool IsPipe()      const  { return true; }
			bool IsAnonymous() const  { return true; }
			
			std::string Pathname() const  { return Name(); }
	};
	
	IOHandle::~IOHandle()
	{
	}
	
	IOHandle* IOHandle::GetBaseForCast( Test test )
	{
		if ( (this->*test)() )
		{
			return this;
		}
		
		if ( IOHandle* next = Next() )
		{
			return next->GetBaseForCast( test );
		}
		
		return NULL;
	}
	
	boost::shared_ptr< IOHandle > IOHandle::Clone()
	{
		return shared_from_this();
	}
	
	void IOHandle::Attach( const boost::shared_ptr< IOHandle >& target )
	{
		p7::throw_errno( EINVAL );
	}
	
	FSTreePtr IOHandle::GetFile()
	{
		return FSTreePtr( new FSTree_IOHandle( this ) );
	}
	
	void IOHandle::IOCtl( unsigned long request, int* argp )
	{
		if ( IOHandle* next = Next() )
		{
			return next->IOCtl( request, argp );
		}
		
		p7::throw_errno( EINVAL );
	}
	
	void Check_IOHandle_Cast( const void* cast, int errorToThrow )
	{
		if ( cast == NULL )
		{
			p7::throw_errno( errorToThrow );
		}
	}
	
}

