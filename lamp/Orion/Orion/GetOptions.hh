/*	=============
 *	GetOptions.hh
 *	=============
 */

#ifndef ORION_GETOPTIONS_HH
#define ORION_GETOPTIONS_HH

// Standard C++
#include <map>
#include <set>
#include <string>
#include <vector>


namespace Orion
{
	
	using std::map;
	using std::pair;
	using std::set;
	using std::string;
	using std::vector;
	
	class Options
	{
		private:
			bool ParamExpected(const string& opt) const;
			
			void SetOption(const string& opt);
			void SetOption(const string& opt, const string& param);
			
			map<string, int> myFlagDefs;
			map<string, int> myIntegerDefs;
			map<string, int> myStringDefs;
			map<string, int> myStringListDefs;
			map<string, pair<int, int> > myEnumDefs;
			
			set<int> myFlags;
			map<int, int> myIntegers;
			map<int, string> myStrings;
			map<int, vector<string> > myStringLists;
			map<int, int> myEnums;
			
			vector<const char*> myFreeParams;
		
		public:
			void DefineSetFlag(const string& opt, int code);
			void DefineSetInteger(const string& opt, int code);
			void DefineSetString(const string& opt, int code);
			void DefineAppendToStringList(const string& opt, int code);
			
			void DefineSelectEnum(const string& opt, int code, int enumParam);
			
			void GetOptions(int argc, const char *const argv[]);
			
			bool GetFlag(int code) const;
			int GetInteger(int code) const;
			string GetString(int code) const;
			vector<string> GetStringList(int code) const;
			int GetEnum(int code) const;
			
			const vector<const char*>& GetFreeParams() const  { return myFreeParams; }
	};
	
}

#endif

