/*
	worldview/Objects.cc
	--------------------
*/

#include "worldview/Objects.hh"

// Standard C/C++
#include <cstdio>

// Vectoria
#include "Vectoria/Coordinates.hh"
#include "Vectoria/LinearAlgebra3D.hh"
#include "Vectoria/PlaneVector.hh"
#include "Vectoria/Transform.hh"
#include "Vectoria/TriColor.hh"


namespace worldview
{
	
	using Vectoria::X;
	using Vectoria::Y;
	using Vectoria::Z;
	using Vectoria::W;
	
	typedef unsigned char Boolean;
	
	
	void ImageTile::Swap( ImageTile& other )
	{
		using std::swap;
		
		swap( itsWidth,  other.itsWidth  );
		swap( itsValues, other.itsValues );
	}
	
	
	void Moveable::ContextTranslate( double dx, double dy, double dz )
	{
		V::Translation trans( dx, dy, dz );
		
		itsTransform = Compose( trans.Make(), itsTransform           );
		itsInverse   = Compose( itsInverse,   trans.Inverse().Make() );
	}
	
	void Moveable::ContextTranslate( const V::Vector3D::Type& vec )
	{
		ContextTranslate( vec[ X ], vec[ Y ], vec[ Z ] );
	}
	
	void Moveable::ContextYaw( double dtheta )
	{
		// Thing's orientation is expressed as yaw-pitch-roll
		// from its context's point of view.
		// Delta theta is from the thing's point of view.
		// If you think of yaw-pitch-roll as a series of transforms
		// that convert from context coordinates to thing coordinates,
		// then we're adding another yaw to the end.
		// So we just multiply onto the existing orientation transform.
		
		V::Yaw yaw = V::Radians( dtheta );
		
		itsTransform = Compose( yaw,        itsTransform );
		itsInverse   = Compose( itsInverse, -yaw         );
	}
	
	void Moveable::ContextPitch( double dphi )
	{
		V::Pitch pitch = V::Radians( dphi );
		
		itsTransform = Compose( pitch,      itsTransform  );
		itsInverse   = Compose( itsInverse, -pitch        );
	}
	
	void Moveable::ContextRoll( double dpsi )
	{
		V::Roll roll = V::Radians( dpsi );
		
		itsTransform = Compose( roll,       itsTransform );
		itsInverse   = Compose( itsInverse, -roll        );
	}
	
	void Moveable::ContextScale( double factor )
	{
		V::Scale scale( factor );
		
		itsTransform = Compose( scale.Make(), itsTransform           );
		itsInverse   = Compose( itsInverse,   scale.Inverse().Make() );
	}
	
	void Moveable::LocalTranslate( double dx, double dy, double dz )
	{
		V::Translation trans( dx, dy, dz );
		
		itsTransform = Compose( itsTransform,           trans.Make() );
		itsInverse   = Compose( trans.Inverse().Make(), itsInverse   );
	}
	
	void Moveable::LocalTranslate( const V::Vector3D::Type& vec )
	{
		LocalTranslate( vec[ X ], vec[ Y ], vec[ Z ] );
	}
	
	void Moveable::LocalYaw( double dtheta )
	{
		V::Yaw yaw = V::Radians( dtheta );
		
		itsTransform = Compose( itsTransform, yaw        );
		itsInverse   = Compose( -yaw,         itsInverse );
	}
	
	void Moveable::LocalPitch( double dphi )
	{
		V::Pitch pitch = V::Radians( dphi );
		
		itsTransform = Compose( itsTransform, pitch      );
		itsInverse   = Compose( -pitch,       itsInverse );
	}
	
	void Moveable::LocalRoll( double dpsi )
	{
		V::Roll roll = V::Radians( dpsi );
		
		itsTransform = Compose( itsTransform, roll       );
		itsInverse   = Compose( -roll,        itsInverse );
	}
	
