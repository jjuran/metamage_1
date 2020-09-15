/*
	Genie/FS/HFS/LongName.cc
	------------------------
*/

#include "Genie/FS/HFS/LongName.hh"

// plus
#include "plus/string.hh"

// Nitrogen
#include "Nitrogen/Files.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	void SetLongName( const FSSpec& item, const plus::string& name )
	{
		if ( name.length() > 31 )
		{
			try
			{
				N::FSpDTSetComment( item, name );
			}
			catch ( const Mac::OSStatus& err )
			{
				if ( err != paramErr )
				{
					throw;
				}
			}
		}
	}
	
}
