/*
	worldview/Render.cc
	-------------------
*/

#include "worldview/Render.hh"

// Standard C
#include <stdint.h>

// Standard C++
#include <vector>

// iota
#include "iota/endian.hh"

// plus
#include "plus/pointer_to_function.hh"

// Vectoria
#include "Vectoria/Units.hh"
#include "Vectoria/Clipping3D.hh"
#include "Vectoria/ViewFrustum.hh"
#include "Vectoria/PlaneVector.hh"
#include "Vectoria/LinearAlgebra3D.hh"
#include "Vectoria/TriColor.hh"

// worldview
#include "worldview/DepthBuffer.hh"
#include "worldview/Model.hh"
#include "worldview/Objects.hh"
#include "worldview/Port.hh"


namespace worldview
{
	
	using V::W;
	using V::X;
	using V::Y;
	using V::Z;
	
	
	template < class Scalar >
	class LinearSpectrum
	{
		private:
			Scalar itsBegin;
			Scalar itsEnd;
		
		public:
			LinearSpectrum( const Scalar& begin,
			                const Scalar& end ) : itsBegin( begin ),
			                                      itsEnd  ( end   )  {}
			
			Scalar operator[]( double t ) const  { return itsBegin * (1 - t) + itsEnd * t; }
	};
	
	template < class Num >
	static inline
	LinearSpectrum< Num > MakeLinearSpectrum( const Num& begin, const Num& end )
	{
		return LinearSpectrum< Num >( begin, end );
	}
	
	
	static
	double FocalLength( V::Radians alpha )
	{
		double e = 1 / std::tan( alpha / 2.0 );
		return e;
	}
	
	static
	V::Radians VerticalFieldOfViewAngle( double aspectRatio, double focalLength )
	{
		double a = aspectRatio;
		double e = focalLength;
		
		V::Radians beta( 2 * std::atan( a / e ) );
		return beta;
	}
	
	
	typedef DepthBuffer< float > DeepPixelDevice;
	
	static DeepPixelDevice gDeepPixelDevice;
	
	
	static const V::Radians sHorizontalFieldOfViewAngle = V::Degrees( 45 );
	
	const double sFocalLength = FocalLength( sHorizontalFieldOfViewAngle );
	
	
	static inline
	ColorMatrix ModulateGray( double gray, const ColorMatrix& light )
	{
		return gray * light;
	}
	
	static inline
	ColorMatrix ModulateColor( const ColorMatrix& color, const ColorMatrix& light )
	{
		using V::Red;
		using V::Green;
		using V::Blue;
		
		return V::MakeRGB( color[ Red   ] * light[ Red   ],
		                   color[ Green ] * light[ Green ],
		                   color[ Blue  ] * light[ Blue  ] );
	}
	
	
	static
	ColorMatrix GetSampleFromMap( const ImageTile& tile, const V::Point2D::Type& point )
	{
		const unsigned width = tile.Width();
		
		const int u = int( std::floor( point[ X ] * width ) ) % width;
		const int v = int( std::floor( point[ Y ] * width ) ) % width;
		
		const unsigned index = v * width + u;
		
		return tile.Values()[ index ];
	}
	
	
	class DeepVertex
	{
		public:
			unsigned            itsIndex;
			const MeshPolygon*  itsPolygon;
			V::Point3D::Type    itsPoint;
			V::Point2D::Type    itsTexturePoint;
			ColorMatrix         itsColor;
			
			DeepVertex()  {}
			
			DeepVertex( const MeshPolygon&       polygon,
			            const V::Point3D::Type&  point,
			            const V::Point2D::Type&  uv    = V::Point2D::Make( 0, 0 ),
			            const ColorMatrix&       color = V::White() );
			
			const MeshPolygon& Polygon() const  { return *itsPolygon; }
			
			template < class Index >
			double operator[]( Index index ) const  { return itsPoint[ index ]; }
	};
	
	DeepVertex::DeepVertex( const MeshPolygon&       polygon,
	                        const V::Point3D::Type&  point,
	                        const V::Point2D::Type&  uv,
	                        const ColorMatrix&       color )
	:
		itsPolygon( &polygon ),
		itsPoint( point ),
		itsTexturePoint( uv ),
		itsColor( color )
	{
	}
	
