// Nitrogen/Sound.hh
// -----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_SOUND_HH
#define NITROGEN_SOUND_HH

#ifndef __SOUND__
#include <Sound.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/flag_ops.hh"

// Nucleus
#ifndef NUCLEUS_INDEXUNTILFAILURECONTAINER_H
#include "Nucleus/IndexUntilFailureContainer.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NUCLEUS_TRANSFERTRAITS_H
#include "Nucleus/TransferTraits.h"
#endif

// Nitrogen
#ifndef NITROGEN_COMPONENTS_HH
#include "Nitrogen/Components.hh"
#endif
#ifndef NITROGEN_DIALOGS_HH
//#include "Nitrogen/Dialogs.hh"
#endif
#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
#ifndef NITROGEN_MACTYPES_HH
#include "Nitrogen/MacTypes.hh"
#endif
#ifndef NITROGEN_MIXEDMODE_HH
#include "Nitrogen/MixedMode.hh"
#endif
#ifndef NITROGEN_STR_HH
#include "Nitrogen/Str.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( SoundManager );
	
	
	enum SoundInputDeviceInfoType
	{
		siActiveChannels            = ::siActiveChannels,
		siActiveLevels              = ::siActiveLevels,
		siAGCOnOff                  = ::siAGCOnOff,
		siAsync                     = ::siAsync,
		siAVDisplayBehavior         = ::siAVDisplayBehavior,
		siChannelAvailable          = ::siChannelAvailable,
		siCompressionAvailable      = ::siCompressionAvailable,
		siCompressionChannels       = ::siCompressionChannels,
		siCompressionFactor         = ::siCompressionFactor,
		siCompressionHeader         = ::siCompressionHeader,
		siCompressionNames          = ::siCompressionNames,
		siCompressionParams         = ::siCompressionParams,
		siCompressionSampleRate     = ::siCompressionSampleRate,
		siCompressionType           = ::siCompressionType,
		siContinuous                = ::siContinuous,
		siDecompressionParams       = ::siDecompressionParams,
		siDeviceBufferInfo          = ::siDeviceBufferInfo,
		siDeviceConnected           = ::siDeviceConnected,
		siDeviceIcon                = ::siDeviceIcon,
		siDeviceName                = ::siDeviceName,
		siEQSpectrumBands           = ::siEQSpectrumBands,
		siEQSpectrumLevels          = ::siEQSpectrumLevels,
		siEQSpectrumOnOff           = ::siEQSpectrumOnOff,
		siEQSpectrumResolution      = ::siEQSpectrumResolution,
		siEQToneControlGain         = ::siEQToneControlGain,
		siEQToneControlOnOff        = ::siEQToneControlOnOff,
		siHardwareBalance           = ::siHardwareBalance,
		siHardwareBalanceSteps      = ::siHardwareBalanceSteps,
		siHardwareBass              = ::siHardwareBass,
		siHardwareBassSteps         = ::siHardwareBassSteps,
		siHardwareBusy              = ::siHardwareBusy,
		siHardwareFormat            = ::siHardwareFormat,
		siHardwareMute              = ::siHardwareMute,
		siHardwareMuteNoPrefs       = ::siHardwareMuteNoPrefs,
		siHardwareTreble            = ::siHardwareTreble,
		siHardwareTrebleSteps       = ::siHardwareTrebleSteps,
		siHardwareVolume            = ::siHardwareVolume,
		siHardwareVolumeSteps       = ::siHardwareVolumeSteps,
		siHeadphoneMute             = ::siHeadphoneMute,
		siHeadphoneVolume           = ::siHeadphoneVolume,
		siHeadphoneVolumeSteps      = ::siHeadphoneVolumeSteps,
		siInputAvailable            = ::siInputAvailable,
		siInputGain                 = ::siInputGain,
		siInputSource               = ::siInputSource,
		siInputSourceNames          = ::siInputSourceNames,
		siLevelMeterOnOff           = ::siLevelMeterOnOff,
		siModemGain                 = ::siModemGain,
		siMonitorAvailable          = ::siMonitorAvailable,
		siMonitorSource             = ::siMonitorSource,
		siNumberChannels            = ::siNumberChannels,
		siOptionsDialog             = ::siOptionsDialog,
		siOSTypeInputSource         = ::siOSTypeInputSource,
		siOSTypeInputAvailable      = ::siOSTypeInputAvailable,
		siOutputDeviceName          = ::siOutputDeviceName,
		siPlayThruOnOff             = ::siPlayThruOnOff,
		siPostMixerSoundComponent   = ::siPostMixerSoundComponent,
		siPreMixerSoundComponent    = ::siPreMixerSoundComponent,
		siQuality                   = ::siQuality,
		siRateMultiplier            = ::siRateMultiplier,
		siRecordingQuality          = ::siRecordingQuality,
		siSampleRate                = ::siSampleRate,
		siSampleRateAvailable       = ::siSampleRateAvailable,
		siSampleSize                = ::siSampleSize,
		siSampleSizeAvailable       = ::siSampleSizeAvailable,
		siSetupCDAudio              = ::siSetupCDAudio,
		siSetupModemAudio           = ::siSetupModemAudio,
		siSlopeAndIntercept         = ::siSlopeAndIntercept,
		siSoundClock                = ::siSoundClock,
		siUseThisSoundClock         = ::siUseThisSoundClock,
		siSpeakerMute               = ::siSpeakerMute,
		siSpeakerVolume             = ::siSpeakerVolume,
		siSSpCPULoadLimit           = ::siSSpCPULoadLimit,
		siSSpLocalization           = ::siSSpLocalization,
		siSSpSpeakerSetup           = ::siSSpSpeakerSetup,
		siStereoInputGain           = ::siStereoInputGain,
		siSubwooferMute             = ::siSubwooferMute,
		siTerminalType              = ::siTerminalType,
		siTwosComplementOnOff       = ::siTwosComplementOnOff,
		siVendorProduct             = ::siVendorProduct,
		siVolume                    = ::siVolume,
		siVoxRecordInfo             = ::siVoxRecordInfo,
		siVoxStopInfo               = ::siVoxStopInfo,
		siWideStereo                = ::siWideStereo,
		siSupportedExtendedFlags    = ::siSupportedExtendedFlags,
		siRateConverterRollOffSlope = ::siRateConverterRollOffSlope,
		siOutputLatency             = ::siOutputLatency,
	
		siCloseDriver       = ::siCloseDriver,
		siInitializeDriver  = ::siInitializeDriver,
		siPauseRecording    = ::siPauseRecording,
		siUserInterruptProc = ::siUserInterruptProc,
		
		kSoundInputDeviceInfoType_Max = nucleus::enumeration_traits< ::OSType >::max
	};
	
	typedef SoundInputDeviceInfoType SoundInputDeviceInformationSelector;
	
	using ::SPB;
	
	enum SoundInputReferenceNumber
	{
		kSoundInputReferenceNumber_Max = nucleus::enumeration_traits< long >::max
	};
	
	typedef SoundInputReferenceNumber SoundInputRefNum;
	
	enum SoundInputPermissions
	{
		kSoundInputPermissions_Max = nucleus::enumeration_traits< short >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( SoundInputPermissions )
	
}

