/*	===============
 *	Vertice/Port.hh
 *	===============
 */

#ifndef VERTICE_PORT_HH
#define VERTICE_PORT_HH

// Standard C++
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
			std::vector< MeshModel > itsModels;
		
		public:
			std::vector< MeshModel > const& Models() const  { return itsModels; }
			std::vector< MeshModel >      & Models()        { return itsModels; }
			
			void AddModel( const MeshModel& model )  { itsModels.push_back( model ); }
			
			void SortByDepth();
			
			MeshModel* HitTest( const Vectoria::Point3D::Type& pt1 );
			
			void Swap( Frame& other )
			{
				using std::swap;
				
				swap( itsModels, other.itsModels );
			}
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
			Scene&  itsScene;
			Frame   itsFrame;
		
		public:
			Port( Scene& scene ) : itsScene( scene )
			{
			}
			
			Frame& Contents()  { return itsFrame; }
			
			void SendCameraCommand( std::size_t contextIndex, short cmd );
			
			void MakeFrame( Frame& outFrame ) const;
			
			std::pair< int, int > HitTest( double x, double y ) const;
	};
	
}

#endif

