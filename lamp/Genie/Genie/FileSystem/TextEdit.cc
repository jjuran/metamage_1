/*	===========
 *	TextEdit.cc
 *	===========
 */

#include "Genie/FileSystem/TextEdit.hh"

// Standard C++
#include <map>

// Nitrogen
#include "Nitrogen/QuickDraw.h"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	
	typedef std::map< const FSTree*, TextEditParameters > TextEditParametersMap;
	
	static TextEditParametersMap gTextEditParametersMap;
	
	
	TextEditParameters::TextEditParameters()
	:
		itsTextDimensions( N::SetPt( 0, 0 ) ),
		itsValidLength(),
		itHasChangedAttributes(),
		itIsInterlocked(),
		itIsWrapped( true )
	{
	}
	
	TextEditParameters* TextEditParameters::Find( const FSTree* key )
	{
		TextEditParametersMap::iterator it = gTextEditParametersMap.find( key );
		
		if ( it == gTextEditParametersMap.end() )
		{
			return NULL;
		}
		
		return &it->second;
	}
	
	TextEditParameters& TextEditParameters::Get( const FSTree* key )
	{
		return gTextEditParametersMap[ key ];
	}
	
	void TextEditParameters::Erase( const FSTree* key )
	{
		gTextEditParametersMap.erase( key );
	}
	
}

