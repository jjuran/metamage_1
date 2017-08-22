/*
	Vertice/Render.hh
	-----------------
*/

#ifndef VERTICE_RENDER_HH
#define VERTICE_RENDER_HH

// Standard C
#include <stddef.h>


namespace Vertice
{
	
	class Frame;
	class MeshModel;
	
	typedef void (*render_proc)( const MeshModel*,
	                             const MeshModel*,
	                             void*,
	                             size_t,
	                             size_t,
	                             size_t );
	
	void paint_onto_surface( const MeshModel*  begin,
	                         const MeshModel*  end,
	                         void*             dst,
	                         size_t            width,
	                         size_t            height,
	                         size_t            stride );
	
	void trace_onto_surface( const MeshModel*  begin,
	                         const MeshModel*  end,
	                         void*             dst,
	                         size_t            width,
	                         size_t            height,
	                         size_t            stride );
	
	MeshModel* hit_test( Frame& frame, double x, double y );
	
}

#endif
