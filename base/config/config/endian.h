/*
	endian.h
	--------
*/

#ifndef CONFIG_ENDIAN_HH
#define CONFIG_ENDIAN_HH


#ifndef CONFIG_LITTLE_ENDIAN
	#if defined( __BIG_ENDIAN__ ) || defined( __MC68K__ )
		#define CONFIG_LITTLE_ENDIAN  0
	#endif
	#if defined( __LITTLE_ENDIAN__ ) || defined( __i386__ ) || defined( __x86_64__ )
		#define CONFIG_LITTLE_ENDIAN  1
	#endif
#endif


#endif

