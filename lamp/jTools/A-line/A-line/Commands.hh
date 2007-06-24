/*	===========
 *	Commands.hh
 *	===========
 */

#pragma once

// Standard C++
#include <string>

// MoreFunctional
#include "FunctionalExtensions.hh"

// BitsAndBytes
#include "StringFilters.hh"

// CompileDriver
#include "CompileDriver/Platform.hh"


namespace ALine
{
	
	namespace CD = CompileDriver;
	
	using BitsAndBytes::q;
	using BitsAndBytes::qq;
	
	static const std::string space = " ";
	
	
	struct CommandGenerator
	{
		TargetInfo target;
		
		bool m68k, ppc, x86;
		bool a4, a5, cfm, machO, elf;
		bool classic, carbon;
		bool debug;
		bool gnu;
		
		CommandGenerator( const TargetInfo& target )
		:
			target( target ),
			m68k( target.platform.arch == CD::arch68K ),
			ppc ( target.platform.arch == CD::archPPC ),
			x86 ( target.platform.arch == CD::archX86 ),
			a4( target.platform.runtime == CD::runtimeA4CodeResource ),
			a5( target.platform.runtime == CD::runtimeA5CodeSegments ),
			cfm( target.platform.runtime == CD::runtimeCodeFragments ),
			machO( target.platform.runtime == CD::runtimeMachO ),
			elf( target.platform.runtime == CD::runtimeELF ),
			classic( target.platform.api == CD::apiMacToolbox ),
			carbon( target.platform.api == CD::apiMacCarbon ),
			debug( target.build == buildDebug ),
			gnu( target.toolkit == toolkitGNU )
		{}
		
		std::string UnixCompilerName() const  { return gnu ? "gcc" : "mwcc"; }
		
		std::string CompilerName() const
		{
			return UnixCompilerName() + " -c";
		}
		
		std::string LinkerName() const
		{
			return gnu ? "g++"
			           : "ld";
		}
		
		std::string TargetArchitecture() const
		{
			if ( elf )  return "";
			
			return   ppc  ? "-arch ppc"
			       : m68k ? "-arch m68k"
			       :        "-arch i386";
		}
		
		std::string LibraryMakerName() const
		{
			return gnu ? "libtool"
			           : "ld -static " + TargetArchitecture();
		}
		
		// This means that we pass the precompiled output, not the header source.
		// Required for CodeWarrior (i.e. otherwise you lose precompile benefit).
		// Not supported by gcc.
		//bool PrecompiledHeaderIsImage() const  { return !gnu; }
		
		// This means that the prefix header is given by name only
		// and the parent directory of the precompiled header image
		// is in the search path.
		// Required for gcc unless the precompiled header image is in the same
		// directory as the precompiled header source.
		// Not supported by CodeWarrior.
		//bool PrecompiledHeaderIsSearched() const  { return gnu; }
		
		std::string Prefix( const std::string& pathname ) const
		{
			return "-include " + q( pathname );
		}
		
		std::string LanguageOptions() const
		{
			std::string result = " ";
			
			// Pascal strings are Mac-only; always on before OS X
			if ( machO )  result += "-fpascal-strings ";
			
			// gcc won't compile shared_ptr without RTTI
			if ( !gnu )  result += "-fno-rtti ";
			
			result.resize( result.size() - 1 );
			
			return result;
		}
		
		
		// CodeWarrior only
		std::string MWDebugSymbols() const
		{
			return debug ? "-sym full"
			             : "-sym off";
		}
		
		// CodeWarrior only
		std::string CFMTracebackTables() const  { return carbon && cfm && debug ? "-tb on" : ""; }
		
		// CodeWarrior only
		std::string MWCodeModel() const
		{
			return m68k && cfm ? "-mCFM" : "";
		}
		
		// CodeWarrior only
		std::string MW68KGlobals() const  { return a4 ? "-mA4-globals" : ""; }
		
		std::string Optimization() const
		{
			return   debug ? "-O0"
			       : gnu   ? "-O2"
			       :         "-O4";
		}
		
		std::string Debugging() const
		{
			return debug ? "-g" : "";
		}
		
		std::string CodeGenOptions() const
		{
			std::string result = Optimization() + " " + Debugging();
			
			if ( !gnu )
			{
				result += " " + CFMTracebackTables()
			            + " " + MWCodeModel()
			            + " " + MW68KGlobals();
			}
			
			return result;
		}
		
		std::string AllCompilerOptions() const
		{
			std::string result;
			
			result =         TargetArchitecture()
			         + " " + LanguageOptions()
			         + " " + CodeGenOptions();
			
			return result;
		}
		
		std::string Output( const std::string& pathname ) const
		{
			return "-o " + q( pathname );
		}
		
		std::string Input( const std::string& pathname ) const
		{
			return q( pathname );
		}
		
		std::string OutputType( const std::string& type ) const
		{
			return std::string( "-t " ) + type;
		}
		
		std::string OutputCreator( const std::string& creator ) const
		{
			return std::string( "-c " ) + q( creator );
		}
		
		std::string ResourceTypeAndID( const std::string& type, const std::string& id ) const
		{
			return std::string( "-rt " ) + type + "=" + id;
		}
		
		std::string ResourceName( const std::string& name ) const
		{
			return "-rsrcname " + name;
		}
		
		std::string CustomDriverHeader() const  { return "-custom"; }
		
		std::string AppHeapSize() const
		{
			return "-sizemin 4096 -sizemax 8192";
		}
		
		std::string TargetApplication() const
		{
			//return gnu ? "" : "-xm a -dead " + std::string( m68k ? "code" : "off" ) + space + AppHeapSize();
			return gnu ? "" : "-bundle";
		}
		
		// CodeWarrior only
		std::string MWTargetCodeResource() const
		{
			return "-object";
		}
		
		// CodeWarrior only
		std::string MWTargetSharedLibrary() const
		{
			//return "-xm s -init __initialize -term __terminate -export pragma";
			return "-dynamic";
		}
		
		// CodeWarrior only
		std::string MWTargetKeroseneShLib() const
		{
			//return "-xm s -init InitializeFragment -term TerminateFragment -export pragma -name " + std::string( carbon ? "Carbon" : "classic" ) + space + AppHeapSize();
			return "-execute";
		}
		
		std::string TargetCommandLineTool() const
		{
			return gnu ? ""
			           : "-execute";
		}
		
		std::string MWLinkerOptions() const
		{
			return MWDebugSymbols() + " " + MWCodeModel() + " " + CFMTracebackTables();
		}
		
		std::string LinkerOptions() const
		{
			return debug ? "" : " -s";
		}
		
	};
	
	
	template < class F, class Iter >
	std::string join( Iter begin, Iter end, const std::string& glue = "", F f = F() )
	{
		if ( begin == end )
		{
			return "";
		}
		
		std::string result = f( *begin++ );
		
		while ( begin != end )
		{
			result += glue;
			result += f( *begin++ );
		}
		
		return result;
	}
	
	template < class Iter >
	std::string join( Iter begin, Iter end, const std::string& glue = "" )
	{
		return join( begin, end, glue, more::identity< std::string >() );
	}
	
}

