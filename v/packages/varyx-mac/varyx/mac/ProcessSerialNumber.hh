/*
	ProcessSerialNumber.hh
	----------------------
*/

#ifndef VARYX_MAC_PROCESSSERIALNUMBER_HH
#define VARYX_MAC_PROCESSSERIALNUMBER_HH

// vlib
#include "vlib/types/struct/struct.hh"


struct ProcessSerialNumber;

namespace vlib
{
	
	class Struct_Type;
	
}

namespace varyx
{
namespace mac
{
	
	using namespace vlib;
	
	const Struct_Type& struct_ProcessSerialNumber();
	
	class ProcessSerialNumber : public Struct
	{
		public:
			static bool test( const Value& v );
			
			ProcessSerialNumber();
			ProcessSerialNumber( const ::ProcessSerialNumber& psn );
			
			const ::ProcessSerialNumber& get() const
			{
				return get_data().pod_cast< ::ProcessSerialNumber >();
			}
	};
	
}
}

#endif
