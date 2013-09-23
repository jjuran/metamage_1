/*	===================
 *	FSTree_IconSuite.cc
 *	===================
 */

#include "Genie/FS/FSTree_IconSuite.hh"

// POSIX
#include <sys/stat.h>

// vfs
#include "vfs/node.hh"
#include "vfs/primitives/attach.hh"

// Genie
#include "Genie/FS/file_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static void iconsuite_copyfile( const FSTree* that, const FSTree* target );
	
	static const file_method_set iconsuite_file_methods =
	{
		NULL,
		&iconsuite_copyfile
	};
	
	static const node_method_set iconsuite_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&iconsuite_file_methods
	};
	
	
	static void dispose_iconsuite( const FSTree* that )
	{
		::IconSuiteRef& extra = *(::IconSuiteRef*) that->extra();
		
		const bool disposeData = true;
		
		(void) ::DisposeIconSuite( extra, disposeData );
	}
	
	
	static N::IconSuiteRef gStoredIconSuite;
	
	struct stored_IconSuite_scope
	{
		stored_IconSuite_scope( N::IconSuiteRef icon )
		{
			gStoredIconSuite = icon;
		}
		
		~stored_IconSuite_scope()
		{
			gStoredIconSuite = NULL;
		}
	};
	
	static void iconsuite_copyfile( const FSTree* that, const FSTree* target )
	{
		::IconSuiteRef extra = *(::IconSuiteRef*) that->extra();
		
		stored_IconSuite_scope scope( extra );
		
		attach( *target, *that );
	}
	
	
	FSTreePtr
	//
	New_FSTree_IconSuite( const FSTree*                parent,
			              const plus::string&          name,
			              n::owned< N::IconSuiteRef >  iconSuite )
	{
		FSTree* result = new FSTree( parent,
		                             name,
		                             S_IFREG | 0400,
		                             &iconsuite_methods,
		                             sizeof (::IconSuiteRef),
		                             &dispose_iconsuite );
		
		::IconSuiteRef& extra = *(::IconSuiteRef*) result->extra();
		
		extra = iconSuite.release();
		
		return result;
	}
	
	const N::IconSuiteRef Fetch_IconSuite()
	{
		return gStoredIconSuite;
	}
	
}

