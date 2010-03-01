/*	===================
 *	CRMSerialDevices.hh
 *	===================
 */

#ifndef CLASSICTOOLBOX_CRMSERIALDEVICES_HH
#define CLASSICTOOLBOX_CRMSERIALDEVICES_HH

// Mac OS
#ifndef __CRMSERIALDEVICES__
#include <CRMSerialDevices.h>
#endif

// Standard C++
#include <functional>

// Nitrogen
#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_COMMRESOURCES_HH
#include "ClassicToolbox/CommResources.hh"
#endif


enum
{
	typeCRMSerialPtr = 'CRMs'
};

namespace Nitrogen
{
	
	using ::CRMIconRecord;
	using ::CRMIconPtr;
	using ::CRMIconHandle;
	
	using ::CRMSerialRecord;
	using ::CRMSerialPtr;
	
	static const CRMDeviceType crmSerialDevice = CRMDeviceType( ::crmSerialDevice );
	
	template <> struct CRMAttributes_Traits< crmSerialDevice >  { typedef CRMSerialPtr Type; };
	
}

namespace nucleus
{
	
	template <> struct disposer< Nitrogen::CRMIconHandle > : public std::unary_function< Nitrogen::CRMIconHandle, void >
	{
		void operator()( Nitrogen::CRMIconHandle deviceIcon ) const
		{
			Nitrogen::IconSuiteRef iconSuite = (*deviceIcon)->theSuite;
			
			disposer< Nitrogen::IconSuiteRef >()( iconSuite  );
			disposer< Nitrogen::Handle       >()( deviceIcon );
		}
	};
	
	template <> struct disposer< Nitrogen::CRMSerialPtr > : public std::unary_function< Nitrogen::CRMSerialPtr, void >
	{
		void operator()( Nitrogen::CRMSerialPtr crmSerial ) const
		{
			disposer< Nitrogen::Handle        >()( crmSerial->inputDriverName  );
			disposer< Nitrogen::Handle        >()( crmSerial->outputDriverName );
			disposer< Nitrogen::Handle        >()( crmSerial->name             );
			disposer< Nitrogen::CRMIconHandle >()( crmSerial->deviceIcon       );
			disposer< Nitrogen::Ptr           >()( crmSerial                   );
		}
	};
	
}

namespace Nitrogen
{
	
	inline Str255 GetCRMSerialName( CRMSerialPtr crmSerial )  { return *crmSerial->name; }
	
	nucleus::owned< CRMSerialPtr > New_CRMSerialRecord( ConstStr255Param inputDriverName,
	                                                    ConstStr255Param outputDriverName,
	                                                    ConstStr255Param portName );
	
	class CRMSerialDevice_Container : public CRMResource_Container
	{
		friend CRMSerialDevice_Container CRMSerialDevices();
		
		private:
			CRMSerialDevice_Container() : CRMResource_Container( crmSerialDevice )  {}
	};
	
	inline CRMSerialDevice_Container CRMSerialDevices()
	{
		return CRMSerialDevice_Container();
	}
	
}

#endif

