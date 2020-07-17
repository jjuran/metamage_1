/*
	worldview/Parser.cc
	-------------------
*/

#include "worldview/Parser.hh"

// Standard C/C++
#include <cstring>

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// gear
#include "gear/find.hh"
#include "gear/parse_float.hh"

// Vectoria
#include "Vectoria/TriColor.hh"

// worldview
#include "worldview/Model.hh"


namespace worldview
{
	
	using V::X;
	using V::Y;
	using V::Z;
	
	
	class ParseError {};
	
	
	template < class Point >
	inline V::Translation MakeTranslation( const Point& pt )
	{
		return V::Translation( pt[ X ],
		                       pt[ Y ],
		                       pt[ Z ] );
	}
	
	
	static bool scan_float( const char*& mark, double* f )
	{
		const char* saved_mark = mark;
		
		*f = gear::parse_float( &mark );
		
		return mark != saved_mark;
	}
	
	static int scan_3_floats( const char* mark, double* a, double* b, double* c )
	{
		int n = 0;
		
		n += scan_float( mark, a );
		n += scan_float( mark, b );
		n += scan_float( mark, c );
		
		return n;
	}
	
	static int scan_4_floats( const char* mark, double* a, double* b, double* c, double* d )
	{
		int n = scan_float( mark, a );
		
		n += scan_3_floats( mark, b, c, d );
		
		return n;
	}
	
	ColorMatrix Parser::ReadColor( const char* begin, const char* end ) const
	{
		double red, green, blue;
		
		int scanned = scan_3_floats( begin, &red, &green, &blue );
		
		if ( scanned == 3 )
		{
			return V::MakeRGB( red, green, blue );
		}
		else
		{
			ColorMap::const_iterator it = itsColors.find( plus::string( begin, end ) );
			
			if ( it != itsColors.end() )
			{
				return it->second;
			}
		}
		
		throw ParseError();
	}
	
	static std::vector< double > ReadValues( const char* begin, const char* end )
	{
		std::vector< double > values;
		
		while ( begin < end )
		{
			const char* saved = begin;
			
			double value = gear::parse_float( &begin );
			
			if ( begin == saved )
			{
				break;
			}
			
			values.push_back( value );
		}
		
		return values;
	}
	
	static unsigned WidthFromTileSize( std::size_t size )
	{
		bool size_is_even_power_of_2 = (size & (size - 1)) == 0  &&  (size & 0x55555555) != 0;
		
		if ( !size_is_even_power_of_2 )
		{
			throw ParseError();
		}
		
		// Look, ma!  No branches!
		unsigned width = (size >> 0 &   1)
		               | (size >> 1 &   2)
		               | (size >> 2 &   4)
		               | (size >> 3 &   8)
		               | (size >> 4 &  16)
		               | (size >> 5 &  32)
		               | (size >> 6 &  64)
		               | (size >> 7 & 128)
		               | (size >> 8 & 256);
		
		return width;
	}
	
	static ImageTile MakeImageTile( const std::vector< double >& values )
	{
		std::vector< ColorMatrix > colors;
		
		std::size_t size = values.size();
		
		if ( size % 3 != 0 )
		{
			colors.reserve( size );
			
			for ( std::size_t i = 0;  i < size;  ++i )
			{
				colors.push_back( V::MakeGray( values[ i ] ) );
			}
		}
		else
		{
			colors.reserve( size / 3 );
			
			for ( unsigned i = 0;  i < colors.size();  ++i )
			{
				colors.push_back( V::MakeRGB( values[ 3 * i     ],
				                              values[ 3 * i + 1 ],
				                              values[ 3 * i + 2 ] ) );
			}
		}
		
		return ImageTile( WidthFromTileSize( colors.size() ), colors );
	}
	
	static
	const char* find_space( const char* begin, const char* end )
	{
		return std::find( begin, end, ' ' );
	}
	
