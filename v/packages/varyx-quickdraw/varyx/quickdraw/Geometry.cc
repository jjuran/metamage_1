/*
	Geometry.cc
	-----------
*/

#include "varyx/quickdraw/Geometry.hh"

// vlib
#include "vlib/types/endec/stdint.hh"
#include "vlib/types/string.hh"
#include "vlib/types/struct/struct.hh"
#include "vlib/types/struct/type.hh"


#if __GNUC__  &&  __GNUC__ < 4
#define VEXING_CONSTRUCTION( Class, name )  Class name = Class
#else
#define VEXING_CONSTRUCTION( Class, name )  Class name
#endif


namespace varyx
{
namespace quickdraw
{

#define FIELD( name, type )  Value( String( name ), Op_mapping, type )

#define STRUCT( name )  const Struct_Type name( name##_fields, String( #name ) )

const Struct_Type& get_Point()
{
	static const VEXING_CONSTRUCTION( Value, Point_fields )
	(
		FIELD( "v", i16_type() ),
		FIELD( "h", i16_type() )
	);
	
	static STRUCT( Point );
	
	return Point;
}

const Struct_Type& get_Rect()
{
	static const VEXING_CONSTRUCTION( Value, Rect_fields )
	(
		FIELD( "top", i16_type() ),
		Value(
			FIELD( "left", i16_type() ),
			Value(
				FIELD( "bottom", i16_type() ),
				FIELD( "right", i16_type() ) ) )
	);
	
	static STRUCT( Rect );
	
	return Rect;
}

}
}
