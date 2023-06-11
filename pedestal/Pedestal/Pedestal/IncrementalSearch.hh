/*	====================
 *	IncrementalSearch.hh
 *	====================
 */

#ifndef PEDESTAL_INCREMENTALSEARCH_HH
#define PEDESTAL_INCREMENTALSEARCH_HH

// Debug
#include "debug/boost_assert.hh"

// plus
#include "plus/string_fwd.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// Pedestal
#ifndef PEDESTAL_QUASIMODE_HH
#include "Pedestal/Quasimode.hh"
#endif


namespace Pedestal
{
	
	class TextEdit;
	
	struct TextSelection
	{
		unsigned  start;
		unsigned  end;
		
		TextSelection() : start(), end()
		{
		}
		
		bool defined() const  { return int( start ) >= 0; }
		
		void undefine()  { start = unsigned( -1 ); }
	};
	
	inline bool operator==( const TextSelection& a, const TextSelection& b )
	{
		return a.start == b.start  &&  a.end == b.end;
	}
	
	inline bool operator!=( const TextSelection& a, const TextSelection& b )
	{
		return !( a == b );
	}
	
	
	short TextSearch( const char*           text,
	                  std::size_t           text_length,
	                  const char*           pattern,
	                  std::size_t           pattern_length,
	                  const TextSelection&  selection,
	                  bool                  backward,
	                  bool                  matchAtPosition );
	
	
	void ResetArrowKeyChordability();
	
	bool Try_ArrowKeyChord( TextEdit& editor, char c );
	
	
	boost::intrusive_ptr< Quasimode >
	//
	New_IncrementalSearchQuasimode( TextEdit& editor, bool backward );
	
	bool Try_RepeatSearch( TextEdit& editor, const EventRecord& event );
	
}

#endif