	static inline
	uint8_t* inscribe_argb_pixel( uint8_t* p, uint8_t r, uint8_t g, uint8_t b )
	{
		if ( iota::is_little_endian() )
		{
			*p++ = b;
			*p++ = g;
			*p++ = r;
			
			p++;  // skip alpha
		}
		else
		{
			p++;  // skip alpha
			
			*p++ = r;
			*p++ = g;
			*p++ = b;
		}
		
		return p;
	}
	
	static inline
	uint8_t* inscribe_argb_pixel( uint8_t* p, double r, double g, double b )
	{
		uint8_t red   = uint8_t( r < 1.0  ?  r * 255  :  255 );
		uint8_t green = uint8_t( g < 1.0  ?  g * 255  :  255 );
		uint8_t blue  = uint8_t( b < 1.0  ?  b * 255  :  255 );
		
		return inscribe_argb_pixel( p, red, green, blue );
	}
	
	static inline
	uint8_t* inscribe_argb_pixel( uint8_t* p, const ColorMatrix& color )
	{
		return inscribe_argb_pixel( p, color[ V::Red   ],
		                               color[ V::Green ],
		                               color[ V::Blue  ] );
	}
	
	
	template < class DoubleSpectrum,
	           class ColorSpectrum >
	static
	void DrawDeepScanLine( int                    y,
	                       double                 left,
	                       double                 right,
	                       const DoubleSpectrum&  w_spectrum,
	                       const ColorSpectrum&   colors,
	                       uint8_t*               rowAddr )
	{
		for ( int x = int( std::ceil( left ) );  x < right;  ++x )
		{
			double tX = (x - left) / (right - left);
			
			double w = w_spectrum[ tX ];
			
			double z = -1.0 / w;
			
			if ( gDeepPixelDevice.SetIfNearer( x, y, -z ) )
			{
				uint8_t* pixelAddr = rowAddr + x * 32/8;
				
				ColorMatrix color = colors[ tX ];
				
				double red   = color[ V::Red   ] / w;
				double green = color[ V::Green ] / w;
				double blue  = color[ V::Blue  ] / w;
				
				inscribe_argb_pixel( pixelAddr, red, green, blue );
			}
		}
	}
	
	template < class DoubleSpectrum,
	           class ColorSpectrum,
	           class UVSpectrum >
	static
	void DrawDeepScanLine( int                    y,
	                       double                 left,
	                       double                 right,
	                       const DoubleSpectrum&  w_spectrum,
	                       const ColorSpectrum&   colors,
	                       const UVSpectrum&      uv_spectrum,
	                       const MeshPolygon&     polygon,
	                       uint8_t*               rowAddr )
	{
		for ( int x = int( std::ceil( left ) );  x < right;  ++x )
		{
			double tX = (x - left) / (right - left);
			
			double w = w_spectrum[ tX ];
			
			double z = -1.0 / w;
			
			if ( gDeepPixelDevice.SetIfNearer( x, y, -z ) )
			{
				uint8_t* pixelAddr = rowAddr + x * 32/8;
				
				ColorMatrix lightColor = colors[ tX ] / w;
				
				ColorMatrix color = ModulateColor( GetSampleFromMap( polygon.Tile(),
				                                                     uv_spectrum[ tX ] / w ),
				                                   lightColor );
				
				inscribe_argb_pixel( pixelAddr, color );
			}
		}
	}
	
	template < class T, class U >
	static inline
	void pin_to_minimum( T& a, U b )
	{
		if ( a < b )
		{
			a = b;
		}
	}
	
	template < class T, class U >
	static inline
	void pin_to_maximum( T& a, U b )
	{
		if ( a > b )
		{
			a = b;
		}
	}
	
