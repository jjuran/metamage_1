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
			Parser();
			~Parser()  {}
			
			void ParseLine( Model& model, const std::string& line );
			
			ColorMatrix myColor;
			V::Point3D::Type myOrigin;
			V::Degrees myTheta;
			V::Degrees myPhi;
			std::size_t myContext;
			std::map< std::string, V::Point3D::Type > myPoints;
	};
	
	
	static void SetContext( Parser& parser, Model& model, const std::string& commandLine )
	{
		std::string contextName = commandLine.substr( 8, std::string::npos );
		
		parser.myContext = model.AddSubcontext( parser.myContext,
		                                        contextName,
		                                        MakeTranslation(  parser.myOrigin ).Make(),
		                                        MakeTranslation( -parser.myOrigin ).Make() );
		
		parser.myOrigin = V::Point3D::Make( 0, 0, 0 );
	}
	
	static void MakeCamera( Parser& parser, Model& model, const std::string& commandLine )
	{
		model.Cameras().push_back( Camera( parser.myContext ) );
	}
	
	static void SetColor( Parser& parser, Model& model, const std::string& commandLine )
	{
		const char* str = commandLine.c_str();
		str += 5;
		double red, green, blue;
		int scanned = std::sscanf( str, "%lf %lf %lf", &red, &green, &blue );
		
		if ( scanned == 3 )
		{
			parser.myColor = V::MakeRGB( red, green, blue );
		}
	}
	
	static void SetOrigin( Parser& parser, Model& model, const std::string& commandLine )
	{
		const char* str = commandLine.c_str();
		str += commandLine.find( ' ' );
		double x, y, z;
		int scanned = std::sscanf( str, "%lf %lf %lf", &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.myOrigin = V::Point3D::Make( x, y, z );
		}
	}
	
	static void Translate( Parser& parser, Model& model, const std::string& commandLine )
	{
		const char* str = commandLine.c_str();
		str += commandLine.find( ' ' );
		double x, y, z;
		int scanned = std::sscanf( str, "%lf %lf %lf", &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.myOrigin += V::Vector3D::Make( x, y, z );
		}
	}
	
	static void SetTheta( Parser& parser, Model& model, const std::string& commandLine )
	{
		const char* str = commandLine.c_str();
		str += commandLine.find( ' ' );
		double theta;
		int scanned = std::sscanf( str, "%lf", &theta );
		
		if ( scanned == 1 )
		{
			parser.myTheta = V::Degrees( theta );
		}
	}
	
	static void SetPhi( Parser& parser, Model& model, const std::string& commandLine )
	{
		const char* str = commandLine.c_str();
		str += commandLine.find( ' ' );
		double phi;
		int scanned = std::sscanf( str, "%lf", &phi );
		
		if ( scanned == 1 )
		{
			parser.myPhi = V::Degrees( phi );
		}
	}
	
	static void AddMeshPoint( Parser& parser, Model& model, const std::string& commandLine )
	{
		std::string::size_type firstSpace = commandLine.find( ' ' );
		std::string::size_type firstArg = firstSpace + 1;
		std::string::size_type secondSpace = commandLine.find( ' ', firstArg );
		std::string name = commandLine.substr( firstArg, secondSpace - firstArg );
		const char* str = commandLine.c_str();
		str += secondSpace;
		double x, y, z;
		int scanned = std::sscanf( str, "%lf %lf %lf", &x, &y, &z );
		
		if ( scanned == 3 )
		{
			parser.myPoints[ name ] = parser.myOrigin + V::Vector3D::Make( x, y, z );
		}
	}
	
	static void AddMeshPoly( Parser& parser, Model& model, const std::string& commandLine )
	{
		std::size_t space = commandLine.find( ' ' );
		if ( space == std::string::npos )  return;
		
		std::size_t arg = space + 1;
		
		std::vector< unsigned > offsets;
		Context& context = model.GetContext( parser.myContext );
		
		while ( arg < std::string::npos )
		{
			space = commandLine.find( ' ', arg );
			std::string ptName = commandLine.substr( arg, space - arg );
			V::Point3D::Type pt = parser.myPoints[ ptName ];
			offsets.push_back( context.AddPointToMesh( pt ) );
			arg = ( space == std::string::npos ) ? space : space + 1;
		}
		
		if ( !offsets.empty() )
		{
			context.AddMeshPoly( offsets, parser.myColor );
		}
	}
	
	typedef void ( *Handler )( Parser&, Model&, const std::string& );
	
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
	
	Parser::Parser()
	: 
		myOrigin( V::Point3D::Make( 0, 0, 0 ) ),
		myTheta( 0 ),
		myPhi( 0 ),
		myContext( 0 )
	{
	}
	
	void Parser::ParseLine( Model& model, const std::string& line )
	{
		std::string::size_type iCmdStart = line.find_first_not_of( " \t" );
		
		if ( iCmdStart == std::string::npos )
		{
			iCmdStart = 0;
		}
		
		std::string::size_type iCmdEnd = line.find( ' ', iCmdStart );
		std::string cmdname = line.substr( iCmdStart, iCmdEnd - iCmdStart );
		
		if ( Handler handler = GetHandler( cmdname ) )
		{
			handler( *this, model, line.substr( iCmdStart, line.npos ) );
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
			
			if ( line.find( '{' ) != std::string::npos )
			{
				savedParsers.push_back( parser );
			}
			else if ( line.find( '}' ) != std::string::npos )
			{
				parser = savedParsers.back();
				savedParsers.pop_back();
			}
			else
			{
				parser.ParseLine( ItsModel(), line );
			}
		}
	}
	
	void Window::Store()
	{

	}
	
}

