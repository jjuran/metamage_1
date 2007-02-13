/*	=======
 *	Base.cc
 *	=======
 */

#include "Genie/IO/Base.hh"

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace P7 = POSeven;
	
	class FSTree_IOHandle : public FSTree
	{
		private:
			boost::shared_ptr< IOHandle > itsHandle;
		
		public:
			FSTree_IOHandle( const boost::shared_ptr< IOHandle >& handle ) : itsHandle( handle )  {}
			
			std::string Name() const  { return "io:[]"; }
			
			FSTreePtr Parent() const  { P7::ThrowErrno( ENOENT );  return FSTreePtr(); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const
			{
				return itsHandle;
			}
	};
	
	
	FSTreePtr IOHandle::GetFile()
	{
		return FSTreePtr( new FSTree_IOHandle( shared_from_this() ) );
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

