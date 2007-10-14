/*	=======
 *	Base.hh
 *	=======
 */

#ifndef GENIE_IO_BASE_HH
#define GENIE_IO_BASE_HH

// boost
#include <boost/enable_shared_from_this.hpp>

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	enum
	{
		kPollRead   = 1,
		kPollWrite  = 2,
		kPollExcept = 4
	};
	
	/*
		Each file descriptor refers to an I/O handle.
		Several file descriptors may share the same I/O handle.
		Each I/O handle has a type.
		I/O handle types are arranged in an inheritence tree, with the base type
		at the root node, and actual (non-abstract) types as leaf nodes.
		Each type (abstract as well as leaf) has a node in the tree.
		
	*/
	
	class IOHandle : public boost::enable_shared_from_this< IOHandle >
	{
		private:
			virtual IOHandle* Next() const  { return NULL; }
		
		public:
			typedef bool (IOHandle::*Test)() const;
			
			virtual ~IOHandle();
			
			IOHandle* GetBaseForCast( TypeCode desiredType );
			
			virtual bool IsStream     () const  { return false; }
			virtual bool IsRegularFile() const  { return false; }
			virtual bool IsSocket     () const  { return false; }
			virtual bool IsTerminal   () const  { return false; }
			virtual bool IsDirectory  () const  { return false; }
			
			IOHandle* GetBaseForCast( Test test );
			
			virtual FSTreePtr GetFile() const;
			
			virtual void IOCtl( unsigned long request, int* argp );
			
	};
	
	
	template < class Handle > struct IOHandle_Downcast_Traits;
	
	template < class Handle >
	Handle* IOHandle_Cast( IOHandle* handle )
	{
		if ( handle == NULL )
		{
			return NULL;
		}
		
		IOHandle* base = handle->GetBaseForCast( IOHandle_Downcast_Traits< Handle >::GetTest() );
		
		return static_cast< Handle* >( base );
	}
	
	void Check_IOHandle_Cast_Result( IOHandle* cast );
	
	template < class Handle >
	Handle& IOHandle_Cast( IOHandle& handle )
	{
		Handle* cast = IOHandle_Cast< Handle >( &handle );
		
		Check_IOHandle_Cast_Result( cast );
		
		return *cast;
	}
	
}

#endif

