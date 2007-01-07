// Sound.h

#ifndef NITROGEN_SOUND_H
#define NITROGEN_SOUND_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __SOUND__
#include FRAMEWORK_HEADER(CarbonSound,Sound.h)
#endif

// Nucleus
#ifndef NUCLEUS_FLAGTYPE_H
#include "Nucleus/Flag.h"
#endif
#ifndef NUCLEUS_IDTYPE_H
#include "Nucleus/ID.h"
#endif
#ifndef NUCLEUS_INDEXUNTILFAILURECONTAINER_H
#include "Nucleus/IndexUntilFailureContainer.h"
#endif
#ifndef NUCLEUS_OWNED_H
#include "Nucleus/Owned.h"
#endif
#ifndef NUCLEUS_SELECTOR_H
#include "Nucleus/Selector.h"
#endif
#ifndef NUCLEUS_TRANSFERTRAITS_H
#include "Nucleus/TransferTraits.h"
#endif

// Nitrogen
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
	
	class SoundManagerErrorsRegistrationDependency
	{
		public:
			SoundManagerErrorsRegistrationDependency();
	};
	
	typedef Nucleus::Selector< class SoundInputDeviceInfoType_Tag, ::OSType >::Type SoundInputDeviceInfoType;
	
	typedef SoundInputDeviceInfoType SoundInputDeviceInformationSelector;
	
	static const SoundInputDeviceInfoType siActiveChannels            = SoundInputDeviceInfoType( ::siActiveChannels            );
	static const SoundInputDeviceInfoType siActiveLevels              = SoundInputDeviceInfoType( ::siActiveLevels              );
	static const SoundInputDeviceInfoType siAGCOnOff                  = SoundInputDeviceInfoType( ::siAGCOnOff                  );
	static const SoundInputDeviceInfoType siAsync                     = SoundInputDeviceInfoType( ::siAsync                     );
	static const SoundInputDeviceInfoType siAVDisplayBehavior         = SoundInputDeviceInfoType( ::siAVDisplayBehavior         );
	static const SoundInputDeviceInfoType siChannelAvailable          = SoundInputDeviceInfoType( ::siChannelAvailable          );
	static const SoundInputDeviceInfoType siCompressionAvailable      = SoundInputDeviceInfoType( ::siCompressionAvailable      );
	static const SoundInputDeviceInfoType siCompressionChannels       = SoundInputDeviceInfoType( ::siCompressionChannels       );
	static const SoundInputDeviceInfoType siCompressionFactor         = SoundInputDeviceInfoType( ::siCompressionFactor         );
	static const SoundInputDeviceInfoType siCompressionHeader         = SoundInputDeviceInfoType( ::siCompressionHeader         );
	static const SoundInputDeviceInfoType siCompressionNames          = SoundInputDeviceInfoType( ::siCompressionNames          );
	static const SoundInputDeviceInfoType siCompressionParams         = SoundInputDeviceInfoType( ::siCompressionParams         );
	static const SoundInputDeviceInfoType siCompressionSampleRate     = SoundInputDeviceInfoType( ::siCompressionSampleRate     );
	static const SoundInputDeviceInfoType siCompressionType           = SoundInputDeviceInfoType( ::siCompressionType           );
	static const SoundInputDeviceInfoType siContinuous                = SoundInputDeviceInfoType( ::siContinuous                );
	static const SoundInputDeviceInfoType siDecompressionParams       = SoundInputDeviceInfoType( ::siDecompressionParams       );
	static const SoundInputDeviceInfoType siDeviceBufferInfo          = SoundInputDeviceInfoType( ::siDeviceBufferInfo          );
	static const SoundInputDeviceInfoType siDeviceConnected           = SoundInputDeviceInfoType( ::siDeviceConnected           );
	static const SoundInputDeviceInfoType siDeviceIcon                = SoundInputDeviceInfoType( ::siDeviceIcon                );
	static const SoundInputDeviceInfoType siDeviceName                = SoundInputDeviceInfoType( ::siDeviceName                );
	static const SoundInputDeviceInfoType siEQSpectrumBands           = SoundInputDeviceInfoType( ::siEQSpectrumBands           );
	static const SoundInputDeviceInfoType siEQSpectrumLevels          = SoundInputDeviceInfoType( ::siEQSpectrumLevels          );
	static const SoundInputDeviceInfoType siEQSpectrumOnOff           = SoundInputDeviceInfoType( ::siEQSpectrumOnOff           );
	static const SoundInputDeviceInfoType siEQSpectrumResolution      = SoundInputDeviceInfoType( ::siEQSpectrumResolution      );
	static const SoundInputDeviceInfoType siEQToneControlGain         = SoundInputDeviceInfoType( ::siEQToneControlGain         );
	static const SoundInputDeviceInfoType siEQToneControlOnOff        = SoundInputDeviceInfoType( ::siEQToneControlOnOff        );
	static const SoundInputDeviceInfoType siHardwareBalance           = SoundInputDeviceInfoType( ::siHardwareBalance           );
	static const SoundInputDeviceInfoType siHardwareBalanceSteps      = SoundInputDeviceInfoType( ::siHardwareBalanceSteps      );
	static const SoundInputDeviceInfoType siHardwareBass              = SoundInputDeviceInfoType( ::siHardwareBass              );
	static const SoundInputDeviceInfoType siHardwareBassSteps         = SoundInputDeviceInfoType( ::siHardwareBassSteps         );
	static const SoundInputDeviceInfoType siHardwareBusy              = SoundInputDeviceInfoType( ::siHardwareBusy              );
	static const SoundInputDeviceInfoType siHardwareFormat            = SoundInputDeviceInfoType( ::siHardwareFormat            );
	static const SoundInputDeviceInfoType siHardwareMute              = SoundInputDeviceInfoType( ::siHardwareMute              );
	static const SoundInputDeviceInfoType siHardwareMuteNoPrefs       = SoundInputDeviceInfoType( ::siHardwareMuteNoPrefs       );
	static const SoundInputDeviceInfoType siHardwareTreble            = SoundInputDeviceInfoType( ::siHardwareTreble            );
	static const SoundInputDeviceInfoType siHardwareTrebleSteps       = SoundInputDeviceInfoType( ::siHardwareTrebleSteps       );
	static const SoundInputDeviceInfoType siHardwareVolume            = SoundInputDeviceInfoType( ::siHardwareVolume            );
	static const SoundInputDeviceInfoType siHardwareVolumeSteps       = SoundInputDeviceInfoType( ::siHardwareVolumeSteps       );
	static const SoundInputDeviceInfoType siHeadphoneMute             = SoundInputDeviceInfoType( ::siHeadphoneMute             );
	static const SoundInputDeviceInfoType siHeadphoneVolume           = SoundInputDeviceInfoType( ::siHeadphoneVolume           );
	static const SoundInputDeviceInfoType siHeadphoneVolumeSteps      = SoundInputDeviceInfoType( ::siHeadphoneVolumeSteps      );
	static const SoundInputDeviceInfoType siInputAvailable            = SoundInputDeviceInfoType( ::siInputAvailable            );
	static const SoundInputDeviceInfoType siInputGain                 = SoundInputDeviceInfoType( ::siInputGain                 );
	static const SoundInputDeviceInfoType siInputSource               = SoundInputDeviceInfoType( ::siInputSource               );
	static const SoundInputDeviceInfoType siInputSourceNames          = SoundInputDeviceInfoType( ::siInputSourceNames          );
	static const SoundInputDeviceInfoType siLevelMeterOnOff           = SoundInputDeviceInfoType( ::siLevelMeterOnOff           );
	static const SoundInputDeviceInfoType siModemGain                 = SoundInputDeviceInfoType( ::siModemGain                 );
	static const SoundInputDeviceInfoType siMonitorAvailable          = SoundInputDeviceInfoType( ::siMonitorAvailable          );
	static const SoundInputDeviceInfoType siMonitorSource             = SoundInputDeviceInfoType( ::siMonitorSource             );
	static const SoundInputDeviceInfoType siNumberChannels            = SoundInputDeviceInfoType( ::siNumberChannels            );
	static const SoundInputDeviceInfoType siOptionsDialog             = SoundInputDeviceInfoType( ::siOptionsDialog             );
	static const SoundInputDeviceInfoType siOSTypeInputSource         = SoundInputDeviceInfoType( ::siOSTypeInputSource         );
	static const SoundInputDeviceInfoType siOSTypeInputAvailable      = SoundInputDeviceInfoType( ::siOSTypeInputAvailable      );
	static const SoundInputDeviceInfoType siOutputDeviceName          = SoundInputDeviceInfoType( ::siOutputDeviceName          );
	static const SoundInputDeviceInfoType siPlayThruOnOff             = SoundInputDeviceInfoType( ::siPlayThruOnOff             );
	static const SoundInputDeviceInfoType siPostMixerSoundComponent   = SoundInputDeviceInfoType( ::siPostMixerSoundComponent   );
	static const SoundInputDeviceInfoType siPreMixerSoundComponent    = SoundInputDeviceInfoType( ::siPreMixerSoundComponent    );
	static const SoundInputDeviceInfoType siQuality                   = SoundInputDeviceInfoType( ::siQuality                   );
	static const SoundInputDeviceInfoType siRateMultiplier            = SoundInputDeviceInfoType( ::siRateMultiplier            );
	static const SoundInputDeviceInfoType siRecordingQuality          = SoundInputDeviceInfoType( ::siRecordingQuality          );
	static const SoundInputDeviceInfoType siSampleRate                = SoundInputDeviceInfoType( ::siSampleRate                );
	static const SoundInputDeviceInfoType siSampleRateAvailable       = SoundInputDeviceInfoType( ::siSampleRateAvailable       );
	static const SoundInputDeviceInfoType siSampleSize                = SoundInputDeviceInfoType( ::siSampleSize                );
	static const SoundInputDeviceInfoType siSampleSizeAvailable       = SoundInputDeviceInfoType( ::siSampleSizeAvailable       );
	static const SoundInputDeviceInfoType siSetupCDAudio              = SoundInputDeviceInfoType( ::siSetupCDAudio              );
	static const SoundInputDeviceInfoType siSetupModemAudio           = SoundInputDeviceInfoType( ::siSetupModemAudio           );
	static const SoundInputDeviceInfoType siSlopeAndIntercept         = SoundInputDeviceInfoType( ::siSlopeAndIntercept         );
	static const SoundInputDeviceInfoType siSoundClock                = SoundInputDeviceInfoType( ::siSoundClock                );
	static const SoundInputDeviceInfoType siUseThisSoundClock         = SoundInputDeviceInfoType( ::siUseThisSoundClock         );
	static const SoundInputDeviceInfoType siSpeakerMute               = SoundInputDeviceInfoType( ::siSpeakerMute               );
	static const SoundInputDeviceInfoType siSpeakerVolume             = SoundInputDeviceInfoType( ::siSpeakerVolume             );
	static const SoundInputDeviceInfoType siSSpCPULoadLimit           = SoundInputDeviceInfoType( ::siSSpCPULoadLimit           );
	static const SoundInputDeviceInfoType siSSpLocalization           = SoundInputDeviceInfoType( ::siSSpLocalization           );
	static const SoundInputDeviceInfoType siSSpSpeakerSetup           = SoundInputDeviceInfoType( ::siSSpSpeakerSetup           );
	static const SoundInputDeviceInfoType siStereoInputGain           = SoundInputDeviceInfoType( ::siStereoInputGain           );
	static const SoundInputDeviceInfoType siSubwooferMute             = SoundInputDeviceInfoType( ::siSubwooferMute             );
	static const SoundInputDeviceInfoType siTerminalType              = SoundInputDeviceInfoType( ::siTerminalType              );
	static const SoundInputDeviceInfoType siTwosComplementOnOff       = SoundInputDeviceInfoType( ::siTwosComplementOnOff       );
	static const SoundInputDeviceInfoType siVendorProduct             = SoundInputDeviceInfoType( ::siVendorProduct             );
	static const SoundInputDeviceInfoType siVolume                    = SoundInputDeviceInfoType( ::siVolume                    );
	static const SoundInputDeviceInfoType siVoxRecordInfo             = SoundInputDeviceInfoType( ::siVoxRecordInfo             );
	static const SoundInputDeviceInfoType siVoxStopInfo               = SoundInputDeviceInfoType( ::siVoxStopInfo               );
	static const SoundInputDeviceInfoType siWideStereo                = SoundInputDeviceInfoType( ::siWideStereo                );
	static const SoundInputDeviceInfoType siSupportedExtendedFlags    = SoundInputDeviceInfoType( ::siSupportedExtendedFlags    );
	static const SoundInputDeviceInfoType siRateConverterRollOffSlope = SoundInputDeviceInfoType( ::siRateConverterRollOffSlope );
	static const SoundInputDeviceInfoType siOutputLatency             = SoundInputDeviceInfoType( ::siOutputLatency             );
	
	static const SoundInputDeviceInfoType siCloseDriver       = SoundInputDeviceInfoType( ::siCloseDriver       );
	static const SoundInputDeviceInfoType siInitializeDriver  = SoundInputDeviceInfoType( ::siInitializeDriver  );
	static const SoundInputDeviceInfoType siPauseRecording    = SoundInputDeviceInfoType( ::siPauseRecording    );
	static const SoundInputDeviceInfoType siUserInterruptProc = SoundInputDeviceInfoType( ::siUserInterruptProc );
	
	using ::SPB;
	
	typedef Nucleus::ID< class SoundInputReferenceNumber_Tag, long >::Type SoundInputReferenceNumber;
	
	typedef SoundInputReferenceNumber SoundInputRefNum;
	
	typedef Nucleus::Flag< class SoundInputPermissions_Tag, short >::Type SoundInputPermissions;
	
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
			(void) Nitrogen::SoundManagerErrorsRegistrationDependency();
			
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
	
}

#endif