	template < class Vertex >
	static
	void DrawDeepTrapezoid( const Vertex&  topLeft,
	                        const Vertex&  topRight,
	                        const Vertex&  bottomLeft,
	                        const Vertex&  bottomRight,
	                        void*          dst,
	                        size_t         height,
	                        size_t         width,
	                        size_t         stride )
	{
		const MeshPolygon& polygon = topLeft.Polygon();
		
		bool using_texture_map = !polygon.Tile().Empty();
		
		double top    = topLeft   [ Y ] * width / -2.0 + height / 2.0;
		double bottom = bottomLeft[ Y ] * width / -2.0 + height / 2.0;
		
		double vdist = bottom - top;
		
		uint8_t* base = (uint8_t*) dst;
		
		short start = short( std::ceil( top    ) );
		short stop  = short( std::ceil( bottom ) );
		
		pin_to_minimum( start, 0            );
		pin_to_maximum( stop,  (int) height );  // unsigned comparison == death
		
		for ( int y = start;  y < stop;  ++y )
		{
			uint8_t* rowAddr = base + y * stride;
			
			double tY = (y - top) / vdist;
			
			double left  = MakeLinearSpectrum( topLeft [ X ], bottomLeft [ X ] )[ tY ] * width / 2.0 + width / 2.0;
			double right = MakeLinearSpectrum( topRight[ X ], bottomRight[ X ] )[ tY ] * width / 2.0 + width / 2.0;
			
			double leftW  = MakeLinearSpectrum( topLeft [ W ], bottomLeft [ W ] )[ tY ];
			double rightW = MakeLinearSpectrum( topRight[ W ], bottomRight[ W ] )[ tY ];
			
			LinearSpectrum< double > w_spectrum = MakeLinearSpectrum( leftW, rightW );
			
			ColorMatrix leftColorW = MakeLinearSpectrum( topLeft.itsColor    * topLeft   [ W ],
			                                             bottomLeft.itsColor * bottomLeft[ W ] )[ tY ];
			
			ColorMatrix rightColorW = MakeLinearSpectrum( topRight.itsColor    * topRight   [ W ],
			                                              bottomRight.itsColor * bottomRight[ W ] )[ tY ];
			
			LinearSpectrum< ColorMatrix > color_spectrum = MakeLinearSpectrum( leftColorW, rightColorW );
			
			if ( !using_texture_map )
			{
				DrawDeepScanLine( y,
				                  left,
				                  right,
				                  w_spectrum,
				                  color_spectrum,
				                  rowAddr );
			}
			else
			{
				V::Point2D::Type leftUV_W = MakeLinearSpectrum( topLeft.itsTexturePoint    * topLeft   [ W ],
				                                                bottomLeft.itsTexturePoint * bottomLeft[ W ] )[ tY ];
				
				V::Point2D::Type rightUV_W = MakeLinearSpectrum( topRight.itsTexturePoint    * topRight   [ W ],
				                                                 bottomRight.itsTexturePoint * bottomRight[ W ] )[ tY ];
				
				DrawDeepScanLine( y,
				                  left,
				                  right,
				                  w_spectrum,
				                  color_spectrum,
				                  MakeLinearSpectrum( leftUV_W, rightUV_W ),
				                  topLeft.Polygon(),
				                  rowAddr );
			}
		}
	}
	
	/*
		C
		
		|\
		| \
		|  \
		|   \
		|    \
		|     \
	  D *- - - > B
		|     /
		|    /
		|   /
		|  /
		| /
		|/
		
		A
	*/
	
	/*
	template < class Vertex >
	static
	void DrawDeepTriangle( const Vertex& A,
	                       const Vertex& B,
	                       const Vertex& C )
	{
		// Assume that the vertices are in port coordinates, and bottom to top.
		double top    = C[ Y ];
		double middle = B[ Y ];
		double bottom = A[ Y ];
		
		double t = (middle - bottom) / (top - bottom);
		
		V::Point3D::Type ptD;
		
		ptD[ X ] = MakeLinearSpectrum( A[X], C[X] )[t];
		ptD[ Y ] = MakeLinearSpectrum( A[Y], C[Y] )[t];
		ptD[ Z ] = MakeLinearSpectrum( A[Z], C[Z] )[t];  // -1
		ptD[ W ] = MakeLinearSpectrum( A[W], C[W] )[t];
		
		V::Point2D::Type uvD;
		
		uvD[ X ] = MakeLinearSpectrum( A.itsTexturePoint[X] * A[ W ], C.itsTexturePoint[X] * C[ W ] )[t] / ptD[ W ];
		uvD[ Y ] = MakeLinearSpectrum( A.itsTexturePoint[Y] * A[ W ], C.itsTexturePoint[Y] * C[ W ] )[t] / ptD[ W ];
		
		ColorMatrix colorD = MakeLinearSpectrum( A.itsColor * A[W], C.itsColor * C[W] )[t] / ptD[ W ];
		
		Vertex D( A.Polygon(), ptD, uvD, colorD );
		
		const Vertex& midLeft  = B[X] < D[X] ? B : D;
		const Vertex& midRight = B[X] < D[X] ? D : B;
		
		if ( top > middle )
		{
			DrawDeepTrapezoid( C,
			                   C,
			                   midLeft,
			                   midRight );
		}
		
		if ( middle > bottom )
		{
			DrawDeepTrapezoid( midLeft,
			                   midRight,
			                   A,
			                   A );
		}
	}
	*/
	
