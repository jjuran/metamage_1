/*	=======
 *	Link.cc
 *	=======
 */

#include "A-line/Link.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// Io
#include "io/io.hh"
#include "io/spray.hh"

// POSeven
#include "POSeven/Open.hh"
#include "POSeven/Pathnames.hh"

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"

// GetPathname
#include "GetPathname.hh"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// BitsAndBytes
#include "StringFilters.hh"

// Orion
#include "Orion/StandardIO.hh"

// A-line
#include "A-line/A-line.hh"
#include "A-line/BuildCommon.hh"
#include "A-line/Commands.hh"
#include "A-line/Locations.hh"
#include "A-line/MWScripter.hh"
#include "A-line/ProjectCommon.hh"


namespace ALine
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	using namespace io::path_descent_operators;
	
	using BitsAndBytes::q;
	using BitsAndBytes::qq;
	
	
	static bool LibNeedsWeakImport( const std::string& lib )
	{
		if ( lib == "InterfaceLib"    )  return true;
		if ( lib == "ControlsLib"     )  return true;
		if ( lib == "MenusLib"        )  return true;
		if ( lib == "WindowsLib"      )  return true;
		if ( lib == "ControlStripLib" )  return true;
		
		return false;
	}
	
	static std::string MakeImport( const std::string& file )
	{
		std::string result;
		
		std::string importName = io::get_filename_string( file );
		
		if ( importName[ 0 ] == '-' )
		{
			importName = importName.substr( 1, importName.npos );
			
			result = "-wi ";
		}
		else if ( LibNeedsWeakImport( importName ) )
		{
			result = "-wi ";
		}
		
		result += q( io::get_preceding_directory( file ) / importName );
		
		return result;
	}
	
	static std::string MakeEchoedIncludedPOSIXPathname( const std::string& file )
	{
		std::string include = "include ";
		std::string echo    = "echo ";
		
		return echo + q( include + qq( file ) + ";" );
	}
	
	static std::string MakeFramework( const std::string& framework )
	{
		return std::string( "-framework " ) + framework;
	}
	
	static N::FSDirSpec InterfacesAndLibraries()
	{
		const N::OSType sigMPWShell = N::OSType( 'MPS ' );
		
		FSSpec mpwShell = N::DTGetAPPL( sigMPWShell );
		
		N::FSDirSpec mpw  = io::get_preceding_directory( mpwShell );
		N::FSDirSpec apps = io::get_preceding_directory( mpw      );
		
		N::FSDirSpec intfsAndLibs( apps / "Interfaces&Libraries" );
		
		return intfsAndLibs;
	}
	
	static FSSpec FindImportLibraryInSystem( const FileName& filename )
	{
		N::FSDirSpec libsFolder( InterfacesAndLibraries() / "Libraries" );
		
		N::FSDirSpec sharedLibs( libsFolder / "SharedLibraries" );
		N::FSDirSpec ppcLibs   ( libsFolder / "PPCLibraries"    );
		N::FSDirSpec libs68K   ( libsFolder / "Libraries"       );
		N::FSDirSpec mwppcLibs ( libsFolder / "MWPPCLibraries"  );
		N::FSDirSpec mw68kLibs ( libsFolder / "MW68KLibraries"  );
		
		std::string actualName = filename;
		
		// This is a hack.  A '-' prefix means to weak-import the library.
		if ( filename[ 0 ] == '-' )
		{
			actualName = filename.substr( 1, std::string::npos );
		}
		
		if ( io::item_exists( sharedLibs / actualName ) )
		{
			return sharedLibs / filename;
		}
		else if ( io::item_exists( ppcLibs / actualName ) )
		{
			return ppcLibs / filename;
		}
		else if ( io::item_exists( libs68K / actualName ) )
		{
			return libs68K / filename;
		}
		else if ( io::item_exists( mwppcLibs / actualName ) )
		{
			return mwppcLibs / filename;
		}
		else if ( io::item_exists( mw68kLibs / actualName ) )
		{
			return mw68kLibs / filename;
		}
		else
		{
			throw N::FNFErr();
		}
		
		// Not reached
		return FSSpec();  // Squelch warning
	}
	
	
	static std::string FindImportLibraryInProject( const std::string& libName, const Project& project )
	{
		std::string lib = project.ProjectFolder() / libName;
		
		if ( !io::item_exists( lib ) )
		{
			throw N::FNFErr();
		}
		
		return lib;
	}
	
	static std::string FindImport( const std::string& name, const Project& project )
	{
		try
		{
			return GetPOSIXPathname( FindImportLibraryInSystem( name ) );
		}
		catch ( const N::FNFErr& )
		{
		}
		
		try
		{
			return FindImportLibraryInProject( name, project );
		}
		catch ( const N::FNFErr& )
		{
		}
		
		std::fprintf( stderr, "### Missing import %s from project %s\n", name.c_str(), project.Name().c_str() );
		
		throw N::FNFErr();
	}
	
	static void CopyImports( const ProjName& projName,
	                         std::back_insert_iterator< std::vector< std::string > > inserter )
	{
		Project& project = GetProject( projName );
		
		std::vector< FileName > importNames( project.LibImports() );
		
		std::vector< std::string > importFiles( importNames.size() );
		
		std::transform( importNames.begin(),
		                importNames.end(),
		                importFiles.begin(),
		                std::bind2nd( N::PtrFun( FindImport ),
		                              project ) );
		
		std::copy( importFiles.begin(),
		           importFiles.end(),
		           inserter );
	}
	
	static std::vector< std::string > GetAllImports( const Project& project, const std::string& targetName )
	{
		const std::vector< ProjName >& used = project.AllUsedProjects();
		
		std::vector< std::string > importFiles;
		
		std::for_each( used.begin(),
		               used.end(),
		               std::bind2nd( N::PtrFun( CopyImports ),
		                             std::back_inserter( importFiles ) ) );
		
		return importFiles;
	}
	
	static std::string GetImports( const Project& project, const std::string& targetName )
	{
		std::vector< std::string > importFiles( GetAllImports( project, targetName ) );
		
		std::string imports;
		
		imports << join( importFiles.begin(),
		                 importFiles.end(),
		                 " ",
		                 N::PtrFun( MakeImport ) );
		
		return imports;
	}
	
	static std::string GetFrameworks( const Project& project )
	{
		std::vector< FileName > frameworkNames( project.Frameworks() );
		
		if ( frameworkNames.size() == 0 )
		{
			frameworkNames.push_back( "Carbon" );
		}
		
		std::string frameworks = join( frameworkNames.begin(),
		                               frameworkNames.end(),
		                               " ",
		                               N::PtrFun( MakeFramework ) );
		
		return frameworks;
	}
	
	// FIXME:  Prebuilt libraries do not work for multiple targets.
	// This needs to be a function of target, not project.
	static TargetName gTargetName;
	
	static void GetProjectLib( const Project& project, std::vector< std::string >* const& outUsed )
	{
		if ( project.Product() == productStaticLib )
		{
			std::string libOutput = ProjectLibrariesDirPath( project.Name(), gTargetName );
			
			std::string lib = libOutput / project.LibraryFilename();
			
			if ( io::item_exists( lib ) )
			{
				outUsed->push_back( lib );
			}
		}
	}
	
	static std::vector< std::string > GetUsedLibraries( const Project& project, const TargetName& targetName )
	{
		gTargetName = targetName;
		std::vector< std::string > usedLibFiles;
		
		const std::vector< ProjName >& usedProjects = project.AllUsedProjects();
		
		std::for_each( usedProjects.begin(),
		               usedProjects.end() - 1,
		               ext::compose1( std::bind2nd( N::PtrFun( GetProjectLib ),
		                                            &usedLibFiles ),
		                              N::PtrFun( GetProject ) ) );
		
		return usedLibFiles;
	}
	
	static std::string DirCreate_Idempotent( const std::string& dir )
	{
		if ( !io::item_exists( dir ) )
		{
			int made = mkdir( dir.c_str(), 0700 );
		}
		
		return dir;
	}
	
	static void CreateAppBundle( const std::string& location, const std::string& name )
	{
		std::string package   = DirCreate_Idempotent( location / name );
		std::string contents  = DirCreate_Idempotent(   package  / "Contents" );
		std::string macOS     = DirCreate_Idempotent(     contents / "MacOS" );
		std::string resources = DirCreate_Idempotent(     contents / "Resources" );
	}
	
	static std::string paren( const std::string& s )
	{
		return std::string( "(" ) + s + ")";
	}
	
	void LinkProduct( const Project& project, TargetInfo targetInfo )
	{
		const bool gnu = targetInfo.toolkit == toolkitGNU;
		
		CommandGenerator cmdgen( targetInfo );
		
		std::string linkName = project.ProgramName();
		
		if ( linkName == "" )
		{
			linkName = project.Name();
		}
		
		std::string command = cmdgen.LinkerName();
		
		command << cmdgen.TargetArchitecture();
		
		bool needLibs = true;
		bool needCarbResource = false;
		bool gccSupported = false;
		bool bundle = false;
		
		switch ( project.Product() )
		{
			case productStaticLib:
				command = cmdgen.LibraryMakerName();
				linkName = project.LibraryFilename();
				needLibs = false;
				gccSupported = true;
				break;
			
			case productApplication:
				command << cmdgen.TargetApplication();
				
				needCarbResource =    targetInfo.platform.api     == CD::apiMacCarbon
				                   && targetInfo.platform.runtime == CD::runtimeCodeFragments;
				gccSupported = true;
				bundle = gnu;
				break;
			
			case productINIT:
				command << cmdgen.MWTargetCodeResource()
				        << cmdgen.ResourceTypeAndID( "INIT", "0" )
				        << cmdgen.OutputType( "INIT" );
				break;
			
			case productDriver:
				command << cmdgen.MWTargetCodeResource()
				        << cmdgen.ResourceTypeAndID( "DRVR", "0" )
				        << cmdgen.ResourceName( "." + project.Name() )
				        << cmdgen.OutputType   ( "INIT" )
				        << cmdgen.OutputCreator( "RSED" )
				        << cmdgen.CustomDriverHeader();
				break;
			
			case productWish:
				gccSupported = true;
				command << cmdgen.TargetCommandLineTool();
				break;
			
			case productSharedLib:
				command << cmdgen.MWTargetSharedLibrary();
				break;
		}
		
		if ( gnu  &&  !gccSupported )
		{
			Io::Err << "Sorry, GCC is not supported for this type of product.\n";
			throw N::ParamErr();
		}
		
		command << cmdgen.LinkerOptions();
		
		TargetName targetName = MakeTargetName( targetInfo );
		
		std::string objectsDir = ProjectObjectsDirPath  ( project.Name(), targetName );
		std::string libsDir    = ProjectLibrariesDirPath( project.Name(), targetName );
		
		std::string outFile = libsDir / linkName;
		bool outFileExists = io::item_exists( outFile );
		
		time_t outFileDate = outFileExists ? ModifiedDate( outFile ) : 0;
		
		bool needToLink = !outFileExists;
		
		std::vector< std::string > objectFiles;
		
		std::vector< std::string >::const_iterator it, end = project.Sources().end();
		
		for ( it = project.Sources().begin();  it != end;  ++it )
		{
			std::string sourceName = io::get_filename_string( *it );
			
			std::string objectFile = objectsDir / ObjectFileName( sourceName );
			
			needToLink =    needToLink
			             || !io::file_exists( objectFile )
			             || ModifiedDate( objectFile ) >= outFileDate;
			
			objectFiles.push_back( objectFile );
		}
		
		std::string objectFilePaths = join( objectFiles.begin(),
		                                    objectFiles.end(),
		                                    " ",
		                                    N::PtrFun( q ) );
		
		std::string link;
		
		std::string expeditedLib;
		
		if ( needLibs )
		{
			std::vector< std::string > usedLibFiles = GetUsedLibraries( project, targetName );
			
			// As long as needToLink is false, continue checking dates.
			// Stop as soon as we know we have to link (or we run out).
			if ( !needToLink )
			{
				std::vector< std::string >::const_iterator found;
				
				found = std::find_if( usedLibFiles.begin(),
				                      usedLibFiles.end(),
				                      ext::compose1( std::bind2nd( std::not2( std::less< time_t >() ),
				                                                   outFileDate ),
				                                     N::PtrFun( static_cast< time_t (*)(const std::string&) >( ModifiedDate ) ) ) );
				
				needToLink = found != usedLibFiles.end();
			}
			
			if ( !gnu )
			{
				for ( std::vector< std::string >::iterator it = usedLibFiles.begin();  it != usedLibFiles.end();  ++it )
				{
					if ( io::get_filename_string( *it ) == "Orion.lib" )
					{
						expeditedLib = q( *it );
						
						std::copy( it + 1, usedLibFiles.end(), it );
						
						usedLibFiles.resize( usedLibFiles.size() - 1 );
						
						break;
					}
				}
			}
			
			// Link the libs in reverse order, so if foo depends on bar, foo will have precedence.
			// Somehow, this is actually required to actually link anything with Mach-O.
			link << join( usedLibFiles.rbegin(),
			              usedLibFiles.rend(),
			              " ",
			              N::PtrFun( q ) );
			
			// FIXME:  This is a hack
			if ( !gnu )
			{
				link << GetImports( project, targetName );
			}
		}
		
		if ( !needToLink )  return;
		
		
		if ( !gnu && project.CreatorCode().size() > 0 )
		{
			command << cmdgen.OutputCreator( project.CreatorCode() );
		}
		
		
		if ( needLibs && gnu )
		{
			link << GetFrameworks( project );
		}
		
		if ( bundle )
		{
			std::string bundleName = linkName + ".app";
			
			CreateAppBundle( libsDir, bundleName );
			
			std::string contents( libsDir / bundleName / "Contents" );
			
			outFile = contents / "MacOS" / linkName;
			
			const N::OSType codeZero = N::OSType( 0 );
			
			std::string pkgInfo = contents / "PkgInfo";
			
			try
			{
				//N::FSpCreate( pkgInfo, codeZero, codeZero );
				P7::Open( pkgInfo.c_str(), O_CREAT, 0600 );
			}
			catch ( ... )  {}
			
			std::string info = "APPL" + project.CreatorCode();
			
			io::spray_file< NN::StringFlattener< std::string > >( pkgInfo, info );
		}
		
		command << cmdgen.Output( outFile );
		
		command << expeditedLib << objectFilePaths << link;
		
		if ( gnu )
		{
			command << "> /tmp/link-errs.txt 2>&1";
		}
		else
		{
			command << " 2>&1 | filter-mwlink-warnings";
		}
		
		QueueCommand( "echo Linking:  " + io::get_filename_string( outFile ) );
		QueueCommand( command );
		
		std::string rsrcFile = outFile;
		
		if ( !project.UsedRezFiles().empty() )
		{
			const std::vector< FileName >& rez = project.UsedRezFiles();
			std::vector< std::string > rezFiles( rez.size() );
			
			std::transform( rez.begin(),
			                rez.end(),
			                rezFiles.begin(),
			                N::PtrFun( RezLocation ) );
			
			if ( needCarbResource )
			{
				rezFiles.push_back( RezLocation( "CarbonApp.r" ) );
			}
			
			std::string includeDir = ProjectIncludesPath( project.ProjectFolder() );
			std::string rezCommand = "mpwrez -append";
			
			if ( gnu )
			{
				std::string bundleName = linkName + ".app";
				std::string rsrcFileName = linkName + ".rsrc";
				
				rsrcFile = libsDir / bundleName / "Contents" / "Resources" / rsrcFileName;
				
				rezCommand = "/Developer/Tools/Rez -append -i /Developer/Headers/FlatCarbon -useDF";
				
			}
			
			rezCommand << "-i " + q( includeDir );
			rezCommand << cmdgen.Output( rsrcFile );
			
			rezCommand << join( rezFiles.begin(),
			                    rezFiles.end(),
			                    " ",
			                    N::PtrFun( q ) );
			
			QueueCommand( "echo Rezzing:  " + io::get_filename_string( rsrcFile ) );
			QueueCommand( rezCommand );
		}
		
		if ( !project.UsedRsrcFiles().empty() )
		{
			const std::vector< FileName >& rsrcs = project.UsedRsrcFiles();
			std::vector< std::string > rsrcFiles( rsrcs.size() );
			
			std::transform( rsrcs.begin(), 
			                rsrcs.end(),
			                rsrcFiles.begin(),
			                N::PtrFun( RezLocation ) );
			
			std::string cpresCommand = "cpres";
			
			cpresCommand << join( rsrcFiles.begin(),
			                      rsrcFiles.end(),
			                      " ",
			                      N::PtrFun( q ) );
			
			
			if ( gnu )
			{
				std::string echoes = join( rsrcFiles.begin(),
				                           rsrcFiles.end(),
				                           "; ",
				                           N::PtrFun( MakeEchoedIncludedPOSIXPathname ) );
				
				cpresCommand = paren( echoes ) + " | /Developer/Tools/Rez -append -useDF -o";
			}
			
			cpresCommand << q( rsrcFile );
			
			QueueCommand( "echo Copying resources:  " + io::get_filename_string( rsrcFile ) );
			QueueCommand( cpresCommand );
		}
	}
	
}

