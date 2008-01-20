/*	===================
 *	Vertice/PortView.cc
 *	===================
 */

#include "Vertice/PortView.hh"

// Standard C++
#include <vector>

// Nucleus
#include "Nucleus/Saved.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Nitrogen
#include "Nitrogen/QuickDraw.h"

// Nitrogen Extras / Templates
#include "Templates/Timing.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"

// GrafX
#include "GrafX/Pixel32.hh"
#include "GrafX/RGBColor.hh"

// Vectoria
#include "Vectoria/Units.hh"
#include "Vectoria/Clipping3D.hh"
#include "Vectoria/HomogeneousClipping.hh"
#include "Vectoria/ViewFrustum.hh"
#include "Vectoria/Polygon3D.hh"
#include "Vectoria/Plane3D.hh"
#include "Vectoria/LinearAlgebra3D.hh"
#include "Vectoria/TriColor.hh"

// Portage
#include "Portage/DepthBuffer.hh"

// Vertice
#include "Vertice/Objects.hh"
#include "Vertice/Model.hh"
#include "Vertice/Port.hh"


namespace Vertice
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace NX = NitrogenExtras;
	namespace GX = GrafX;
	
	
	using V::W;
	
	
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
	
	template < class Scalar >
	LinearSpectrum< Scalar > MakeLinearSpectrum( const Scalar& begin, const Scalar& end )
	{
		return LinearSpectrum< Scalar >( begin, end );
	}
	
	
	static double AspectRatio( double width, double height )
	{
		return height / width;
	}
	
	static double FocalLength( V::Radians alpha )
	{
		double e = 1 / std::tan( alpha / 2.0 );
		return e;
	}
	
	static V::Radians VerticalFieldOfViewAngle( double aspectRatio, double focalLength )
	{
		double a = aspectRatio;
		double e = focalLength;
		
		V::Radians beta( 2 * std::atan( a / e ) );
		return beta;
	}
	
	
	typedef Portage::DepthBuffer< float > DeepPixelDevice;
	
	static DeepPixelDevice gDeepPixelDevice;
	
	
	static double sAspectRatio = 1;
	
	static V::Radians sHorizontalFieldOfViewAngle = V::Degrees( 60 );
	
	static double sFocalLength = FocalLength( sHorizontalFieldOfViewAngle );
	
	static V::XMatrix gPort2Clip;
	
	//static V::Radians sVerticalFieldOfViewAngle;
	
	/*
	template < class Device >
	double AspectRatio( const Device& device )
	{
		return AspectRatio( Width( device ), Height( device ) );
	}
	*/
	
	
	class DeepVertex
	{
		public:
			V::Point3D::Type  itsPoint;
			ColorMatrix       itsColor;
			
			DeepVertex() {}
			
			DeepVertex( const V::Point3D::Type&  point,
			            const ColorMatrix&       color ) : itsPoint( point ),
			                                               itsColor( color )
			{
			}
			
			template < class Index >
			double operator[]( Index index ) const  { return itsPoint[ index ]; }
	};
	
	template < class WSpectrum, class ColorSpectrum, class Device >
	void DrawDeepScanLine( int                   y,
	                       int                   farLeft,
	                       double                left,
	                       double                right,
	                       const WSpectrum&      w,
	                       const ColorSpectrum&  colors,
	                       ::Ptr                 rowAddr,
	                       Device&               deepPixelDevice )
	{
		for ( int x = int( std::ceil( left ) );  x < right;  ++x )
		{
			double tX = (x - left) / (right - left);
			
			double z = -1.0 / w[ tX ];
			
			if ( deepPixelDevice.SetIfNearer( x, y, -z ) )
			{
				ColorMatrix color = colors[ tX ] / w[ tX ];
				
				N::RGBColor rgb = NN::Convert< N::RGBColor >( color );
				
				::Ptr pixelAddr = rowAddr + (x - farLeft) * 32/8;
				
				*reinterpret_cast< GX::Pixel32* >( pixelAddr ) = GX::Pixel32( rgb );
			}
		}
	}
	
	static bool CheckPixMap( PixMapHandle pixMapHandle )
	{
	#if !TARGET_API_MAC_CARBON
		
		PixMap& pixMap = **pixMapHandle;
		
		// We can't deal with BitMap structures
		if ( ( pixMap.rowBytes & 0x8000 ) != 0x8000 )  return false;
		
		// We can't deal with packed data
		if ( pixMap.packType != 0                   )  return false;
		
		// We can't deal with indexed colors (yet)
		if ( pixMap.pixelType != RGBDirect          )  return false;
		if ( pixMap.pixelSize != 32                 )  return false;
		
	#endif
		
		return true;
	}
	
	template < class Vertex, class Device >
	void DrawDeepTrapezoid( Vertex   topLeft,
	                        Vertex   topRight,
	                        Vertex   bottomLeft,
	                        Vertex   bottomRight,
	                        Device&  deepPixelDevice )
	{
		::CGrafPtr port = N::GetQDGlobalsThePort();
		//::CGrafPtr port = itsGWorld.Get();
		// Verify that it's a color port
		if ( !::IsPortColor( port ) )  return;
		
		PixMapHandle pix = ::GetPortPixMap( port );
		
		if ( !CheckPixMap( pix ) ) return;
		
		const Rect& portRect = N::GetPortBounds( port );
		
		short width  = portRect.right  - portRect.left;
		short height = portRect.bottom - portRect.top;
		
		double top    = topLeft   [ Y ] * -width / 2.0 + height / 2.0;
		double bottom = bottomLeft[ Y ] * -width / 2.0 + height / 2.0;
		
		double vdist = bottom - top;
		
		const Rect& bounds   = ( *pix )->bounds;
		::Ptr       base     = ( *pix )->baseAddr;
		unsigned    rowBytes = ( *pix )->rowBytes & 0x3fff;
		
		short start = short( std::ceil( top    ) );
		short stop  = short( std::ceil( bottom ) );
		
		start = std::max( start, std::max( portRect.top,    bounds.top ) );
		stop  = std::min( stop,  std::min( portRect.bottom, bounds.bottom ) );
		
		for ( int y = start;  y < stop;  ++y )
		{
			::Ptr rowAddr = base + ( y - bounds.top ) * rowBytes;
			
			double tY = (y - top) / vdist;
			
			double left  = MakeLinearSpectrum( topLeft [ X ], bottomLeft [ X ] )[ tY ] * width / 2.0 + width / 2.0;
			double right = MakeLinearSpectrum( topRight[ X ], bottomRight[ X ] )[ tY ] * width / 2.0 + width / 2.0;
			
			double leftW  = MakeLinearSpectrum( topLeft [ W ], bottomLeft [ W ] )[ tY ];
			double rightW = MakeLinearSpectrum( topRight[ W ], bottomRight[ W ] )[ tY ];
			
			ColorMatrix leftColorW = MakeLinearSpectrum( topLeft.itsColor    * topLeft   [ W ],
			                                             bottomLeft.itsColor * bottomLeft[ W ] )[ tY ];
			
			ColorMatrix rightColorW = MakeLinearSpectrum( topRight.itsColor    * topRight   [ W ],
			                                              bottomRight.itsColor * bottomRight[ W ] )[ tY ];
			
			DrawDeepScanLine( y,
			                  bounds.left,
			                  left,
			                  right,
			                  MakeLinearSpectrum( leftW,      rightW      ),
			                  MakeLinearSpectrum( leftColorW, rightColorW ),
			                  rowAddr,
			                  deepPixelDevice );
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
	
	template < class Vertex, class Device >
	void DrawDeepTriangle( const Vertex& A,
	                       const Vertex& B,
	                       const Vertex& C, Device& device )
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
		
		ColorMatrix colorD = MakeLinearSpectrum( A.itsColor * A[W], C.itsColor * C[W] )[t] / ptD[ W ];
		
		Vertex D( ptD, colorD );
		
		const Vertex& midLeft  = B[X] < D[X] ? B : D;
		const Vertex& midRight = B[X] < D[X] ? D : B;
		
		if ( top > middle )
		{
			DrawDeepTrapezoid( C,
			                   C,
			                   midLeft,
			                   midRight,
			                   device );
		}
		
		if ( middle > bottom )
		{
			DrawDeepTrapezoid( midLeft,
			                   midRight,
			                   A,
			                   A,
			                   device );
		}
	}
	
	static void SetPortToClipTransform()
	{
		double n = 0.01;  // Think 1cm
		double f = 100;  // Think 100m
		double e = sFocalLength;
		double a = sAspectRatio;
		
		gPort2Clip = V::MakePortToClipTransform( n, f, e, a );
	}
	
	PortView::PortView( const Rect& bounds, Initializer ) : itsPort           ( itsScene                   ),
	                                                        itsSelectedContext(                            ),
	                                                        itsGWorld         ( N::NewGWorld( 32, bounds ) )
	{
		SetBounds( bounds );
		N::LockPixels( N::GetGWorldPixMap( itsGWorld ) );
	}
	
	static V::Point3D::Type PortFromScreen_Point( const V::Point3D::Type&  point,
	                                              short                    width,
	                                              short                    height,
	                                              double                   e )
	{
		V::Point3D::Type result;
		
		double half_width  = width  / 2.0;
		double half_height = height / 2.0;
		
		result[ X ] = ( point[ X ] - half_width  ) / ( e *  half_width );
		result[ Y ] = ( point[ Y ] - half_height ) / ( e * -half_width );
		
		result[ Z ] = point[ Z ];
		result[ W ] = point[ W ];
		
		return result;
	}
	
	static const V::XMatrix& ScreenToPortTransform( short width, short height )
	{
		static V::XMatrix screen2port;
		
		static short oldWidth = 0, oldHeight = 0;
		
		if ( width == oldWidth  &&  height == oldHeight )
		{
			return screen2port;
		}
		
		oldWidth = width;
		oldHeight = height;
		
		double e = sFocalLength;
		
		V::Translation translate( -width / 2.0, -height / 2.0, 0 );
		
		V::XMatrix scale = V::IdentityMatrix();
		
		scale.Cell( 0, 0 ) = 1.0 / (e * width / 2);
		//scale.Cell(1, 1) = 1.0 / (-height / 2);
		scale.Cell( 1, 1 ) = 1.0 / (e * -width / 2);
		
		return screen2port = Compose( translate.Make(), scale );
	}
	
	void PortView::SetBounds( const Rect& bounds )
	{
		itsBounds = bounds;
		
		N::InvalRect( bounds );  // Invalidate the entire window, not just the new area
		
		short width  = NX::RectWidth ( bounds );
		short height = NX::RectHeight( bounds );
		
		itsScreen2Port = ScreenToPortTransform( width, height );
		
		sAspectRatio = AspectRatio( width, height );
		
		SetPortToClipTransform();
	}
	
	static const V::XMatrix& PortToScreenTransform( short width, short height )
	{
		static V::XMatrix port2screen;
		
		static short oldWidth = 0, oldHeight = 0;
		
		if ( width == oldWidth  &&  height == oldHeight )
		{
			return port2screen;
		}
		
		oldWidth = width;
		oldHeight = height;
		
		double e = sFocalLength;
		
		V::XMatrix scale = V::IdentityMatrix();
		
		scale.Cell( 0, 0 ) = e * width / 2;
		//scale.Cell(1, 1) = -height / 2;
		scale.Cell( 1, 1 ) = e * -width / 2;
		
		V::Translation translate( width / 2, height / 2, 0 );
		
		return port2screen = Compose( scale, translate.Make() );
	}
	
	static bool fishEye = false;
	
	/*
	static V::Point3D::Type FishEye( const V::Point3D::Type& pt )
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
	
	static V::Point3D::Type UnFishEye( const V::Point3D::Type& pt )
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
	
	/*
	static V::Point3D::Type PerspectiveDivision( const V::Point3D::Type& pt )
	{
		return pt / -pt[ Z ] * sFocalLength;
	}
	*/
	
	/*
	static V::Point3D::Type HomogeneousPerspectiveDivision( const V::Point3D::Type& pt )
	{
		return V::Point3D::Make( pt[ X ] / pt[ W ],
		                         pt[ Y ] / pt[ W ],
		                         pt[ Z ] / pt[ W ] );
	}
	*/
	
	static V::Point2D::Type Point3DTo2D( const V::Point3D::Type& pt )
	{
		return V::Point2D::Make( pt[ X ], pt[ Y ] );
	}
	
	static DeepVertex Point3DToDeepVertex( const V::Point3D::Type& pt )
	{
		return DeepVertex( pt, V::White() );
	}
	
	template < class PtIter, class TriIter, class TriMaker >
	void PolygonToTriangles( PtIter    ptsBegin,
	                         PtIter    ptsEnd,
	                         TriIter   triBegin,
	                         TriMaker  MakeTriangle )
	{
		if ( ptsEnd - ptsBegin < 3 ) return;
		
		PtIter commonVertex = ptsBegin;
		
		for ( PtIter it = ptsBegin + 1;  it + 1 < ptsEnd;  ++it )
		{
			*triBegin++ = MakeTriangle( *commonVertex, *it, *( it + 1 ) );
		}
	}
	
	template < class Vertex >
	class AdHocTriangle
	{
		public:
			typedef Vertex vertex_type;
		
		//private:
			Vertex a, b, c;
		
		public:
			class Maker
			{
				public:
					AdHocTriangle< Vertex > operator()( const Vertex& A, const Vertex& B, const Vertex& C )
					{
						return AdHocTriangle< Vertex >( A, B, C );
					}
			};
			
			AdHocTriangle()
			{
			}
			
			AdHocTriangle( const Vertex& A,
			               const Vertex& B,
			               const Vertex& C ) : a( A ),
			                                   b( B ),
			                                   c( C )
			{
			}
	};
	
	template < class Triangle >
	struct SortTriangleVertices
	{
		Triangle operator()( const Triangle& triangle )
		{
			typedef typename Triangle::vertex_type Vertex;
			typedef Vertex const *VertexPtr;
			
			const Vertex& A = triangle.a;
			const Vertex& B = triangle.b;
			const Vertex& C = triangle.c;
			
			VertexPtr top = &A, middle = &B, bottom = &C;
			
			if ( A[Y] <= B[Y]  &&  A[Y] <= C[Y] )
			{
				if ( B[Y] > C[Y] )
				{
					std::swap( middle, bottom );
				}
			}
			else if ( B[Y] <= C[Y] )
			{
				std::swap( top, middle );
				
				if ( A[Y] > C[Y] )
				{
					std::swap( middle, bottom );
				}
			}
			else
			{
				std::swap( top, bottom );
				
				if ( B[Y] > A[Y] )
				{
					std::swap( middle, bottom );
				}
			}
			
			return Triangle( *top, *middle, *bottom );
		}
	};
	
	class DeepTriangleDrawer
	{
		private:
			DeepPixelDevice& itsDevice;
		
		public:
			typedef AdHocTriangle< DeepVertex > Triangle;
			
			DeepTriangleDrawer( DeepPixelDevice& device ) : itsDevice( device )
			{
			}
			
			void operator()( const Triangle& triangle ) const
			{
				DrawDeepTriangle( triangle.a, triangle.b, triangle.c, itsDevice );
			}
	};
	
	/*
	class ClippingPlane
	{
		private:
			const V::Plane3D::Type& plane;
		
		public:
			ClippingPlane( const V::Plane3D::Type& plane ) : plane( plane )  {}
			
			template < class Points >
			void operator()( Points& points ) const
			{
				ClipPolygonAgainstPlane( points, plane );
			}
	};
	*/
	
	/*
	template < class Container, class Filter >
	void Trim( Container& cont, const Filter& filter )
	{
		cont.resize( std::remove_if( cont.begin(), cont.end(), filter ) - cont.begin() );
	}
	*/
	
	static double ProximityQuotient( double distance )
	{
		return 1 / (1 + distance * distance);
	}
	
	static ColorMatrix ModulateColor( ColorMatrix color, ColorMatrix light )
	{
		using V::Red;
		using V::Green;
		using V::Blue;
		
		return V::MakeRGB( color[ Red   ] * light[ Red   ],
		                   color[ Green ] * light[ Green ],
		                   color[ Blue  ] * light[ Blue  ] );
	}
	
	static ColorMatrix TweakColor( ColorMatrix  color,
	                               double       distance,
	                               double       incidenceRatio,
	                               bool         selected )
	{
		bool considerColor     = true;
		bool considerAmbience  = true;
		bool considerProximity = true;
		bool considerIncidence = true;
		
		ColorMatrix white = V::MakeGray( 1.0 );
		
		double proximity = ProximityQuotient( distance / 2 );
		
		ColorMatrix ambientLight   = V::MakeRGB( 0.8, 0.8, 1.0 );
		ColorMatrix cameraLight    = V::MakeRGB( 1.0, 1.0, 0.6 );
		ColorMatrix selectionLight = V::MakeRGB( 1.0, 0.8, 0.8 );
		
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
		
		// If the sum of the coefficients below <= 1.0, 
		// then no color clipping ('overexposure') can occur.
		
		ColorMatrix totalLight =   ( 0.3 * ambientLight                             )
		                         + ( 0.9 * cameraLight * proximity * incidenceRatio )
		                         + ( 0.3 * selected * selectionLight );
		
		//return color * totalLight;
		return ModulateColor( color, totalLight );
	}
	
	static double operator*( const V::Vector3D::Type& a, const V::Vector3D::Type& b )
	{
		return DotProduct( a, b );
	}
	
	void PortView::Draw()
	{
		itsPort.MakeFrame( itsFrame );
		
		Redraw();
	}
	
	void PortView::Redraw()
	{
		unsigned width  = NX::RectWidth ( itsBounds );
		unsigned height = NX::RectHeight( itsBounds );
		
		NN::Saved< N::GWorld_Value > savedGWorld;
		N::SetGWorld( itsGWorld );
		
		N::RGBBackColor( NN::Make< RGBColor >( 0 ) );
		
		//fishEye = itsPort.mCamera.fishEyeMode;
		
		N::EraseRect( itsBounds );
		
		gDeepPixelDevice.Resize( width, height );
		
		V::Point3D::Type pt0 = V::Point3D::Make( 0, 0, 0 );
		
		const std::vector< MeshModel >& models = itsFrame.Models();
		
		typedef std::vector< MeshModel >::const_iterator ModelIter;
		
		// For each mesh model...
		for ( ModelIter it = models.begin(), end = models.end();  it != end;  ++it )
		{
			const MeshModel& model = *it;
			
			bool selected = model.Selected();
			
			const PointMesh< V::Point3D::Type >& mesh = model.Mesh();
			
			// Sanity check:  Must have some points to work with.
			if ( mesh.Empty() )  continue;
			
			const std::vector< MeshPoly >& polygons = model.Polygons();
			
			typedef std::vector< MeshPoly >::const_iterator PolygonIter;
			
			// For each polygon in the mesh...
			for ( PolygonIter it = polygons.begin(), end = polygons.end();  it != end;  ++it )
			{
				MeshPoly poly = *it;
				const std::vector< unsigned >& offsets = poly.Vertices();
				
				if ( offsets.empty() )
				{
					continue;
				}
				
				std::vector< V::Point3D::Type > points( offsets.size() );
				
				// Lookup the vertices of this polygon
				// in port coordinates
				std::transform( offsets.begin(),
				                offsets.end(),
				                points.begin(),
				                mesh );
				
				std::vector< V::Point3D::Type > facePoints( 3 );
				
				facePoints[0] = points[0] / points[0][ W ];
				facePoints[1] = points[1] / points[1][ W ];
				facePoints[2] = points[2] / points[2][ W ];
				
				V::Vector3D::Type faceNormal = V::UnitLength( V::FaceNormal( facePoints ) );
				
				V::Plane3D::Type plane = V::PlaneVector( faceNormal, facePoints[ 0 ] );
				
				std::vector< DeepVertex > vertices( points.size() );
				
				std::transform( points.begin(),
				                points.end(),
				                vertices.begin(),
				                std::ptr_fun( Point3DToDeepVertex ) );
				
				// For each vertex in the polygon
				for ( unsigned int i = 0;  i < vertices.size();  ++i )
				{
					DeepVertex& pt = vertices[ i ];
					
					V::Point3D::Type pt1 = V::Point3D::Make( pt[X], pt[Y], -sFocalLength );
					
					if ( fishEye )
					{
					//	pt1 = UnFishEye(pt1);
					}
					
					// The ray is inverted to face the same way as the face normal.
					V::Vector3D::Type ray = UnitLength( pt0 - pt1 );
					
					V::Point3D::Type sectPt = LinePlaneIntersection( ray, pt0, plane );
					
					double dist = Magnitude( V::Vector3D::Type( sectPt - pt0 ) );
					// P . Q = mag(P)*mag(Q)*cos(a)
					// cos(a) = P.Q / mag(P) / mag(Q)
					// The ray and the normal are already unit length,
					// so their magnitudes are 1.
					double cosAlpha = ray * faceNormal;
					double incidenceRatio = cosAlpha;
					
					pt.itsColor = TweakColor( poly.Color(), dist, incidenceRatio, selected );
				}
				
				std::vector< AdHocTriangle< DeepVertex > > triangles( vertices.size() - 2 );
				
				PolygonToTriangles( vertices.begin(),
				                    vertices.end(),
				                    triangles.begin(),
				                    AdHocTriangle< DeepVertex >::Maker() );
				
				std::transform( triangles.begin(),
				                triangles.end(),
				                triangles.begin(),
				                SortTriangleVertices< AdHocTriangle< DeepVertex > >() );
				
				std::for_each( triangles.begin(),
				               triangles.end(),
				               DeepTriangleDrawer( gDeepPixelDevice ) );
				
			}
		}
		
		savedGWorld.Restore();
		
		N::CGrafPtr thePort = N::GetQDGlobalsThePort();
		
		PixMapHandle pix = N::GetGWorldPixMap( thePort );
		NN::Saved< N::PixelsState_Value > savedPixelsState( pix );
		N::LockPixels( pix );
		
		N::CopyBits( N::GetPortBitMapForCopyBits( itsGWorld ),
		             N::GetPortBitMapForCopyBits( thePort ),
		             itsBounds,
		             itsBounds,
		             N::srcCopy );
		
		if ( TARGET_API_MAC_CARBON )
		{
			::QDFlushPortBuffer( ::GetQDGlobalsThePort(), N::RectRgn( itsBounds ) );
		}
	}
	
	
	bool PortView::DispatchCursor( const EventRecord& event )
	{
		N::SetCursor( N::GetCursor( N::crossCursor ) );
		
		return true;
	}
	
	MeshModel* PortView::HitTest( double x, double y )
	{
		int width  = NX::RectWidth ( itsBounds );
		int height = NX::RectHeight( itsBounds );
		
		x = (x + 0.5 - width  / 2.0) / ( width / 2.0);
		y = (y + 0.5 - height / 2.0) / (-width / 2.0);
		
		V::Point3D::Type pt1 = V::Point3D::Make( x, y, -sFocalLength );
		
		if ( fishEye )
		{
		//	pt1 = UnFishEye(pt1);
		}
		
		return itsFrame.HitTest( pt1 );
	}
	
	ColorMatrix PortView::TracePixel( int x, int y )
	{
		/*
		MeshPoly* poly = HitTest( x, y );
		
		return poly ? poly->Color() : V::Black();
		*/
		
		return V::Black();
	}
	
	void PortView::DrawPixel( int x, int y )
	{
		N::SetCPixel( x, y, NN::Convert< N::RGBColor >( TracePixel( x, y ) ) );
	}
	
	/*
	struct TickCounter
	{
		typedef UInt32 Time;
		
		Time operator()() const  { return ::TickCount(); }
	};
	
	struct AnyThreadYielder
	{
		void operator()() const  { N::YieldToAnyThread(); }
	};
	*/
	
	static const bool gBlitting = false;
	
	void PortView::DrawBetter() const
	{
		/*
		typedef NX::Escapement< NX::Timer< TickCounter >,
		                        AnyThreadYielder > Escapement;
		
		Escapement escapement( 10 );
		*/
		
		NN::Saved< N::GWorld_Value > savedGWorld;
		
		if ( gBlitting )
		{
			N::SetGWorld( itsGWorld );
		}
		
		PixMapHandle pix = ::GetPortPixMap( itsGWorld );
		
		if ( !CheckPixMap( pix ) ) return;
		
		const Rect& portRect = N::GetPortBounds( itsGWorld );
		
		const Rect& pixBounds = ( *pix )->bounds;
		::Ptr       baseAddr  = ( *pix )->baseAddr;
		unsigned    rowBytes  = ( *pix )->rowBytes & 0x3fff;
		
		N::RGBForeColor( NN::Make< RGBColor >( 0 ) );
		N::PaintRect( portRect );
		
		short width  = NX::RectWidth ( portRect );
		short height = NX::RectHeight( portRect );
		
		//DeepPixelDevice device( width, height );
		gDeepPixelDevice.Resize( width, height );
		
		// Always 1, but could be altered to make multiple passes with a smaller depth buffer
		unsigned hPasses = (width - 1) / width + 1;
		unsigned vPasses = (height - 1) / height + 1;
		
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
				
				const std::vector< MeshModel >& models = itsFrame.Models();
				
				typedef std::vector< MeshModel >::const_iterator ModelIter;
				
				// For each mesh model...
				for ( ModelIter it = models.begin(), end = models.end();  it != end;  ++it )
				{
					const MeshModel& model = *it;
					
					bool selected = model.Selected();
					
					const PointMesh< V::Point3D::Type >& mesh = model.Mesh();
					
					// Sanity check:  Must have some points to work with.
					if ( mesh.Empty() )  continue;
					
					// Fish-eye view distortion
					if ( fishEye )
					{
					//	transform(points.begin(), points.end(), points.begin(), FishEye);
					}
					
					const std::vector< MeshPoly >& polygons = model.Polygons();
					
					typedef std::vector< MeshPoly >::const_iterator PolygonIter;
					
					// For each polygon in the mesh...
					for ( PolygonIter it = polygons.begin(), end = polygons.end();  it != end;  ++it )
					{
						MeshPoly poly = *it;
						
						std::vector< unsigned > offsets = poly.Vertices();
						
						if ( offsets.empty() )
						{
							continue;
						}
						
						std::vector< V::Point3D::Type > points( offsets.size() );
						//std::vector< V::Point3D::Type > points2( offsets.size() );
						
						// Lookup the vertices of this polygon
						// in port coordinates
						std::transform( offsets.begin(),
						                offsets.end(),
						                points.begin(),
						                mesh );
						
						V::Point3D::Type pt0 = V::Point3D::Make( 0, 0, 0 );
						
						#if 0
						// Port to clip coordinates
						std::transform( points.begin(),
						                points.end(),
						                points2.begin(),
						                V::Transformer< V::Point3D::Type >( gPort2Clip ) );
						#endif
						
						#if 0
						// Perspective division
						std::transform( points2.begin(),
						                points2.end(),
						                points2.begin(),
						                std::ptr_fun( HomogeneousPerspectiveDivision ) );
						#endif
						
						#if 0
						// Perspective division
						std::transform( points.begin(),
						                points.end(),
						                points.begin(),
						                std::ptr_fun( PerspectiveDivision ) );
						#endif
						
						std::vector< V::Point3D::Type > facePoints( 3 );
						
						facePoints[0] = points[0] / points[0][ W ];
						facePoints[1] = points[1] / points[1][ W ];
						facePoints[2] = points[2] / points[2][ W ];
						
						V::Vector3D::Type faceNormal = V::UnitLength( V::FaceNormal( facePoints ) );
						
						V::Plane3D::Type plane = V::PlaneVector( faceNormal, facePoints[ 0 ] );
						
						/*
						for ( int i = 0;  i < points.size();  ++i )
						{
							const V::Point3D::Type& pt  = points [ i ];
							const V::Point3D::Type& pt2 = points2[ i ];
							
							std::printf( "[ %f, %f, -1, %f ]  [ %f, %f, %f, 1 ]\n",
							             pt [ X ], pt [ Y ],           pt [ W ],
							             pt2[ X ], pt2[ Y ], pt2[ Z ] );
						}
						*/
						
						V::Polygon2D poly2d;
						
						std::vector< V::Point2D::Type >& screenPts( poly2d.Points() );
						
						screenPts.resize( points.size() );
						
						std::transform( points.begin(),
						                points.end(),
						                screenPts.begin(),
						                std::ptr_fun( Point3DTo2D ) );
						
						V::Rect2D< double > bounding_rect = poly2d.BoundingRect();
						
						bounding_rect.left  = bounding_rect.left  * width / 2.0 + width / 2.0;
						bounding_rect.right = bounding_rect.right * width / 2.0 + width / 2.0;
						
						bounding_rect.top    = bounding_rect.top    * -width / 2.0 + height / 2.0;
						bounding_rect.bottom = bounding_rect.bottom * -width / 2.0 + height / 2.0;
						
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
							current_pixel_2d[ Y ] = (iY + 0.5 - height / 2.0) / (-width / 2.0);
							
							::Ptr rowAddr = baseAddr + ( iY - pixBounds.top ) * rowBytes;
							
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
								
								double dist = Magnitude( sectPt - pt0 );
								
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
								double cosAlpha = ray * faceNormal / Magnitude( ray );
								double incidenceRatio = cosAlpha;
								
								ColorMatrix tweaked = TweakColor( poly.Color(),
								                                  dist,
								                                  incidenceRatio,
								                                  selected );
								
								N::RGBColor rgb = NN::Convert< N::RGBColor >( tweaked );
								
								::Ptr pixelAddr = rowAddr + (iX - pixBounds.left) * 32/8;
								
								GrafX::Pixel32& pixel = *(GrafX::Pixel32*) pixelAddr;
								
								pixel = rgb;
								
								if ( !gBlitting )
								{
									N::SetCPixel( iX, iY, rgb );
								}
							}
						}
						
						if ( TARGET_API_MAC_CARBON && !gBlitting )
						{
							::QDFlushPortBuffer( ::GetQDGlobalsThePort(), N::RectRgn( itsBounds ) );
						}
					}
				}
			}
		}
		
		if ( gBlitting )
		{
			savedGWorld.Restore();
			
			N::CGrafPtr thePort = N::GetQDGlobalsThePort();
			
			PixMapHandle thePortPix = N::GetGWorldPixMap( thePort );
			NN::Saved< N::PixelsState_Value > savedPixelsState( thePortPix );
			N::LockPixels( thePortPix );
			
			N::CopyBits( N::GetPortBitMapForCopyBits( itsGWorld ),
			             N::GetPortBitMapForCopyBits( thePort ),
			             itsBounds,
			             itsBounds,
			             N::srcCopy );
			
			::QDFlushPortBuffer( ::GetQDGlobalsThePort(), N::RectRgn( itsBounds ) );
		}
	}
	
	void PortView::MouseDown( const EventRecord& event )
	{
		Point macPt = N::GlobalToLocal( event.where );
		
		itsPort.MakeFrame( itsFrame );
		
		MeshModel* model = HitTest( macPt.h, macPt.v );
		
		if ( model != NULL )
		{
			model->Select();
		}
		
		Redraw();
		
		if ( event.modifiers & shiftKey )
		for ( int y = itsBounds.top;  y < itsBounds.bottom;  ++y )
		{
			for ( int x = itsBounds.left;  x < itsBounds.right;  ++x )
			{
				DrawPixel( x, y );
			}
			
			if ( TARGET_API_MAC_CARBON )
			{
				::QDFlushPortBuffer( ::GetQDGlobalsThePort(), N::RectRgn( itsBounds ) );
			}
		}
	}
	
	bool PortView::KeyDown( const EventRecord& event )
	{
		char c = event.message & charCodeMask;
		
		if ( 0 )
		{
			//short code = (inEvent.message & keyCodeMask) >> 8;
			//short code = inEvent.KeyCode();
		}
		else
		{
			return KeyDown( c );
		}
	}
	
	bool PortView::KeyDown(char c)
	{
		//mSuperView.Focus();
		
		if ( c == '~' )
		{
			DrawBetter();
			return true;
		}
		
		short cmd;
		bool shooter = true;
		if ( shooter )
		{
			switch ( c )
			{
				case 0x1e: // up arrow
					c = '0';
				break;
				case 0x1f: // down arrow
					c = '.';
				break;
			}
		}
		else
		{
			switch ( c )
			{
				case 0x1e: // up arrow
					c = '8';
					break;
				case 0x1f: // down arrow
					c = '5';
					break;
			}
		}
		switch ( c )
		{
			case '7':
				cmd = cmdMoveLeft;
				break;
			case '9':
				cmd = cmdMoveRight;
				break;
			case '4':
			case 0x1c: // left arrow
				cmd = cmdYawLeft;
				break;
			case '6':
			case 0x1d: // right arrow
				cmd = cmdYawRight;
				break;
			case '1':
				cmd = cmdRollLeft;
				break;
			case '3':
				cmd = cmdRollRight;
				break;
			case '8':
				cmd = cmdPitchDown;
				break;
			case '5':
				cmd = cmdPitchUp;
				break;
			case '0':
				cmd = cmdMoveForward;
				break;
			case '.':
				cmd = cmdMoveBackward;
				break;
			case '-':
				cmd = cmdMoveUp;
				break;
			case '+':
				cmd = cmdMoveDown;
				break;
			case '*':
				cmd = cmdExpand;
				break;
			case '/':
				cmd = cmdContract;
				break;
			case '2':
				cmd = cmdLevelPitch;
				break;
			case '=':
				cmd = cmdLevelRoll;
				break;
			case '_':
			case 0x03:
				cmd = cmdGroundHeight;
				break;
			case ' ':
				cmd = cmdPlayPause;
				break;
			case 'p':
				//cmd = cmdProjectionMode;
				break;
			case '[':
				cmd = cmdPrevCamera;
				break;
			case ']':
				cmd = cmdNextCamera;
				break;
			default:
				return false;
				break;
		}
		
		if ( itsSelectedContext == 0 )
		{
			itsSelectedContext = itsScene.Cameras().front().ContextIndex();
		}
		
		itsPort.SendCameraCommand( itsSelectedContext, cmd );
		
		Draw();
		
		return true;
	}
	
	static bool operator==( const Rect& a, const Rect& b )
	{
		return std::equal( (const SInt16*)&a, (const SInt16*)&a + 4, (const SInt16*)&b );
	}
	
	void PortView::Resize( const Rect& newBounds )
	{
		using namespace Nucleus::Operators;
		
		//if ( newBounds == itsBounds )  return;
		
		SetBounds( NX::NormalRect( newBounds ) );
		
		itsGWorld = N::NewGWorld( 32, itsBounds );
		
		N::LockPixels( N::GetGWorldPixMap( itsGWorld ) );
	}
	
}