	static inline
	bool VerticallyGreater( const DeepVertex& a, const DeepVertex& b )
	{
		return a[ Y ] > b[ Y ];
	}
	
	static
	void AdvanceVertexIterator( std::vector< DeepVertex >::const_iterator&  it,
	                            unsigned                                    top_index,
	                            unsigned                                    bottom_index,
	                            bool                                        on_right )
	{
		bool seam_is_on_right = top_index < bottom_index;
		
		while (    it->itsIndex != bottom_index
		        && (( it->itsIndex < top_index ) == ( it->itsIndex < bottom_index )) != (seam_is_on_right == on_right) )
		{
			++it;
		}
	}
	
	static
	DeepVertex InterpolateDeepVertex( const DeepVertex& a, const DeepVertex& b, double t )
	{
		V::Point3D::Type point;
		
		point[ X ] = MakeLinearSpectrum( a[X], b[X] )[t];
		point[ Y ] = MakeLinearSpectrum( a[Y], b[Y] )[t];
		point[ Z ] = MakeLinearSpectrum( a[Z], b[Z] )[t];  // -1
		point[ W ] = MakeLinearSpectrum( a[W], b[W] )[t];
		
		V::Point2D::Type uv;
		
		uv[ X ] = MakeLinearSpectrum( a.itsTexturePoint[X] * a[ W ], b.itsTexturePoint[X] * b[ W ] )[t] / point[ W ];
		uv[ Y ] = MakeLinearSpectrum( a.itsTexturePoint[Y] * a[ W ], b.itsTexturePoint[Y] * b[ W ] )[t] / point[ W ];
		
		ColorMatrix color = MakeLinearSpectrum( a.itsColor * a[ W ], b.itsColor * b[ W ] )[t] / point[ W ];
		
		DeepVertex result( a.Polygon(), point, uv, color );
		
		return result;
	}
	
	static
	void DrawDeepPolygon( std::vector< DeepVertex >  vertices,
	                      void*                      dst,
	                      size_t                     height,
	                      size_t                     width,
	                      size_t                     stride )
	{
		std::vector< DeepVertex >& sorted_vertices = vertices;
		
		// sort by Y
		std::sort( sorted_vertices.begin(),
		           sorted_vertices.end(),
		           plus::ptr_fun( VerticallyGreater ) );
		
		double top    = sorted_vertices.front()[ Y ];
		double bottom = sorted_vertices.back ()[ Y ];
		
		unsigned top_index    = sorted_vertices.front().itsIndex;
		unsigned bottom_index = sorted_vertices.back ().itsIndex;
		
		typedef std::vector< DeepVertex >::const_iterator Iter;
		
		DeepVertex prev_left  = sorted_vertices.front();
		DeepVertex prev_right = sorted_vertices.front();
		
		Iter prev_left_it  = sorted_vertices.begin();
		Iter prev_right_it = sorted_vertices.begin();
		
		Iter left_it  = sorted_vertices.begin() + 1;
		Iter right_it = sorted_vertices.begin() + 1;
		
		Iter last = sorted_vertices.end() - 1;
		
		AdvanceVertexIterator( left_it, top_index, bottom_index, false );
		AdvanceVertexIterator( right_it, top_index, bottom_index, true );
		
		while ( left_it < last  ||  right_it < last )
		{
			if ( left_it < right_it )
			{
				prev_left_it = left_it;
				
				double t = ( (*left_it)[ Y ] - (*prev_right_it)[ Y ] )  /  ( (*right_it)[ Y ] - (*prev_right_it)[ Y ] );
				
				DeepVertex interpolated = InterpolateDeepVertex( *prev_right_it, *right_it, t );
				
				DrawDeepTrapezoid( prev_left,
				                   prev_right,
				                   *left_it,
				                   interpolated,
				                   dst,
				                   height,
				                   width,
				                   stride );
				
				prev_left  = *left_it;
				prev_right = interpolated;
				
				AdvanceVertexIterator( ++left_it, top_index, bottom_index, false );
			}
			else
			{
				prev_right_it = right_it;
				
				double t = ( (*right_it)[ Y ] - (*prev_left_it)[ Y ] )  /  ( (*left_it)[ Y ] - (*prev_left_it)[ Y ] );
				
				DeepVertex interpolated = InterpolateDeepVertex( *prev_left_it, *left_it, t );
				
				DrawDeepTrapezoid( prev_left,
				                   prev_right,
				                   interpolated,
				                   *right_it,
				                   dst,
				                   height,
				                   width,
				                   stride );
				
				prev_left  = interpolated;
				prev_right = *right_it;
				
				AdvanceVertexIterator( ++right_it, top_index, bottom_index, true );
			}
		}
		
		DrawDeepTrapezoid( prev_left,
		                   prev_right,
		                   sorted_vertices.back(),
		                   sorted_vertices.back(),
		                   dst,
		                   height,
		                   width,
		                   stride );
	}
	
	
	static bool fishEye = false;
	
