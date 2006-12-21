/*	===============
 *	Vertice/Port.hh
 *	===============
 */

#ifndef VERTICE_PORT_HH
#define VERTICE_PORT_HH

// Standard C++
#include <string>
#include <vector>

// Vertice
#include "Vectoria/Polygon2D.hh"
#include "Vertice/Model.hh"


namespace Vertice
{
	
	using V::X;
	using V::Y;
	using V::Z;
	
	class Frame
	{
		private:
			typedef std::vector< PointMesh< V::Point3D::Type > > MeshContainer;
			MeshContainer myMeshes;
		
		public:
			MeshContainer const& Meshes() const  { return myMeshes; }
			MeshContainer      & Meshes()        { return myMeshes; }
			
			void AddMesh( const PointMesh< V::Point3D::Type >& mesh )  { myMeshes.push_back( mesh ); }
			
			//MeshPoly* HitTest( const Point3D::Type& pt0, const Vector3D& ray );
	};
	
	
	enum
	{
		cmdMoveLeft,
		cmdMoveRight,
		cmdMoveUp,
		cmdMoveDown,
		cmdMoveForward,
		cmdMoveBackward,
		cmdYawLeft,
		cmdYawRight,
		cmdPitchUp,
		cmdPitchDown,
		cmdRollLeft,
		cmdRollRight,
		cmdExpand,
		cmdContract,
		cmdLevelRoll,
		cmdLevelPitch,
		cmdGroundHeight,
		cmdPlayPause,
		cmdProjectionMode,
		cmdPrevCamera,
		cmdNextCamera
	};
	
	class Camera;
	
	class FlatLens
	{
		public:
			V::Point3D::Type Bend  ( const V::Point3D::Type& pt ) const  { return pt; }
			V::Point3D::Type Unbend( const V::Point3D::Type& pt ) const  { return pt; }
	};
	
	class FisheyeLens
	{
		public:
			V::Point3D::Type Bend  ( const V::Point3D::Type& pt ) const;
			V::Point3D::Type Unbend( const V::Point3D::Type& pt ) const;
	};
	
	class Port
	{
		public:
			Model& myModel;
			Frame myFrame;
		
		public:
			Port( Model& inModel ) : myModel( inModel )  {}
			
			Frame& Contents()  { return myFrame; }
			
			void SendCameraCommand( std::size_t contextIndex, short inCmd );
			
			void MakeFrame( Frame& outFrame ) const;
			
			std::pair< int, int > HitTest( double inX, double inY ) const;
	};
	
	inline V::Point3D::Type FisheyeLens::Bend( const V::Point3D::Type& pt ) const
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
	
	inline V::Point3D::Type FisheyeLens::Unbend( const V::Point3D::Type& pt ) const
	{
		double x = pt[ X ];
		double y = pt[ Y ];
		double z = pt[ Z ];
		
		double r, theta;
		V::XY2Polar( x, y, r, theta );
		
		r = tan( r );
		V::Polar2XY( r, theta, x, y );
		
		return V::Point3D::Make( x, y, z );
	}
	
}

#endif

