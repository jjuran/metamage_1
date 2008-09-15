/*	======================
 *	FSTree_sys_mac_gdev.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESYSMACGDEV_HH
#define GENIE_FILESYSTEM_FSTREESYSMACGDEV_HH

#ifndef NUCLEUS_LINKEDLISTCONTAINER_H
#include "Nucleus/LinkedListContainer.h"
#endif

// Nitrogen
#include "Nitrogen/QuickDraw.h"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"


namespace Nitrogen
{
	
	class DeviceList_ContainerSpecifics
	{
		public:
			typedef GDHandle  value_type;
			typedef UInt32    size_type;
			typedef SInt32    difference_type;
			
			value_type GetNextValue( value_type value )
			{
				return GetNextDevice( value );
			}
			
			static value_type begin_value()  { return GetDeviceList(); }
	};
	
	class DeviceList_Container: public Nucleus::LinkedListContainer< ::Nitrogen::DeviceList_ContainerSpecifics >
	{
		friend DeviceList_Container DeviceList();
		
		private:
			DeviceList_Container()
			: Nucleus::LinkedListContainer< ::Nitrogen::DeviceList_ContainerSpecifics >( ::Nitrogen::DeviceList_ContainerSpecifics() )
			{
			}
	};
	
	inline DeviceList_Container DeviceList()
	{
		return DeviceList_Container();
	}
	
}

namespace Genie
{
	
	struct GDHandle_KeyName_Traits : public Pointer_KeyName_Traits< Nitrogen::GDHandle >  {};
	
	struct sys_mac_gdev_Details : public GDHandle_KeyName_Traits
	{
		typedef Nitrogen::DeviceList_Container Sequence;
		
		static Sequence ItemSequence()  { return Nitrogen::DeviceList(); }
		
		static Key KeyFromValue( const Sequence::value_type& value )  { return value; }
		
		static bool KeyIsValid( const Key& key );
		
		static FSTreePtr GetChildNode( const FSTreePtr&    parent,
		                               const std::string&  name,
		                               const Key&          key );
	};
	
	typedef FSTree_Sequence< sys_mac_gdev_Details > FSTree_sys_mac_gdev;
	
}

#endif

