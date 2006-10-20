// Sound.h

#ifndef NITROGEN_SOUND_H
#define NITROGEN_SOUND_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __SOUND__
#include FRAMEWORK_HEADER(CarbonSound,Sound.h)
#endif

// Nitrogen Core
#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/FlagType.h"
#endif
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/IDType.h"
#endif
#ifndef NUCLEUS_INDEXUNTILFAILURECONTAINER_H
#include "Nucleus/IndexUntilFailureContainer.h"
#endif
#ifndef NUCLEUS_SELECTORTYPE_H
#include "Nucleus/SelectorType.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NUCLEUS_TRANSFERTRAITS_H
#include "Nucleus/TransferTraits.h"
#endif

// Nitrogen Carbon support
#ifndef NITROGEN_COMPONENTS_H
#include "Nitrogen/Components.h"
#endif
#ifndef NITROGEN_DIALOGS_H
//#include "Nitrogen/Dialogs.h"
#endif
#ifndef NITROGEN_ICONS_H
#include "Nitrogen/Icons.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_MIXEDMODE_H
#include "Nitrogen/MixedMode.h"
#endif
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif


namespace Nitrogen
{
	
	void RegisterSoundManagerErrors();
	
	struct SoundInputDeviceInfoType_Tag  {};
	typedef Nucleus::SelectorType< SoundInputDeviceInfoType_Tag, ::OSType, 0 > SoundInputDeviceInfoType;
	
	typedef SoundInputDeviceInfoType SoundInputDeviceInformationSelector;
	
	using ::SPB;
	
	struct SoundInputReferenceNumber_Tag {};
	typedef Nucleus::IDType< SoundInputReferenceNumber_Tag, long, 0 > SoundInputReferenceNumber;
	
	typedef SoundInputReferenceNumber SoundInputRefNum;
	
	struct SoundInputPermissions_Tag {};
	typedef Nucleus::FlagType< SoundInputPermissions_Tag, short, 0 > SoundInputPermissions;
	
  }

namespace Nucleus
  {
	template <>
	struct Disposer< Nitrogen::SoundInputRefNum > : public std::unary_function< Nitrogen::SoundInputRefNum, void >,
                                                    private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::SoundInputRefNum refNum ) const
		{
			Nucleus::OnlyOnce< Nitrogen::RegisterSoundManagerErrors >();
			
			HandleDestructionOSStatus( ::SPBCloseDevice( refNum ) );
		}
	};
  }

