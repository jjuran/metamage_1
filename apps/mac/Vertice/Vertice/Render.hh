/*
	Vertice/Render.hh
	-----------------
*/

#ifndef VERTICE_RENDER_HH
#define VERTICE_RENDER_HH

// Standard C
#include <stddef.h>

// Vertice
#include "Vertice/Objects.hh"


namespace Vertice
{
	
	class Frame;
	
	MeshModel* hit_test( Frame& frame, double x, double y );
	
	void paint_onto_surface( const std::vector< MeshModel >&  models,
	                         void*                            dst,
	                         size_t                           width,
	                         size_t                           height,
	                         size_t                           stride );
	
	void trace_onto_surface( const std::vector< MeshModel >&  models,
	                         void*                            dst,
	                         size_t                           width,
	                         size_t                           height,
	                         size_t                           stride );
	
}

#endif
