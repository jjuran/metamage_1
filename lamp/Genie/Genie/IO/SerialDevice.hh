/*	===============
 *	SerialDevice.hh
 *	===============
 */

#ifndef GENIE_IO_SERIALDEVICE_HH
#define GENIE_IO_SERIALDEVICE_HH

// ClassicToolbox
#include "ClassicToolbox/Serial.h"

 // Genie
#include "Genie/IO/Device.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	class SerialDeviceHandle : public DeviceHandle
	{
		private:
			NN::Owned< N::DriverRefNum > itsOutputRefNum;
			NN::Owned< N::DriverRefNum > itsInputRefNum;
		
		public:
			SerialDeviceHandle( const std::string& portName = "A" );
			
			static TypeCode Type()  { return kDeviceFileType; }  // FIXME:  Need a new type
			
			TypeCode ActualType() const  { return Type(); }
			
			unsigned int SysPoll() const;
			
			int SysRead( char* data, std::size_t byteCount );
			
			int SysWrite( const char* data, std::size_t byteCount );
	};
	
}

#endif