namespace Nitrogen
  {	
#if PRAGMA_STRUCT_ALIGN
	#pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
	#pragma pack(2)
#endif
	
	struct SPBGetDeviceInfo_SampleInfoAvailable_Result
	{
		UInt16 count;
		::Handle data;
	};
	
#if PRAGMA_STRUCT_ALIGN
	#pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
	#pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
	#pragma pack()
#endif
	
	template < class Type >  struct InfoData_Traits;
	
	template <>  struct InfoData_Traits< UInt16 >
	{
		typedef UInt16 Result;
		typedef UInt16 Parameter;
		
		typedef UInt16 GetBuffer;
		typedef UInt16 SetBuffer;
		
		static Result    ProcessGetBuffer( const GetBuffer& buffer )  { return buffer; }
		static SetBuffer PrepareSetBuffer( const Parameter& param  )  { return param;  }
	};
	
	template <>  struct InfoData_Traits< bool >
	{
		typedef bool Result;
		typedef bool Parameter;
		
		typedef UInt16 GetBuffer;
		typedef UInt16 SetBuffer;
		
		static Result    ProcessGetBuffer( const GetBuffer& buffer )  { return buffer != 0; }
		static SetBuffer PrepareSetBuffer( const Parameter& param  )  { return param;       }
	};
	
	template <>  struct InfoData_Traits< double >
	{
		typedef double Result;
		typedef double Parameter;
		
		typedef ::UnsignedFixed GetBuffer;
		typedef ::UnsignedFixed SetBuffer;
		
		static Result    ProcessGetBuffer( const GetBuffer& buffer )  { return UnsignedFixedToDouble( buffer ); }
		static SetBuffer PrepareSetBuffer( const Parameter& param  )  { return DoubleToUnsignedFixed( param  ); }
	};
	
	template < class T >  struct InfoData_Traits< T** >
	{
		struct Result
		{
			UInt16 count;
			Nucleus::Owned< T**, Nucleus::Disposer< Handle > > data;
			
			struct Transfer
			{
				UInt16 count;
				Nucleus::OwnershipTransfer< T**, Nucleus::Disposer< Handle > > data;
				
				explicit Transfer( Result* s )
				:
					count( s->count ), data( s->data )
				{}
			};
			
			Result() : count( 0 )  {}
			
			Result( Transfer s )
			:
				count( s.count ),
				data ( s.data  )
			{}
			
			Result& operator=( Transfer s )
			{
				count = s.count;
				data  = s.data;
				
				return *this;
			}
			
			operator Transfer()  { return Transfer( this ); }
		};
		
		typedef SPBGetDeviceInfo_SampleInfoAvailable_Result GetBuffer;
	
		static Result ProcessGetBuffer( const GetBuffer& buffer )
		{
			T** handle = Handle_Cast< T >( Handle( buffer.data ) );
			
			Result result;
			result.count = buffer.count;
			result.data = Nucleus::Owned< T**, Nucleus::Disposer< Handle > >::Seize( handle );
			return result;
		}
	};
	
	template < ::OSType infoType >  struct SoundInputDeviceInfoType_Traits;
	
	template <>  struct SoundInputDeviceInfoType_Traits< siAGCOnOff            > : InfoData_Traits< bool >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siOptionsDialog       > : InfoData_Traits< bool >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siPlayThruOnOff       > : InfoData_Traits< bool >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siTwosComplementOnOff > : InfoData_Traits< bool >  {};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siInputSource    > : InfoData_Traits< UInt16 >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siNumberChannels > : InfoData_Traits< UInt16 >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleSize     > : InfoData_Traits< UInt16 >  {};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siInputGain  > : InfoData_Traits< double >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleRate > : InfoData_Traits< double >  {};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siDeviceName >
	{
		typedef Str255 Result;
		typedef Str255 GetBuffer;
		
		static Result ProcessGetBuffer( const GetBuffer& buffer )  { return buffer; }
	};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siInputSourceNames >
	{
		typedef Nucleus::Owned< Handle > Result;
		typedef ::Handle GetBuffer;
		
		static Result ProcessGetBuffer( const GetBuffer& buffer )  { return Nucleus::Owned< Handle >::Seize( buffer ); }
	};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siDeviceIcon >
	{
		typedef Nucleus::Owned< MaskedIconHandle > Result;
		typedef ::Handle GetBuffer;
		
		static Result ProcessGetBuffer( const GetBuffer& buffer )  { return Result::Seize( Handle_Cast< MaskedIcon >( buffer ) ); }
	};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleSizeAvailable > : InfoData_Traits< UInt16** >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleRateAvailable > : InfoData_Traits< ::UnsignedFixed** >  {};
	
	#pragma mark -
	#pragma mark ¥ Routines ¥
	
	inline void SysBeep( short duration = 30 )  { ::SysBeep( duration ); }
	
	// SndDoCommand
	// SndDoImmediate
	// SndNewChannel
	// SndDisposeChannel
	// SndPlay
	// old SndAddModifier
	// old SndControl
	// SndSoundManagerVersion
	// old SndStartFilePlay
	// old SndPauseFilePlay
	// old SndStopFilePlay
	// SndChannelStatus
	// SndManagerStatus
	// SndGetSysBeepState
	// SndSetSysBeepState
	// old SndPlayDoubleBuffer
	// old MACEVersion
	// old Comp3to1
	// old Exp1to3
	// old Comp6to1
	// old Exp1to6
	// GetSysBeepVolume
	// SetSysBeepVolume
	// GetDefaultOutputVolume
	// SetDefaultOutputVolume
	// GetSoundHeaderOffset
	// UnsignedFixedMulDiv
	// GetCompressionInfo
	// SetSoundPreference
	// GetSoundPreference
	// OpenMixerSoundComponent
	// CloseMixerSoundComponent
	// SndGetInfo
	// SndSetInfo
	// GetSoundOutputInfo
	// SetSoundOutputInfo
	// GetCompressionName
	// SoundConverterOpen
	// SoundConverterClose
	// SoundConverterGetBufferSizes
	// SoundConverterBeginConversion
	// SoundConverterConvertBuffer
	// SoundConverterEndConversion
	// SoundConverterGetInfo
	// SoundConverterSetInfo
	// SoundConverterFillBuffer
	// SoundManagerGetInfo
	// SoundManagerSetInfo
	// SoundComponentInitOutputDevice
	// SoundComponentSetSource
	// SoundComponentGetSource
	// SoundComponentGetSourceData
	// SoundComponentSetOutput
	// SoundComponentAddSource
	// SoundComponentRemoveSource
	// SoundComponentGetInfo
	// SoundComponentSetInfo
	// SoundComponentStartSource
	// SoundComponentStopSource
	// SoundComponentPauseSource
	// SoundComponentPlaySourceBuffer
	// old AudioGetVolume
	// old AudioGetVolume
	// old AudioGetMute
	// old AudioSetMute
	// old AudioSetToDefaults
	// old AudioGetInfo
	// old AudioGetBass
	// old AudioSetBass
	// old AudioGetTreble
	// old AudioSetTreble
	// old AudioGetOutputDevice
	// old AudioMuteOnEvent
	// SPBVersion
	// SndRecord
	// old SndRecordToFile
	// SPBSignInDevice
	// SPBSignOutDevice
	
	struct SPBGetIndexedDevice_Result
	{
		struct Transfer
		{
			Str255                                           deviceName;
			Nucleus::OwnershipTransfer< Handle, Nucleus::Disposer< Handle > >  deviceIconHandle;
			
			explicit Transfer( SPBGetIndexedDevice_Result* value )
			:
				deviceName      ( value->deviceName       ),
				deviceIconHandle( value->deviceIconHandle )
			{}
		};
		
		Str255 deviceName;
		Nucleus::Owned< Handle > deviceIconHandle;
		
		SPBGetIndexedDevice_Result()  {}
		
		SPBGetIndexedDevice_Result( Transfer s )
		:
			deviceName      ( s.deviceName       ),
			deviceIconHandle( s.deviceIconHandle )
		{}
		
		SPBGetIndexedDevice_Result& operator=( Transfer s )
		{
			deviceName       = s.deviceName;
			deviceIconHandle = s.deviceIconHandle;
			
			return *this;
		}
		
		operator Transfer()
		{
			return Transfer( this );
		}
		
		operator ConstStr255Param() const  { return deviceName; }
	};
  }

