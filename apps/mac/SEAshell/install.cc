/*
	install.cc
	----------
	
	SEA Shell for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	SEA Shell is the core of a self-extracting archive (SEA).
	
	This module implements the actual file extraction.
	
*/

#include "install.hh"

// mac-types
#include "mac_types/VRefNum_DirID.hh"

// Nitrogen
#include "Mac/Toolbox/Types/OSStatus.hh"

// MacBinaryLib
#include "MacBinary.hh"

// SEAshell
#include "payload.hh"


OSErr install_to( const FSSpec& location )
{
	using mac::types::VRefNum_DirID;
	
	const VRefNum_DirID& dir = *(const VRefNum_DirID*) &location;
	
	MacBinary::Decoder decoder( dir, payload_name );
	
	decoder.SetAlwaysInit( true );
	
	try
	{
		decoder.Write( (Ptr) payload_data, payload_size );
		
		return noErr;
	}
	catch ( const Mac::OSStatus& err )
	{
		return err;
	}
	catch ( ... )
	{
		return ioErr;
	}
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
