/*	==================
 *	CRMSerialDevices.h
 *	==================
 */

#ifndef CLASSICTOOLBOX_CRMSERIALDEVICES_HH
#define CLASSICTOOLBOX_CRMSERIALDEVICES_HH

// Universal Interfaces
#ifndef __MACH__
#ifndef __CRMSERIALDEVICES__
#include <CRMSerialDevices.h>
#endif
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

// Nitrogen
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


enum
{
	typeCRMSerialPtr = 'CRMs'
};

namespace Nitrogen
{
	
#ifdef __MWERKS__
	namespace ext = std;
#else
	namespace ext = __gnu_cxx;
#endif
	
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
	
	template <> struct Converter< CRMSerialPtr, CRMRecPtr > : public std::unary_function< CRMRecPtr, CRMSerialPtr >
	{
		CRMSerialPtr operator()( CRMRecPtr crmRec )
		{
			// FIXME:  Throw if device type is wrong
			return GetCRMAttributes< crmSerialDevice >( crmRec );
		}
	};
	
	template <> struct Converter< CRMRecPtr, CRMSerialPtr > : public std::unary_function< CRMSerialPtr, CRMRecPtr >
	{
		CRMRecPtr operator()( CRMSerialPtr crmSerialPtr )
		{
			typedef CRMResource_Container::const_iterator const_iterator;
			
			CRMResource_Container crmResources = CRMResources( crmSerialDevice );
			
			const_iterator it = std::find_if( crmResources.begin(),
			                                  crmResources.end(),
			                                  ext::compose1( std::bind2nd( std::equal_to< CRMSerialPtr >(),
			                                                               crmSerialPtr ),
			                                                 CRMAttributes_Getter< crmSerialDevice >() ) );
			if ( it == crmResources.end() )
			{
				throw CRMSearch_Failed();
			}
			
			return *it;
		}
	};
	
	Owned< CRMSerialPtr > New_CRMSerialRecord( Owned< StringHandle > inputDriverName,
	                                           Owned< StringHandle > outputDriverName,
	                                           Owned< StringHandle > portName );
	
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
	
#endif  // #if CALL_NOT_IN_CARBON
	
}

#endif

