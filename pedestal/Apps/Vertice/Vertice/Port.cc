/*	===============
 *	Vertice/Port.cc
 *	===============
 */

#include "Vertice/Port.hh"

// Standard C
#include <stddef.h>

// Standard C/C++
#include <cmath>

// Standard C++
#include <numeric>
#include <string>
#include <utility>

// Nitrogen
#include "Nitrogen/Sound.h"

// MoreFunctional
//#include "PointerToFunction.hh"

// Vectoria
#include "Vectoria/Clipping3D.hh"
#include "Vectoria/LinearAlgebra3D.hh"
#include "Vectoria/Polygon3D.hh"
#include "Vectoria/ViewFrustum.hh"

// Vertice
#include "Vertice/Objects.hh"
#include "Vertice/Model.hh"


namespace Vertice
{
	
	namespace N = Nitrogen;
	
	/*
	RGB::Color Tile::Color( const Point2D& pt ) const
	{
		RGB::Color color = Color();
		const double scale = 20;
		
		int x = std::floor( pt.x * scale );
		int y = std::floor( pt.y * scale );
		
		if ( std::abs( x % 2 )  ==  std::abs( y % 2 ) )
		{
			color *= 0.9;
		}
		
		return color;
	}
	*/
	
	void Port::SendCameraCommand( std::size_t contextIndex, short cmd )
	{
		double incrMove = 0.5;
		double incrRotate = 5.0 * 3.14159 / 180;
		
		if ( contextIndex == 0 )
		{
			N::SysBeep();
			return;
		}
		
		Moveable& target = itsScene.GetContext( contextIndex );
		
		switch ( cmd )
		{
			case cmdMoveLeft:
				target.ContextTranslate(-incrMove, 0, 0);
				break;
			case cmdMoveRight:
				target.ContextTranslate(incrMove, 0, 0);
				break;
			case cmdMoveForward:
				target.ContextTranslate(0, incrMove, 0);
				break;
			case cmdMoveBackward:
				target.ContextTranslate(0, -incrMove, 0);
				break;
			case cmdMoveUp:
				target.ContextTranslate(0, 0, incrMove);
				break;
			case cmdMoveDown:
				target.ContextTranslate(0, 0, -incrMove);
				break;
			case cmdYawLeft:
				target.ContextYaw(incrRotate);
				break;
			case cmdYawRight:
				target.ContextYaw(-incrRotate);
				break;
			case cmdPitchUp:
				target.ContextPitch(incrRotate);
				break;
			case cmdPitchDown:
				target.ContextPitch(-incrRotate);
				break;
			case cmdRollLeft:
				target.ContextRoll(-incrRotate);
				break;
			case cmdRollRight:
				target.ContextRoll(incrRotate);
				break;
			case cmdExpand:
				target.ContextScale(1.05);
				break;
			case cmdContract:
				target.ContextScale(1.0 / 1.05);
				break;
			case cmdLevelPitch:
				//xform = target.GetTransform();
				break;
			case cmdLevelRoll:
				//xform = target.GetTransform();
				break;
			case cmdGroundHeight:
				{
					V::Point3D::Type loc = Transformation( V::Point3D::Make( 0, 0, 0 ), target.itsTransform );
					
					V::Vector3D::Type vec = V::Vector3D::Make( 0, 0, -loc[ Z ] /* + target.groundHeight*/ );
					
					target.LocalTranslate( vec );
				}
				break;
			case cmdProjectionMode:
				//mCamera.fishEyeMode = !mCamera.fishEyeMode;
				break;
			default:
				break;
		}
	}
	
	
	class GetMeshModels
	{
		private:
			Scene&  itsScene;
			Frame&  itsResultFrame;
		
		public:
			GetMeshModels( Scene&  scene,
			               Frame&  outFrame ) : itsScene      ( scene    ),
			                                    itsResultFrame( outFrame )
			{
			}
			
			void operator()( std::size_t index );
	};
	
	
	void GetMeshModels::operator()( std::size_t index )
	{
		const Context& context = itsScene.GetContext( index );
		
		MeshModel model = context;
		
		itsResultFrame.AddModel( model );
		
		const std::vector< std::size_t >& subs = context.Subcontexts();
		
		std::for_each( subs.begin(), subs.end(), *this );
	}
	
	
	static double FocalLength( V::Radians alpha )
	{
		double e = 1 / std::tan( alpha / 2.0 );
		return e;
	}
	
	static double sAspectRatio = 1;
	
	static V::Radians sHorizontalFieldOfViewAngle = V::Degrees( 60 );
	
