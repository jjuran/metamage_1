/*	==========
 *	Control.hh
 *	==========
 */

#ifndef PEDESTAL_CONTROL_HH
#define PEDESTAL_CONTROL_HH

// Nitrogen
#include "Nitrogen/Controls.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	inline void SetBounds( ControlRef control, const Rect& bounds )
	{
		N::SetControlBounds( control, bounds );
	}
	
	using N::ControlPartCode;
	using N::ControlPartCode;
	using N::FindControl_Result;
	
	bool TrackControl( const FindControl_Result& param, Point startPoint );
	
	void SetValueStretch( ControlRef control, short value );
	
	typedef void ( *ControlTracker )( ControlRef control, ControlPartCode part, Point point );
	
	struct Control_Hooks
	{
		void* data;
		ControlTracker trackHook;
		
		Control_Hooks()	: data( NULL ), trackHook( NULL )  {}

	};
	
	class Control_InvariantDetails
	{
		public:
			typedef ControlRef object_type;
			typedef WindowRef container_type;
			
			static void Move( object_type object, short h, short v );
			
			static void Resize( object_type object, short width, short height );
			
			static void SetBounds( object_type object, const Rect& bounds );
	};
	
	class Control_DefaultDetails : public Control_InvariantDetails
	{
		public:
			static void TrackControl( object_type object, ControlPartCode part, Point point );
	};
	
	class Control
	{
		protected:
			ControlRef macControl;
			Control_Hooks controlHooks;
		
		public:
			Control(const Rect& bounds);
			~Control()  {}
			
			ControlRef Get() const  { return macControl; }
			
			// Pane management
			//void SetMinimumClip(short min);
			//void SetMaximumClip(short max);
			//void SetValuePin(short val);
			void SetValueStretch(short val);
			
			void Activate();
			void Deactivate();
			void Resize(short width, short height);
	};
	
	inline Rect Bounds( const Control& view )
	{
		return N::GetControlBounds( view.Get() );
	}
	
}

#endif

