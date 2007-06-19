/*	=============
 *	Exceptions.hh
 *	=============
 */

#pragma once

// A-line
#include "A-line/Project.hh"


namespace ALine
{
	
	struct NoSuchProject  {};
	
	struct NoSuchUsedProject
	{
		NoSuchUsedProject( const ProjName& projName, const std::string& used )
		:
			projName( projName ), 
			used    ( used )
		{}
		
		ProjName projName;
		std::string used;
	};
	
	/*
	struct BadSourceAlias
	{
		BadSourceAlias( const Project& proj, const FSSpec& alias ) 
		:
			proj ( proj ), 
			alias( alias )
		{}
		
		const Project& proj;
		FSSpec alias;
	};
	*/
	
}

