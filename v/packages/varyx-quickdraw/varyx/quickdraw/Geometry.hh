/*
	Geometry.hh
	-----------
*/

#ifndef VARYX_QUICKDRAW_GEOMETRY_HH
#define VARYX_QUICKDRAW_GEOMETRY_HH


namespace vlib
{
	
	class Struct_Type;
	
	struct proc_info;
	
}

namespace varyx
{
namespace quickdraw
{
	
	using namespace vlib;
	
	const Struct_Type& get_Point();
	const Struct_Type& get_Rect();
	
	#define DECLARE_PROC(name)  extern const proc_info proc_##name
	
	DECLARE_PROC( SetRect );
	
	DECLARE_PROC( OffsetRect );
	DECLARE_PROC( InsetRect  );
	
	DECLARE_PROC( SectRect  );
	DECLARE_PROC( UnionRect );
	
	DECLARE_PROC( PtInRect );
	DECLARE_PROC( Pt2Rect  );
	
	DECLARE_PROC( PtToAngle );
	
	DECLARE_PROC( EqualRect );
	DECLARE_PROC( EmptyRect );
	
	DECLARE_PROC( AddPt );
	DECLARE_PROC( SubPt );
	DECLARE_PROC( SetPt );
	DECLARE_PROC( EqualPt );
	
	DECLARE_PROC( ScalePt );
	DECLARE_PROC( MapPt   );
	DECLARE_PROC( MapRect );
	
	DECLARE_PROC( DeltaPoint );
	
	#undef DECLARE_PROC
	
}
}

#endif