	void Moveable::LocalScale( double factor )
	{
		V::Scale scale( factor );
		
		itsTransform = Compose( itsTransform,           scale.Make() );
		itsInverse   = Compose( scale.Inverse().Make(), itsInverse   );
	}
	
	
	double PointMesh::MinimumDepth() const
	{
		double result = -INFINITY;
		
		typedef std::vector< V::Point3D::Type >::const_iterator PointIter;
		
		for ( PointIter it = itsPoints.begin();  it != itsPoints.end();  ++it )
		{
			const V::Point3D::Type& point = *it;
			
			double z = point[ Z ] / point[ W ];
			
			// Z is negative, so the least deep is the greatest.
			
			if ( z > result )
			{
				result = z;
			}
		}
		
		return -result;
	}
	
	
	MeshPolygon::MeshPolygon( const std::vector< Offset >&  offsets,
			                  const ColorMatrix&            color ) : itsVertices( offsets ),
			                                                          itsColor   ( color   )
	{
		std::copy( offsets.begin(), offsets.begin() + 3, itsSavedOffsets );
	}
	
	MeshPolygon::MeshPolygon( const std::vector< Offset >&  offsets,
	                          const ImageTile&              tile,
	                          const V::Point2D::Type&       ptA,
	                          const V::Point2D::Type&       ptB ) : itsVertices ( offsets ),
	                                                                itsImageTile( tile    )
	{
		itsSavedOffsets[ 0 ] = offsets[ 0 ];
		itsSavedOffsets[ 1 ] = offsets[ 1 ];
		itsSavedOffsets[ 2 ] = offsets[ 2 ];
		
		V::Point2D::Type diff = ptB - ptA;
		
		diff = V::Point2D::Make( -diff[ Y ], diff[ X ] );
		
		itsMapPoints[ 0 ] = ptA;
		itsMapPoints[ 1 ] = ptB;
		itsMapPoints[ 2 ] = ptA + diff;
	}
	
	void MeshPolygon::Swap( MeshPolygon& other )
	{
		using std::swap;
		
		swap( itsVertices,        other.itsVertices        );
		swap( itsMapPoints   [0], other.itsMapPoints   [0] );
		swap( itsMapPoints   [1], other.itsMapPoints   [1] );
		swap( itsMapPoints   [2], other.itsMapPoints   [2] );
		swap( itsSavedOffsets[0], other.itsSavedOffsets[0] );
		swap( itsSavedOffsets[1], other.itsSavedOffsets[1] );
		swap( itsSavedOffsets[2], other.itsSavedOffsets[2] );
		swap( itsColor,           other.itsColor           );
		swap( itsImageTile,       other.itsImageTile       );
	}
	
	void MeshPolygon::SwapVertexOffsets( std::vector< Offset >& vertexOffsets )
	{
		using std::swap;
		
		swap( itsVertices, vertexOffsets );
	}
	
	
	// This is not suitable for general use, but works for the case below
	static inline bool close_enough( double a, double b )
	{
		return std::abs( (a - b) / b ) < 0.001;
	}
	
	static double MeasureAngularDeviation( const V::Vector3D::Type& normal,
	                                       const V::Vector3D::Type& a,
	                                       const V::Vector3D::Type& b )
	{
		V::Vector3D::Type cross_a = V::CrossProduct( normal, a );
		
		double cosine = V::DotProduct( b, a ) / V::Magnitude( a ) / V::Magnitude( b );
		
		double angle = std::acos( cosine );
		
		double tilt = V::DotProduct( b, cross_a );
		
		if ( tilt < 0.0 )
		{
			angle = -angle;
		}
		
		return angle;
	}
	
	class Geometry_Error {};
	
