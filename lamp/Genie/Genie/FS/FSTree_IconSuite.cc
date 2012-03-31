/*	===================
 *	FSTree_IconSuite.cc
 *	===================
 */

#include "Genie/FS/FSTree_IconSuite.hh"

// POSIX
#include <sys/stat.h>

// Genie
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/file_method_set.hh"
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static void iconsuite_copyfile( const FSTree* node, const FSTreePtr& target );
	
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
	
	
	class FSTree_IconSuite : public FSTree
	{
		private:
			typedef n::owned< N::IconSuiteRef > Value;
			
			Value itsIconSuite;
		
		public:
			FSTree_IconSuite( const FSTreePtr&     parent,
			                  const plus::string&  name,
			                  Value                iconSuite );
			
			void CopyFile( const FSTreePtr& destination ) const;
	};
	
	
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
	
	static void iconsuite_copyfile( const FSTree* node, const FSTreePtr& target )
	{
		const FSTree_IconSuite* file = static_cast< const FSTree_IconSuite* >( node );
		
		file->CopyFile( target );
	}
	
	void FSTree_IconSuite::CopyFile( const FSTreePtr& destination ) const
	{
		stored_IconSuite_scope scope( itsIconSuite );
		
		destination->Attach( this );
	}
	
	
	FSTreePtr
	//
	New_FSTree_IconSuite( const FSTreePtr&             parent,
			              const plus::string&          name,
			              n::owned< N::IconSuiteRef >  iconSuite )
	{
		return new FSTree_IconSuite( parent, name, iconSuite );
	}
	
	FSTree_IconSuite::FSTree_IconSuite( const FSTreePtr&     parent,
	                                    const plus::string&  name,
	                                    Value                iconSuite )
	:
		FSTree( parent, name, S_IFREG | 0400, &iconsuite_methods ),
		itsIconSuite( iconSuite )
	{
	}
	
	const N::IconSuiteRef Fetch_IconSuite()
	{
		return gStoredIconSuite;
	}
	
}

