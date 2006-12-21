/*	==================
 *	Vertice/Objects.hh
 *	==================
 */

#ifndef VERTICE_OBJECTS_HH
#define VERTICE_OBJECTS_HH

#include <map>
#include <string>
#include <vector>

//#include "Vectoria/TriColor.hh"
#include "Vectoria/Transform.hh"
#include "Vectoria/Polygon2D.hh"
#include "Vectoria/Vector3D.hh"
#include "Vectoria/Point3D.hh"


namespace Vertice
{
	
	namespace V = Vectoria;
	
	
	typedef Vectoria::Matrix< double, 3, 1 > ColorMatrix;
	
	
	class Moveable
	{
		public:
			V::XMatrix xform;
			V::XMatrix inverse;
		
		public:
			Moveable()  {}
			Moveable( const V::XMatrix& xf, const V::XMatrix& inv ) : xform( xf ), inverse( inv )  {}
			
			void ContextTranslate(double dx, double dy, double dz);
			void ContextTranslate(const V::Vector3D::Type& inVector);
			void ContextYaw(double dtheta);
			void ContextPitch(double dphi);
			void ContextRoll(double dpsi);
			void ContextScale(double factor);
			void LocalTranslate(double dx, double dy, double dz);
			void LocalTranslate(const V::Vector3D::Type& inVector);
			void LocalYaw(double dtheta);
			void LocalPitch(double dphi);
			void LocalRoll(double dpsi);
			void LocalScale(double factor);
	};
	
	template < class Point >
	class PointMesh
	{
		private:
			std::vector< Point > points;
		
		public:
			typedef Point point_type;
			
			std::size_t AddPoint( const Point& pt )
			{
				points.push_back( pt );
				
				return points.size() - 1;
			}
			
			bool Empty() const  { return points.empty(); }
			
			template < class Transformer >
			void Transform( const Transformer& transformer )
			{
				std::transform( points.begin(),
				                points.end(),
				                points.begin(),
				                transformer );
			}
			
			Point operator()( std::size_t offset ) const  { return points[ offset ]; }
	};
	
	class MeshPoly
	{
		public:
			typedef unsigned Offset;
			
		private:
			std::vector< Offset > myVertices;
			ColorMatrix color;
		
		public:
			MeshPoly()  {}
			MeshPoly( const std::vector< Offset >&  offsets,
			          const ColorMatrix&            color ) 
			:
				myVertices( offsets ), color( color )
			{}
			
			const std::vector< Offset >& Vertices() const  { return myVertices; }
			      std::vector< Offset >& Vertices()        { return myVertices; }
			
			const ColorMatrix& Color() const  { return color; }
	};
	
	class MeshModel
	{
		private:
			PointMesh< V::Point3D::Type > myMesh;
			std::vector< MeshPoly > myPolygons;
		
		public:
			PointMesh< V::Point3D::Type > const& Points() const  { return myMesh; }
			PointMesh< V::Point3D::Type >      & Points()        { return myMesh; }
			
			std::vector< MeshPoly > const& Polies() const  { return myPolygons; }
			std::vector< MeshPoly >      & Polies()        { return myPolygons; }
			
			std::size_t AddPointToMesh( const V::Point3D::Type& pt )  { return myMesh.AddPoint( pt ); }
			
			void AddMeshPoly( const std::vector< unsigned >& offsets, const ColorMatrix& color )
			{
				myPolygons.push_back( MeshPoly( offsets, color ) );
			}
	};
	
	class Context : public Moveable
	{
		private:
			std::size_t parentIndex;
			std::string name;
			
			PointMesh< V::Point3D::Type > fMesh;
			std::vector< MeshPoly > fPolygons;
			std::vector< std::size_t > mySubcontexts;
		
		public:
			Context() : parentIndex( 0 ), name( " " )  {}
			
			Context( std::size_t         parentIndex,
			         const std::string&  name,
			         const V::XMatrix&   offset,
	                 const V::XMatrix&   inv )
			:
				Moveable( offset, inv ),
				parentIndex( parentIndex ),
				name( name )
			{}
			
			std::size_t ParentIndex() const  { return parentIndex; }
			
			const std::string& Name() const  { return name; }
			
			int AddPointToMesh( const V::Point3D::Type& pt )  { return fMesh.AddPoint( pt ); }
			
			void AddMeshPoly( const std::vector< unsigned >& offsets, const ColorMatrix& color )
			{
				fPolygons.push_back( MeshPoly( offsets, color ) );
			}
			
			PointMesh< V::Point3D::Type > const& Mesh() const  { return fMesh; }
			
			const std::vector< MeshPoly >& Polygons() const  { return fPolygons; }
			
			std::vector< std::size_t > const& Subcontexts() const  { return mySubcontexts; }
			
			void AddSubcontext( std::size_t index )
			{
				mySubcontexts.push_back( index );
			}
	};
	
}

#endif

