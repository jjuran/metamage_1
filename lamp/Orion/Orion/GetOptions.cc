/*	=============
 *	GetOptions.cc
 *	=============
 */

#include "Orion/GetOptions.hh"

// Standard C++
#include <algorithm>
#include <cstring>

// Option vocabulary

// Syntax
// 
// --				end of option marker
// --foo=bar		long format, inline param (always accepted)
// --foo bar		long format, param is required (strings and lists)
// --foo bar		long format, no param, arg follows (enums, flags, counters)
// -bar				long format, X11 style (current behavior, but deprecated)
// -bar				short format, same as -b -a -r (to be the default)
// -bar				short format, "ar" is inline param to -b (short-inlines)
// +z				inverse of -z (not supported)

// Semantics
// 
// Param types:  string, integer, decimal
// Param behaviors:  replace, concat (string only)
// Non-param types:  flag, enum, counter
// Non-param behaviors:  set (flag), select (enum), increment (counter)


namespace Orion
{
	
	using std::make_pair;
	
	void
	Options::DefineSetFlag(const string& opt, int code)
	{
		myFlagDefs[opt] = code;
	}
	
	void
	Options::DefineSetInteger(const string& opt, int code)
	{
		myIntegerDefs[opt] = code;
	}
	
	void
	Options::DefineSetString(const string& opt, int code)
	{
		myStringDefs[opt] = code;
	}
	
	void
	Options::DefineAppendToStringList(const string& opt, int code)
	{
		myStringListDefs[opt] = code;
	}
	
	void
	Options::DefineSelectEnum(const string& opt, int code, int enumParam)
	{
		myEnumDefs[opt] = make_pair(code, enumParam);
	}
	
	bool Options::ParamExpected(const string& opt) const
	{
		if (myFlagDefs      .count(opt))  return false;
		if (myIntegerDefs   .count(opt))  return true;
		if (myStringDefs    .count(opt))  return true;
		if (myStringListDefs.count(opt))  return true;
		if (myEnumDefs      .count(opt))  return false;
		
		return false;  // FIXME
	}
	
	void
	Options::SetOption(const string& opt)
	{
		if (myFlagDefs.count(opt)) {
			myFlags.insert(myFlagDefs[opt]);
		} else if (myEnumDefs.count(opt)) {
			myEnums[myEnumDefs[opt].first] = myEnumDefs[opt].second;
		}
	}
	
	void
	Options::SetOption(const string& opt, const string& param)
	{
		if (myIntegerDefs.count(opt)) {
			// FIXME:  This should check for conversion error
			myIntegers[myIntegerDefs[opt]] = std::atoi(param.c_str());
		} else if (myStringDefs.count(opt)) {
			myStrings[myStringDefs[opt]] = param;
		} else if (myStringListDefs.count(opt)) {
			myStringLists[myStringListDefs[opt]].push_back(param);
		}
	}
	
	void
	Options::GetOptions(int argc, const char *const argv[])
	{
		char const* const* begin = argv + 1;  // Skip the command
		char const* const* end = argv + argc;
		
		for (char const* const* it = begin;  it != end;  ++it) {
			const char* token = *it;
			if (token[0] == '-') {
				if (token[1] == '-') {
					if (token[2] == '\0') {
						// End of option processing
						std::copy(++it, end, back_inserter(myFreeParams));
						break;
					} else {
						// Long format option
						unsigned int len = std::strlen(token);
						const char* eq = std::find(token + 2, token + len, '=');
						if (eq == token + len) {
							// No inline parameter to option
							if (ParamExpected(token)) {
								SetOption(token, *++it);
							} else {
								SetOption(token);
							}
						} else {
							// Option has parameter
							string opt(token, eq - token);
							const char* paramStart = eq + 1;
							unsigned int paramLen = token + len - paramStart;
							string param(paramStart, paramLen);
							SetOption(opt, param);
						}
					}
				} else {
					// Short format option
					if (ParamExpected(token)) {
						SetOption(token, *++it);
					} else {
						SetOption(token);
					}
				}
			} else {
				// Not an option
				myFreeParams.push_back(token);
			}
		}
	}
	
	bool
	Options::GetFlag(int code) const
	{
		return myFlags.count(code);
	}
	
	int
	Options::GetInteger(int code) const
	{
		if (myIntegers.count(code)) {
			return myIntegers.find(code)->second;
		} else {
			return 0;  // FIXME
		}
	}
	
	string
	Options::GetString(int code) const
	{
		if (myStrings.count(code)) {
			return myStrings.find(code)->second;
		} else {
			return string();  // FIXME
		}
	}
	
	vector<string>
	Options::GetStringList(int code) const
	{
		if (myStringLists.count(code)) {
			return myStringLists.find(code)->second;
		} else {
			return vector<string>();  // FIXME
		}
	}
	
	int
	Options::GetEnum(int code) const
	{
		if (myEnums.count(code)) {
			return myEnums.find(code)->second;
		} else {
			return 0;  // FIXME
		}
	}
	
}

