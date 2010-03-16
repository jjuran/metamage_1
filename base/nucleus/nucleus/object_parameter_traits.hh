// nucleus/object_parameter_traits.hh
// ----------------------------------

// Part of the Nitrogen project.
//
// Written 2002 by Lisa Lippincott.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_OBJECTPARAMETERTRAITS_HH
#define NUCLEUS_OBJECTPARAMETERTRAITS_HH


namespace nucleus
{
	
	template < class Parameter > struct object_parameter_traits;
	
	template < class Object >
	struct object_parameter_traits< Object * >
	{
		typedef Object *type;
		
		static const void *convert_to_pointer( type object )	{ return object; }
		static type convert_from_pointer( const void *p )		{ return const_cast< Object* >( static_cast< const Object* >( p ) ); }
	};
	
	template < class Object >
	struct object_parameter_traits< const Object * >
	{
		typedef const Object *type;
		
		static const void *convert_to_pointer( type object )	{ return object; }
		static type convert_from_pointer( const void *p )		{ return static_cast< const Object* >( p ); }
	};
	
	template < class Object >
	struct object_parameter_traits< Object& >
	{
		typedef Object& type;
		
		static const void *convert_to_pointer( type object )	{ return &object; }
		static type convert_from_pointer( const void *p )		{ return *const_cast< Object* >( static_cast< const Object* >( p ) ); }
	};
	
	template < class Object >
	struct object_parameter_traits< const Object& >
	{
		typedef const Object& type;
		
		static const void *convert_to_pointer( type object )	{ return &object; }
		static type convert_from_pointer( const void *p )		{ return *static_cast< const Object* >( p ); }
	};
	
	template <>
	struct object_parameter_traits< void >
	{
		// When void is used as an object type, we specify a parameter of type
		// Null so that templated functions don't have to be specialized to take
		// one less parameter when void is chosen.  This parameter should always
		// be given a default value object_parameter_traits< void >::type().
		
		class Null {};
		typedef Null type;
		
		static const void *convert_to_pointer( type )			{ return 0; }
	};
	
}

#endif

