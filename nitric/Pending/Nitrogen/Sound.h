// Sound.h

#ifndef NITROGEN_SOUND_H
#define NITROGEN_SOUND_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __SOUND__
#include FRAMEWORK_HEADER(CarbonSound,Sound.h)
#endif

#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_COMPONENTS_H
#include "Nitrogen/Components.h"
#endif
#ifndef NITROGEN_MIXEDMODE_H
#include "Nitrogen/MixedMode.h"
#endif
#ifndef NITROGEN_DIALOGS_H
//#include "Nitrogen/Dialogs.h"
#endif

#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_SHARED_H
#include "Nitrogen/Shared.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif
#ifndef NITROGEN_INDEXUNTILFAILURECONTAINER_H
#include "Nitrogen/IndexUntilFailureContainer.h"
#endif

namespace Nitrogen
{
	
	void RegisterSoundManagerErrors();
	
	using ::SPB;
	
	struct SoundInputReferenceNumber_Tag {};
	typedef IDType< SoundInputReferenceNumber_Tag, long, 0 > SoundInputReferenceNumber;
	
	typedef SoundInputReferenceNumber SoundInputRefNum;
	
	struct SoundInputPermissions_Tag {};
	typedef FlagType< SoundInputPermissions_Tag, short, 0 > SoundInputPermissions;
	
	inline SoundInputPermissions SIReadPermission()   { return siReadPermission;  }
	inline SoundInputPermissions SIWritePermission()  { return siWritePermission; }
	
	struct SPBGetIndexedDevice_Result
	{
		::Str255 deviceName;
		Shared< Handle > deviceIconHandle;
	};
	
	template <>
	struct Disposer< SoundInputRefNum > : public std::unary_function< SoundInputRefNum, void >, 
	                                      private DefaultDestructionOSStatusPolicy
	{
		void operator()( SoundInputRefNum refNum ) const
		{
			OnlyOnce< RegisterSoundManagerErrors >();
			
			DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::SPBCloseDevice( refNum ) );
		}
	};
	
	// 2729
	SPBGetIndexedDevice_Result SPBGetIndexedDevice( std::size_t count );
	
	// 2744
	Owned< SoundInputRefNum > SPBOpenDevice( ConstStr255Param deviceName, SoundInputPermissions permission );
	
	// 2759
	void SPBCloseDevice( Owned< SoundInputRefNum > );
	
	class SoundInputDevice_ContainerSpecifics
	{
		private:
			Shared< Handle > deviceIconHandle;
		
		public:
			typedef Str255 value_type;
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			static size_type begin_position()   { return 1; }
			static size_type end_position()     { return 0; }
			
			// FIXME
			typedef ErrorCode< OSStatus, siBadSoundInDevice > EndOfEnumeration;
			
			value_type GetValue( size_type position )
			{
				SPBGetIndexedDevice_Result result = SPBGetIndexedDevice( position );
				deviceIconHandle = result.deviceIconHandle;
				return result.deviceName;
			}
	};
	
	class SoundInputDevice_Container: public IndexUntilFailureContainer< SoundInputDevice_ContainerSpecifics >
	{
		friend SoundInputDevice_Container SoundInputDevices();
		
		private:
			SoundInputDevice_Container()
			: IndexUntilFailureContainer< SoundInputDevice_ContainerSpecifics >( SoundInputDevice_ContainerSpecifics() )
			{}
	};
	
	inline SoundInputDevice_Container SoundInputDevices()
	{
		return SoundInputDevice_Container();
	}
	
}

#endif

