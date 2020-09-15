/*
	in-flight.hh
	------------
*/

#ifndef VLIB_INFLIGHT_HH
#define VLIB_INFLIGHT_HH

// vlib
#include "vlib/tracker.hh"
#include "vlib/value.hh"


namespace vlib
{
	
	class Value_in_flight
	{
		private:
			Value its_value;
			
			// non-assignable
			Value_in_flight& operator=( const Value_in_flight& );
		
		public:
			Value_in_flight( const Value& v ) : its_value( v )
			{
				add_root( its_value );
			}
			
			~Value_in_flight()
			{
				del_root( its_value );
			}
			
			Value_in_flight( const Value_in_flight& that )
			:
				its_value( that.its_value )
			{
				add_root( its_value );
			}
			
			const Value& get() const  { return its_value; }
			
			operator const Value&() const  { return get(); }
	};
	
}

#endif
