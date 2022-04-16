/*
	string_list.hh
	--------------
*/

#ifndef STRINGLIST_HH
#define STRINGLIST_HH

struct Point;
struct Rect;
struct string_list_record;

typedef string_list_record** string_list_handle;

string_list_handle new_string_list( const Rect& view );

void dispose_string_list( string_list_handle slh );

void clear_string_list( string_list_handle slh );

void append_to_string_list( string_list_handle slh, const unsigned char* str );

void draw_string_list( string_list_handle slh );

void scroll_string_list_to( string_list_handle slh, short offset );

bool string_list_click( string_list_handle slh, Point pt );

const unsigned char* get_string_list_selection( string_list_handle slh );

#endif
