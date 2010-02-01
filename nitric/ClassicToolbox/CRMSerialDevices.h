/*	==================
 *	CRMSerialDevices.h
 *	==================
 */

#ifndef CLASSICTOOLBOX_CRMSERIALDEVICES_HH
#define CLASSICTOOLBOX_CRMSERIALDEVICES_HH

// Mac OS
#ifndef __CRMSERIALDEVICES__
#include <CRMSerialDevices.h>
#endif

// Standard C++
#include <algorithm>
#include <functional>

// STL extensions
#ifdef __MWERKS__
#include <functional_ext>
#else
#include <ext/functional>
#endif

// Nucleus
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif

// Nitrogen
#ifndef NITROGEN_ICONS_H
#include "Nitrogen/Icons.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif

// Nitrogen Extras / ClassicToolbox
#ifndef CLASSICTOOLBOX_COMMRESOURCES_H
#include "ClassicToolbox/CommResources.h"
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

namespace Nucleus
{
	
	template <> struct Disposer< Nitrogen::CRMIconHandle > : public std::unary_function< Nitrogen::CRMIconHandle, void >
	{
		void operator()( Nitrogen::CRMIconHandle deviceIcon ) const
		{
			Nitrogen::IconSuiteRef iconSuite = (*deviceIcon)->theSuite;
			
			Disposer< Nitrogen::IconSuiteRef >()( iconSuite  );
			Disposer< Nitrogen::Handle       >()( deviceIcon );
		}
	};
	
	template <> struct Disposer< Nitrogen::CRMSerialPtr > : public std::unary_function< Nitrogen::CRMSerialPtr, void >
	{
		void operator()( Nitrogen::CRMSerialPtr crmSerial ) const
		{
			Disposer< Nitrogen::Handle        >()( crmSerial->inputDriverName  );
			Disposer< Nitrogen::Handle        >()( crmSerial->outputDriverName );
			Disposer< Nitrogen::Handle        >()( crmSerial->name             );
			Disposer< Nitrogen::CRMIconHandle >()( crmSerial->deviceIcon       );
			Disposer< Nitrogen::Ptr           >()( crmSerial                   );
		}
	};
	
	template <> struct Converter< Nitrogen::CRMSerialPtr, Nitrogen::CRMRecPtr > : public std::unary_function< Nitrogen::CRMRecPtr, Nitrogen::CRMSerialPtr >
	{
		Nitrogen::CRMSerialPtr operator()( Nitrogen::CRMRecPtr crmRec ) const
		{
			using Nitrogen::crmSerialDevice;
			
			// FIXME:  Throw if device type is wrong
			return Nitrogen::GetCRMAttributes< crmSerialDevice >( crmRec );
		}
	};
	
#ifdef __MWERKS__
	namespace ext = std;
#else
	namespace ext = __gnu_cxx;
#endif
	
	template <> struct Converter< Nitrogen::CRMRecPtr, Nitrogen::CRMSerialPtr > : public std::unary_function< Nitrogen::CRMSerialPtr, Nitrogen::CRMRecPtr >
	{
		Nitrogen::CRMRecPtr operator()( Nitrogen::CRMSerialPtr crmSerialPtr ) const
		{
			using Nitrogen::crmSerialDevice;
			
			typedef Nitrogen::CRMResource_Container::const_iterator const_iterator;
			
			Nitrogen::CRMResource_Container crmResources = Nitrogen::CRMResources( crmSerialDevice );
			
			const_iterator it = std::find_if( crmResources.begin(),
			                                  crmResources.end(),
			                                  ext::compose1( std::bind2nd( std::equal_to< Nitrogen::CRMSerialPtr >(),
			                                                               crmSerialPtr ),
			                                                 Nitrogen::CRMAttributes_Getter< crmSerialDevice >() ) );
			if ( it == crmResources.end() )
			{
				throw Nitrogen::CRMSearch_Failed();
			}
			
			return *it;
		}
	};
	
}

namespace Nitrogen
{
	
	inline Str255 GetCRMSerialName( CRMSerialPtr crmSerial )  { return *crmSerial->name; }
	
	Nucleus::Owned< CRMSerialPtr > New_CRMSerialRecord( ConstStr255Param inputDriverName,
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

