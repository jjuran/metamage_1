/*
	Scrap.hh
	--------
*/

#ifndef SCRAP_HH
#define SCRAP_HH

typedef char** Handle;
typedef unsigned long ResType;

struct ScrapStuff;

pascal ScrapStuff* InfoScrap_patch();

pascal long UnloadScrap_patch();
pascal long LoadScrap_patch();

pascal long ZeroScrap_patch();

pascal long PutScrap_patch( long length, ResType type, const char* src );

pascal long GetScrap_patch( Handle dst, ResType type, long* offset );

#endif
