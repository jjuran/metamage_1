// Resources.h

#ifndef NITROGEN_RESOURCES_H
#define NITROGEN_RESOURCES_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __RESOURCES__
#include FRAMEWORK_HEADER(CarbonCore,Resources.h)
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
	
	template <> struct Disposer< ResFileRefNum >
	:
		public std::unary_function< ResFileRefNum, void >,
		private DefaultDestructionOSStatusPolicy
	{
		void operator()( ResFileRefNum resFile ) const
		{
			OnlyOnce< RegisterResourceManagerErrors >();
			::CloseResFile( resFile );
			DefaultDestructionOSStatusPolicy::HandleDestructionOSStatus( ::ResError() );
		}
	};
	
}

#endif

