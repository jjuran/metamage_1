/*	======================
 *	FSTree_Stamp_Action.hh
 *	======================
 */

#ifndef GENIE_FILESYSTEM_FSTREESTAMPACTION_HH
#define GENIE_FILESYSTEM_FSTREESTAMPACTION_HH

// POSIX
#include <errno.h>

// POSeven
#include "POSeven/Errno.hh"

// Genie
#include "Genie/FileSystem/FSTree.hh"


// From utime.h
struct utimbuf;


namespace Genie
{
	
	template < class Action >
	class FSTree_Stamp_Action : public FSTree
	{
		private:
			Action itsAction;
		
		public:
			FSTree_Stamp_Action( const FSTreePtr&    parent,
			                     const std::string&  name,
			                     const Action&       action ) : FSTree( parent, name ),
			                                                    itsAction( action  )
			{
			}
			
			void SetUTime( const struct utimbuf* utime_buf ) const;
	};
	
	template < class Action >
	void FSTree_Stamp_Action< Action >::SetUTime( const struct utimbuf* utime_buf ) const
	{
		if ( utime_buf != NULL )
		{
			poseven::throw_errno( EPERM );
		}
		
		itsAction();
	}
	
}

#endif

