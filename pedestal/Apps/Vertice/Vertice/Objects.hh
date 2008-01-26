/*	==================
 *	Vertice/Objects.hh
 *	==================
 */

#ifndef VERTICE_OBJECTS_HH
#define VERTICE_OBJECTS_HH

#include <map>
#include <string>
#include <vector>

#include "Vectoria/Plane3D.hh"
#include "Vectoria/Point3D.hh"
#include "Vectoria/Polygon2D.hh"
#include "Vectoria/Transform.hh"
#include "Vectoria/Vector3D.hh"


namespace Vertice
{
	
	namespace V = Vectoria;
	
	
	typedef Vectoria::Matrix< double, 3, 1 > ColorMatrix;
	
	class IntensityMap
	{
		private:
			unsigned               itsWidth;
			std::vector< double >  itsValues;
		
		public:
			IntensityMap()  {}
			
			IntensityMap( unsigned width, const std::vector< double >& values ) : itsWidth( width ), itsValues( values )  {}
			
			void Swap( IntensityMap& other );
			
			bool Empty() const  { return itsValues.empty(); }
			
			unsigned Width() const  { return itsWidth; }
			
			const std::vector< double >& Values() const  { return itsValues; }
	};
	
	
	// A Moveable object maintains a transform describing its movements relative to some context.
	// For convenience, it also stores the inverse transform.
	
	class Moveable
	{
		public:
			V::XMatrix itsTransform;
			V::XMatrix itsInverse;
		
		public:
			Moveable()  {}
			
			Moveable( const V::XMatrix& xform,
			          const V::XMatrix& inverse ) : itsTransform( xform   ),
			                                        itsInverse  ( inverse )
			{
			}
			
			void ContextTranslate( double dx, double dy, double dz );
			
			void ContextTranslate( const V::Vector3D::Type& v );
			
			void ContextYaw  ( double dtheta );
			void ContextPitch( double dphi   );
			void ContextRoll ( double dpsi   );
			
			void ContextScale( double factor );
			
			void LocalTranslate( double dx, double dy, double dz );
			
			void LocalTranslate( const V::Vector3D::Type& v );
			
			void LocalYaw  ( double dtheta );
			void LocalPitch( double dphi   );
			void LocalRoll ( double dpsi   );
			
			void LocalScale( double factor );
	};
	
	// PointMesh stores an ordered collection of points that can be transformed all at once.
	
	class PointMesh
	{
		private:
			std::vector< V::Point3D::Type > itsPoints;
		
		public:
			void Swap( PointMesh& other )
			{
				std::swap( itsPoints, other.itsPoints );
			}
			
			typedef V::Point3D::Type point_type;
			
			std::vector< V::Point3D::Type > const& Points() const  { return itsPoints; }
			std::vector< V::Point3D::Type >      & Points()        { return itsPoints; }
			
			std::size_t AddPoint( const V::Point3D::Type& pt )
			{
				itsPoints.push_back( pt );
				
				return itsPoints.size() - 1;
			}
			
			bool Empty() const  { return itsPoints.empty(); }
			
			// Not called MinumumZ(), because Z is negative.
			double MinimumDepth() const;
			
			template < class Transformer >
			void Transform( const Transformer& transformer )
			{
				std::transform( itsPoints.begin(),
				                itsPoints.end(),
				                itsPoints.begin(),
				                transformer );
			}
			
			const V::Point3D::Type& operator()( std::size_t offset ) const  { return itsPoints[ offset ]; }
	};
	
	// MeshPolygon models a polygon as a sequence of indices into a PointMesh, plus a color.
	// The user must keep track of which PointMesh stores the points, as it's not specified here.
	
	class MeshPolygon
	{
		public:
			typedef unsigned Offset;
			
		private:
			std::vector< Offset >  itsVertices;
			V::Point2D::Type       itsMapPoints[ 3 ];
			Offset                 itsSavedOffsets[ 3 ];
			ColorMatrix            itsColor;
			IntensityMap           itsMap;
		
		public:
			MeshPolygon()  {}
			
			MeshPolygon( const std::vector< Offset >&  offsets,
			             const ColorMatrix&            color );
			
