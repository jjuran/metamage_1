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
	
	template < class Point >
	class PointMesh
	{
		private:
			std::vector< Point > itsPoints;
		
		public:
			typedef Point point_type;
			
			std::vector< Point > const& Points() const  { return itsPoints; }
			std::vector< Point >      & Points()        { return itsPoints; }
			
			std::size_t AddPoint( const Point& pt )
			{
				itsPoints.push_back( pt );
				
				return itsPoints.size() - 1;
			}
			
			bool Empty() const  { return itsPoints.empty(); }
			
			template < class Transformer >
			void Transform( const Transformer& transformer )
			{
				std::transform( itsPoints.begin(),
				                itsPoints.end(),
				                itsPoints.begin(),
				                transformer );
			}
			
			Point operator()( std::size_t offset ) const  { return itsPoints[ offset ]; }
			
			void Swap( PointMesh& other )
			{
				std::swap( itsPoints, other.itsPoints );
			}
	};
	
	// MeshPoly models a polygon as a sequence of indices into a PointMesh, plus a color.
	// The user must keep track of which PointMesh stores the points, as it's not specified here.
	
	class MeshPoly
	{
		public:
			typedef unsigned Offset;
			
		private:
			std::vector< Offset >  itsVertices;
			ColorMatrix            itsColor;
		
		public:
			MeshPoly()  {}
			MeshPoly( const std::vector< Offset >&  offsets,
			          const ColorMatrix&            color ) : itsVertices( offsets ),
			                                                  itsColor   ( color   )
			{}
			
			const std::vector< Offset >& Vertices() const  { return itsVertices; }
			      std::vector< Offset >& Vertices()        { return itsVertices; }
			
			const ColorMatrix& Color() const  { return itsColor; }
			
			void Swap( MeshPoly& other )
			{
				std::swap( itsVertices, other.itsVertices );
				std::swap( itsColor,    other.itsColor    );
			}
	};
	
	// MeshModel combines a PointMesh and the polygons defined in terms of it.
	
	class MeshModel
	{
		private:
			PointMesh< V::Point3D::Type >  itsMesh;
			std::vector< MeshPoly >        itsPolygons;
			bool                           itIsSelected;
		
		public:
			MeshModel::MeshModel() : itIsSelected()  {}
			
			PointMesh< V::Point3D::Type > const& Mesh() const  { return itsMesh; }
			
			const std::vector< MeshPoly >& Polygons() const  { return itsPolygons; }
			
			std::size_t AddPointToMesh( const V::Point3D::Type& pt )  { return itsMesh.AddPoint( pt ); }
			
			void AddMeshPoly( const std::vector< unsigned >& offsets, const ColorMatrix& color )
			{
				itsPolygons.push_back( MeshPoly( offsets, color ) );
			}
			
			bool Selected() const  { return itIsSelected; }
			
			void Select()  { itIsSelected = true; }
			
			template < class Transformer >
			void Transform( const Transformer& transformer )
			{
				itsMesh.Transform( transformer );
			}
			
			void CullBackfaces( const V::Point3D::Type& eye );
			
			void CullDeadPoints();
			
			void ClipAgainstPlane( const V::Plane3D::Type& plane );
			
			void Swap( MeshModel& other )
			{
				itsMesh.Swap( other.itsMesh );
				
				std::swap( itsPolygons, other.itsPolygons );
			}
	};
	
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

#endif