	static double sFocalLength = FocalLength( sHorizontalFieldOfViewAngle );
	
	
	void Port::MakeFrame( Frame& outFrame ) const
	{
		if ( itsScene.Cameras().empty() )  return;
		
		Camera& camera = itsScene.Cameras().front();
		
		const V::XMatrix& world2eye = camera.WorldToEyeTransform( itsScene );
		const V::XMatrix& eye2port  = camera.EyeToPortTransform();
		
		V::XMatrix world2port = Compose( world2eye, eye2port );
		
		V::XMatrix port2worldInverseTranspose = Transpose( world2port );
		
		V::Plane3D::Type nearPlane = port2worldInverseTranspose * V::NearPlane(   0.01 );
		V::Plane3D::Type farPlane  = port2worldInverseTranspose * V::FarPlane ( 100    );
		
		V::Plane3D::Type leftPlane  = port2worldInverseTranspose * V::LeftPlane (  sFocalLength );
		V::Plane3D::Type rightPlane = port2worldInverseTranspose * V::RightPlane(  sFocalLength );
		
		V::Plane3D::Type bottomPlane = port2worldInverseTranspose * V::BottomPlane( sFocalLength, sAspectRatio );
		V::Plane3D::Type topPlane    = port2worldInverseTranspose * V::TopPlane   ( sFocalLength, sAspectRatio );
		
		Frame newFrame;
		
		GetMeshModels( itsScene, newFrame )( 0 );
		
		typedef std::vector< MeshModel >::iterator ModelIter;
		
		std::vector< MeshModel >& models = newFrame.Models();
		
		V::Transformer< V::Point3D::Type > transformer( world2port );
		
		for ( ModelIter it = models.begin();  it != models.end();  ++it )
		{
			it->ClipAgainstPlane( nearPlane   );
			it->ClipAgainstPlane( farPlane    );
			it->ClipAgainstPlane( leftPlane   );
			it->ClipAgainstPlane( rightPlane  );
			it->ClipAgainstPlane( bottomPlane );
			it->ClipAgainstPlane( topPlane    );
			
			it->Transform( transformer );
		}
		
		outFrame.Swap( newFrame );
	}
	
	
	#if 0
	static V::Point2D::Type ProjectionOfPointOntoPolygonForPtrFun( const std::vector< V::Point3D::Type >& v,
	                                                               const V::Point3D::Type& pt )
	{
		return V::ProjectionOfPointOntoPolygon< std::vector< V::Point3D::Type > >( v, pt );
	}
	
	static bool PolygonContainsPoint( const std::vector< V::Point3D::Type >& polygon,
	                                  const V::Point3D::Type& pt )
	{
		V::Polygon2D poly2d;
		poly2d.Points().resize( polygon.size() );
		
		std::transform( polygon.begin(),
		                polygon.end(),
		                poly2d.Points().begin(),
		                std::bind1st( N::PtrFun( ProjectionOfPointOntoPolygonForPtrFun ),
		                              polygon ) );
		
		return poly2d.ContainsPoint( V::ProjectionOfPointOntoPolygon( polygon, pt ) );
	}
	
	MeshPoly*
	Frame::HitTest( const V::Point3D::Type& pt0, const Vector3D::Type& ray )
	{
		double minZ = 0;
		MeshModel* closestModel = NULL;
		MeshPoly* closestPoly = NULL;
		
		for ( std::vector< MeshModel >::iterator it = myMeshes.begin();  it != myMeshes.end();  ++it )
		{
			MeshModel& model = *it;
			std::vector< MeshPoly >& polies = model.Polygons();
			
			for ( std::vector< MeshPoly >::iterator it = polies.begin();  it != polies.end();  ++it )
			{
				MeshPoly& poly = *it;
				
				const std::vector< int >& offsets = poly.Vertices();
				std::vector< V::Point3D::Type > points( offsets.size() );
				
				std::transform( offsets.begin(),
				                offsets.end(),
				                points.begin(),
				                model.Mesh() );
				
				Plane3D plane = PlaneVector( points );
				
				if ( !ClipPointAgainstPlane( pt0, plane ) )
				{
					continue;
				}
				
				if ( !LineIntersectsPlane( ray, plane ) )
				{
					continue;
				}
				
				V::Point3D::Type sectPt = LinePlaneIntersection( ray, pt0, plane );
				
				/*
				if (!ClipPointAgainstPlane(sectPt, NearPlane(0.01))) {
					continue;
				}
				*/
				
				double dist = Magnitude( Vector3D::Type( sectPt - pt0 ) );
				
				if (    dist > 0
				     && (dist < minZ  ||  minZ == 0)
				     && PolygonContainsPoint( points, sectPt ) )
				{
					//
				}
				else
				{
					continue;
				}
				
				minZ = dist;
				closestPoly = &*it;
				closestModel = &model;
				
			}
		}
		
		return closestPoly;
		
		/*
		Trim(intersections, ZIsZero);
		
		if (intersections.empty()) {
			//return make_pair(std::string(""), -1);
			return NULL;
		}
		
		double minZ = 0;
		Point2D ptInPolygon;
		const Tile* theTile = NULL;
		
		std::vector<Intersection>::const_iterator it, closest, done = intersections.end();
		
		for (it = intersections.begin(), closest = done; it != done; ++it) {
			const Intersection& intersection = *it;
			double z = intersection.first.distanceSquared;
			if (z > minZ  &&  closest != done) {
				// There's already a closer match
				continue;
			}
			Point2D pt = intersection.first.pointOnPlane;
			const Polygon2D& polygon = intersection.second->Polygon();
			if (polygon.ContainsPoint(pt)) {
				minZ = z;
				closest = it;
				ptInPolygon = pt;
				theTile = intersection.second;
			}
		}
		
		return (closest == done) 
			? std::make_pair(std::string(""), -1) 
			: std::make_pair(theTile->ContextIndex(), theTile->PolyIndex());
		*/
	}
	#endif
	
}

