/*	=============
 *	FSTree_Dev.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_Dev.hh"

// Nucleus
#include "Nucleus/Convert.h"

// Genie
#include "Genie/Console.hh"
#include "Genie/Devices.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/SimpleDevice.hh"


namespace Genie
{
	
	namespace NN = Nucleus;
	
	class FSTree_dev : public FSTree_Virtual
	{
		public:
			FSTree_dev();
			
			std::string Name() const  { return "dev"; }
			
			FSTreePtr Self()   const  { return GetSingleton< FSTree_dev >(); }
			FSTreePtr Parent() const  { return FSRoot(); }
	};
	
	class FSTree_Device : public FSTree
	{
		private:
			std::string deviceName;
		
		public:
			FSTree_Device( const std::string& name ) : deviceName( name )  {}
			
			std::string Name() const  { return deviceName; }
			
			FSTreePtr Parent() const  { return GetDevFSTree(); }
			
			void Stat( struct ::stat& sb ) const  { StatGeneric( &sb ); }
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	
	struct dev_term_Details;
	
	typedef FSTree_Special< dev_term_Details > FSTree_dev_term;
	
	struct dev_term_Details
	{
		static std::string Name()  { return "term"; }
		
		FSTreePtr Self()   const  { return FSTreePtr( GetSingleton< FSTree_dev_term >() ); }
		FSTreePtr Parent() const  { return FSTreePtr( GetSingleton< FSTree_dev      >() ); }
		
		FSTreePtr Lookup( const std::string& name ) const;
		
		const ConsolesOwner::Map& ItemSequence() const  { return GetConsoleMap(); }
		
		static FSNode ConvertToFSNode( ConsolesOwner::Map::value_type console );
		
		FSNode operator()( ConsolesOwner::Map::value_type console ) const  { return ConvertToFSNode( console ); }
	};
	
	
	class FSTree_dev_term_N : public FSTree
	{
		private:
			unsigned itsIndex;
		
		public:
			FSTree_dev_term_N( unsigned index ) : itsIndex( index )  {}
			
			std::string Name() const;
			
			FSTreePtr Parent() const  { return GetSingleton< FSTree_dev_term >(); }
	};
	
	
	FSTreePtr GetDevFSTree()
	{
		return GetSingleton< FSTree_dev >();
	}
	
	
	boost::shared_ptr< IOHandle > FSTree_Device::Open( OpenFlags flags ) const
	{
		return GetSimpleDeviceHandle( deviceName );
	}
	
	
	FSTree_dev::FSTree_dev()
	{
		Map( "null",    FSTreePtr( new FSTree_Device( "null"    ) ) );
		Map( "zero",    FSTreePtr( new FSTree_Device( "zero"    ) ) );
		Map( "console", FSTreePtr( new FSTree_Device( "console" ) ) );
		
		Map( "term", FSTreePtr( GetSingleton< FSTree_dev_term >() ) );
	}
	
	
	FSTreePtr dev_term_Details::Lookup( const std::string& name ) const
	{
		unsigned index = std::atoi( name.c_str() );
		
		return FSTreePtr( new FSTree_dev_term_N( index ) );
	}
	
	FSNode dev_term_Details::ConvertToFSNode( ConsolesOwner::Map::value_type consoleMapping )
	{
		Console* console = consoleMapping.first;
		
		const std::string& pathname = console->TTYName();
		
		const std::string& name = pathname.substr( sizeof "/dev/term/" - 1, pathname.npos );
		
		unsigned index = std::atoi( name.c_str() );
		
		FSTreePtr tree( new FSTree_dev_term_N( index ) );
		
		return FSNode( name, tree );
	}
	
	
	std::string FSTree_dev_term_N::Name() const
	{
		return NN::Convert< std::string >( itsIndex );
	}
	
}

