/*	=================
 *	PlainIcon_data.hh
 *	=================
 */

#ifndef GENIE_FILESYSTEM_PLAINICON_DATA_HH
#define GENIE_FILESYSTEM_PLAINICON_DATA_HH

// Nucleus
#include "Nucleus/Shared.h"

// Nitrogen
#include "Nitrogen/MacMemory.h"

// Genie
#include "Genie/FileSystem/Views.hh"


namespace Genie
{
	
	class FSTree_Icon_data : public FSTree
	{
		private:
			Nucleus::Shared< Nitrogen::Handle > itsData;
		
		public:
			FSTree_Icon_data( const FSTreePtr&                            parent,
			                  const std::string&                          name,
			                  const Nucleus::Shared< Nitrogen::Handle >&  data );
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			off_t GetEOF() const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
}

#endif