namespace Nucleus
  {	
	template <> struct Transfer_Traits< Nitrogen::SPBGetIndexedDevice_Result >
	{
		static const bool mayCopyConstSource = false;
		
		typedef Nitrogen::SPBGetIndexedDevice_Result           Type;
		typedef Nitrogen::SPBGetIndexedDevice_Result::Transfer Transfer;
	};
  }

namespace Nitrogen
  {	
	SPBGetIndexedDevice_Result SPBGetIndexedDevice( std::size_t count );
	
	Nucleus::Owned< SoundInputRefNum > SPBOpenDevice( ConstStr255Param       deviceName,
	                                         SoundInputPermissions  permission );
	
	void SPBCloseDevice( Nucleus::Owned< SoundInputRefNum > );
	
	// SPBRecord
	// old SPBRecordToFile
	// SPBPauseRecording
	// SPBResumeRecording
	// SPBStopRecording
	// SPBGetRecordingStatus
	
	void SPBGetDeviceInfo( SoundInputRefNum refNum, SoundInputDeviceInfoType infoType, void* infoData );
	
	// 2873
	void SPBSetDeviceInfo( SoundInputRefNum refNum, SoundInputDeviceInfoType infoType, void* infoData );
	
	template < ::OSType infoType >
	typename SoundInputDeviceInfoType_Traits< infoType >::Result SPBGetDeviceInfo( SoundInputRefNum refNum )
	{
		typedef SoundInputDeviceInfoType_Traits< infoType > Traits;
		
		typename Traits::GetBuffer infoData;
		
		SPBGetDeviceInfo( refNum, infoType, &infoData );
		
		return Traits::ProcessGetBuffer( infoData );
	}
	
	template < ::OSType infoType >
	void SPBSetDeviceInfo( SoundInputRefNum                                                 refNum,
	                       typename SoundInputDeviceInfoType_Traits< infoType >::Parameter  param )
	{
		typedef SoundInputDeviceInfoType_Traits< infoType > Traits;
		
		typename Traits::SetBuffer infoData = Traits::PrepareSetBuffer( param );
		
		SPBSetDeviceInfo( refNum, infoType, &infoData );
	}
	
	// SPBMillisecondsToBytes
	// SPBBytesToMilliseconds
	// SetupSndHeader
	// SetupAIFFHeader
	// ParseAIFFHeader
	// ParseSndHeader
	
	// Mac OS X only bits for sound input components
	
	class SoundInputDevice_ContainerSpecifics
	{
		public:
			typedef SPBGetIndexedDevice_Result value_type;
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			static size_type begin_position()   { return 1; }
			static size_type end_position()     { return 0; }
			
			typedef Nucleus::ErrorCode< OSStatus, siBadSoundInDevice > EndOfEnumeration;
			
			value_type GetValue( size_type position )
			{
				return SPBGetIndexedDevice( position );
			}
	};
	
	class SoundInputDevice_Container: public Nucleus::IndexUntilFailureContainer< SoundInputDevice_ContainerSpecifics >
	{
		friend SoundInputDevice_Container SoundInputDevices();
		
		private:
			SoundInputDevice_Container()
			: Nucleus::IndexUntilFailureContainer< ::Nitrogen::SoundInputDevice_ContainerSpecifics >( ::Nitrogen::SoundInputDevice_ContainerSpecifics() )
			{}
	};
	
	inline SoundInputDevice_Container SoundInputDevices()
	{
		return SoundInputDevice_Container();
	}
	
	namespace Tests
	{
		
		SoundInputDevice_Container::const_iterator Bar();
		
		static inline void Foo()
		{
			typedef SoundInputDevice_Container::const_iterator const_iterator;
			
			SPBGetIndexedDevice_Result result = SPBGetIndexedDevice( 1 );
			
			result = SPBGetIndexedDevice( 2 );
			
			const_iterator one;
			
			const_iterator two = one;
			
			const_iterator three( const_iterator() );
			
			SoundInputDevices().begin();
			
			const_iterator::Transfer( Bar() );
			
			one = const_iterator::Transfer( SoundInputDevices().begin() );
			one = SoundInputDevices().begin();
			
			const_iterator four = SoundInputDevices().begin();
		}
		
	}
	
}

#endif