	/*
	static
	V::Point3D::Type FishEye( const V::Point3D::Type& pt )
	{
		double x = pt[ X ];
		double y = pt[ Y ];
		double z = pt[ Z ];
		
		double r, theta;
		V::XY2Polar( x, y, r, theta );
		r = atan( r / z );
		r *= z;  // Because the clipping code expects we haven't divided by z yet
		V::Polar2XY( r, theta, x, y );
		
		return V::Point3D::Make( x, y, z );
	}
	
	static
	V::Point3D::Type UnFishEye( const V::Point3D::Type& pt )
	{
		double x = pt[ X ];
		double y = pt[ Y ];
		double z = pt[ Z ];
		
		double r, theta;
		V::XY2Polar( x, y, r, theta );
		r = std::tan( r );
		V::Polar2XY( r, theta, x, y );
		
		return V::Point3D::Make( x, y, z );
	}
	*/
	
	static inline
	V::Point3D::Type PerspectiveDivision( const V::Point3D::Type& pt )
	{
		return pt / -pt[ Z ] * sFocalLength;
	}
	
	static inline
	V::Point2D::Type Point3DTo2D( const V::Point3D::Type& pt )
	{
		return V::Point2D::Make( pt[ X ], pt[ Y ] );
	}
	
	static
	V::Point2D::Type InterpolatedUV( const V::Point3D::Type&  intersection,
	                                 const V::Point3D::Type*  savedPoints,
	                                 const V::Point2D::Type*  uv_points )
	{
		V::Vector3D::Type edgeU = savedPoints[ 1 ] - savedPoints[ 0 ];
		V::Vector3D::Type edgeV = savedPoints[ 2 ] - savedPoints[ 1 ];
		
		V::Vector3D::Type vector = intersection - savedPoints[ 0 ];
		
		double tU = V::ProjectionProfile( vector, edgeU );
		double tV = V::ProjectionProfile( vector, edgeV );
		
		V::Point2D::Type u = ( uv_points[ 1 ] - uv_points[ 0 ] ) * tU;
		V::Point2D::Type v = ( uv_points[ 2 ] - uv_points[ 0 ] ) * tV;
		
		return u + v + uv_points[ 0 ];
	}
	
	
	static inline
	double ProximityQuotient( double distance )
	{
		return 1 / (1 + distance * distance);
	}
	
	
	//static const ColorMatrix gWhite = V::MakeGray( 1.0 );
	
	static const ColorMatrix gAmbientLight   = 0.3 * V::MakeRGB( 0.8, 0.8, 1.0 );
	static const ColorMatrix gCameraLight    = 0.9 * V::MakeRGB( 1.0, 1.0, 0.6 );
	static const ColorMatrix gSelectionLight = 0.3 * V::MakeRGB( 1.0, 0.8, 0.8 );
	
