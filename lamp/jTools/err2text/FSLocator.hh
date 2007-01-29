/*	============
 *	FSLocator.hh
 *	============
 */

#pragma once

#ifndef __FILES__
#include <Files.h>
#endif

// Nitrogen
#include "Nitrogen/Str.h"


namespace FSLocator
{
	
	namespace N = Nitrogen;
	
	class FSLocatorAppBySignature
	{
		private:
			N::OSType mySig;
		
		public:
			FSLocatorAppBySignature( N::OSType sig ) : mySig( sig )  {}
			
			FSSpec Locate() const;
	};
	
	class FSLinkNewName
	{
		private:
			std::string myName;
		
		public:
			FSLinkNewName(const std::string& name) : myName(name)  {}
			
			FSSpec operator()(const FSSpec& source) const
			{
				FSSpec link = source;
				N::CopyToPascalString( myName, link.name, 63 );
				return link;
			}
	};
	
	template < class Locator, class Link >
	class FSLocatorChainedT
	{
		private:
			Locator myPrevNode;
			Link myLink;
		
		public:
			FSLocatorChainedT(const Locator& prevNode, const Link& link)
			:
				myPrevNode(prevNode),
				myLink(link)
			{}
			
			FSSpec Locate() const  { return myLink(myPrevNode.Locate()); }
	};
	
}

