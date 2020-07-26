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

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

#if ! __LP64__

// Mac OS
#ifndef __SOUND__
#include <Sound.h>
#endif

// nucleus
#ifndef NUCLEUS_ENUMERATIONTRAITS_HH
#include "nucleus/enumeration_traits.hh"
#endif
#ifndef NUCLEUS_FLAGOPS_HH
#include "nucleus/flag_ops.hh"
#endif
#ifndef NUCLEUS_INDEXUNTILERRORSEQUENCE_HH
#include "nucleus/index_until_error_sequence.hh"
#endif

// Nitrogen
#ifndef MAC_TOOLBOX_TYPES_FIXED_HH
#include "Mac/Toolbox/Types/Fixed.hh"
#endif

#ifndef NITROGEN_ICONS_HH
#include "Nitrogen/Icons.hh"
#endif
#ifndef NITROGEN_MACMEMORY_HH
#include "Nitrogen/MacMemory.hh"
#endif
#ifndef NITROGEN_OSSTATUS_HH
#include "Nitrogen/OSStatus.hh"
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

namespace nucleus
{
	
	template <>
	struct disposer< Nitrogen::SoundInputRefNum >
	{
		typedef Nitrogen::SoundInputRefNum  argument_type;
		typedef void                        result_type;
		
		void operator()( Nitrogen::SoundInputRefNum refNum ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::SoundManager );
			
			(void) ::SPBCloseDevice( refNum );
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
	
	struct SPBGetDeviceInfo_VariableLengthArray
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
	
}

namespace nucleus
{
	
	template <>
	struct disposer< Nitrogen::SPBGetDeviceInfo_VariableLengthArray >
	{
		typedef Nitrogen::SPBGetDeviceInfo_VariableLengthArray  argument_type;
		typedef void                                            result_type;
		
		void operator()( const Nitrogen::SPBGetDeviceInfo_VariableLengthArray& array ) const
		{
			disposer< Nitrogen::Handle >()( array.data );
		}
	};
	
}

namespace Nitrogen
{
	
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
		
		static Result    ProcessGetBuffer( const GetBuffer& buffer )  { return Mac::UnsignedFixedToDouble( buffer ); }
		static SetBuffer PrepareSetBuffer( const Parameter& param  )  { return Mac::DoubleToUnsignedFixed( param  ); }
	};
	
	template < class T >  struct InfoData_Traits< T** >
	{
		typedef nucleus::owned< SPBGetDeviceInfo_VariableLengthArray > Result;
		
		typedef SPBGetDeviceInfo_VariableLengthArray GetBuffer;
	
		static Result ProcessGetBuffer( const GetBuffer& buffer )
		{
			return Result::seize( buffer );
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
		typedef nucleus::owned< Handle > Result;
		typedef ::Handle GetBuffer;
		
		static Result ProcessGetBuffer( const GetBuffer& buffer )  { return nucleus::owned< Handle >::seize( buffer ); }
	};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siDeviceIcon >
	{
		typedef nucleus::owned< MaskedIconHandle > Result;
		typedef ::Handle GetBuffer;
		
		static Result ProcessGetBuffer( const GetBuffer& buffer )  { return Result::seize( Handle_Cast< MaskedIcon >( buffer ) ); }
	};
	
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleSizeAvailable > : InfoData_Traits< UInt16** >  {};
	template <>  struct SoundInputDeviceInfoType_Traits< siSampleRateAvailable > : InfoData_Traits< ::UnsignedFixed** >  {};
	
	// SysBeep
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
	
	struct SPB_IndexedDevice
	{
		Str255  deviceName;
		Handle  deviceIconHandle;
		
		operator ConstStr255Param() const  { return deviceName; }
	};
	
}

namespace nucleus
{
	
	template <>
	struct disposer< Nitrogen::SPB_IndexedDevice >
	{
		typedef Nitrogen::SPB_IndexedDevice  argument_type;
		typedef void                         result_type;
		
		void operator()( const Nitrogen::SPB_IndexedDevice& device ) const
		{
			NUCLEUS_REQUIRE_ERRORS( Nitrogen::SoundManager );
			
			disposer< Nitrogen::Handle >()( device.deviceIconHandle );
		}
	};
	
}

namespace Nitrogen
{	
	
	typedef nucleus::owned< SPB_IndexedDevice > SPBGetIndexedDevice_Result;
	
	SPBGetIndexedDevice_Result SPBGetIndexedDevice( std::size_t count );
	
	Str255 SPBGetIndexedDevice_Name( std::size_t index );
	
	nucleus::owned< SoundInputRefNum > SPBOpenDevice( ConstStr255Param       deviceName,
	                                                  SoundInputPermissions  permission );
	
	void SPBCloseDevice( nucleus::owned< SoundInputRefNum > );
	
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
	
	class SoundInputDevice_Name_Specifics : public OSStatus_EndOfEnumeration< siBadSoundInDevice >
	{
		public:
			typedef Str255 value_type;
			typedef UInt16 size_type;
			typedef SInt16 difference_type;
			
			static size_type begin_position()   { return 1; }
			static size_type end_position()     { return 0; }
			
			value_type get_value( size_type position )
			{
				return SPBGetIndexedDevice_Name( position );
			}
	};
	
	class SoundInputDevice_Name_Sequence : public nucleus::index_until_error_sequence< SoundInputDevice_Name_Specifics >
	{
		friend SoundInputDevice_Name_Sequence SoundInputDevice_Names();
		
		private:
			typedef ::Nitrogen::SoundInputDevice_Name_Specifics Specifics;
			
			SoundInputDevice_Name_Sequence()
			:
				nucleus::index_until_error_sequence< Specifics >( Specifics() )
			{}
	};
	
	inline SoundInputDevice_Name_Sequence SoundInputDevice_Names()
	{
		return SoundInputDevice_Name_Sequence();
	}
	
}

#endif  // #if ! __LP64__

#endif
