// AppleTalk.h

#ifndef CLASSICTOOLBOX_APPLETALK_H
#define CLASSICTOOLBOX_APPLETALK_H

// Universal Interfaces
#ifndef __APPLETALK__
#include <AppleTalk.h>
#endif

// Nitrogen
#ifndef NITROGEN_STR_H
#include "Nitrogen/Str.h"
#endif
#ifndef NITROGEN_MAKE_H
#include "Nitrogen/Make.h"
#endif


namespace Nitrogen
{
	
	void RegisterAppleTalkErrors();
	
#if CALL_NOT_IN_CARBON
	
	using ::EntityName;
	
	template <>  struct Maker< EntityName >
	{
		EntityName operator()
		(
			ConstStr32Param object, 
			ConstStr32Param type, 
			ConstStr32Param zone = "\p*"
		)
		const
		{
			EntityName entityName;
			
			CopyToPascalString( object, entityName.objStr,  32 );
			CopyToPascalString( type,   entityName.typeStr, 32 );
			CopyToPascalString( zone,   entityName.zoneStr, 32 );
			
			return entityName;
		}
	};
	
	static EntityName NBPSetEntity
	(
		ConstStr32Param object, 
		ConstStr32Param type, 
		ConstStr32Param zone = "\p*" )
	{
		EntityName entityName;
		::NBPSetEntity( reinterpret_cast< char* >( &entityName ), object, type, zone );
		return entityName;
	}
	
#endif  // #if CALL_NOT_IN_CARBON
	
}

#endif
