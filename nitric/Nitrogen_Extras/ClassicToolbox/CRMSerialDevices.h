/*	==================
 *	CRMSerialDevices.h
 *	==================
 */

#ifndef CLASSICTOOLBOX_CRMSERIALDEVICES_HH
#define CLASSICTOOLBOX_CRMSERIALDEVICES_HH

// Universal Interfaces
#ifndef __CRMSERIALDEVICES__
#include <CRMSerialDevices.h>
#endif

// Nitrogen
#ifndef NITROGEN_ADVANCEUNTILFAILURECONTAINER_H
#include "Nitrogen/AdvanceUntilFailureContainer.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_COMMRESOURCES_H
#include "ClassicToolbox/CommResources.h"
#endif


namespace Nitrogen
{
	
#if CALL_NOT_IN_CARBON
	
	using ::CRMIconRecord;
	using ::CRMIconPtr;
	using ::CRMIconHandle;
	
	using ::CRMSerialRecord;
	using ::CRMSerialPtr;
	
	template <> struct CRMAttributes_Traits< crmSerialDevice >  { typedef CRMSerialPtr Type; };
	
	template <> struct Disposer< CRMIconHandle > : public std::unary_function< CRMIconHandle, void >
	{
		void operator()( CRMIconHandle deviceIcon ) const
		{
			Disposer< Handle >()( (*deviceIcon)->theSuite );
			Disposer< Handle >()(   deviceIcon            );
		}
	};
	
	template <> struct Disposer< CRMSerialPtr > : public std::unary_function< CRMSerialPtr, void >
	{
		void operator()( CRMSerialPtr crmSerial ) const
		{
			Disposer< Handle >()( crmSerial->inputDriverName  );
			Disposer< Handle >()( crmSerial->outputDriverName );
			Disposer< Handle >()( crmSerial->name             );
			Disposer< Ptr    >()( crmSerial                   );
		}
	};
	
	Owned< CRMSerialPtr > New_CRMSerialRecord( Owned< StringHandle > inputDriverName,
	                                           Owned< StringHandle > outputDriverName,
	                                           Owned< StringHandle > portName );
	
	class CRMSerialDevice_ContainerSpecifics
	{
		public:
			typedef CRMRecPtr value_type;
			typedef UInt32 size_type;
			typedef SInt32 difference_type;
			
			typedef CRMSearch_Failed EndOfEnumeration;
			
			value_type GetNextValue( const value_type& value )
			{
				return CRMSearch< crmSerialDevice >( CRMDeviceID( value != NULL ? value->crmDeviceID : 0 ) );
			}
			
			static value_type begin_value()  { return NULL; }
			static value_type end_value()    { return NULL; }
	};
	
	class CRMSerialDevice_Container: public AdvanceUntilFailureContainer< CRMSerialDevice_ContainerSpecifics >
	{
		friend CRMSerialDevice_Container CRMSerialDevices();
		
		private:
			CRMSerialDevice_Container()
			: AdvanceUntilFailureContainer< CRMSerialDevice_ContainerSpecifics >( CRMSerialDevice_ContainerSpecifics() )
			{}
	};
	
	inline CRMSerialDevice_Container CRMSerialDevices()
	{
		return CRMSerialDevice_Container();
	}
	
#endif  // #if CALL_NOT_IN_CARBON
	
}

#endif