			MeshPolygon( const std::vector< Offset >&  offsets,
			             const IntensityMap&           map,
			             const V::Point2D::Type&       ptA,
			             const V::Point2D::Type&       ptB );
			
			void Swap( MeshPolygon& other );
			
			const std::vector< Offset >& Vertices() const  { return itsVertices; }
			
			void SwapVertexOffsets( std::vector< Offset >& vertexOffsets );
			
			const V::Point2D::Type* MapPoints() const  { return itsMapPoints; }
			
			const Offset* SavedOffsets() const  { return itsSavedOffsets; }
			
			const ColorMatrix& Color() const  { return itsColor; }
			
			const IntensityMap& Map() const  { return itsMap; }
	};
	
	// MeshModel combines a PointMesh and the polygons defined in terms of it.
	
	class MeshModel
	{
		private:
			PointMesh                   itsMesh;
			std::vector< MeshPolygon >  itsPolygons;
			bool                        itIsSelected;
		
		public:
			MeshModel() : itIsSelected()  {}
			
			void Swap( MeshModel& other )
			{
				itsMesh.Swap( other.itsMesh );
				
				std::swap( itsPolygons, other.itsPolygons );
			}
			
			PointMesh const& Mesh() const  { return itsMesh; }
			
			const std::vector< MeshPolygon >& Polygons() const  { return itsPolygons; }
			
			std::size_t AddPointToMesh( const V::Point3D::Type& pt )
			{
				return itsMesh.AddPoint( pt );
			}
			
			void AddMeshPolygon( const std::vector< unsigned >& offsets, const ColorMatrix& color )
			{
				itsPolygons.push_back( MeshPolygon( offsets, color ) );
			}
			
			void AddMeshPolygon( const std::vector< unsigned >&  offsets,
			                     const IntensityMap&             map,
			                     const V::Point2D::Type&         ptA,
			                     const V::Point2D::Type&         ptB )
			{
				itsPolygons.push_back( MeshPolygon( offsets, map, ptA, ptB ) );
			}
			
			bool Selected() const  { return itIsSelected; }
			
			void Select()  { itIsSelected = true; }
			
			template < class Transformer >
			void Transform( const Transformer& transformer )
			{
				itsMesh.Transform( transformer );
			}
			
			double MinimumDepth() const  { return itsMesh.MinimumDepth(); }
			
			void CullBackfaces( const V::Point3D::Type& eye );
			
			void ClipAgainstPlane( const V::Plane3D::Type& plane );
	};
	
	inline bool operator<( const MeshModel& a, const MeshModel& b )
	{
		return a.MinimumDepth() < b.MinimumDepth();
	}
	
	class Context : public Moveable, public MeshModel
	{
		private:
			std::size_t itsParentIndex;
			std::string itsName;
			
			std::vector< std::size_t > itsSubcontexts;
		
		public:
			Context() : Moveable( V::XMatrix( V::IdentityMatrix() ),
			                      V::XMatrix( V::IdentityMatrix() ) ),
			            itsParentIndex( 0 ),
			            itsName( " " )
			{
			}
			
			Context( std::size_t         parentIndex,
			         const std::string&  name,
			         const V::XMatrix&   transform,
	                 const V::XMatrix&   inverse ) : Moveable( transform, inverse ),
	                                                 itsParentIndex( parentIndex ),
	                                                 itsName       ( name        )
			{
			}
			
			std::size_t ParentIndex() const  { return itsParentIndex; }
			
			const std::string& Name() const  { return itsName; }
			
			std::vector< std::size_t > const& Subcontexts() const  { return itsSubcontexts; }
			
			void AddSubcontext( std::size_t index )
			{
				itsSubcontexts.push_back( index );
			}
	};
	
}

namespace std
{
	
	template <>
	inline void swap( Vertice::IntensityMap& a, Vertice::IntensityMap& b )
	{
		a.Swap( b );
	}
	
	template <>
	inline void swap( Vertice::PointMesh& a, Vertice::PointMesh& b )
	{
		a.Swap( b );
	}
	
	template <>
	inline void swap( Vertice::MeshPolygon& a, Vertice::MeshPolygon& b )
	{
		a.Swap( b );
	}
	
}

#endif

