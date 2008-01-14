/*	===================
 *	Vertice/Document.cc
 *	===================
 */

#include "Vertice/Document.hh"

// Standard C
#include <ctype.h>
#include <stdio.h>

// Standard C/C++
#include <cstdlib>

// Standard C++
#include <list>
#include <map>
#include <string>
#include <vector>

// Io
#include "Io/TextInput.hh"

// Vectoria
#include "Vectoria/Point3D.hh"
#include "Vectoria/TriColor.hh"
#include "Vectoria/Units.hh"

// Vertice
#include "Vertice/Model.hh"
#include "Vertice/Port.hh"


namespace Vertice
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	using V::X;
	using V::Y;
	using V::Z;
	
	template < class Point >
	inline V::Translation MakeTranslation( const Point& pt )
	{
		return V::Translation( pt[ X ],
		                       pt[ Y ],
		                       pt[ Z ] );
	}
	
	static Rect MakeWindowRect()
	{
		Rect screenBounds = N::GetQDGlobalsScreenBits().bounds;
		short mbarHeight = ::GetMBarHeight();
		Rect rect = screenBounds;
		rect.top += mbarHeight + 22;
		rect.bottom -= 7;
		rect.left = 6;
		rect.right -= 7;
		short length = std::min( rect.bottom - rect.top, 400 );
		rect.bottom = rect.top + length;
		//short vMargin = ( screenBounds.bottom - rect.bottom ) - mbarHeight;
		short hMargin = ( screenBounds.right - length );
		rect.left = hMargin / 2;
		rect.right = hMargin / 2 + length;
		
		return rect;
		return N::InsetRect( rect, 4, 4 );
	}
	
	Window::Window( const boost::shared_ptr< Ped::WindowCloseHandler >&  handler, ConstStr255Param title )
	: 
		Ped::Window< PortView >( Ped::NewWindowContext( MakeWindowRect(),
		                                                title ),
		                         handler )
	{
	}
	
	
	class Parser
	{
		public:
			typedef std::map< std::string, V::Point3D::Type > Map;
			
			Parser();
			
			~Parser()  {}
			
			void ParseLine( Scene& scene, const std::string& line );
			
			ColorMatrix       itsColor;
			V::Point3D::Type  itsOrigin;
			V::Degrees        itsTheta;
			V::Degrees        itsPhi;
			std::size_t       itsContextID;
			Map               itsPoints;
	};
	
	
	static void SetContext( Parser&      parser,
	                        Scene&       scene,
	                        const char*  begin,
	                        const char*  end )
	{
		std::string contextName( begin, end );
		
		parser.itsContextID = scene.AddSubcontext( parser.itsContextID,
		                                           contextName,
		                                           MakeTranslation(  parser.itsOrigin ).Make(),
		                                           MakeTranslation( -parser.itsOrigin ).Make() );
		
		parser.itsOrigin = V::Point3D::Make( 0, 0, 0 );
	}
	
	static void MakeCamera( Parser&      parser,
	                        Scene&       scene,
	                        const char*  begin,
	                        const char*  end )
	{
		scene.Cameras().push_back( Camera( parser.itsContextID ) );
	}
	
	static void SetColor( Parser&      parser,
	                      Scene&       scene,
	                      const char*  begin,
	                      const char*  end )
	{
		double red, green, blue;
		
		int scanned = std::sscanf( begin, "%lf %lf %lf", &red, &green, &blue );
		
		if ( scanned == 3 )
		{
			parser.itsColor = V::MakeRGB( red, green, blue );
		}
	}
	
	static void SetOrigin( Parser&      parser,
	                       Scene&       scene,
	                       const char*  begin,
	                       const char*  end )
	{
		double x, y, z;
		
		int scanned = std::sscanf( begin, "%lf %lf %lf", &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.itsOrigin = V::Point3D::Make( x, y, z );
		}
	}
	
	static void Translate( Parser&      parser,
	                       Scene&       scene,
	                       const char*  begin,
	                       const char*  end )
	{
		double x, y, z;
		
		int scanned = std::sscanf( begin, "%lf %lf %lf", &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.itsOrigin += V::Vector3D::Make( x, y, z );
		}
	}
	
	static void SetTheta( Parser&      parser,
	                      Scene&       scene,
	                      const char*  begin,
	                      const char*  end )
	{
		double theta;
		
		int scanned = std::sscanf( begin, "%lf", &theta );
		
		if ( scanned == 1 )
		{
			parser.itsTheta = V::Degrees( theta );
		}
	}
	
	static void SetPhi( Parser&      parser,
	                    Scene&       scene,
	                    const char*  begin,
	                    const char*  end )
	{
		double phi;
		
		int scanned = std::sscanf( begin, "%lf", &phi );
		
		if ( scanned == 1 )
		{
			parser.itsPhi = V::Degrees( phi );
		}
	}
	
	static void AddMeshPoint( Parser&      parser,
	                          Scene&       scene,
	                          const char*  begin,
	                          const char*  end )
	{
		const char* space = std::find( begin, end, ' ' );
		
		std::string name( begin, space );
		
		begin = space;
		
		while ( *begin == ' ' )
		{
			++begin;
		}
		
		double x, y, z;
		
		int scanned = std::sscanf( begin, "%lf %lf %lf", &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.itsPoints[ name ] = parser.itsOrigin + V::Vector3D::Make( x, y, z );
		}
	}
	
	static void AddMeshPoly( Parser&      parser,
	                         Scene&       scene,
	                         const char*  begin,
	                         const char*  end )
	{
		std::vector< unsigned > offsets;
		
		Context& context = scene.GetContext( parser.itsContextID );
		
		while ( begin < end )
		{
			const char* space = std::find( begin, end, ' ' );
			
			std::string ptName( begin, space );
			
			V::Point3D::Type pt = parser.itsPoints[ ptName ];
			
			offsets.push_back( context.AddPointToMesh( pt ) );
			
			begin = space;
			
			while ( *begin == ' ' )
			{
				++begin;
			}
		}
		
		if ( !offsets.empty() )
		{
			context.AddMeshPoly( offsets, parser.itsColor );
		}
	}
	
	typedef void ( *Handler )( Parser&, Scene&, const char*, const char* );
	
	static std::map< std::string, Handler > MakeHandlers()
	{
		std::map< std::string, Handler > handlers;
		
		handlers[ "camera"    ] = MakeCamera;
		handlers[ "context"   ] = SetContext;
		handlers[ "color"     ] = SetColor;
		handlers[ "origin"    ] = SetOrigin;
		handlers[ "translate" ] = Translate;
		handlers[ "theta"     ] = SetTheta;
		handlers[ "phi"       ] = SetPhi;
		handlers[ "pt"        ] = AddMeshPoint;
		handlers[ "poly"      ] = AddMeshPoly;
		
		return handlers;
	}
	
	static Handler GetHandler( const std::string& command )
	{
		typedef std::map< std::string, Handler > Handlers;
		
		static Handlers handlers = MakeHandlers();
		
		Handlers::const_iterator it = handlers.find( command );
		
		if ( it == handlers.end() )  return NULL;
		
		return it->second;
	}
	
	Parser::Parser() : itsOrigin( V::Point3D::Make( 0, 0, 0 ) ),
	                   itsTheta    ( 0 ),
	                   itsPhi      ( 0 ),
	                   itsContextID( 0 )
	{
	}
	
	void Parser::ParseLine( Scene& scene, const std::string& line )
	{
		std::size_t iCmdStart = line.find_first_not_of( " \t" );
		
		if ( iCmdStart == std::string::npos )
		{
			return;
		}
		
		const char* end = &*line.end();
		
		const char* start = line.c_str() + iCmdStart;
		
		const char* stop = std::find( start, end, ' ' );
		
		std::string cmdname( start, stop );
		
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
			
			handler( *this, scene, start, end );
		}
	}
	
	void Window::Load( const FSSpec& file )
	{
		N::SetWTitle( Get(), file.name );
		
		Io::TextInputAdapter< NN::Owned< N::FSFileRefNum > > input( io::open_for_reading( file ) );
		
		Parser parser;
		std::list< Parser > savedParsers;
		
		while ( input.Ready() )
		{
			std::string line = input.Read();
			
			if ( std::strchr( line.c_str(), '{' ) )
			{
				savedParsers.push_back( parser );
			}
			else if ( std::strchr( line.c_str(), '}' ) )
			{
				parser = savedParsers.back();
				savedParsers.pop_back();
			}
			else
			{
				parser.ParseLine( ItsScene(), line );
			}
		}
	}
	
	void Window::Store()
	{

	}
	
}

