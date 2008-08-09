/*	====================
 *	FSTree_crm_serial.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACCRMSERIAL_HH
#define GENIE_FILESYSTEM_FSTREESYSMACCRMSERIAL_HH

// ClassicToolbox
#include "ClassicToolbox/CRMSerialDevices.h"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"


namespace Genie
{
	
	struct CRMDeviceID_KeyName_Traits : Integer_KeyName_Traits< Nitrogen::CRMDeviceID >
	{
	};
	
	struct sys_mac_crm_serial_Details : public CRMDeviceID_KeyName_Traits
	{
		typedef Nitrogen::CRMSerialDevice_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::CRMSerialDevices(); }
		
		static Key KeyFromValue( Sequence::const_reference ref )  { return Key( ref->crmDeviceID ); }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_crm_serial_Details > FSTree_sys_mac_crm_serial;
	
	
}

#endif

