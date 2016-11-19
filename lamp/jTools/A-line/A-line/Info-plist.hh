/*
	Info-plist.hh
	-------------
*/

#ifndef ALINE_INFOPLIST_HH
#define ALINE_INFOPLIST_HH

// plus
#include "plus/string.hh"

// A-line
#include "A-line/Task.hh"


namespace tool
{
	
	struct Info_plist_error {};
	
	class InfoPListTask : public FileTask
	{
		private:
			const plus::string&  its_name;
			plus::string         its_input;
		
		public:
			InfoPListTask( const plus::string&  name,
			               const plus::string&  input,
			               const plus::string&  contents );
			
			void Make();
			
			void Return( bool succeeded );
	};
	
}

#endif
