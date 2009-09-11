/*	===============
 *	iota/strings.hh
 *	===============
 */

#ifndef IOTA_STRINGS_HH
#define IOTA_STRINGS_HH

// Expands to a value equal to strlen( str ), but for string constants only,
// and in constant (zero) time instead of linear time.
#define STRLEN( str )  (sizeof "" str - 1)

// Expands to two comma-separated values -- the string constant and its length.
#define STR_LEN( str )  str, STRLEN( str )


#endif

