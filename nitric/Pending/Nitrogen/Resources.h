// Resources.h

#ifndef NITROGEN_RESOURCES_H
#define NITROGEN_RESOURCES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __RESOURCES__
#include FRAMEWORK_HEADER(CarbonCore,Resources.h)
#endif
#ifndef NITROGEN_FILES_H
#include "Nitrogen/Files.h"
#endif
#ifndef NITROGEN_MACMEMORY_H
#include "Nitrogen/MacMemory.h"
#endif
#ifndef NITROGEN_MACTYPES_H
#include "Nitrogen/MacTypes.h"
#endif
#ifndef NITROGEN_IDTYPE_H
#include "Nitrogen/IDType.h"
#endif
#ifndef NITROGEN_FLAGTYPE_H
#include "Nitrogen/FlagType.h"
#endif
#ifndef NITROGEN_ONLYONCE_H
#include "Nitrogen/OnlyOnce.h"
#endif
#ifndef NITROGEN_OWNED_H
#include "Nitrogen/Owned.h"
#endif
#ifndef NITROGEN_PSEUDOREFERENCE_H
#include "Nitrogen/Pseudoreference.h"
#endif

namespace Nitrogen
{
	
	void RegisterResourceManagerErrors();
	
	class ResFileRefNum_Tag {};
	typedef IDType< ResFileRefNum_Tag, ::ResFileRefNum, -1 > ResFileRefNum;
	
	class ResID_Tag {};
	typedef IDType< ResID_Tag, ::ResID, 0 > ResID;
	typedef ResID ResourceID;
	
	class ResAttributes_Tag {};
	typedef FlagType< ResAttributes_Tag, ::ResAttributes, 0 > ResAttributes;
	
	class ResFileAttributes_Tag {};
	typedef FlagType< ResFileAttributes_Tag, ::ResFileAttributes, 0 > ResFileAttributes;
	
	// ResourceReleaser is not used as a Disposer for Owned because resource 
	// handles are owned by the Resource Manager, not the application.
	// But here it is anyway for completeness, in case someone finds it useful.
	
	struct ResourceReleaser : public std::unary_function< Handle, void >, 
	                          private DefaultDestructionOSStatusPolicy
	{
		void operator()( Handle h ) const
		{
			OnlyOnce< RegisterResourceManagerErrors >();
			::ReleaseResource( h );
			HandleDestructionOSStatus( ::ResError() );
		}
	};
	
	template <>
	struct Disposer< ResFileRefNum > : public std::unary_function< ResFileRefNum, void >,
	                                   private DefaultDestructionOSStatusPolicy
	{
		void operator()( ResFileRefNum resFile ) const
		{
			OnlyOnce< RegisterResourceManagerErrors >();
			::CloseResFile( resFile );
			HandleDestructionOSStatus( ::ResError() );
		}
	};
	
	Handle CheckResource( Handle h );
	
	template < class T >
	T** CheckResource( T** h )
	{
		return Handle_Cast< T >( CheckResource( Handle( h ) ) );
	}
	
	// 185
	void CloseResFile( Owned< ResFileRefNum > resFileRefNum );
	
	// 197
	void ResError();
	
	// 209
	ResFileRefNum CurResFile();
	
	// 260
	void UseResFile( ResFileRefNum resFileRefNum );
	
	// 272
	short CountTypes();
	
	// 284
	short Count1Types();
	
	// 296
	ResType GetIndType( short index );
	
	// 310
	ResType Get1IndType( short index );
	
	// 336
	short CountResources( ResType type );
	
	// 348
	short Count1Resources( ResType type );
	
	// 360
	Handle GetIndResource( ResType type, short index );
	
	// 374
	Handle Get1IndResource( ResType type, short index );
	
	// 388
	Handle GetResource( ResType type, ResID resID );
	
	// 402
	Handle Get1Resource( ResType type, ResID resID );
	
	// 471
	void DetachResource( Owned< Handle > h );  // invalid, not defined
	Owned< Handle >  DetachResource( Handle h );
	
	struct GetResInfo_Result
	{
		ResID   id;
		ResType type;
		::Str255  name;
		
		/*
		operator ResID           () const  { return id; }
		operator ResType         () const  { return type; }
		operator ConstStr255Param() const  { return name; }
		*/
	};
	
	// 519
	GetResInfo_Result GetResInfo( Handle h );
	
	// 550
	Handle AddResource( Owned< Handle > h, ResType type, ResID resID, ConstStr255Param name );
	Handle AddResource( Owned< Handle > h, const GetResInfo_Result& resInfo );
	
	// 768
	Owned< ResFileRefNum > FSpOpenResFile( const FSSpec&   spec,
	                                       FSIOPermssn     permissions );
	
	// 1256
	Owned< ResFileRefNum > FSOpenResourceFile(
		const FSRef& ref, 
		UniCharCount forkNameLength, 
		const UniChar* forkName, 
		FSIOPermssn permissions
	);
	
	Owned< ResFileRefNum > FSOpenResourceFile(
		const FSRef& ref, 
		const UniString& forkName, 
		FSIOPermssn permissions
	);
	
	class ResFile_Details
	{
		public:
			typedef ResFileRefNum Value;
			typedef Value GetResult;
			typedef Value SetParameter;
			
			GetResult Get() const                        { return CurResFile();   }
			void Set( SetParameter resFile ) const       { UseResFile( resFile ); }
	};
	
	typedef Pseudoreference< ResFile_Details > ResFile;
	
}

#endif

