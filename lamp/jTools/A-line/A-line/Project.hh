/*	==========
 *	Project.hh
 *	==========
  */

#ifndef ALINE_PROJECT_HH
#define ALINE_PROJECT_HH

// Standard C++
#include <string>
#include <vector>

// A-line
#include "A-line/TargetInfo.hh"


namespace ALine
{
	
	typedef std::string ProjName, FileName, IncludePath;
	
	class Project
	{
		public:
			Project( const ProjName& proj );
			
			ProjName Name() const  { return projName; }
			FileName ProgramName() const  { return progName; }
			std::string ProjectFolder() const  { return projFolderPath; }
			FileName LibraryFilename() const  { return Name() + ".a"; }
			ProductType Product() const  { return product; }
			const IncludePath& PrecompiledHeaderSource() const  { return precompiledHeaderSource; }
			bool NeedsCwdSourceOption() const  { return needsCwdSourceOption; }
			
			const std::vector< ProjName >& AllUsedProjects() const  { return allUsedProjects; }
			
			const std::vector< std::string >& SearchDirs() const  { return sourceDirs; }
			
			std::vector< FileName > LibImports()    const  { return myImports; }
			std::vector< FileName > Frameworks()    const  { return myFrameworks; }
			std::vector< FileName > UsedRezFiles()  const  { return rezFiles; }
			std::vector< FileName > UsedRsrcFiles() const  { return rsrcFiles; }
			
			std::string CreatorCode() const  { return creator; }
			
			const std::vector< std::string >& Sources() const  { return mySources; }
			
			void Study();
			
			bool FindInclude( const IncludePath& includePath );
			
		private:
			// This project's name within the A-line system.
			ProjName projName;
			// The name of the output file, if different from the project name.
			FileName progName;
			// The project's main folder.
			std::string projFolderPath;
			// The include path to a header to precompile.
			IncludePath precompiledHeaderSource;
			// What kind of product this project creates.
			ProductType product;
			// Ugly hack used for OpenSSL.
			bool needsCwdSourceOption;
			// The names of all projects used directly or indirectly by this one.
			std::vector< ProjName > allUsedProjects;
			// Directories to search for source files.
			std::vector< std::string > sourceDirs;
			// Various things to link with.
			std::vector< FileName > rezFiles, rsrcFiles, myImports, myFrameworks;
			// Creator code / signature for output files.
			std::string creator;
			// Source files to compile.
			std::vector< std::string > mySources;
	};
	
}

#endif

