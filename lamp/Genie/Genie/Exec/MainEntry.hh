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

// Iota
#include "iota/argv.hh"
#include "iota/environ.hh"

// Genie
#include "Genie/BinaryImage.hh"


namespace Genie
{
	
	typedef int (*Main0)();
	typedef int (*Main2)( int argc, iota::argv_t argv );
	typedef int (*Main3)( int argc, iota::argv_t argv, iota::environ_t envp );
	
	class MainEntryPoint
	{
		public:
			virtual ~MainEntryPoint();
			
			virtual void Invoke( int argc, iota::argv_t argv, iota::environ_t envp ) = 0;
	};
	
	typedef boost::shared_ptr< MainEntryPoint > MainEntry;
	
	
	MainEntry GetMainEntryFromAddress( Main3 address );
	
	inline MainEntry GetMainEntryFromAddress( Main2 address )
	{
		return GetMainEntryFromAddress( reinterpret_cast< Main3 >( address ) );
	}
	
	inline MainEntry GetMainEntryFromAddress( Main0 address )
	{
		return GetMainEntryFromAddress( reinterpret_cast< Main3 >( address ) );
	}
	
	
	MainEntry GetMainEntryFromBinaryImage( const BinaryImage& file );
	
}

#endif