	template < class Inserter >
	static void Convexify( const std::vector< unsigned >&  offsets,
	                       const PointMesh&                mesh,
	                       Inserter                        output )
	{
		std::size_t size = offsets.size();
		
		std::vector< V::Point3D::Type > points;
		
		points.reserve( size );
		
		for ( std::size_t i = 0;  i < size;  ++i )
		{
			points.push_back( mesh( offsets[ i ] ) );
		}
		
		V::Vector3D::Type A = points[1] - points[0];
		V::Vector3D::Type B = points[2] - points[1];
		
		V::Vector3D::Type normal = V::CrossProduct( A, B );
		
		points.push_back( points[0] );
		points.push_back( points[1] );
		
		double total_deviation = 0.0;
		
		unsigned concave_vertices = 0;
		
		std::vector< Boolean > concave( size );
		
		for ( unsigned i = 0;  i < points.size() - 2;  ++i )
		{
			unsigned prev = (i + size - 1) % size;
			unsigned next = (i        + 1) % size;
			
			double deviation = MeasureAngularDeviation( normal,
			                                            points[ i    ] - points[ prev ],
			                                            points[ next ] - points[ i    ] );
			
			if ( deviation < 0.0 )
			{
				++concave_vertices;
				
				concave[i] = true;
			}
			
			total_deviation += deviation;
		}
		
		if ( !close_enough( total_deviation, 2 * 3.14159 ) )
		{
			//throw Geometry_Error();
			
		#ifndef __RELIX__
			
			std::fprintf( stderr, "Angular deviation: %f\n", total_deviation );
			
		#endif
			
			return;
		}
		
		if ( concave_vertices == 0 )
		{
			output++ = offsets;
			
			return;
		}
		
		points.resize( points.size() - 2 );
		
		std::vector< unsigned > remaining_offsets = offsets;
		
		std::vector< unsigned > triangle( 3 );
		
		std::size_t i = 0;
		
		while ( concave_vertices )
		{
			unsigned next = (i + 1) % size;
			
			if ( concave[ i ]  &&  !concave[ next ] )
			{
				unsigned next2 = (i + 2) % size;
				
				triangle[ 0 ] = remaining_offsets[ i     ];
				triangle[ 1 ] = remaining_offsets[ next  ];
				triangle[ 2 ] = remaining_offsets[ next2 ];
				
				output++ = triangle;
				
				remaining_offsets.erase( remaining_offsets.begin() + next );
				points           .erase( points           .begin() + next );
				concave          .erase( concave          .begin() + next );
				
				--size;
				
				if ( next == 0 )
				{
					--i;
				}
				else
				{
					next = (i + 1) % size;
				}
				
				unsigned prev = (i + size - 1) % size;
				
				double deviation = MeasureAngularDeviation( normal,
		                                                    points[ i    ] - points[ prev ],
		                                                    points[ next ] - points[ i    ] );
				
				concave[ i ] = deviation < 0.0;
				
				if ( !concave[ i ] )
				{
					--concave_vertices;
				}
			}
			else
			{
				i = next;
			}
		}
		
		output++ = remaining_offsets;
	}
	
	void MeshModel::AddMeshPolygon( const std::vector< unsigned >&  offsets,
	                                const ColorMatrix&              color )
	{
		std::vector< std::vector< unsigned > > offset_groups;
		
		Convexify( offsets, itsMesh, std::back_inserter( offset_groups ) );
		
		typedef std::vector< std::vector< unsigned > >::const_iterator Iter;
		
		for ( Iter it = offset_groups.begin();  it != offset_groups.end();  ++it )
		{
			itsPolygons.push_back( MeshPolygon( *it, color ) );
		}
	}
	
	void MeshModel::AddMeshPolygon( const std::vector< unsigned >&  offsets,
	                                const ImageTile&                tile,
	                                const V::Point2D::Type&         ptA,
	                                const V::Point2D::Type&         ptB )
	{
		itsPolygons.push_back( MeshPolygon( offsets, tile, ptA, ptB ) );
	}
	
	void MeshModel::CullBackfaces( const V::Point3D::Type& eye )
	{
		V::Point3D::Type points[3];
		
		typedef std::vector< MeshPolygon >::iterator PolygonIter;
		
		typedef MeshPolygon::Offset Offset;
		
		for ( PolygonIter it = itsPolygons.begin();  it != itsPolygons.end();  ++it )
		{
			MeshPolygon& polygon = *it;
			
			const std::vector< Offset >& offsets = polygon.Vertices();
			
			bool visible = offsets.size() >= 3;
			
			if ( visible )
			{
				for ( std::size_t i = 0;  i < 3;  ++i )
				{
					points[ i ] = itsMesh( offsets[ i ] );
				}
				
				V::Plane3D::Type plane = V::PlaneVector( points );
				
				visible = DotProduct( plane, eye ) > 0.0;
			}
			
			if ( !visible )
			{
				it->Swap( itsPolygons.back() );
				
				itsPolygons.pop_back();
				
				--it;
			}
		}
	}
	
