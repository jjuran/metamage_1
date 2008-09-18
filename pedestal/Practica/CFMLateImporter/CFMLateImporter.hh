/*	==================
 *	CFMLateImporter.hh
 *	==================
 */

#ifndef CFMLATEIMPORTER_HH
#define CFMLATEIMPORTER_HH

#if PRAGMA_ONCE
#pragma once
#endif

#include <CodeFragments.h>
#include <MacTypes.h>


namespace CFMLateImporter
{
	
	OSStatus ImportLibrary( ConstStr255Param   weakLinkedLibraryName,
	                        CFragConnectionID  connIDToImport );
	
}

#endif