namespace Nucleus
{
	
	template <>
	struct Disposer< Nitrogen::SoundInputRefNum > : public std::unary_function< Nitrogen::SoundInputRefNum, void >,
                                                    private Nitrogen::DefaultDestructionOSStatusPolicy
	{
		void operator()( Nitrogen::SoundInputRefNum refNum ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::SoundManager );
			
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
			result.data  = Nucleus::Owned< T**, Nucleus::Disposer< Handle > >::Seize( handle );
			
			return result;
		}
	};
	
	template < SoundInputDeviceInfoType infoType >  struct SoundInputDeviceInfoType_Traits;
	
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
			Str255                                                             deviceName;
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
	
	void SPBGetDeviceInfo( SoundInputRefNum          refNum,
	                       SoundInputDeviceInfoType  infoType,
	                       void*                     infoData );
	
	// 2873
	void SPBSetDeviceInfo( SoundInputRefNum          refNum,
	                       SoundInputDeviceInfoType  infoType,
	                       void*                     infoData );
	
	template < SoundInputDeviceInfoType infoType >
	typename SoundInputDeviceInfoType_Traits< infoType >::Result SPBGetDeviceInfo( SoundInputRefNum refNum )
	{
		typedef SoundInputDeviceInfoType_Traits< infoType > Traits;
		
		typename Traits::GetBuffer infoData;
		
		SPBGetDeviceInfo( refNum, infoType, &infoData );
		
		return Traits::ProcessGetBuffer( infoData );
	}
	
	template < SoundInputDeviceInfoType infoType >
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
	
	class SoundInputDevice_ContainerSpecifics : public OSStatus_EndOfEnumeration< siBadSoundInDevice >
	{
		public:
			typedef SPBGetIndexedDevice_Result value_type;
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			static size_type begin_position()   { return 1; }
			static size_type end_position()     { return 0; }
			
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
	
}

#endif

