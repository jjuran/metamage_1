/*
	AEDesc.hh
	---------
*/

#ifndef VARYX_MAC_AEDESC_HH
#define VARYX_MAC_AEDESC_HH

// vlib
#include "vlib/value.hh"


struct AEDesc;


namespace vlib
{
	
	struct dispatch;
	struct type_info;
	
}

namespace varyx
{
namespace mac
{
	
	using namespace vlib;
	
	void throw_MacOS_error( int err, const char* from );
	
	extern const dispatch AEDesc_dispatch;
	
	class AEDesc : public Value
	{
		public:
			enum list_type_to_create
			{
				Create_AEDescList = false,
				Create_AERecord   = true,
			};
			
			static bool test( const Value& v )
			{
				return v.dispatch_methods() == &AEDesc_dispatch;
			}
			
			static Value coerce( const Value& v );
			
			AEDesc();
			
			AEDesc( unsigned long type, const void* data, unsigned long size );
			
			AEDesc( list_type_to_create list_type );
			
			const ::AEDesc& get() const
			{
				return *(const ::AEDesc*) pointer();
			}
			
			::AEDesc* pointer() const
			{
				return (::AEDesc*) Value::pointer();
			}
			
			Value get_descriptorType() const;
			Value get_dataHandle    () const;
			
			unsigned long data_size(         ) const;
			char*         copy_data( char* p ) const;
	};
	
	extern const type_info AEDesc_vtype;
	
}
}

#endif