	void MeshModel::ClipAgainstPlane( const V::Plane3D::Type& plane )
	{
		if ( itsPolygons.empty() )
		{
			return;
		}
		
		const std::vector< V::Point3D::Type >& points = itsMesh.Points();
		
		std::vector< Boolean > positive_mask( points.size() );
		std::vector< Boolean > zero_mask    ( points.size() );
		std::vector< Boolean > negative_mask( points.size() );
		
		typedef std::vector< V::Point3D::Type >::const_iterator PointIter;
		
		unsigned i = 0;
		
		for ( PointIter it = points.begin();  it != points.end();  ++it, ++i )
		{
			double product = DotProduct( plane, *it );
			
			if ( product > 0 )
			{
				positive_mask[ i ] = true;
			}
			else if ( product < 0 )
			{
				negative_mask[ i ] = true;
			}
			else
			{
				zero_mask[ i ] = true;
			}
		}
		
		typedef MeshPolygon::Offset Offset;
		
		typedef std::vector< MeshPolygon >::iterator PolygonIter;
		
		typedef std::vector< Offset >::const_iterator OffsetIter;
		
		for ( PolygonIter it = itsPolygons.begin();  it != itsPolygons.end();  ++it )
		{
			MeshPolygon& polygon = *it;
			
			const std::vector< Offset >& offsets = polygon.Vertices();
			
			if ( offsets.empty() )
			{
				continue;
			}
			
			std::vector< Offset > new_offsets;
			
			bool have_previous = false;
			
			bool previous_is_positive;
			
			for ( OffsetIter it2 = offsets.begin();  it2 != offsets.end();  ++it2 )
			{
				Offset offset = *it2;
				
				bool is_positive = positive_mask[ offset ];
				
				bool crossed = have_previous  &&  ( previous_is_positive != is_positive );
				
				if ( is_positive )
				{
					if ( crossed )
					{
						const V::Point3D::Type& this_point = points[ it2[  0 ] ];
						const V::Point3D::Type& prev_point = points[ it2[ -1 ] ];
						
						V::Point3D::Type clipped_point = LinePlaneIntersection( this_point - prev_point,
						                                                        this_point,
						                                                        plane );
						
						new_offsets.push_back( itsMesh.AddPoint( clipped_point ) );
					}
					
					new_offsets.push_back( *it2 );
				}
				else if ( crossed )
				{
					const V::Point3D::Type& this_point = points[ it2[  0 ] ];
					const V::Point3D::Type& prev_point = points[ it2[ -1 ] ];
					
					V::Point3D::Type clipped_point = LinePlaneIntersection( this_point - prev_point,
					                                                        this_point,
					                                                        plane );
					
					new_offsets.push_back( itsMesh.AddPoint( clipped_point ) );
				}
				
				previous_is_positive = is_positive;
				
				have_previous = true;
			}
			
			Offset first_offset = offsets.front();
			
			bool first_is_positive = positive_mask[ first_offset ];
			
			if ( first_is_positive != previous_is_positive )
			{
				const V::Point3D::Type& first_point = points[ offsets.front() ];
				const V::Point3D::Type& last_point  = points[ offsets.back () ];
				
				V::Point3D::Type clipped_point = LinePlaneIntersection( first_point - last_point,
				                                                        first_point,
				                                                        plane );
				
				new_offsets.push_back( itsMesh.AddPoint( clipped_point ) );
			}
			
			if ( new_offsets.empty() )
			{
				it->Swap( itsPolygons.back() );
				
				itsPolygons.pop_back();
				
				--it;
			}
			else
			{
				polygon.SwapVertexOffsets( new_offsets );
			}
		}
	}
	
}
