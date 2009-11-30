// nucleus/flag_ops.hh
// -------------------

// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_FLAGOPS_HH
#define NUCLEUS_FLAGOPS_HH


namespace nucleus
{
	
	template < unsigned size > struct enum_size_traits;
	
	template <> struct enum_size_traits< sizeof (unsigned char     ) > { typedef unsigned char       type; };
	template <> struct enum_size_traits< sizeof (unsigned short    ) > { typedef unsigned short      type; };
	template <> struct enum_size_traits< sizeof (unsigned int      ) > { typedef unsigned int        type; };
	template <> struct enum_size_traits< sizeof (unsigned long long) > { typedef unsigned long long  type; };
	
}

#define NUCLEUS_DEFINE_FLAG_OPS( enum_t )                                      \
	inline enum_t operator&( enum_t a, enum_t b )                              \
	{                                                                          \
		typedef ::nucleus::enum_size_traits< sizeof (enum_t) >::type integer;  \
		return enum_t( integer( a ) & integer( b ) );                          \
	}                                                                          \
	                                                                           \
	inline enum_t operator|( enum_t a, enum_t b )                              \
	{                                                                          \
		typedef ::nucleus::enum_size_traits< sizeof (enum_t) >::type integer;  \
		return enum_t( integer( a ) | integer( b ) );                          \
	}                                                                          \
	                                                                           \
	inline enum_t operator^( enum_t a, enum_t b )                              \
	{                                                                          \
		typedef ::nucleus::enum_size_traits< sizeof (enum_t) >::type integer;  \
		return enum_t( integer( a ) ^ integer( b ) );                          \
	}                                                                          \
	                                                                           \
	inline enum_t& operator&=( enum_t& a, enum_t b )                           \
	{                                                                          \
		return a = a & b;                                                      \
	}                                                                          \
	                                                                           \
	inline enum_t& operator|=( enum_t& a, enum_t b )                           \
	{                                                                          \
		return a = a | b;                                                      \
	}                                                                          \
	                                                                           \
	inline enum_t& operator^=( enum_t& a, enum_t b )                           \
	{                                                                          \
		return a = a ^ b;                                                      \
	}

#endif

