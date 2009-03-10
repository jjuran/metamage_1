/*	=================
 *	Pedestal/Icons.hh
 *	=================
 */

#ifndef PEDESTAL_ICONS_HH
#define PEDESTAL_ICONS_HH

// Nitrogen
#include "Nitrogen/Icons.h"

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class PlainIcon : public View
	{
		public:
			void Draw( const Rect& bounds, bool erasing );
			
			virtual Nitrogen::Handle Data() const = 0;
			
			virtual Nitrogen::IconAlignmentType Alignment() const  { return Nitrogen::kAlignNone; }
			
			virtual Nitrogen::IconTransformType Transform() const  { return Nitrogen::kTransformNone; }
	};
	
	class IconID : public View
	{
		public:
			struct Undefined {};
			
			void Draw( const Rect& bounds, bool erasing );
			
			virtual Nitrogen::ResID ID() const = 0;
			
			virtual Nitrogen::IconAlignmentType Alignment() const  { return Nitrogen::kAlignNone; }
			
			virtual Nitrogen::IconTransformType Transform() const  { return Nitrogen::kTransformNone; }
	};
	
	class IconSuite : public View
	{
		public:
			void Draw( const Rect& bounds, bool erasing );
			
			virtual Nitrogen::IconSuiteRef Ref() const = 0;
			
			virtual Nitrogen::IconAlignmentType Alignment() const  { return Nitrogen::kAlignNone; }
			
			virtual Nitrogen::IconTransformType Transform() const  { return Nitrogen::kTransformNone; }
	};
	
}

#endif