	void Parser::Define( Parser& parser, const char* begin, const char* end )
	{
		const char* space = find_space( begin, end );
		
		plus::string type( begin, space );
		
		begin = space + (space != end);
		
		space = find_space( begin, end );
		
		plus::string name( begin, space );
		
		begin = space + (space != end);
		
		if ( type == "color" )
		{
			parser.itsColors[ name ] = parser.ReadColor( begin, end );
		}
		else if ( type == "tile" )
		{
			ImageTile tile = MakeImageTile( ReadValues( begin, end ) );
			
			parser.itsImageTiles[ name ] = tile;
		}
	}
	
	void Parser::SetContext( Parser& parser, const char* begin, const char* end )
	{
		plus::string contextName( begin, end );
		
		Scene*                   itsScene     = parser.itsScene;
		std::size_t&             itsContextID = parser.itsContextID;
		Vectoria::Point3D::Type& itsOrigin    = parser.itsOrigin;
		
		itsContextID = itsScene->AddSubcontext( itsContextID,
		                                        contextName,
		                                        MakeTranslation(  itsOrigin ).Make(),
		                                        MakeTranslation( -itsOrigin ).Make() );
		
		itsOrigin = V::Point3D::Make( 0, 0, 0 );
	}
	
	void Parser::MakeCamera( Parser& parser, const char* begin, const char* end )
	{
		parser.itsScene->Cameras().push_back( Camera( parser.itsContextID ) );
	}
	
	void Parser::SetColor( Parser& parser, const char* begin, const char* end )
	{
		parser.itsColor = parser.ReadColor( begin, end );
		
		parser.itsImageTile = NULL;
	}
	
	void Parser::SetTile( Parser& parser, const char* begin, const char* end )
	{
		const char* space = find_space( begin, end );
		
		plus::string name( begin, space );
		
		ImageTileMap::const_iterator it = parser.itsImageTiles.find( name );
		
		if ( it == parser.itsImageTiles.end() )
		{
			throw ParseError();
		}
		
		begin = space + (space != end);
		
		parser.itsImageTile = &it->second;
		
		double& its1U = parser.its1U;
		double& its1V = parser.its1V;
		double& its2U = parser.its2U;
		double& its2V = parser.its2V;
		
		its1U = 1.0;
		
		its1V =
		its2U =
		its2V = 0.0;
		
		int scanned = scan_4_floats( begin, &its1U, &its1V, &its2U, &its2V );
		
		if ( scanned < 3 )
		{
			using std::swap;
			
			swap( its1U, its2U );
			
			if ( scanned == 2 )
			{
				swap( its1V, its2V );
			}
		}
	}
	
	void Parser::SetOrigin( Parser& parser, const char* begin, const char* end )
	{
		double x, y, z;
		
		int scanned = scan_3_floats( begin, &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.itsOrigin = V::Point3D::Make( x, y, z );
		}
	}
	
	void Parser::Translate( Parser& parser, const char* begin, const char* end )
	{
		double x, y, z;
		
		int scanned = scan_3_floats( begin, &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.itsOrigin += V::Vector3D::Make( x, y, z );
		}
	}
	
	void Parser::SetTheta( Parser& parser, const char* begin, const char* end )
	{
		double theta;
		
		int scanned = scan_float( begin, &theta );
		
		if ( scanned == 1 )
		{
			parser.itsTheta = V::Degrees( theta );
		}
	}
	
	void Parser::SetPhi( Parser& parser, const char* begin, const char* end )
	{
		double phi;
		
		int scanned = scan_float( begin, &phi );
		
		if ( scanned == 1 )
		{
			parser.itsPhi = V::Degrees( phi );
		}
	}
	
	void Parser::AddMeshPoint( Parser& parser, const char* begin, const char* end )
	{
		const char* space = find_space( begin, end );
		
		plus::string name( begin, space );
		
		begin = space;
		
		while ( *begin == ' ' )
		{
			++begin;
		}
		
		double x, y, z;
		
		int scanned = scan_3_floats( begin, &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.itsPoints[ name ] = parser.itsOrigin + V::Vector3D::Make( x, y, z );
		}
	}
	
