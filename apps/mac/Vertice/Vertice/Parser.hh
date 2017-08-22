/*
	Vertice/Parser.hh
	-----------------
*/

#ifndef VERTICE_PARSER_HH
#define VERTICE_PARSER_HH

// Standard C++
#include <map>

// plus
#include "plus/string.hh"

// Vectoria
#include "Vectoria/Point3D.hh"
#include "Vectoria/Units.hh"

// Vertice
#include "Vertice/Objects.hh"


namespace Vertice
{
	
	class Scene;
	
	class Parser
	{
		public:
			typedef std::map< plus::string, Vectoria::Point3D::Type > PointMap;
			typedef std::map< plus::string, ColorMatrix      > ColorMap;
			typedef std::map< plus::string, ImageTile        > ImageTileMap;
		
		private:
			Scene*                   itsScene;
			ColorMatrix              itsColor;
			Vectoria::Point3D::Type  itsOrigin;
			Vectoria::Degrees        itsTheta;
			Vectoria::Degrees        itsPhi;
			std::size_t              itsContextID;
			PointMap                 itsPoints;
			ColorMap                 itsColors;
			ImageTileMap             itsImageTiles;
			const ImageTile*         itsImageTile;
			double                   its1U;
			double                   its1V;
			double                   its2U;
			double                   its2V;
		
		public:
			Parser()  {}
			
			Parser( Scene& scene );
			
			~Parser()  {}
			
			void ParseLine( const plus::string& line );
			
			ColorMatrix ReadColor( const char* begin, const char* end ) const;
			
			void Define        ( const char* begin, const char* end );
			void SetContext    ( const char* begin, const char* end );
			void MakeCamera    ( const char* begin, const char* end );
			void SetColor      ( const char* begin, const char* end );
			void SetTile       ( const char* begin, const char* end );
			void SetOrigin     ( const char* begin, const char* end );
			void Translate     ( const char* begin, const char* end );
			void SetTheta      ( const char* begin, const char* end );
			void SetPhi        ( const char* begin, const char* end );
			void AddMeshPoint  ( const char* begin, const char* end );
			void AddMeshPolygon( const char* begin, const char* end );
	};
	
}

#endif
