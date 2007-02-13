/*	===============
 *	SimpleDevice.hh
 *	===============
 */

#ifndef GENIE_IO_SIMPLEDEVICE_HH
#define GENIE_IO_SIMPLEDEVICE_HH

// Nitrogen
#include "Nitrogen/Files.h"

 // Genie
#include "Genie/IO/Device.hh"


namespace Genie
{
	
	typedef int (*Reader)( char      *, std::size_t );
	typedef int (*Writer)( char const*, std::size_t );
	
	struct DeviceIOSpec
	{
		const char* name;
		Reader reader;
		Writer writer;
	};
	
	class SimpleDeviceHandle : public DeviceHandle
	{
		private:
			const DeviceIOSpec& io;
		
		public:
			SimpleDeviceHandle( const DeviceIOSpec& io ) : io( io )  {}
			
			static TypeCode Type()  { return kSimpleDeviceType; }
			
			TypeCode ActualType() const  { return Type(); }
			
			FSTreePtr GetFile();
			
			unsigned int SysPoll() const  { return kPollRead | kPollWrite | kPollExcept; }
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
	};
	
}

#endif

