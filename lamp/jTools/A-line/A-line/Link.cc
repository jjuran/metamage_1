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

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

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
	namespace NX = NitrogenExtras;
	
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
	
	static std::string MakeImport( const FSSpec& file )
	{
		std::string result;
		
		FSSpec import = file;
		
		if ( import.name[ 1 ] == '-' )
		{
			std::copy( &import.name[ 2 ],
			           &import.name[ 1 ] + import.name[ 0 ],
			           &import.name[ 1 ] );
			
			--import.name[ 0 ];
			
			result = "-wi ";
		}
		else if ( LibNeedsWeakImport( NN::Convert< std::string >( import.name ) ) )
		{
			result = "-wi ";
		}
		
		result += q( N::FSpGetMacPathname( import ) );
		
		return result;
	}
	
	static std::string MakeEchoedIncludedPOSIXPathname( const FSSpec& file )
	{
		std::string include = "include ";
		std::string echo    = "echo ";
		
		return echo + q( include + qq( N::FSpGetPOSIXPathname( file ) ) + ";" );
	}
	
	static std::string MakeFramework( const std::string& framework )
	{
		return std::string( "-framework " ) + framework;
	}
	
	static N::FSDirSpec InterfacesAndLibraries()
	{
		const N::OSType sigMPWShell = N::OSType( 'MPS ' );
		
		FSSpec mpwShell = NX::DTGetAPPL( sigMPWShell );
		N::FSDirSpec mpw = N::FSpGetParent( mpwShell );
		
		return N::FSpGetParent( NN::Convert< FSSpec >( mpw ) ) << "Interfaces&Libraries";
	}
	
	static std::string GetFileName( const FSSpec& file )
	{
		return NN::Convert< std::string >( file.name );
	}
	
	static std::string Pathname( const FSSpec& file )
	{
		return N::FSpGetMacPathname( file );
	}
	
	static FSSpec FindImportLibrary( const FileName& filename )
	{
		N::FSDirSpec libsFolder = InterfacesAndLibraries() << "Libraries";
		
		N::FSDirSpec sharedLibs = libsFolder << "SharedLibraries";
		N::FSDirSpec ppcLibs    = libsFolder << "PPCLibraries";
		N::FSDirSpec libs68K    = libsFolder << "Libraries";
		N::FSDirSpec mwppcLibs  = libsFolder << "MWPPCLibraries";
		N::FSDirSpec mw68kLibs  = libsFolder << "MW68KLibraries";
		
		std::string actualName = filename;
		
		// This is a hack.  A '-' prefix means to weak-import the library.
		if ( filename[ 0 ] == '-' )
		{
			actualName = filename.substr( 1, std::string::npos );
		}
		
		if ( N::FSpTestItemExists( sharedLibs & actualName ) )
		{
			return sharedLibs & filename;
		}
		else if ( N::FSpTestItemExists( ppcLibs & actualName ) )
		{
			return ppcLibs & filename;
		}
		else if ( N::FSpTestItemExists( libs68K & actualName ) )
		{
			return libs68K & filename;
		}
		else if ( N::FSpTestItemExists( mwppcLibs & actualName ) )
		{
			return mwppcLibs & filename;
		}
		else if ( N::FSpTestItemExists( mw68kLibs & actualName ) )
		{
			return mw68kLibs & filename;
		}
		else
		{
			throw N::FNFErr();
		}
		
		// Not reached
		return FSSpec();  // Squelch warning
	}
	
	static FSSpec FindImportProject( const ProjName& name, const std::string& targetName )
	{
		Project* proj = NULL;
		
		try
		{
			proj = &GetProject( name );
		}
		catch ( ... )
		{
			throw N::FNFErr();
		}
		
		Project& project = *proj;
		
		using namespace NN::Operators;
		
		switch ( project.Product() )
		{
			case productNotBuilt:
				// This will be the case for projects that provide a stub library
				// but no source.
				
				// This doesn't compile -- CW says "Output" is an illegal operand.
				//return NN::Convert< FSSpec >( project.ProjectFolder() << "Output" & name );
				
				{
					N::FSDirSpec projectFolder = project.ProjectFolder();
					N::FSDirSpec prebuiltOutput = projectFolder << "Output";
					
					return prebuiltOutput & name;
				}
				break;
			
			case productSharedLib:
				// This will be the case for shared libraries built by A-line.
				return ProjectLibrariesFolder( name, targetName ) & name;
				break;
			
			default:
				// Throw shortly
				break;
		}
		
		throw N::FNFErr();
	}
	
	static FSSpec FindImport( const std::string& name, const std::string& targetName )
	{
		try
		{
			return FindImportProject( name, targetName );
		}
		catch ( N::FNFErr )
		{
			return FindImportLibrary( name );
		}
	}
	
	static void CopyImports( const ProjName& projName,
	                         std::back_insert_iterator< std::vector< ProjName > > inserter )
	{
		Project& project = GetProject( projName );
		
		std::vector< FileName > importNames( project.LibImports() );
		
		std::copy( importNames.begin(),
		           importNames.end(),
		           inserter );
	}
	
	static std::vector< FSSpec > GetAllImports( const Project& project, const std::string& targetName )
	{
		const std::vector< ProjName >& used = project.AllUsedProjects();
		
		std::vector< FileName > importNames;
		
		std::for_each( used.begin(),
		               used.end(),
		               std::bind2nd( N::PtrFun( CopyImports ),
		                             std::back_inserter( importNames ) ) );
		
		std::vector< FSSpec > importFiles( importNames.size() );
		
		std::transform( importNames.begin(),
		                importNames.end(),
		                importFiles.begin(),
		                std::bind2nd( N::PtrFun( FindImport ),
		                              targetName ) );
		
		return importFiles;
	}
	
	static std::string GetImports( const Project& project, const std::string& targetName )
	{
		std::vector< FSSpec > importFiles( GetAllImports( project, targetName ) );
		
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
	
	static void GetProjectLib( const Project& project, std::vector< FSSpec >* const& outUsed )
	{
		using namespace NN::Operators;
		
		try
		{
			bool staticLib = project.Product() == productStaticLib;
			
			FSSpec lib = ( staticLib ? ProjectLibrariesFolder( project.Name(),
			                                                   gTargetName )
			                         : project.ProjectFolder() << "Output" )
			             & project.LibraryFilename();
			
			if ( N::FSpTestItemExists( lib ) )
			{
				outUsed->push_back( lib );
			}
		}
		catch ( ... )
		{
			
		}
	}
	
	static std::vector< FSSpec > GetUsedLibraries( const Project& project, const TargetName& targetName )
	{
		gTargetName = targetName;
		std::vector< FSSpec > usedLibFiles;
		
		const std::vector< ProjName >& usedProjects = project.AllUsedProjects();
		
		std::for_each( usedProjects.begin(),
		               usedProjects.end() - 1,
		               ext::compose1( std::bind2nd( N::PtrFun( GetProjectLib ),
		                                            &usedLibFiles ),
		                              N::PtrFun( GetProject ) ) );
		
		return usedLibFiles;
	}
	
	static N::FSDirSpec DirCreate_Idempotent( const FSSpec& dir )
	{
		if ( !N::FSpTestItemExists( dir ) )
		{
			return N::FSpDirCreate( dir );
		}
		
		return NN::Convert< N::FSDirSpec >( dir );
	}
	
	static void CreateAppBundle( const N::FSDirSpec& location, const std::string& name )
	{
		N::FSDirSpec package   = DirCreate_Idempotent( location & name );
		N::FSDirSpec contents  = DirCreate_Idempotent(   package  & "Contents" );
		N::FSDirSpec macOS     = DirCreate_Idempotent(     contents & "MacOS" );
		N::FSDirSpec resources = DirCreate_Idempotent(     contents & "Resources" );
	}
	
	static std::string paren( const std::string& s )
	{
		return std::string( "(" ) + s + ")";
	}
	
	void LinkProduct( const Project& project, TargetInfo targetInfo )
	{
		const bool gnu = targetInfo.toolkit == toolkitGNU;
		
		typedef std::string (*GetPathnameProcPtr)( const FSSpec& );
		
		GetPathnameProcPtr GetPathname = gnu ? N::FSpGetPOSIXPathname
		                                     : N::FSpGetMacPathname;
		
		CommandGenerator cmdgen( targetInfo );
		
		std::string linkName = project.ProgramName();
		
		if ( linkName == "" )
		{
			linkName = project.Name();
		}
		
		std::string command = cmdgen.LinkerName();
		
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
				command << cmdgen.TargetArchitecture();
				
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
				command << cmdgen.TargetArchitecture();
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
		
		N::FSDirSpec objectsDir = ProjectObjectsFolder  ( project.Name(), targetName );
		N::FSDirSpec libsDir    = ProjectLibrariesFolder( project.Name(), targetName );
		
		using namespace NN::Operators;
		
		FSSpec outFile = libsDir & linkName;
		bool outFileExists = N::FSpTestItemExists( outFile );
		
		MacDate outFileDate = outFileExists ? ModifiedDate( outFile ) : 0;
		
		bool needToLink = !outFileExists;
		
		std::vector< FSSpec > objectFiles;
		
		std::vector< FSSpec >::const_iterator it, end = project.Sources().end();
		
		for ( it = project.Sources().begin();  it != end;  ++it )
		{
			std::string sourceName = NN::Convert< std::string >( it->name );
			
			FSSpec objectFile = objectsDir & ObjectFileName( sourceName );
			
			needToLink =    needToLink
			             || !N::FSpTestFileExists( objectFile )
			             || ModifiedDate( objectFile ) >= outFileDate;
			
			objectFiles.push_back( objectFile );
		}
		
		std::string objectFilePaths = join( objectFiles.begin(),
		                                    objectFiles.end(),
		                                    " ",
		                                    ext::compose1( N::PtrFun( q ),
		                                                   N::PtrFun( GetPathname ) ) );
		
		std::string link;
		
		std::string expeditedLib;
		
		if ( needLibs )
		{
			std::vector< FSSpec > usedLibFiles = GetUsedLibraries( project, targetName );
			
			// As long as needToLink is false, continue checking dates.
			// Stop as soon as we know we have to link (or we run out).
			if ( !needToLink )
			{
				std::vector< FSSpec >::const_iterator found;
				
				found = std::find_if( usedLibFiles.begin(),
				                      usedLibFiles.end(),
				                      ext::compose1( std::bind2nd( std::not2( std::less< MacDate >() ),
				                                                   outFileDate ),
				                                     N::PtrFun( ModifiedDate ) ) );
				
				needToLink = found != usedLibFiles.end();
			}
			
			for ( std::vector< FSSpec >::iterator it = usedLibFiles.begin();  it != usedLibFiles.end();  ++it )
			{
				if ( NN::Convert< std::string >( it->name ) == "Orion.lib" )
				{
					expeditedLib = q( GetPathname( *it ) );
					
					std::copy( it + 1, usedLibFiles.end(), it );
					
					usedLibFiles.resize( usedLibFiles.size() - 1 );
					
					break;
				}
			}
			
			// Link the libs in reverse order, so if foo depends on bar, foo will have precedence.
			// Somehow, this is actually required to actually link anything with Mach-O.
			link << join( usedLibFiles.rbegin(),
			              usedLibFiles.rend(),
			              " ",
			              ext::compose1( N::PtrFun( q ),
			                             N::PtrFun( GetPathname ) ) );
			
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
			N::FSDirSpec contents = libsDir << bundleName << "Contents";
			outFile = contents << "MacOS" & linkName;
			
			const N::OSType codeZero = N::OSType( 0 );
			
			FSSpec pkgInfo = contents & "PkgInfo";
			
			try
			{
				N::FSpCreate( pkgInfo, codeZero, codeZero );
			}
			catch ( ... )  {}
			
			using N::fsWrPerm;
			
			std::string info = std::string( "APPL" ) + project.CreatorCode();
			
			io::write( N::FSpOpenDF( pkgInfo, fsWrPerm ),
			           info.data(),
			           info.size() );
		}
		
		std::string outputPathname = GetPathname( outFile );
		
		command << cmdgen.Output( outputPathname );
		
		command << expeditedLib << objectFilePaths << link;
		
		if ( gnu )
		{
			command << "> /tmp/link-errs.txt 2>&1";
		}
		else
		{
			command << " 2>&1 | filter-mwlink-warnings";
		}
		
		command = cmdgen.MakeNativeCommand( command );
		
		QueueCommand( "echo Linking:  " + NN::Convert< std::string >( outFile.name ) );
		QueueCommand( command );
		
		FSSpec rsrcFile = outFile;
		
		if ( !project.UsedRezFiles().empty() )
		{
			const std::vector< FileName >& rez = project.UsedRezFiles();
			std::vector< FSSpec > rezFiles( rez.size() );
			
			std::transform( rez.begin(),
			                rez.end(),
			                rezFiles.begin(),
			                N::PtrFun( RezLocation ) );
			
			if ( needCarbResource )
			{
				rezFiles.push_back( RezLocation( "CarbonApp.r" ) );
			}
			
			FSSpec includeDir = NN::Convert< FSSpec >( ProjectIncludesFolder( project.ProjectFolder() ) );
			std::string rezCommand = "Rez -append -d __ALINE_REZ_MPW__";
			
			if ( gnu )
			{
				std::string bundleName = linkName + ".app";
				std::string rsrcFileName = linkName + ".rsrc";
				
				rsrcFile = libsDir << bundleName
				                   << "Contents"
				                   << "Resources"
				           & rsrcFileName;
				
				//rezCommand = "/Developer/Tools/Rez -append -d __ALINE_REZ_OSX__ -useDF";
				rezCommand = "/Developer/Tools/Rez -append -i /Developer/Headers/FlatCarbon -useDF";
				
			}
			
			rezCommand << "-i " + q( GetPathname( includeDir ) );
			rezCommand << cmdgen.Output( GetPathname( rsrcFile ) );
			
			rezCommand << join( rezFiles.begin(),
			                    rezFiles.end(),
			                    " ",
			                    ext::compose1( N::PtrFun( q ),
			                                   N::PtrFun( GetPathname ) ) );
			
			rezCommand = cmdgen.MakeNativeCommand( rezCommand );
			
			QueueCommand( "echo Rezzing:  " + NN::Convert< std::string >( rsrcFile.name ) );
			QueueCommand( rezCommand );
		}
		
		if ( !project.UsedRsrcFiles().empty() )
		{
			const std::vector< FileName >& rsrcs = project.UsedRsrcFiles();
			std::vector< FSSpec > rsrcFiles( rsrcs.size() );
			
			std::transform( rsrcs.begin(), 
			                rsrcs.end(),
			                rsrcFiles.begin(),
			                N::PtrFun( RezLocation ) );
			
			std::string cpresCommand = "cpres";
			
			cpresCommand << join( rsrcFiles.begin(),
			                      rsrcFiles.end(),
			                      " ",
			                      ext::compose1( N::PtrFun( q ),
			                                     N::PtrFun( N::FSpGetPOSIXPathname ) ) );
			
			
			if ( gnu )
			{
				std::string echoes = join( rsrcFiles.begin(),
				                           rsrcFiles.end(),
				                           "; ",
				                           N::PtrFun( MakeEchoedIncludedPOSIXPathname ) );
				
				cpresCommand = paren( echoes ) + " | /Developer/Tools/Rez -append -useDF -o";
			}
			
			cpresCommand << q( N::FSpGetPOSIXPathname( rsrcFile ) );
			
			QueueCommand( "echo Copying resources:  " + NN::Convert< std::string >( rsrcFile.name ) );
			QueueCommand( cpresCommand );
		}
	}
	
}

