// nucleus/scoped.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2010 by Lisa Lippincott and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_SCOPED_HH
#define NUCLEUS_SCOPED_HH

#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif


namespace nucleus
{
	
	template < class Value >
	class scoped
	{
		private:
			typedef Value   value_type;
			typedef Value&  reference;
			typedef Value  *pointer;
			
			reference   its_reference;
			value_type  its_old_value;
		
		public:
			explicit scoped( reference variable )
			:
				its_reference( variable      ),
				its_old_value( its_reference )
			{
			}
			
			explicit scoped( reference variable, value_type new_value )
			:
				its_reference( variable      ),
				its_old_value( its_reference )
			{
				its_reference = new_value;
			}
			
			scoped( const scoped& source )
			:
				its_reference( source.its_reference ),
				its_old_value( its_reference )
			{
			}
			
			~scoped()
			{
				its_reference = its_old_value;
			}
			
			pointer operator&() const                              { return &its_reference; }
			
			value_type get() const                                 { return its_reference;  }
			void set( value_type value ) const                     { its_reference = value; }
			
			operator value_type() const                            { return get(); }
			
			const scoped& operator=( value_type value ) const      { Set( value ); return *this; }
			const scoped& operator=( const scoped& source ) const  { Set( source.get() ); return *this; }
	};
	
}

namespace nucleus
{
	
	template < class Value >
	struct convert_input_traits< scoped< Value > >
	{
		typedef Value converter_input_type;
	};
	
}

#endif