	static
	ColorMatrix LightColor( double  distance,
	                        double  incidenceRatio,
	                        bool    selected )
	{
		double proximity = ProximityQuotient( distance / 2 );
		
		/*
		const bool considerColor     = true;
		const bool considerAmbience  = true;
		const bool considerProximity = true;
		const bool considerIncidence = true;
		
		if ( !considerColor )
		{
			color = white;
		}
		
		if ( !considerAmbience )
		{
			ambientLight = white;
		}
		
		if ( !considerProximity )
		{
			proximity = 1.0;
		}
		
		if ( !considerIncidence )
		{
			incidenceRatio = 1.0;
		}
		*/
		
		// If the sum of the coefficients below <= 1.0,
		// then no color clipping ('overexposure') can occur.
		
		ColorMatrix totalLight =   gAmbientLight
		                         + gCameraLight * proximity * incidenceRatio;
		
		if ( selected )
		{
			totalLight += gSelectionLight;
		}
		
		return totalLight;
	}
	
	static inline
	ColorMatrix TweakColor( const ColorMatrix&  color,
	                        double              distance,
	                        double              incidenceRatio,
	                        bool                selected )
	{
		return ModulateColor( color, LightColor( distance, incidenceRatio, selected ) );
	}
	
	static
	double operator*( const V::Vector3D::Type& a, const V::Vector3D::Type& b )
	{
		return DotProduct( a, b );
	}
	
	void paint_onto_surface( const MeshModel*  begin,
	                         const MeshModel*  end,
	                         void*             dst,
	                         size_t            width,
	                         size_t            height,
	                         size_t            stride )
	{
		uint8_t* base = (uint8_t*) dst;
		
		//fishEye = itsPort.mCamera.fishEyeMode;
		
		gDeepPixelDevice.Resize( width, height );
		
		const V::Point3D::Type pt0 = V::Point3D::Make( 0, 0, 0 );
		
		// For each mesh model...
		for ( const MeshModel* it = begin;  it != end;  ++it )
		{
			const MeshModel& model = *it;
			
			bool selected = model.Selected();
			
			const PointMesh& mesh = model.Mesh();
			
			// Sanity check:  Must have some points to work with.
			if ( mesh.Empty() )  continue;
			
			const std::vector< MeshPolygon >& polygons = model.Polygons();
			
			typedef std::vector< MeshPolygon >::const_iterator PolygonIter;
			
			// For each polygon in the mesh...
			for ( PolygonIter it = polygons.begin(), end = polygons.end();  it != end;  ++it )
			{
				const MeshPolygon& polygon = *it;
				
				const std::vector< unsigned >& offsets = polygon.Vertices();
				
				if ( offsets.empty() )
				{
					continue;
				}
				
				V::Point3D::Type savedPoints[3];
				
				unsigned const *const savedOffsets = polygon.SavedOffsets();
				
				savedPoints[0] = mesh.Points()[ savedOffsets[ 0 ] ];
				savedPoints[1] = mesh.Points()[ savedOffsets[ 1 ] ];
				savedPoints[2] = mesh.Points()[ savedOffsets[ 2 ] ];
				
				std::vector< V::Point3D::Type > points;
				
				points.reserve( offsets.size() );
				
				// Lookup the vertices of this polygon
				// in port coordinates
				for ( std::size_t i = 0;  i < offsets.size();  ++i )
				{
					points.push_back( mesh( offsets[ i ] ) );
				}
				
				V::Vector3D::Type faceNormal = V::UnitLength( V::FaceNormal( points ) );
				
				V::Plane3D::Type plane = V::PlaneVector( faceNormal, points[ 0 ] );
				
				std::transform( points.begin(),
				                points.end(),
				                points.begin(),
				                plus::ptr_fun( PerspectiveDivision ) );
				
				std::vector< DeepVertex > vertices( points.size() );
				
				// For each vertex in the polygon
				for ( unsigned int i = 0;  i < vertices.size();  ++i )
				{
					DeepVertex& pt = vertices[ i ] = DeepVertex( polygon, points[ i ] );
					
					pt.itsIndex = i;
					
					V::Point3D::Type pt1 = V::Point3D::Make( pt[X], pt[Y], -sFocalLength );
					
					if ( fishEye )
					{
					//	pt1 = UnFishEye(pt1);
					}
					
					// The ray is inverted to face the same way as the face normal.
					V::Vector3D::Type ray = UnitLength( pt0 - pt1 );
					
					V::Point3D::Type sectPt = LinePlaneIntersection( ray, pt0, plane );
					
					double dist = V::Magnitude( V::Vector3D::Type( sectPt - pt0 ) );
					// P . Q = mag(P)*mag(Q)*cos(a)
					// cos(a) = P.Q / mag(P) / mag(Q)
					// The ray and the normal are already unit length,
					// so their magnitudes are 1.
					double cosAlpha = ray * faceNormal;
					double incidenceRatio = cosAlpha;
					
					pt.itsColor = LightColor( dist, incidenceRatio, selected );
					
					if ( polygon.Tile().Empty() )
					{
						pt.itsColor = ModulateColor( polygon.Color(), pt.itsColor );
					}
					else
					{
						pt.itsTexturePoint = InterpolatedUV( sectPt, savedPoints, polygon.MapPoints() );
					}
				}
				
				DrawDeepPolygon( vertices,
				                 base,
				                 height,
				                 width,
				                 stride );
			}
		}
	}
	
