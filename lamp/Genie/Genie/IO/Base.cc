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
				p7::throw_errno( ENOENT );
				
				return FSTreePtr();
			}
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

