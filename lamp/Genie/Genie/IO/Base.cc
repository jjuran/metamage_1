/*	=======
 *	Base.cc
 *	=======
 */

#include "Genie/IO/Base.hh"

// iota
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	static plus::string IOName( const void* address, bool is_pipe )
	{
		plus::var_string name = is_pipe ? "pipe" : "socket";
		
		const size_t hex_offset = name.size() + STRLEN( ":[" );
		
		name += ":[12345678]";
		
		iota::encode_32_bit_hex( (long) address, &name[ hex_offset ] );
		
		return name;
	}
	
	class FSTree_IOHandle : public FSTree
	{
		private:
			boost::shared_ptr< IOHandle > itsHandle;
		
		public:
			FSTree_IOHandle( const boost::shared_ptr< IOHandle >& handle )
			:
				FSTree( null_FSTreePtr, IOName( handle.get(), true ) ),
				itsHandle( handle )
			{
			}
			
			bool IsPipe()      const  { return true; }
			bool IsAnonymous() const  { return true; }
			
			plus::string Pathname() const  { return Name(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return itsHandle;
			}
	};
	
	IOHandle::IOHandle( OpenFlags flags ) : itsOpenFlags( flags )
	{
	}
	
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
		return seize_ptr( new FSTree_IOHandle( shared_from_this() ) );
	}
	
	void IOHandle::IOCtl( unsigned long request, int* argp )
	{
		if ( IOHandle* next = Next() )
		{
			return next->IOCtl( request, argp );
		}
		
		p7::throw_errno( EINVAL );
	}
	
	boost::shared_ptr< memory_mapping >
	//
	IOHandle::Map( size_t length, off_t offset )
	{
		p7::throw_errno( ENODEV );
		
		return boost::shared_ptr< memory_mapping >();
	}
	
	void Check_IOHandle_Cast( const void* cast, int errorToThrow )
	{
		if ( cast == NULL )
		{
			p7::throw_errno( errorToThrow );
		}
	}
	
}

