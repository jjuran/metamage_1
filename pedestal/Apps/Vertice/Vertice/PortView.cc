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
	
	template < class ZSpectrum, class ColorSpectrum, class Device >
	void DrawDeepScanLine( int                   y,
	                       int                   farLeft,
	                       double                left,
	                       double                right,
	                       const ZSpectrum&      inverseZ,
	                       const ColorSpectrum&  colors,
	                       ::Ptr                 rowAddr,
	                       Device&               deepPixelDevice )
	{
		for ( int x = int( std::ceil( left ) );  x < right;  ++x )
		{
			double tX = (x - left) / (right - left);
			
			double z = 1 / inverseZ[ tX ];
			
			if ( deepPixelDevice.SetIfNearer( x, y, -z ) )
			{
				ColorMatrix color = z * colors[ tX ];
				
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
		double top    = topLeft   [ Y ];
		double bottom = bottomLeft[ Y ];
		
		double vdist = bottom - top;
		
		::CGrafPtr port = N::GetQDGlobalsThePort();
		//::CGrafPtr port = itsGWorld.Get();
		// Verify that it's a color port
		if ( !::IsPortColor( port ) )  return;
		
		PixMapHandle pix = ::GetPortPixMap( port );
		
		if ( !CheckPixMap( pix ) ) return;
		
		const Rect& portRect = N::GetPortBounds( port );
		
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
			
			double left  = MakeLinearSpectrum( topLeft [ X ], bottomLeft [ X ] )[ tY ];
			double right = MakeLinearSpectrum( topRight[ X ], bottomRight[ X ] )[ tY ];
			
			double leftZ  = 1 / MakeLinearSpectrum( 1 / topLeft [ Z ], 1 / bottomLeft [ Z ] )[ tY ];
			double rightZ = 1 / MakeLinearSpectrum( 1 / topRight[ Z ], 1 / bottomRight[ Z ] )[ tY ];
			
			ColorMatrix leftColor = leftZ * MakeLinearSpectrum( topLeft.itsColor    / topLeft   [ Z ],
			                                                    bottomLeft.itsColor / bottomLeft[ Z ] )[ tY ];
			
			ColorMatrix rightColor = rightZ * MakeLinearSpectrum( topRight.itsColor    / topRight   [ Z ],
			                                                      bottomRight.itsColor / bottomRight[ Z ] )[ tY ];
			
			DrawDeepScanLine( y,
			                  bounds.left,
			                  left,
			                  right,
			                  MakeLinearSpectrum(         1 / leftZ,          1 / rightZ ),
			                  MakeLinearSpectrum( leftColor / leftZ, rightColor / rightZ ),
			                  rowAddr,
			                  deepPixelDevice );
		}
	}
	
	template < class Vertex, class Device >
	void DrawDeepTriangle( const Vertex& A,
	                       const Vertex& B,
	                       const Vertex& C, Device& device )
	{
		// Assume that the vertices are in device coordinates, and top to bottom.
		double top    = A[ Y ];
		double middle = B[ Y ];
		double bottom = C[ Y ];
		
		double t = (middle - top) / (bottom - top);
		
		double choppedAC = MakeLinearSpectrum( A[X], C[X] )[t];
		
		double midLeft  = std::min( B[X], choppedAC );
		double midRight = std::max( B[X], choppedAC );
		
		double z = 1 / MakeLinearSpectrum( 1 / A[Z], 1 / C[Z] )[t];
		
		double midLeftZ  = ( B[X] <= choppedAC ) ? B[Z] : z;
		double midRightZ = ( B[X] >  choppedAC ) ? B[Z] : z;
		
		ColorMatrix choppedACColor = z * MakeLinearSpectrum( A.itsColor / A[Z], C.itsColor / C[Z] )[t];
		
		ColorMatrix midLeftColor  = ( B[X] <= choppedAC ) ? B.itsColor : choppedACColor;
		ColorMatrix midRightColor = ( B[X] >  choppedAC ) ? B.itsColor : choppedACColor;
		
		
		if ( top < middle )
		{
			DrawDeepTrapezoid( A,
			                   A,
			                   Vertex( V::Point3D::Make( midLeft,  B[Y], midLeftZ  ), midLeftColor  ),
			                   Vertex( V::Point3D::Make( midRight, B[Y], midRightZ ), midRightColor ),
			                   device );
		}
		
		if ( middle < bottom )
		{
			DrawDeepTrapezoid( Vertex( V::Point3D::Make( midLeft,  B[Y], midLeftZ  ), midLeftColor  ),
			                   Vertex( V::Point3D::Make( midRight, B[Y], midRightZ ), midRightColor ),
			                   C,
			                   C,
			                   device );
		}
	}
	
	
	static V::XMatrix MakePortToClipTransform( double  near,
	                                           double  far,
	                                           double  focalLength,
	                                           double  aspectRatio )
	{
		V::XMatrix xform = V::ZeroMatrix();
		
		double n = near;
		double f = far;
		double e = focalLength;
		double a = aspectRatio;
		
		double l = -n / e;
		double r = -l;
		double b = a * l;
		double t = -b;
		
		xform.Cell( 0, 0 ) =    2*n   / (r - l);
		// 0
		xform.Cell( 0, 2 ) =  (r + l) / (r - l);
		// 0
		
		// 0
		xform.Cell( 1, 1 ) =    2*n   / (t - b);
		xform.Cell( 1, 2 ) =  (t + b) / (t - b);
		// 0
		
		// 0
		// 0
		
		#if 0
		
		xform.Cell( 2, 2 ) = -(f + n) / (f - n);
		xform.Cell( 2, 3 ) =  -2*n*f  / (f - n);
		
		#else
		
		xform.Cell( 2, 2 ) =  (f + n) / (f - n);
		xform.Cell( 2, 3 ) =   2*n*f  / (f - n);
		
		#endif
		
		// 0
		// 0
		xform.Cell( 3, 2 ) =         -1;
		// 0
		
		return xform;
	}
	
	static void SetPortToClipTransform()
	{
		double n = -0.01;  // Think 1cm
		double f = -100;  // Think 100m
		double e = sFocalLength;
		double a = sAspectRatio;
		
		gPort2Clip = MakePortToClipTransform( n, f, e, a );
	}
	
	PortView::PortView( const Rect& bounds, Initializer ) : itsPort           ( itsScene                   ),
	                                                        itsSelectedContext(                            ),
	                                                        itsGWorld         ( N::NewGWorld( 32, bounds ) )
	{
		SetBounds( bounds );
		N::LockPixels( N::GetGWorldPixMap( itsGWorld ) );
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
		
		V::Translation translate( -width / 2, -height / 2, 0 );
		
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
	
	static V::Point3D::Type PerspectiveDivision( const V::Point3D::Type& pt )
	{
		return V::Point3D::Make( -pt[ X ] / pt[ Z ],
		                         -pt[ Y ] / pt[ Z ],
		                          pt[ Z ] );
	}
	
	static V::Point3D::Type HomogeneousPerspectiveDivision( const V::Point3D::Type& pt )
	{
		return V::Point3D::Make( pt[ X ] / pt[ W ],
		                         pt[ Y ] / pt[ W ],
		                         pt[ Z ] / pt[ W ] );
	}
	
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
	
	static std::vector< V::Point3D::Type > Port2ScreenPolygon( const std::vector< V::Point3D::Type >&  poly,
	                                                           const V::XMatrix&                       port2Screen )
	{
		std::vector< V::Point3D::Type > points = poly;
		
		if ( fishEye )
		{
		//	transform(points.begin(), points.end(), points.begin(), FishEye);
		}
		
		std::transform( points.begin(),
		                points.end(),
		                points.begin(),
		                std::ptr_fun( PerspectiveDivision ) );
		
		std::transform( points.begin(),
		                points.end(),
		                points.begin(),
		                V::Transformer< V::Point3D::Type >( port2Screen ) );
		
		return points;
	}
	
	
	template < class Container, class Filter >
	void Trim( Container& cont, const Filter& filter )
	{
		cont.resize( std::remove_if( cont.begin(), cont.end(), filter ) - cont.begin() );
	}
	
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
	                               double       incidenceRatio )
	{
		bool considerColor     = true;
		bool considerAmbience  = true;
		bool considerProximity = true;
		bool considerIncidence = true;
		
		ColorMatrix white = V::MakeGray( 1.0 );
		
		double proximity = ProximityQuotient( distance / 2 );
		
		ColorMatrix ambientLight = V::MakeRGB( 0.8, 0.8, 1.0 );
		ColorMatrix cameraLight  = V::MakeRGB( 1.0, 1.0, 0.6 );
		
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
		                         + ( 0.9 * cameraLight * proximity * incidenceRatio );
		
		//return color * totalLight;
		return ModulateColor( color, totalLight );
	}
	
	static double operator*( const V::Vector3D::Type& a, const V::Vector3D::Type& b )
	{
		return DotProduct( a, b );
	}
	
	void PortView::Draw()
	{
		unsigned width  = NX::RectWidth ( itsBounds );
		unsigned height = NX::RectHeight( itsBounds );
		
		NN::Saved< N::GWorld_Value > savedGWorld;
		N::SetGWorld( itsGWorld );
		
		N::RGBBackColor( NN::Make< RGBColor >( 0 ) );
		
		// Plot the images in screen coordinates
		
		const V::XMatrix& port2screen = PortToScreenTransform( width, height );
		
		//fishEye = itsPort.mCamera.fishEyeMode;
		
		itsPort.MakeFrame( itsFrame );
		
		N::EraseRect( itsBounds );
		
		gDeepPixelDevice.Resize( width, height );
		
		V::Point3D::Type pt0 = V::Point3D::Make( 0, 0, 0 );
		
		const std::vector< MeshModel >& models = itsFrame.Models();
		
		typedef std::vector< MeshModel >::const_iterator ModelIter;
		
		// For each mesh model...
		for ( ModelIter it = models.begin(), end = models.end();  it != end;  ++it )
		{
			const MeshModel& model = *it;
			
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
				
				V::Plane3D::Type plane = V::PlaneVector( points );
				
				// Skip backfaces
				if ( !ClipPointAgainstPlane( pt0, plane ) )
				{
					continue;
				}
				
				V::Vector3D::Type faceNormal = V::UnitLength( V::FaceNormal( points ) );
				
				points = Port2ScreenPolygon( points, port2screen );
				
				std::vector< DeepVertex > vertices( points.size() );
				
				std::transform( points.begin(),
				                points.end(),
				                vertices.begin(),
				                std::ptr_fun( Point3DToDeepVertex ) );
				
				// For each vertex in the polygon
				for ( unsigned int i = 0;  i < vertices.size();  ++i )
				{
					DeepVertex& pt = vertices[ i ];
					V::Point3D::Type pt1 = V::Point3D::Make( pt[X], pt[Y], -1 );
					
					pt1 = Transformation( pt1, itsScreen2Port );
					
					if ( fishEye )
					{
					//	pt1 = UnFishEye(pt1);
					}
					
					// The ray is inverted to face the same way as the face normal.
					V::Vector3D::Type ray = UnitLength( pt0 - pt1 );
					
					if ( !LineIntersectsPlane( ray, plane ) )
					{
						// Viewing ray doesn't intersect plane.  Probably parallel.
						continue;
					}
					
					V::Point3D::Type sectPt = LinePlaneIntersection( ray, pt0, plane );
					
					double dist = Magnitude( V::Vector3D::Type( sectPt - pt0 ) );
					// P . Q = mag(P)*mag(Q)*cos(a)
					// cos(a) = P.Q / mag(P) / mag(Q)
					// The ray and the normal are already unit length,
					// so their magnitudes are 1.
					double cosAlpha = ray * faceNormal;
					double incidenceRatio = cosAlpha;
					
					pt.itsColor = TweakColor( poly.Color(), dist, incidenceRatio );
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
	
	MeshPoly* PortView::HitTest( double x, double y )
	{
		V::Point3D::Type pt0 = V::Point3D::Make( 0, 0, 0 );
		V::Point3D::Type pt1 = V::Point3D::Make( x, y, -1 );
		
		if ( fishEye )
		{
		//	pt1 = UnFishEye(pt1);
		}
		
		// The ray is inverted to face the same way as the face normal.
		V::Vector3D::Type ray = UnitLength( pt0 - pt1 );
		
		//return itsFrame.HitTest( pt0, ray );
		
		return NULL;
	}
	
	ColorMatrix PortView::TracePixel( int x, int y )
	{
		V::Point3D::Type pt1 = V::Point3D::Make( x + 0.5, y + 0.5, 1 );
		
		pt1 = Transformation( pt1, itsScreen2Port );
		
		//std::pair<string, int> index = HitTest(pt1[X], pt1[Y]);
		MeshPoly* poly = HitTest( pt1[ X ], pt1[ Y ] );
		
		return poly ? poly->Color() : V::Black();
	}
	
	void PortView::DrawPixel( int x, int y )
	{
		N::SetCPixel( x, y, NN::Convert< N::RGBColor >( TracePixel( x, y ) ) );
	}
	
	struct TickCounter
	{
		typedef UInt32 Time;
		
		Time operator()() const  { return ::TickCount(); }
	};
	
	struct AnyThreadYielder
	{
		void operator()() const  { N::YieldToAnyThread(); }
	};
	
	void PortView::DrawBetter() const
	{
		typedef NX::Escapement< NX::Timer< TickCounter >,
		                        AnyThreadYielder > Escapement;
		
		Escapement escapement( 10 );
		
		NN::Saved< N::GWorld_Value > savedGWorld;
		
		if ( TARGET_API_MAC_CARBON )
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
		
		unsigned width  = NX::RectWidth ( portRect );
		unsigned height = NX::RectHeight( portRect );
		
		V::XMatrix port2Screen = PortToScreenTransform( width, height );
		
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
						
						// Lookup the vertices of this polygon
						// in port coordinates
						std::transform( offsets.begin(),
						                offsets.end(),
						                points.begin(),
						                mesh );
						
						V::Point3D::Type pt0 = V::Point3D::Make( 0, 0, 0 );
						V::Plane3D::Type plane = V::PlaneVector( points );
						
						if ( !ClipPointAgainstPlane( pt0, plane ) )
						{
							// cull backface
							continue;
						}
						
						V::Vector3D::Type faceNormal = V::UnitLength( V::FaceNormal( points ) );
						
						/*
						V::Point3D::Type polygonOrigin = points[ 0 ];
						V::Vector3D::Type polygonLeadingEdge = points[ 1 ] - points[ 0 ];
						V::Vector3D::Type polygonSecondEdge  = points[ 2 ] - points[ 1 ];
						*/
						
						#if 0
						
						// Port to clip coordinates
						std::transform( points.begin(),
						                points.end(),
						                points.begin(),
						                V::Transformer< V::Point3D::Type >( gPort2Clip ) );
						
						// Perspective division
						std::transform( points.begin(),
						                points.end(),
						                points.begin(),
						                std::ptr_fun( HomogeneousPerspectiveDivision ) );
						
						#else
						
						// Perspective division
						std::transform( points.begin(),
						                points.end(),
						                points.begin(),
						                std::ptr_fun( PerspectiveDivision ) );
						
						#endif
						
						// Port to screen coordinates
						std::transform( points.begin(),
						                points.end(),
						                points.begin(),
						                V::Transformer< V::Point3D::Type >( port2Screen ) );
						
						V::Polygon2D poly2d;
						std::vector< V::Point2D::Type >& screenPts( poly2d.Points() );
						screenPts.resize( points.size() );
						
						std::transform( points.begin(),
						                points.end(),
						                screenPts.begin(),
						                std::ptr_fun( Point3DTo2D ) );
						
						V::Rect2D< int > bounds;
						bounds = poly2d.BoundingRect();
						
						// Extend the rect to account for truncation error
						bounds.right += 1;
						bounds.top += 1;
						
						// Intersect the polygon bounds with the depth buffer bounds
						V::Rect2D< int > rect = depthRect * bounds;
						
						// For each row
						for ( unsigned iY = rect.bottom;  iY < rect.top;  ++iY )
						{
							//escapement();
							
							::Ptr rowAddr = baseAddr + ( iY - pixBounds.top ) * rowBytes;
							
							// For each pixel in the row
							for ( unsigned iX = rect.left;  iX < rect.right;  ++iX )
							{
								V::Point3D::Type pt1 = V::Point3D::Make( iX + 0.5, iY + 0.5, -1 );
								pt1 = Transformation( pt1, itsScreen2Port );
								
								if ( fishEye )
								{
								//	pt1 = UnFishEye(pt1);
								}
								
								// The ray is inverted to face the same way as the face normal.
								V::Vector3D::Type ray = UnitLength( pt0 - pt1 );
								
								if ( !LineIntersectsPlane( ray, plane ) )
								{
									// Viewing ray parallel to polygon plane
									continue;
								}
								
								V::Point3D::Type sectPt = LinePlaneIntersection( ray, pt0, plane );
								double dist = Magnitude( sectPt - pt0 );
								
								if (    dist > 0
								     && gDeepPixelDevice.Nearer( iX % width, iY % height, dist ) 
								     && poly2d.ContainsPoint( V::Point2D::Make( iX + 0.5, iY + 0.5 ) ) )
								{
									// set the pixel, below
								}
								else
								{
									continue;
								}
								
								gDeepPixelDevice.SetIfNearer( iX % width, iY % height, dist );
								
								// P . Q = mag(P)*mag(Q)*cos(a)
								// cos(a) = P.Q / mag(P) / mag(Q)
								// The ray and the normal are already unit length,
								// so their magnitudes are 1.
								/*
								double cosTheta = DotProduct(ray, faceNormal) 
									/ Magnitude(ray) / Magnitude(faceNormal);
								*/
								double cosAlpha = ray * faceNormal;
								double incidenceRatio = cosAlpha;
								
								ColorMatrix tweaked = TweakColor( poly.Color(),
								                                  dist,
								                                  incidenceRatio );
								
								N::RGBColor rgb = NN::Convert< N::RGBColor >( tweaked );
								
								::Ptr pixelAddr = rowAddr + (iX - pixBounds.left) * 32/8;
								
								GrafX::Pixel32& pixel = *(GrafX::Pixel32*) pixelAddr;
								
								pixel = rgb;
								
								if ( !TARGET_API_MAC_CARBON )
								{
									N::SetCPixel( iX, iY, rgb );
								}
							}
						}
					}
				}
			}
		}
		
		if ( TARGET_API_MAC_CARBON )
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
		
		V::Point3D::Type pt1 = V::Point3D::Make( macPt.h, macPt.v, 1 );
		
		pt1 = Transformation( pt1, itsScreen2Port );
		
		//std::pair<string, int> index = HitTest(pt1[X], pt1[Y]);
		MeshPoly* poly = HitTest( pt1[X], pt1[Y] );
		
		if ( poly != NULL )
		{
			N::SetCPixel( macPt.h, macPt.v, NN::Convert< N::RGBColor >( poly->Color() ) );
			
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
		else if ( c == '!' )
		{
			for ( int i = 0;  i < NX::RectWidth( Bounds() );  ++i )
			{
				for ( int j = 0;  j < NX::RectHeight( Bounds() );  ++j )
				{
					DrawPixel( i, j );
					
					if ( ::Button() )
					{
						// Bail out
						return true;
					}
				}
			}
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

