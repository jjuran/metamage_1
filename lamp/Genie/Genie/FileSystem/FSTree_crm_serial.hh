/*	====================
 *	FSTree_crm_serial.hh
 *	====================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACCRMSERIAL_HH
#define GENIE_FILESYSTEM_FSTREESYSMACCRMSERIAL_HH

// Nucleus
#include "Nucleus/AdvanceUntilFailureContainer.h"
#include "Nucleus/IndexedContainer.h"

// ClassicToolbox
#include "ClassicToolbox/CRMSerialDevices.h"

// Genie
#include "Genie/FileSystem/FSTree_sys_mac_crm.hh"


namespace Genie
{
	
	struct CRMDeviceID_KeyName_Traits : Integer_KeyName_Traits< Nitrogen::CRMDeviceID >
	{
	};
	
	class FSTree_sys_mac_crm_serial_N;
	
	struct sys_mac_crm_serial_Details : public CRMDeviceID_KeyName_Traits
	{
		typedef Nitrogen::CRMSerialDevice_Container Sequence;
		
		typedef FSTree_sys_mac_crm_serial_N ChildNode;
		
		static const Sequence& ItemSequence()  { return Nitrogen::CRMSerialDevices(); }
		
		static Key KeyFromValue( Sequence::const_reference ref )  { return Key( ref->crmDeviceID ); }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_crm_serial_Details > FSTree_sys_mac_crm_serial;
	
	
	class FSTree_sys_mac_crm_serial_N : public FSTree_Functional< CRMDeviceID_KeyName_Traits::Key >
	{
		private:
			typedef CRMDeviceID_KeyName_Traits::Key Key;
			
			typedef FSTree_Functional< Key > Base;
		
		public:
			FSTree_sys_mac_crm_serial_N( const FSTreePtr&    parent,
			                             const std::string&  name,
			                             const Key&          key ) : Base( parent, name, key )
			{
			}
			
			void Init();
	};
	
}

#endif