	MeshModel* hit_test( Frame& frame, double x, double y )
	{
		V::Point3D::Type pt1 = V::Point3D::Make( x, y, -sFocalLength );
		
		if ( fishEye )
		{
		//	pt1 = UnFishEye(pt1);
		}
		
		return frame.HitTest( pt1 );
	}
	
	void trace_onto_surface( const MeshModel*  begin,
	                         const MeshModel*  end,
	                         void*             dst,
	                         size_t            width,
	                         size_t            height,
	                         size_t            stride )
	{
		//DeepPixelDevice device( width, height );
		gDeepPixelDevice.Resize( width, height );
		
		// Always 1, but could be altered to make multiple passes with a smaller depth buffer
		unsigned hPasses = (width - 1) / width + 1;
		unsigned vPasses = (height - 1) / height + 1;
		
		const V::Point3D::Type pt0 = V::Point3D::Make( 0, 0, 0 );
		
		// For each vertical pass
		for ( unsigned vp = 0;  vp < vPasses;  ++vp )
		{
			// For each horizontal pass
			for ( unsigned hp = 0;  hp < hPasses;  ++hp )
			{
				gDeepPixelDevice.Reset();
				V::Rect2D< int > depthRect;
				
				depthRect.left   = hp * width;
				depthRect.right  = (hp + 1) * width;
				depthRect.bottom = vp * height;
				depthRect.top    = (vp + 1) * height;
				
				// For each mesh model...
				for ( const MeshModel* it = begin;  it != end;  ++it )
				{
					const MeshModel& model = *it;
					
					bool selected = model.Selected();
					
					const PointMesh& mesh = model.Mesh();
					
					// Sanity check:  Must have some points to work with.
					if ( mesh.Empty() )  continue;
					
					// Fish-eye view distortion
					if ( fishEye )
					{
					//	transform(points.begin(), points.end(), points.begin(), FishEye);
					}
					
					const std::vector< MeshPolygon >& polygons = model.Polygons();
					
					typedef std::vector< MeshPolygon >::const_iterator PolygonIter;
					
					// For each polygon in the mesh...
					for ( PolygonIter it = polygons.begin(), end = polygons.end();  it != end;  ++it )
					{
						const MeshPolygon& polygon = *it;
						
						const std::vector< unsigned >& offsets = polygon.Vertices();
						
						unsigned const *const savedOffsets = polygon.SavedOffsets();
						
						if ( offsets.empty() )
						{
							continue;
						}
						
						const ImageTile& tile = polygon.Tile();
						
						V::Point3D::Type savedPoints[3];
						
						savedPoints[0] = mesh.Points()[ savedOffsets[ 0 ] ];
						savedPoints[1] = mesh.Points()[ savedOffsets[ 1 ] ];
						savedPoints[2] = mesh.Points()[ savedOffsets[ 2 ] ];
						
						std::vector< V::Point3D::Type > points;
						
						points.reserve( offsets.size() );
						
						// Lookup the vertices of this polygon
						// in port coordinates
						for ( std::size_t i = 0;  i < offsets.size();  ++i )
						{
							points.push_back( mesh( offsets[ i ] ) );
						}
						
						V::Vector3D::Type faceNormal = V::UnitLength( V::FaceNormal( points ) );
						
						V::Plane3D::Type plane = V::PlaneVector( faceNormal, points[ 0 ] );
						
						// Perspective division
						std::transform( points.begin(),
						                points.end(),
						                points.begin(),
						                plus::ptr_fun( PerspectiveDivision ) );
						
						V::Polygon2D poly2d;
						
						std::vector< V::Point2D::Type >& screenPts( poly2d.Points() );
						
						screenPts.reserve( points.size() );
						
						for ( std::size_t i = 0;  i < points.size();  ++i )
						{
							screenPts.push_back( Point3DTo2D( points[ i ] ) );
						}
						
						V::Rect2D< double > bounding_rect = poly2d.BoundingRect();
						
						bounding_rect.left  = bounding_rect.left  * width / 2.0 + width / 2.0;
						bounding_rect.right = bounding_rect.right * width / 2.0 + width / 2.0;
						
						bounding_rect.top    = bounding_rect.top    * width / -2.0 + height / 2.0;
						bounding_rect.bottom = bounding_rect.bottom * width / -2.0 + height / 2.0;
						
						V::Rect2D< int > bounds;
						bounds = bounding_rect;
						
						// Extend the rect to account for truncation error
						bounds.right  += 1;
						bounds.bottom += 1;
						
						// Intersect the polygon bounds with the depth buffer bounds
						V::Rect2D< int > rect = depthRect * bounds;
						
						V::Point3D::Type current_pixel_3d;
						V::Point2D::Type current_pixel_2d;
						
						current_pixel_3d[ Z ] = -sFocalLength;
						current_pixel_3d[ W ] =  1.0;
						current_pixel_2d[ W ] =  1.0;
						
						// For each row
						for ( unsigned iY = rect.top;  iY < rect.bottom;  ++iY )
						{
							//escapement();
							
							current_pixel_3d[ Y ] =
							current_pixel_2d[ Y ] = (iY + 0.5 - height / 2.0) / (width / -2.0);
							
							uint8_t* rowAddr = (uint8_t*) dst + iY * stride;
							
							// For each pixel in the row
							for ( unsigned iX = rect.left;  iX < rect.right;  ++iX )
							{
								current_pixel_3d[ X ] =
								current_pixel_2d[ X ] = (iX + 0.5 - width / 2.0) / (width / 2.0);
								
								const V::Point3D::Type& pt1 = current_pixel_3d;
								
								if ( fishEye )
								{
								//	pt1 = UnFishEye(pt1);
								}
								
								// The ray is inverted to face the same way as the face normal.
								V::Vector3D::Type ray = pt0 - pt1;
								
								V::Point3D::Type sectPt = LinePlaneIntersection( ray, pt0, plane );
								
								double dist = V::Magnitude( sectPt - pt0 );
								
								if (    dist > 0
								     && gDeepPixelDevice.Nearer( iX % width, iY % height, dist )
								     && poly2d.ContainsPoint( current_pixel_2d ) )
								{
									// set the pixel, below
								}
								else
								{
									continue;
								}
								
								gDeepPixelDevice.Set( iX % width, iY % height, dist );
								
								// P . Q = mag(P) * mag(Q) * cos(a)
								// cos(a) = P.Q / mag(P) / mag(Q)
								// The normal is already unit length, so its magnitude is 1.
								/*
								double cosTheta = DotProduct( ray, faceNormal )
									/ Magnitude( ray ) / Magnitude( faceNormal );
								*/
								double cosAlpha = ray * faceNormal / V::Magnitude( ray );
								double incidenceRatio = cosAlpha;
								
								ColorMatrix lightColor = LightColor( dist, incidenceRatio, selected );
								
								ColorMatrix sample = tile.Empty() ? polygon.Color()
								                                  : GetSampleFromMap( tile,
								                                                      InterpolatedUV( sectPt,
								                                                                      savedPoints,
								                                                                      polygon.MapPoints() ) );
								
								ColorMatrix tweaked = ModulateColor( sample, lightColor );
								
								uint8_t* pixelAddr = rowAddr + iX * 32/8;
								
								inscribe_argb_pixel( pixelAddr, tweaked );
							}
						}
					}
				}
			}
		}
	}
	
}
