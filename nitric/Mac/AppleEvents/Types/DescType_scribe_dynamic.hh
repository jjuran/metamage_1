/*
	Mac/AppleEvents/Types/DescType_scribe_dynamic.hh
	------------------------------------------------
*/

#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBEDYNAMIC_HH
#define MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBEDYNAMIC_HH

// Mac OS
#ifndef __AEREGISTRY__
#include <AERegistry.h>
#endif

// nucleus
#ifndef NUCLEUS_VARIABLELENGTHPODSCRIBE_HH
#include "nucleus/variable_length_POD_scribe.hh"
#endif

// Nitrogen
#ifndef MAC_APPLEEVENTS_TYPES_DESCTYPESCRIBE_HH
#include "Mac/AppleEvents/Types/DescType_scribe.hh"
#endif
#ifndef MAC_APPLEEVENTS_UTILITIES_SIZEOFOFFSETARRAY_HH
#include "Mac/AppleEvents/Utilities/sizeof_OffsetArray.hh"
#endif
#ifndef MAC_APPLEEVENTS_UTILITIES_SIZEOFTEXTRANGEARRAY_HH
#include "Mac/AppleEvents/Utilities/sizeof_TextRangeArray.hh"
#endif
#ifndef MAC_PROCESSES_UTILITIES_SIZEOFAPPPARAMETERS_HH
#include "Mac/Processes/Utilities/sizeof_AppParameters.hh"
#endif
#ifndef MAC_TOOLBOX_UTILITIES_SIZEOFVERSREC_HH
#include "Mac/Toolbox/Utilities/SizeOf_VersRec.hh"
#endif


namespace Mac
{
	
	// AEDataModel
	// -----------
	
	template <> struct DescType_scribe< typeAppParameters > : type_< nucleus::variable_length_POD_scribe< AppParameters, sizeof_AppParameters > > {};
	
	// AERegistry
	// ----------
	
	template <> struct DescType_scribe< typeVersion        > : type_< nucleus::variable_length_POD_scribe< VersRec,        SizeOf_VersRec        > > {};
	template <> struct DescType_scribe< typeOffsetArray    > : type_< nucleus::variable_length_POD_scribe< OffsetArray,    sizeof_OffsetArray    > > {};
	template <> struct DescType_scribe< typeTextRangeArray > : type_< nucleus::variable_length_POD_scribe< TextRangeArray, sizeof_TextRangeArray > > {};
	
}

#endif

