/*	=================
 *	FSSpecForkUser.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_FSSPECFORKUSER_HH
#define GENIE_FILESYSTEM_FSSPECFORKUSER_HH

// Nitrogen
#include "Nitrogen/Files.h"

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	class FSSpecForkUser
	{
		public:
			virtual Nucleus::Owned< Nitrogen::FSFileRefNum > OpenFork( const FSSpec& fileSpec, Nitrogen::FSIOPermissions perm ) const = 0;
			
			virtual boost::shared_ptr< IOHandle > NewFileHandle( Nucleus::Owned< Nitrogen::FSFileRefNum > refNum, OpenFlags flags ) const = 0;
			
			boost::shared_ptr< IOHandle > OpenFileHandle( const FSSpec& fileSpec, OpenFlags flags ) const;
	};
	
	class DataForkUser : public FSSpecForkUser
	{
		public:
			Nucleus::Owned< Nitrogen::FSFileRefNum > OpenFork( const FSSpec& fileSpec, Nitrogen::FSIOPermissions perm ) const
			{
				return Nitrogen::FSpOpenDF( fileSpec, perm );
			}
			
			boost::shared_ptr< IOHandle > NewFileHandle( Nucleus::Owned< Nitrogen::FSFileRefNum > refNum, OpenFlags flags ) const;
	};
	
	class ResourceForkUser : public FSSpecForkUser
	{
		public:
			Nucleus::Owned< Nitrogen::FSFileRefNum > OpenFork( const FSSpec& fileSpec, Nitrogen::FSIOPermissions perm ) const
			{
				return Nitrogen::FSpOpenRF( fileSpec, perm );
			}
			
			boost::shared_ptr< IOHandle > NewFileHandle( Nucleus::Owned< Nitrogen::FSFileRefNum > refNum, OpenFlags flags ) const;
	};
	
}

#endif