	void Parser::AddMeshPolygon( Parser& parser, const char* begin, const char* end )
	{
		std::vector< unsigned > offsets;
		
		Context& context = parser.itsScene->GetContext( parser.itsContextID );
		
		while ( begin < end )
		{
			const char* space = find_space( begin, end );
			
			plus::string ptName( begin, space );
			
			offsets.push_back( context.AddPointToMesh( parser.itsPoints[ ptName ] ) );
			
			begin = space;
			
			while ( *begin == ' ' )
			{
				++begin;
			}
		}
		
		if ( !offsets.empty() )
		{
			if ( parser.itsImageTile != NULL )
			{
				context.AddMeshPolygon( offsets,
				                        *parser.itsImageTile,
				                        V::Point2D::Make( parser.its1U, parser.its1V ),
				                        V::Point2D::Make( parser.its2U, parser.its2V ) );
			}
			else
			{
				context.AddMeshPolygon( offsets, parser.itsColor );
			}
		}
	}
	
	typedef void ( *Handler )( Parser&, const char*, const char* );
	
	static std::map< plus::string, Handler > MakeHandlers()
	{
		std::map< plus::string, Handler > handlers;
		
		handlers[ "camera"    ] = &Parser::MakeCamera;
		handlers[ "color"     ] = &Parser::SetColor;
		handlers[ "context"   ] = &Parser::SetContext;
		handlers[ "define"    ] = &Parser::Define;
		handlers[ "origin"    ] = &Parser::SetOrigin;
		handlers[ "phi"       ] = &Parser::SetPhi;
		handlers[ "poly"      ] = &Parser::AddMeshPolygon;
		handlers[ "polygon"   ] = &Parser::AddMeshPolygon;
		handlers[ "pt"        ] = &Parser::AddMeshPoint;
		handlers[ "theta"     ] = &Parser::SetTheta;
		handlers[ "tile"      ] = &Parser::SetTile;
		handlers[ "translate" ] = &Parser::Translate;
		
		return handlers;
	}
	
	static Handler GetHandler( const plus::string& command )
	{
		typedef std::map< plus::string, Handler > Handlers;
		
		static Handlers handlers = MakeHandlers();
		
		Handlers::const_iterator it = handlers.find( command );
		
		if ( it == handlers.end() )  return NULL;
		
		return it->second;
	}
	
	Parser::Parser( Scene& scene ) : itsScene( &scene ),
	                                 itsOrigin( V::Point3D::Make( 0, 0, 0 ) ),
	                                 itsTheta    ( 0 ),
	                                 itsPhi      ( 0 ),
	                                 itsContextID( 0 ),
	                                 itsImageTile()
	{
	}
	
	void Parser::ParseLine( const plus::string& line )
	{
		const unsigned char whitespace[] = { 2, ' ', '\t' };
		
		const char* cmd_start = gear::find_first_nonmatch( line.data(),
		                                                   line.size(),
		                                                   whitespace );
		
		if ( cmd_start == NULL )
		{
			return;
		}
		
		const char* end = &*line.end();
		
		// We are guaranteed a non-empty line since we didn't return above.
		
		const char* start = cmd_start;
		
		const char* stop = std::find( start, end, ' ' );
		
		plus::string cmdname( start, stop );
		
		if ( Handler handler = GetHandler( cmdname ) )
		{
			for ( start = stop;  *start == ' ';  ++start )
			{
				
			}
			
			start = stop;
			
			while ( *start == ' ' )
			{
				++start;
			}
			
			handler( *this, start, end );
		}
	}
	
	void Loader::LoadLine( const plus::string& line )
	{
		if ( std::strchr( line.c_str(), '{' ) )
		{
			itsSavedParsers.push_back( itsParser );
		}
		else if ( std::strchr( line.c_str(), '}' ) )
		{
			itsParser = itsSavedParsers.back();
			itsSavedParsers.pop_back();
		}
		else
		{
			itsParser.ParseLine( line );
		}
	}
	
}
