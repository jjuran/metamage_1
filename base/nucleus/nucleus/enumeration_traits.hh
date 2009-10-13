// nucleus/enumeration_traits.hh
// -----------------------------

// Written 2006-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#define NUCLEUS_ENUMERATIONTRAITS_HH


namespace nucleus
{
	
	template < class Int >
	struct enumeration_traits
	{
		typedef Int integral_type;
		
		const static integral_type zero = integral_type( 0 );
		const static integral_type one  = zero + 1;
		
		const static bool is_signed = ~zero < zero;
		
		const static int byte_width = sizeof (integral_type);
		const static int bit_width  = byte_width * 8;
		
		const static int binary_digits = bit_width - int( is_signed );
		
		const static integral_type high_bit_mask = one << bit_width - 1;
		
		const static integral_type min = is_signed ? high_bit_mask : zero;
		const static integral_type max = integral_type( ~zero ^ min );
	};
	
}

#endif

