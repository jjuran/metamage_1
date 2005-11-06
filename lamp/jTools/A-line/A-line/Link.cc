/*	=======
 *	Link.cc
 *	=======
 */

#include "A-line/Link.hh"

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// Nitrogen
#include "Nitrogen/MacErrors.h"
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Templates
#include "Templates/PointerToFunction.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Io
#include "Io/Files.hh"

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
	namespace NX = NitrogenExtras;
	
	using BitsAndBytes::q;
	using BitsAndBytes::qq;
	
	
	static std::string ShellScript( const std::string& command )
	{
#ifdef __MACH__
		return command;
#else
		return std::string( "dss " ) + qq( command + " && true" );
#endif
	}
	
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
		else if ( LibNeedsWeakImport( N::Convert< std::string >( import.name ) ) )
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
		FSSpec mpwShell = NX::DTGetAPPL( 'MPS ' );
		N::FSDirSpec mpw = N::FSpGetParent( mpwShell );
		
		return N::FSpGetParent( N::Convert< FSSpec >( mpw ) ) << "Interfaces&Libraries";
	}
	
	static std::string GetFileName( const FSSpec& file )
	{
		return N::Convert< std::string >( file.name );
	}
	
	static std::string Pathname( const FSSpec& file )
	{
		return N::FSpGetMacPathname( file );
	}
	
	static std::string MacSystemLibs( TargetInfo targetInfo, bool extraLibs )
	{
		bool custom = false;  // FIXME
		bool usesInterfaceLib =  ( targetInfo.platform.runtime == CD::runtimeCodeFragments )
		                      && ( targetInfo.platform.api     == CD::apiMacToolbox );
		
		N::FSDirSpec libsFolder = InterfacesAndLibraries() << "Libraries";
		N::FSDirSpec sharedLibs = libsFolder               << "SharedLibraries";
		
		std::string systemLibs;
		
		if ( targetInfo.platform.arch == CD::arch68K )
		{
			N::FSDirSpec mw68kLibs  = libsFolder << "MW68KLibraries";
			N::FSDirSpec mpw68kLibs = libsFolder << "Libraries";
			
			/*
			systemLibs << "-l" << q( N::FSpGetMacPathname( libsFolder & "MW68KLibraries" ) );
			systemLibs << "-l" << q( N::FSpGetMacPathname( libsFolder & "Libraries" ) );
			*/
			
			if ( custom )
			{
				systemLibs << q( Pathname( mw68kLibs & "MSL CustomCRStartup.Lib" ) );
			}
			else
			{
				if ( targetInfo.platform.runtime == CD::runtimeCodeFragments )
				{
					systemLibs << q( Pathname( mw68kLibs & "MSL C.CFM68K Fa(4i_8d).Lib"   ) )
					           << q( Pathname( mw68kLibs & "MSL C++.CFM68k Fa(4i_8d).Lib" ) )
					           << q( Pathname( mw68kLibs & "MSL MWCFM68KRuntime.Lib"      ) )
					           << q( Pathname( mw68kLibs & "MathLibCFM68K (4i_8d).Lib"    ) )
					           << q( Pathname( mw68kLibs & "PLStringFuncsCFM68K.lib"      ) )
					           << q( Pathname( sharedLibs & "InterfaceLib"                ) )
					           << q( Pathname( sharedLibs & "ThreadsLib"                  ) );
				}
				else
				{
					if ( targetInfo.platform.runtime == CD::runtimeA4CodeResource )
					{
						systemLibs << q( Pathname( mw68kLibs & "MSL C.68K (4i_8d).A4.Lib"     ) )
						           << q( Pathname( mw68kLibs & "MSL C++.68K Fa(4i_8d).A4.Lib" ) )
						           << q( Pathname( mw68kLibs & "MSL Runtime68K.A4.Lib"        ) );
					}
					else
					{
						systemLibs << q( Pathname( mw68kLibs & "MSL C.68K (4i_8d).Lib"     ) )
						           << q( Pathname( mw68kLibs & "MSL C++.68K Fa(4i_8d).Lib" ) )
						           << q( Pathname( mw68kLibs & "MSL Runtime68K.Lib"        ) )
						           << q( Pathname( mw68kLibs & "MathLib68K (4i_8d).Lib"    ) );
					}
					
					systemLibs << q( Pathname( mw68kLibs & "MacOS.lib" ) );
				}
			}
			
		}
		else
		{
			N::FSDirSpec mwppcLibs = libsFolder << "MWPPCLibraries";
			N::FSDirSpec ppcLibs   = libsFolder << "PPCLibraries";
			
			/*
			systemLibs << "-l" << q( N::FSpGetMacPathname( libsFolder & "MWPPCLibraries"  ) );
			systemLibs << "-l" << q( N::FSpGetMacPathname( libsFolder & "PPCLibraries"    ) );
			systemLibs << "-l" << q( N::FSpGetMacPathname( libsFolder & "SharedLibraries" ) );
			*/
			
			if ( targetInfo.platform.api == CD::apiMacCarbon )
			{
				systemLibs << q( Pathname( mwppcLibs  & "MSL C.Carbon.Lib" ) )
				           << q( Pathname( sharedLibs & "CarbonLib"        ) );
			}
			else
			{
				systemLibs << q( Pathname( mwppcLibs  & "MSL C.PPC.Lib"            ) )
				           << q( Pathname( mwppcLibs  & "PLStringFuncsPPC.lib"     ) )
				           << q( Pathname( sharedLibs & "MathLib"                  ) )
				           << q( Pathname( sharedLibs & "ThreadsLib"               ) )
				           << q( Pathname( ppcLibs    & "CarbonAccessors.o"        ) )
				           << q( Pathname( ppcLibs    & "PascalPreCarbonUPPGlue.o" ) );
				
				systemLibs << "-wi" << q( Pathname( sharedLibs & "InterfaceLib" ) );
				
				if ( extraLibs )
				{
					systemLibs          << q( Pathname( sharedLibs & "ObjectSupportLib" ) )
					           << "-wi" << q( Pathname( sharedLibs & "ControlsLib"      ) )
					           << "-wi" << q( Pathname( sharedLibs & "WindowsLib"       ) )
					           << "-wi" << q( Pathname( sharedLibs & "MenusLib"         ) );
				}
			}
			
			systemLibs << q( Pathname( mwppcLibs & "MSL C++.PPC.Lib"    ) )
			           << q( Pathname( mwppcLibs & "MSL RuntimePPC.Lib" ) );
		}
		
		return systemLibs;
	}
	
	static FSSpec FindImportLibrary( const FileName& filename )
	{
		N::FSDirSpec libsFolder = InterfacesAndLibraries() << "Libraries";
		
		N::FSDirSpec sharedLibs = libsFolder << "SharedLibraries";
		N::FSDirSpec ppcLibs    = libsFolder << "PPCLibraries";
		N::FSDirSpec libs68K    = libsFolder << "Libraries";
		N::FSDirSpec mwppcLibs  = libsFolder << "MWPPCLibraries";
		
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
		
		using namespace N::Operators;
		
		switch ( project.Product() )
		{
			case productNotBuilt:
				// This will be the case for projects that provide a stub library
				// but no source.
				
				// This doesn't compile -- CW says "Output" is an illegal operand.
				//return N::Convert< FSSpec >( project.ProjectFolder() << "Output" & name );
				
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
	
	static std::vector< FileName > GetAllImports( const Project& project )
	{
		const std::vector< ProjName >& used = project.AllUsedProjects();
		
		std::vector< FileName > result;
		
		std::for_each( used.begin(),
		               used.end(),
		               std::bind2nd( N::PtrFun( CopyImports ),
		                             std::back_inserter( result ) ) );
		
		return result;
	}
	
	static bool LibSubsumedByCarbonLib( const FileName& name )
	{
		std::string lib = name[ 0 ] == '-' ? name.substr( 1, name.npos )
		                                   : name;
		
		if ( lib == "AppleScriptLib"         )  return true;
		if ( lib == "OpenTransportLib"       )  return true;
		if ( lib == "OpenTptInternetLib"     )  return true;
		if ( lib == "OpenTransportAppPPC.o"  )  return true;
		if ( lib == "OpenTransportExtnPPC.o" )  return true;
		
		return false;
	}
	
	static std::string GetImports( const Project& project, const std::string& targetName )
	{
		//std::vector< FileName > importNames( project.LibImports() );
		std::vector< FileName > importNames( GetAllImports( project ) );
		
		std::string carbonTarget = "PPC-CFM-Carbon";
		
		bool carbon = std::equal( carbonTarget.begin(),
		                          carbonTarget.end(),
		                          targetName.begin() );
		
		if ( carbon )
		{ 
			importNames.resize( std::remove_if( importNames.begin(),
			                                    importNames.end(),
			                                    std::ptr_fun( LibSubsumedByCarbonLib ) )
			                    - importNames.begin() );
		}
		
		std::vector< FSSpec   > importFiles( importNames.size()   );
		
		std::transform( importNames.begin(),
		                importNames.end(),
		                importFiles.begin(),
		                std::bind2nd( N::PtrFun( FindImport ),
		                              targetName ) );
		
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
		using namespace N::Operators;
		
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
		
		return N::Convert< N::FSDirSpec >( dir );
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
		
		N::FSDirSpec objectsDir = ProjectObjectsFolder  ( project.Name(), targetName );
		N::FSDirSpec libsDir    = ProjectLibrariesFolder( project.Name(), targetName );
		
		using namespace N::Operators;
		
		FSSpec outFile = libsDir & linkName;
		bool outFileExists = N::FSpTestItemExists( outFile );
		
		MacDate outFileDate = outFileExists ? ModifiedDate( outFile ) : 0;
		
		bool needToLink = !outFileExists;
		
		std::vector< FSSpec > objectFiles;
		
		std::vector< FSSpec >::const_iterator it, end = project.Sources().end();
		
		for ( it = project.Sources().begin();  it != end;  ++it )
		{
			std::string sourceName = N::Convert< std::string >( it->name );
			
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
		
		
		if ( needLibs )
		{
			link << ( gnu ? GetFrameworks( project )
			              : MacSystemLibs( targetInfo, project.Product() == productApplication ) );
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
			
			Io::Write( N::FSpOpenDF( pkgInfo, fsWrPerm ),
			           std::string( "APPL" ) + project.CreatorCode() );
		}
		
		std::string outputPathname = GetPathname( outFile );
		
		command << cmdgen.Output( outputPathname );
		
		command << objectFilePaths << link;
		
		if ( gnu )
		{
			command << "> /tmp/link-errs.txt 2>&1";
		}
		
		command = cmdgen.MakeNativeCommand( command );
		
		QueueCommand( "echo Linking:  " + N::Convert< std::string >( outFile.name ) );
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
			
			FSSpec includeDir = N::Convert< FSSpec >( ProjectIncludesFolder( project.ProjectFolder() ) );
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
			
			QueueCommand( "echo Rezzing:  " + N::Convert< std::string >( rsrcFile.name ) );
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
			
			QueueCommand( "echo Copying resources:  " + N::Convert< std::string >( rsrcFile.name ) );
			QueueCommand( cpresCommand );
		}
	}
	
}

