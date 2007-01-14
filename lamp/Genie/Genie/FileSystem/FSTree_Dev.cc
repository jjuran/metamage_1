/*	=============
 *	FSTree_Dev.cc
 *	=============
 */

#include "Genie/FileSystem/FSTree_Dev.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/StatFile.hh"
#include "Genie/IO/SimpleDevice.hh"


namespace Genie
{
	
	class FSTree_dev : public FSTree_Virtual
	{
		public:
			FSTree_dev();
			
			static FSTreePtr GetSingleton()  { return Genie::GetSingleton< FSTree_dev >(); }
			
			std::string Name() const  { return "dev"; }
			
			FSTreePtr Self()   const  { return GetSingleton(); }
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
	
	
	FSTreePtr GetDevFSTree()
	{
		static FSTreePtr dev = FSTreePtr( new FSTree_dev() );
		
		return dev;
	}
	
	
	boost::shared_ptr< IOHandle > FSTree_Device::Open( OpenFlags flags ) const
	{
		return GetSimpleDeviceHandle( deviceName );
	}
	
	
	FSTree_dev::FSTree_dev()
	{
		Map( "null", FSTreePtr( new FSTree_Device( "null" ) ) );
		Map( "zero", FSTreePtr( new FSTree_Device( "zero" ) ) );
		Map( "console", FSTreePtr( new FSTree_Device( "console" ) ) );
	}
	
}

