/*	==========
 *	Project.hh
 *	==========
 *	
 *	Implemented by Project.cc
 */

#ifndef ALINE_PROJECT_HH
#define ALINE_PROJECT_HH

// Standard C++
#include <string>
#include <vector>

// Nitrogen
#include "Nitrogen/Files.h"

// A-line
#include "A-line/TargetInfo.hh"


namespace ALine {
	
	namespace N = Nitrogen;
	
	using std::string;
	using std::vector;
	
	
	typedef string ProjName, FileName, IncludePath;
	
	class Project
	{
		public:
			Project( const ProjName& proj );
			
			ProjName Name() const  { return projName; }
			FileName ProgramName() const  { return progName; }
			N::FSDirSpec ProjectFolder() const  { return projFolder; }
			FileName LibraryFilename() const  { return Name() + ".lib"; }
			ProductType Product() const  { return product; }
			const IncludePath& PrecompiledHeaderSource() const  { return precompiledHeaderSource; }
			bool NeedsCwdSourceOption() const  { return needsCwdSourceOption; }
			
			const vector< ProjName >& AllUsedProjects() const  { return allUsedProjects; }
			
			FSSpec PrecompiledHeaderImage() const;
			
			const vector< N::FSDirSpec >& SearchDirs() const  { return sourceDirs; }
			
			vector< FileName > ExtraSources()  const  { return myExtraSources; }
			vector< FileName > LibImports()    const  { return myImports; }
			vector< FileName > Frameworks()    const  { return myFrameworks; }
			vector< FileName > UsedRezFiles()  const  { return rezFiles; }
			vector< FileName > UsedRsrcFiles() const  { return rsrcFiles; }
			
			string CreatorCode() const  { return creator; }
			
			const vector< FSSpec >& Sources() const  { return mySources; }
			
			void Study();
			
			bool FindInclude( const IncludePath& includePath );
			
		private:
			// This project's name within the A-line system.
			ProjName projName;
			// The name of the output file, if different from the project name.
			FileName progName;
			// The project's main folder.
			N::FSDirSpec projFolder;
			// The include path to a header to precompile.
			IncludePath precompiledHeaderSource;
			// What kind of product this project creates.
			ProductType product;
			// Ugly hack used for OpenSSL.
			bool needsCwdSourceOption;
			// The names of all projects used directly or indirectly by this one.
			vector< ProjName > allUsedProjects;
			// Directories to search for source files.
			vector< N::FSDirSpec > sourceDirs;
			// Include dirs from used projects.
			vector< N::FSDirSpec > myUsedIncludeDirs;
			// System include dirs from used projects.
			vector< N::FSDirSpec > myUsedSystemIncludeDirs;
			// Various things to link with.
			vector< FileName > rezFiles, rsrcFiles, myExtraSources, myImports, myFrameworks;
			// Creator code / signature for output files.
			string creator;
			// Source files to compile.
			vector< FSSpec > mySources;
	};
	
}

#endif

