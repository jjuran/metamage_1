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
#ifndef NITROGEN_SELECTORTYPE_H
#include "Nitrogen/SelectorType.h"
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
	
	struct SoundInputDeviceInfoType_Tag  {};
	typedef SelectorType< SoundInputDeviceInfoType_Tag, ::OSType, 0 > SoundInputDeviceInfoType;
	
	typedef SoundInputDeviceInfoType SoundInputDeviceInformationSelector;
	
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
		Str255 deviceName;
		Shared< Handle > deviceIconHandle;
		
		operator const unsigned char*() const  { return deviceName; }
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
	
	template < class Type >  struct InfoData_Traits;
	
	template <>  struct InfoData_Traits< UInt16 >
	{
		typedef UInt16 Result;
		typedef UInt16 Parameter;
		
		typedef UInt16 GetBuffer;
		typedef UInt16 SetBuffer;
		
		static Result    ProcessGetBuffer( const GetBuffer& buffer )  { return buffer; }
		static SetBuffer PrepareSetBuffer( const Parameter& param  )  { return param; }
	};
	
	template <>  struct InfoData_Traits< bool >
	{
		typedef bool Result;
		typedef bool Parameter;
		
		typedef UInt16 GetBuffer;
		typedef UInt16 SetBuffer;
		
		static Result    ProcessGetBuffer( const GetBuffer& buffer )  { return buffer != 0; }
		static SetBuffer PrepareSetBuffer( const Parameter& param  )  { return param; }
	};
	
	template <>  struct InfoData_Traits< double >
	{
		typedef double Result;
		typedef double Parameter;
		
		typedef ::UnsignedFixed GetBuffer;
		typedef ::UnsignedFixed SetBuffer;
		
		static Result    ProcessGetBuffer( const GetBuffer& buffer )  { return UnsignedFixedToDouble( buffer ); }
		static SetBuffer PrepareSetBuffer( const Parameter& param  )  { return DoubleToUnsignedFixed( param ); }
	};
	
	template < ::OSType infoType >  struct SoundInputDeviceInfoType_Traits;
	
	template <>  struct SoundInputDeviceInfoType_Traits< siAGCOnOff            > : InfoData_Traits< bool >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siPlayThruOnOff       > : InfoData_Traits< bool >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siTwosComplementOnOff > : InfoData_Traits< bool >  {};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siInputSource    > : InfoData_Traits< UInt16 >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siNumberChannels > : InfoData_Traits< UInt16 >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleSize     > : InfoData_Traits< UInt16 >  {};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siInputGain  > : InfoData_Traits< double >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleRate > : InfoData_Traits< double >  {};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siInputSourceNames >
	{
		typedef Owned< Handle > Result;
		typedef ::Handle GetBuffer;
		
		static Result ProcessGetBuffer( const GetBuffer& buffer )  { return Owned< Handle >::Seize( buffer ); }
	};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleRateAvailable >
	{
		struct Result
		{
			UInt16 count;
			Shared< ::UnsignedFixed**, Disposer< Handle > > rates;
		};
		
	#if PRAGMA_STRUCT_ALIGN
		#pragma options align=mac68k
	#elif PRAGMA_STRUCT_PACKPUSH
		#pragma pack(push, 2)
	#elif PRAGMA_STRUCT_PACK
		#pragma pack(2)
	#endif
		
		struct GetBuffer
		{
			UInt16 count;
			UnsignedFixed** rates;
		};
		
	#if PRAGMA_STRUCT_ALIGN
		#pragma options align=reset
	#elif PRAGMA_STRUCT_PACKPUSH
		#pragma pack(pop)
	#elif PRAGMA_STRUCT_PACK
		#pragma pack()
	#endif
	
		static Result ProcessGetBuffer( const GetBuffer& buffer )
		{
			Result result;
			result.count = buffer.count;
			result.rates = Owned< ::UnsignedFixed**, Disposer< Handle > >::Seize( buffer.rates );
			return result;
		}
	};
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	// 2729
	SPBGetIndexedDevice_Result SPBGetIndexedDevice( std::size_t count );
	
	// 2744
	Owned< SoundInputRefNum > SPBOpenDevice( ConstStr255Param deviceName, SoundInputPermissions permission );
	
	// 2759
	void SPBCloseDevice( Owned< SoundInputRefNum > );
	
	// 2858
	void SPBGetDeviceInfo( SoundInputRefNum refNum, SoundInputDeviceInfoType infoType, void* infoData );
	
	// 2873
	void SPBSetDeviceInfo( SoundInputRefNum refNum, SoundInputDeviceInfoType infoType, void* infoData );
	
	template < ::OSType infoType >
	typename SoundInputDeviceInfoType_Traits< infoType >::Result SPBGetDeviceInfo( SoundInputRefNum refNum )
	{
		typename SoundInputDeviceInfoType_Traits< infoType >::GetBuffer infoData;
		SPBGetDeviceInfo( refNum, infoType, &infoData );
		return SoundInputDeviceInfoType_Traits< infoType >::ProcessGetBuffer( infoData );
	}
	
	template < ::OSType infoType >
	void SPBSetDeviceInfo( SoundInputRefNum refNum, typename SoundInputDeviceInfoType_Traits< infoType >::Parameter param )
	{
		typename SoundInputDeviceInfoType_Traits< infoType >::SetBuffer infoData 
			= SoundInputDeviceInfoType_Traits< infoType >::PrepareSetBuffer( param );
		SPBGetDeviceInfo( refNum, infoType, &infoData );
	}
	
	class SoundInputDevice_ContainerSpecifics
	{
		public:
			typedef SPBGetIndexedDevice_Result value_type;
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			static size_type begin_position()   { return 1; }
			static size_type end_position()     { return 0; }
			
			typedef ErrorCode< OSStatus, siBadSoundInDevice > EndOfEnumeration;
			
			value_type GetValue( size_type position )
			{
				return SPBGetIndexedDevice( position );
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

