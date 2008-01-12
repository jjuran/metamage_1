/*	==================
 *	Vertice/Objects.cc
 *	==================
 */

#include "Vertice/Objects.hh"

// Vectoria
#include "Vectoria/Coordinates.hh"
#include "Vectoria/LinearAlgebra3D.hh"
#include "Vectoria/Transform.hh"


namespace Vertice
{
	
	using Vectoria::X;
	using Vectoria::Y;
	using Vectoria::Z;
	
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
	
	
	void MeshModel::ClipAgainstPlane( const V::Plane3D::Type& plane )
	{
		const std::vector< V::Point3D::Type >& points = itsMesh.Points();
		
		std::vector< bool > positive_mask( points.size() );
		std::vector< bool > zero_mask    ( points.size() );
		std::vector< bool > negative_mask( points.size() );
		
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
		
		typedef MeshPoly::Offset Offset;
		
		typedef std::vector< MeshPoly >::iterator PolygonIter;
		
		typedef std::vector< Offset >::const_iterator OffsetIter;
		
		for ( PolygonIter it = itsPolygons.begin();  it != itsPolygons.end();  ++it )
		{
			MeshPoly& polygon = *it;
			
			std::vector< Offset >& offsets = polygon.Vertices();
			
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
			
			std::swap( offsets, new_offsets );
		}
	}
	
}

