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
	
	enum TypeCode
	{
		kBaseType = 1,
			kStreamType    = 2,
				kFileType   = kStreamType * 2,  // 4
					kRegularFileType = kFileType * 2,  // 8
					kDeviceFileType  = kFileType * 3,  // 12
						kSimpleDeviceType = kDeviceFileType * 2,  // 24
						kTTYType = kDeviceFileType * 3,  // 36
							kConsoleTTYType = kTTYType * 2,  // 72
							kPseudoTTYType  = kTTYType * 3,  // 108
				kPipeType   = kStreamType * 3,  // 6
				kSocketType = kStreamType * 5,  // 10
			kResFileType   = 3,
			kDirectoryType = 5,
			kWindowType    = 7,
			kFooType       = 11,
			kBarType       = 13
	};
	
	class IOHandle : public boost::enable_shared_from_this< IOHandle >
	{
		public:
			virtual ~IOHandle()  {}
			
			static TypeCode Type()  { return kBaseType; }
			
			virtual TypeCode ActualType() const  { return Type(); }
			
			virtual FSTreePtr GetFile();
	};
	
	void Check_IOHandle_Cast( IOHandle& handle, TypeCode desiredType );
	
	template < class Handle >
	Handle& IOHandle_Cast( IOHandle& handle )
	{
		Check_IOHandle_Cast( handle, Handle::Type() );
		
		return static_cast< Handle& >( handle );
	}
	
}

#endif

