/*
	Region-procs.hh
	---------------
*/

#ifndef VARYX_QUICKDRAW_REGIONPROCS_HH
#define VARYX_QUICKDRAW_REGIONPROCS_HH


namespace vlib
{
	
	struct proc_info;
	
}

namespace varyx
{
namespace quickdraw
{
	
	using namespace vlib;
	
	#define DECLARE_PROC(name)  extern const proc_info proc_##name
	
	DECLARE_PROC( SetRectRgn );
	DECLARE_PROC( RectRgn    );
	
	DECLARE_PROC( OffsetRgn );
	DECLARE_PROC( InsetRgn  );
	
	DECLARE_PROC( SectRgn  );
	DECLARE_PROC( UnionRgn );
	DECLARE_PROC( DiffRgn  );
	DECLARE_PROC( XorRgn   );
	
	DECLARE_PROC( PtInRgn   );
	DECLARE_PROC( RectInRgn );
	
	DECLARE_PROC( EqualRgn );
	DECLARE_PROC( EmptyRgn );
	
	DECLARE_PROC( MapRgn );
	
	#undef DECLARE_PROC
	
}
}

#endif
