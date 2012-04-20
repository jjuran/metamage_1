/*
	MacGlue/MacGlue.hh
	------------------
*/

#ifndef MACGLUE_MACGLUE_HH
#define MACGLUE_MACGLUE_HH


namespace MacGlue
{
	
#if defined( __MC68K__ )  &&  !defined( __CFM68K__ )
	
	#define MACGLUE_CONCAT( a, b )  a##b
	
	#define MACGLUE_PROTOTYPE( routine )  \
		asm pascal \
		MACGLUE_CONCAT( MACGLUE_RETURN_, routine ) \
		routine \
		MACGLUE_CONCAT( MACGLUE_ARGS_, routine )
	
	#define DECLARE_MAC_GLUE( routine )  MACGLUE_PROTOTYPE( routine )
	
#else
	
	#define DECLARE_MAC_GLUE( routine )  using ::routine
	
#endif
	
	#define MACGLUE_ARGS_NewWindow  ( void*,                 \
	                                  const struct Rect*,    \
	                                  const unsigned char*,  \
	                                  unsigned char,         \
	                                  short,                 \
	                                  struct GrafPort*,      \
	                                  unsigned char,         \
	                                  long )
	
	#define MACGLUE_RETURN_NewWindow  struct GrafPort*
	
	#define MACGLUE_ARGS_NewCWindow    MACGLUE_ARGS_NewWindow
	#define MACGLUE_RETURN_NewCWindow  MACGLUE_RETURN_NewWindow
	
	#define MACGLUE_ARGS_DisposeWindow  ( struct GrafPort* )
	
	#define MACGLUE_RETURN_DisposeWindow  void
	
	#define MACGLUE_ARGS_DisposeGWorld  ( struct CGrafPort* )
	
	#define MACGLUE_RETURN_DisposeGWorld  void
	
	#define MACGLUE_ARGS_ClosePort  ( struct GrafPort* )
	
	#define MACGLUE_RETURN_ClosePort  void
	
	#define MACGLUE_ARGS_CloseCPort  ( struct CGrafPort* )
	
	#define MACGLUE_RETURN_CloseCPort  void
	
}

#endif

