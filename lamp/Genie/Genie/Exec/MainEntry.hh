/*	============
 *	MainEntry.hh
 *	============
 */

#ifndef GENIE_MAINENTRY_HH
#define GENIE_MAINENTRY_HH

// Standard C++
#include <string>

// Boost
#include <boost/shared_ptr.hpp>

// Genie
#include "Genie/FileSystem/FSTree.hh"


namespace Genie
{
	
	class MainEntryPoint
	{
		public:
			typedef char const *const *Environ;
			
			typedef int (*Main)( int argc, char const *const *argv, Environ envp );
			
		private:
			Main      itsMain;
			int*      itsErrno;
			Environ*  itsEnviron;
		
		protected:
			MainEntryPoint( Main      main,
			                int*      err,
			                Environ*  env ) : itsMain   ( main ),
			                                  itsErrno  ( err  ),
			                                  itsEnviron( env  )
			{
			}
			
			virtual ~MainEntryPoint();
		
		public:
			Main GetMainPtr() const  { return itsMain; }
			
			int* GetErrnoPtr() const  { return itsErrno; }
			
			Environ* GetEnvironPtr() const  { return itsEnviron; }
	};
	
	typedef boost::shared_ptr< MainEntryPoint > MainEntry;
	
	MainEntry GetMainEntryFromFile( const FSSpec& file );
	
	MainEntry GetMainEntryFromFile( const FSTreePtr& file );
	
}

#endif

