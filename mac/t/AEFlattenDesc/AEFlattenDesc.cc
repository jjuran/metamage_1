/*
	AEFlattenDesc.cc
	----------------
*/

// Standard C
#include <stdlib.h>
#include <string.h>

// Mac OS X
#ifdef __APPLE__
#include <ApplicationServices/ApplicationServices.h>
#endif

// Mac OS
#ifndef __AEDATAMODEL__
#include <AEDataModel.h>
#endif
#ifndef __PROCESSES__
#include <Processes.h>
#endif

#undef check

// iota
#include "iota/iterator.hh"
#include "iota/strings.hh"

// mac-sys-utils
#include "mac_sys/gestalt.hh"

// tap-out
#include "tap/check.hh"
#include "tap/test.hh"


#define ZERO "\0\0\0\0"
#define CACA "\xCA\xCA\xCA\xCA"

#define LEN_CHR   "\0\0\0\x01"
#define LEN_i16   "\0\0\0\x02"
#define LEN_i32   "\0\0\0\x04"
#define LEN_PSN   "\0\0\0\x08"
#define LEN_OS9   "\0\0\0\x5A"
#define LEN_OS9x  "\0\0\0\x6A"
#define LEN_10_2  "\0\0\0\xC4"
#define LEN_10_2x "\0\0\0\xD4"
#define LEN_10_3  "\0\0\1\x08"
#define LEN_PPC   "\0\0\1\x1c"
#define LEN_X86   "\0\0\1\x2c"


#define AEVT_9( ret_id )            \
"dle2" ZERO    "aevt"  LEN_OS9      \
ZERO   "/\0ba" LEN_OS9 ZERO         \
ZERO   ZERO    ZERO    ZERO         \
ZERO   ZERO    ZERO    ZERO "\0\0"  \
"eCls" "e_ID"  "\0\1"  ret_id       \
"psn " LEN_PSN "1234"  "5678"       \
"aevt" "\0\1\0\1"                   \
";;;;"

#define AEVT_9_TXN( ret_id, txn_id )  \
"dle2" ZERO    "aevt"   LEN_OS9x      \
ZERO   "/\0ba" LEN_OS9x "\0\0\0\1"    \
ZERO   ZERO    ZERO     ZERO          \
ZERO   ZERO    ZERO     ZERO "\0\0"   \
"eCls" "e_ID"  "\0\0"   ret_id        \
"psn " LEN_PSN "1234"   "5678"        \
"aevt" "\0\1\0\1"                     \
"tran" "long"  LEN_i32  txn_id        \
";;;;"

#define AEVT_10_2( ret_id, txn_id )   \
"dle2" ZERO   "aevt"      LEN_10_2    \
ZERO   ZERO   LEN_10_2    "\0\0\0\3"  \
ZERO   ZERO   ZERO        ZERO        \
"eCls" "e_ID" "__" ret_id ZERO        \
ZERO   ZERO   ZERO   ZERO             \
ZERO   ZERO   ZERO   ZERO             \
ZERO   ZERO   ZERO   ZERO             \
ZERO   ZERO   ZERO   ZERO             \
ZERO   ZERO   ZERO   ZERO             \
"aevt" "\0\1\0\1"                     \
"addr" "psn " LEN_PSN "1234" "5678"   \
"tbsc" "psn " LEN_PSN ZERO   ZERO     \
"inte" "long" LEN_i32 "\0\0\0\x70"    \
";;;;"

#define AEVT_10_2_TXN( ret_id, txn_id )  \
"dle2" ZERO   "aevt"      LEN_10_2x      \
ZERO   ZERO   LEN_10_2x   "\0\0\0\4"     \
ZERO   ZERO   ZERO        ZERO           \
"eCls" "e_ID" "__" ret_id ZERO           \
ZERO   ZERO   ZERO   ZERO                \
ZERO   ZERO   ZERO   ZERO                \
ZERO   ZERO   ZERO   ZERO                \
ZERO   ZERO   ZERO   ZERO                \
ZERO   ZERO   ZERO   ZERO                \
"aevt" "\0\1\0\1"                        \
"tran" "long" LEN_i32 txn_id             \
"addr" "psn " LEN_PSN "1234" "5678"      \
"tbsc" "psn " LEN_PSN ZERO   ZERO        \
"inte" "long" LEN_i32 "\0\0\0\x70"       \
";;;;"

#define AEVT_10_3( ret_id, txn_id )  \
"dle2" ZERO   "aevt"      LEN_10_3   \
ZERO   ZERO   LEN_10_3    "\0\0\0\4" \
ZERO   ZERO   ZERO        ZERO       \
"eCls" "e_ID" "__" ret_id CACA       \
CACA   CACA   CACA   CACA            \
CACA   CACA   CACA   CACA            \
CACA   CACA   CACA   CACA            \
CACA   CACA   CACA   CACA            \
CACA   CACA   CACA   CACA            \
"aevt" "\0\1\0\1"                    \
"tran" "long" LEN_i32 txn_id         \
"addr" "psn " LEN_PSN "1234" "5678"  \
"tbsc" "psn " LEN_PSN ZERO   ZERO    \
"inte" "long" LEN_i32 "\0\0\0\x70"   \
"repq" "long" LEN_i32 ZERO           \
"tbsc" "psn " LEN_PSN ZERO   ZERO    \
"remo" "long" LEN_i32 ZERO           \
";;;;"

#define AEVT_PPC( ret_id, txn_id )      \
"dle2" ZERO   "aevt"      LEN_PPC       \
ZERO   ZERO   LEN_PPC     "\0\0\0\4"    \
ZERO   ZERO   ZERO        ZERO          \
"eCls" "e_ID" "__" ret_id CACA          \
CACA   CACA   CACA   CACA               \
CACA   CACA   CACA   CACA               \
CACA   CACA   CACA   CACA               \
CACA   CACA   CACA   CACA               \
CACA   CACA   CACA   CACA               \
"aevt" "\0\1\0\1"                       \
"tran" "long" LEN_i32 txn_id            \
"addr" "psn " LEN_PSN "1234" "5678"     \
"tbsc" "psn " LEN_PSN ZERO   ZERO       \
"inte" "long" LEN_i32 "\0\0\0\x70"      \
"repq" "long" LEN_i32 ZERO              \
"tbsc" "psn " LEN_PSN ZERO   ZERO       \
"remo" "long" LEN_i32 ZERO              \
"from" "psn " LEN_PSN "\0\0\0_" "____"  \
";;;;"

#define AEVT_X86( ret_id, txn_id )      \
"dle2" ZERO   "aevt"      LEN_X86       \
ZERO   ZERO   LEN_X86     "\0\0\0\4"    \
ZERO   ZERO   ZERO        ZERO          \
"eCls" "e_ID" "__" ret_id CACA          \
CACA   CACA   CACA   CACA               \
CACA   CACA   CACA   CACA               \
CACA   CACA   CACA   CACA               \
CACA   CACA   CACA   CACA               \
CACA   CACA   CACA   CACA               \
"aevt" "\0\1\0\1"                       \
"tran" "long" LEN_i32 txn_id            \
"addr" "psn " LEN_PSN "1234" "5678"     \
"tbsc" "psn " LEN_PSN ZERO   ZERO       \
"inte" "long" LEN_i32 "\0\0\0\x70"      \
"repq" "long" LEN_i32 ZERO              \
"tbsc" "psn " LEN_PSN ZERO   ZERO       \
"remo" "long" LEN_i32 ZERO              \
"from" "psn " LEN_PSN "\0\0\0_" "____"  \
"frec" "long" LEN_i32 ZERO              \
";;;;"

// Override enum definition to avoid warnings in EXPECT_EQ()
#define noErr 0


static const unsigned n_tests = 2
                              + 2
                              + 2
                              + 3
                              + 2
                              + 2
                              + 8
                              + 4;


using mac::sys::gestalt;

const uint32_t ppcf = TARGET_API_MAC_CARBON ? gestalt( 'ppcf' ) : 0;
const uint32_t sysa = TARGET_API_MAC_CARBON ? gestalt( 'sysa' ) : 0;
const uint32_t sysv = TARGET_API_MAC_CARBON ? gestalt( 'sysv' ) : 0;

const bool os_x    = TARGET_API_MAC_CARBON  &&  sysv >= 0x1000;
const bool panther = TARGET_API_MAC_CARBON  &&  sysv >= 0x1030;
const bool tiger   = TARGET_API_MAC_CARBON  &&  sysv >= 0x1040;
const bool x86     = TARGET_API_MAC_CARBON  &&  (sysa > 2 || ppcf == 0x0011);

static
void null_NULL()
{
	char buffer[] = "1234567890abcdef";
	
	const AEDesc null = { typeNull, NULL };
	
	Size actualSize;
	
	OSErr err = AEFlattenDesc( &null, buffer, sizeof buffer - 1, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	iota::span desc( STR_LEN( "dle2" ZERO "null" ZERO ) );
	
	EXPECT_CMP( buffer, actualSize, desc.data(), desc.size() );
}

static
void word_NULL()
{
	char buffer[] = "1234567890abcdef";
	
	const DescType word = '!@#$';
	
	const AEDesc word_null = { word, NULL };
	
	Size actualSize;
	
	OSErr err = AEFlattenDesc( &word_null, buffer, sizeof buffer - 1, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	iota::span dang( STR_LEN( "dle2" ZERO "!@#$" ZERO ) );
	iota::span null( STR_LEN( "dle2" ZERO "null" ZERO ) );
	
	const iota::span& desc = (panther  &&  ! tiger) ? null : dang;
	
	EXPECT_CMP( buffer, actualSize, desc.data(), desc.size() );
}

static
void true_NULL()
{
	char buffer[] = "1234567890abcdef";
	
	const AEDesc true_null = { typeTrue, NULL };
	
	Size actualSize;
	
	OSErr err = AEFlattenDesc( &true_null, buffer, sizeof buffer - 1, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	iota::span True( STR_LEN( "dle2" ZERO "true" ZERO ) );
	iota::span null( STR_LEN( "dle2" ZERO "null" ZERO ) );
	
	const iota::span& desc = (panther  &&  ! tiger) ? null : True;
	
	EXPECT_CMP( buffer, actualSize, desc.data(), desc.size() );
}

static
void create_typeNull_with_data()
{
	char buffer[] = "1234567890abcdef";
	
	AEDesc null_h = { typeNull, NULL };
	
	if ( AECreateDesc( typeNull, STR_LEN( "Josh" ), &null_h ) != noErr )
	{
		exit( 1 );
	}
	
	EXPECT_NULL( null_h.dataHandle );
	
	Size actualSize;
	
	OSErr err = AEFlattenDesc( &null_h, buffer, sizeof buffer - 1, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	iota::span desc( STR_LEN( "dle2" ZERO "null" ZERO ) );
	
	EXPECT_CMP( buffer, actualSize, desc.data(), desc.size() );
}

static
void create_with_data_and_set_typeNull()
{
	char buffer[] = "1234567890abcdefghij";
	
	AEDesc null_h = { typeNull, NULL };
	
	const OSType word = 'Josh';
	
	if ( AECreateDesc( typeType, &word, sizeof word, &null_h ) != noErr )
	{
		exit( 1 );
	}
	
	null_h.descriptorType = typeNull;
	
	Size actualSize;
	
	OSErr err = AEFlattenDesc( &null_h, buffer, sizeof buffer - 1, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	iota::span zero( STR_LEN( "dle2" ZERO "null" ZERO ) );
	iota::span josh( STR_LEN( "dle2" ZERO "null" LEN_i32 "Josh" ) );
	iota::span type( STR_LEN( "dle2" ZERO "type" LEN_i32 "Josh" ) );
	
	const iota::span& desc = ! panther ? zero
	                       : ! tiger   ? type
	                       :             josh;
	
	EXPECT_CMP( buffer, actualSize, desc.data(), desc.size() );
	
	AEDisposeDesc( &null_h );
}

static
void create_typeType()
{
	char buffer[] = "1234567890abcdefghijk";
	
	AEDesc desc = { typeNull, NULL };
	
	const OSType word = 'Josh';
	
	if ( AECreateDesc( typeType, &word, sizeof word, &desc ) != noErr )
	{
		exit( 1 );
	}
	
	Size actualSize;
	
	OSErr err = AEFlattenDesc( &desc, buffer, sizeof buffer - 1, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	iota::span josh( STR_LEN( "dle2" ZERO "type" LEN_i32 "Josh" ) );
	
	EXPECT_CMP( buffer, actualSize, josh.data(), josh.size() );
	
	AEDisposeDesc( &desc );
}

static
void create_list()
{
	const OSType word = 'Test';
	
	char buffer[ 80 ] = { 0 };
	
	AEDesc desc = { typeNull, NULL };
	
	OSErr err;
	
	Size actualSize;
	
	// list
	// ----
	
	if ( AECreateList( NULL, 0, false, &desc ) != noErr )
	{
		exit( 1 );
	}
	
	err = AEFlattenDesc( &desc, buffer, sizeof buffer, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	iota::span list( STR_LEN( "dle2" ZERO "list" "\0\0\0\x18"
	                          ZERO   ZERO "\0\0\0\x18" "list"
	                          ZERO   ZERO ) );
	
	if ( sysv < 0x1000 )
	{
		// Mac OS 9 usually puts $8001 $7fff here.
		buffer[ 20 ] = '\0';
		buffer[ 21 ] = '\0';
		buffer[ 22 ] = '\0';
		buffer[ 23 ] = '\0';
	}
	
	EXPECT_CMP( buffer, actualSize, list.data(), list.size() );
	
	AEPutPtr( &desc, 1, typeChar, STR_LEN( "/" ) );
	AEPutPtr( &desc, 2, typeType, &word, 4 );
	
	err = AEFlattenDesc( &desc, buffer, sizeof buffer, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	list = iota::span( STR_LEN( "dle2"     ZERO "list" "\0\0\0\x2e"
	                            ZERO       ZERO "\0\0\0\x18" "list"
	                            LEN_i16 ZERO "TEXT" LEN_CHR
	                            "/\0" "type" LEN_i32 "Test" ) );
	
	if ( sysv < 0x1000 )
	{
		// Mac OS 9 usually puts $8001 $7fff here.
		buffer[ 20 ] = '\0';
		buffer[ 21 ] = '\0';
		buffer[ 22 ] = '\0';
		buffer[ 23 ] = '\0';
	}
	
	if ( sysv >= 0x1000  &&  ! panther )
	{
		buffer[ 49 ] = '\0';  // 10.2 sets padding byte to $0B
	}
	
	EXPECT_CMP( buffer, actualSize, list.data(), list.size() );
	
	AEDisposeDesc( &desc );
	
	// record
	// ------
	
	if ( AECreateList( NULL, 0, true, &desc ) != noErr )
	{
		exit( 1 );
	}
	
	err = AEFlattenDesc( &desc, buffer, sizeof buffer, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	list = iota::span( STR_LEN( "dle2"     ZERO "reco" "\0\0\0\x18"
	                            ZERO ZERO "\0\0\0\x18" "reco"
	                            ZERO ZERO ) );
	
	if ( sysv < 0x1000 )
	{
		// Mac OS 9 usually puts $8001 $7fff here.
		buffer[ 20 ] = '\0';
		buffer[ 21 ] = '\0';
		buffer[ 22 ] = '\0';
		buffer[ 23 ] = '\0';
	}
	
	EXPECT_CMP( buffer, actualSize, list.data(), list.size() );
	
	AEPutParamPtr( &desc, 'foo ', typeChar, STR_LEN( "/" ) );
	AEPutParamPtr( &desc, 'bar ', typeType, &word, 4 );
	
	err = AEFlattenDesc( &desc, buffer, sizeof buffer, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	list = iota::span( STR_LEN( "dle2"     ZERO "reco" "\0\0\0\x36"
	                            ZERO ZERO "\0\0\0\x18" "reco"
	                            LEN_i16 ZERO "foo " "TEXT"
	                            LEN_CHR "/\0" "bar " "type"
	                            LEN_i32    "Test" ) );
	
	if ( sysv < 0x1000 )
	{
		// Mac OS 9 usually puts $8001 $7fff here.
		buffer[ 20 ] = '\0';
		buffer[ 21 ] = '\0';
		buffer[ 22 ] = '\0';
		buffer[ 23 ] = '\0';
	}
	
	if ( sysv >= 0x1000  &&  ! panther )
	{
		buffer[ 53 ] = '\0';  // 10.2 sets padding byte to $60
	}
	
	EXPECT_CMP( buffer, actualSize, list.data(), list.size() );
	
	AEDisposeDesc( &desc );
}

static
void create_apple_event()
{
	const OSType word = 'Test';
	
	char buffer[ 512 ];
	
	memset( buffer, 0xCA, sizeof buffer );
	
	AEDesc desc = { typeNull, NULL };
	
	OSErr err;
	
	Size actualSize;
	
	// list
	// ----
	
	AEDesc target = desc;
	
	ProcessSerialNumber psn = { '1234', '5678' };
	
	if ( AECreateDesc( typeProcessSerialNumber, &psn, sizeof psn, &target ) != noErr )
	{
		exit( 1 );
	}
	
	const long returnID = kAutoGenerateReturnID;
	
	const long transID = kAnyTransactionID;
	
	if ( AECreateAppleEvent( 'eCls', 'e_ID', &target, returnID, transID, &desc ) != noErr )
	{
		exit( 1 );
	}
	
	err = AEFlattenDesc( &desc, buffer, sizeof buffer, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	iota::span aevt( "", 0 );
	
	if ( ! os_x )
	{
		aevt = iota::span( STR_LEN( AEVT_9( "__" ) ) );
		
		// auto-generated return ID
		buffer[ 76 ] = '_';
		buffer[ 77 ] = '_';
	}
	else
	{
		aevt = x86     ? iota::span( STR_LEN( AEVT_X86 ( "__", ZERO ) ) )
		     : tiger   ? iota::span( STR_LEN( AEVT_PPC ( "__", ZERO ) ) )
		     : panther ? iota::span( STR_LEN( AEVT_10_3( "__", ZERO ) ) )
		     :           iota::span( STR_LEN( AEVT_10_2( "__", ZERO ) ) );
		
		/*
			auto-generated return ID
			
			In OS X 10.4, all four bytes are written.
			In OS X 10.8, only the lower two bytes are written.
		*/
		
		buffer[ 56 ] = '_';
		buffer[ 57 ] = '_';
		buffer[ 58 ] = '_';
		buffer[ 59 ] = '_';
		
		// sender PSN
		buffer[ 291 ] = '_';
		buffer[ 292 ] = '_';
		buffer[ 293 ] = '_';
		buffer[ 294 ] = '_';
		buffer[ 295 ] = '_';
	}
	
	EXPECT_CMP( buffer, actualSize, aevt.data(), aevt.size() );
	
	AEDisposeDesc( &desc );
	
	memset( buffer, 0xCA, sizeof buffer );
	
	if ( AECreateAppleEvent( 'eCls', 'e_ID', &target, 0x4e75, 'txnI', &desc ) != noErr )
	{
		exit( 1 );
	}
	
	err = AEFlattenDesc( &desc, buffer, sizeof buffer, &actualSize );
	
	EXPECT_EQ( err, noErr );
	
	if ( ! os_x )
	{
		aevt = iota::span( STR_LEN( AEVT_9_TXN( "Nu", "txnI" ) ) );
	}
	else
	{
		aevt = x86     ? iota::span( STR_LEN( AEVT_X86     ( "Nu", "txnI" ) ) )
		     : tiger   ? iota::span( STR_LEN( AEVT_PPC     ( "Nu", "txnI" ) ) )
		     : panther ? iota::span( STR_LEN( AEVT_10_3    ( "Nu", "txnI" ) ) )
		     :           iota::span( STR_LEN( AEVT_10_2_TXN( "Nu", "txnI" ) ) );
		
		// return ID padding
		buffer[ 56 ] = '_';
		buffer[ 57 ] = '_';
		
		// sender PSN
		buffer[ 291 ] = '_';
		buffer[ 292 ] = '_';
		buffer[ 293 ] = '_';
		buffer[ 294 ] = '_';
		buffer[ 295 ] = '_';
	}
	
	EXPECT_CMP( buffer, actualSize, aevt.data(), aevt.size() );
	
	AEDisposeDesc( &desc );
}

int main( int argc, char** argv )
{
	tap::start( "AEFlattenDesc", n_tests );
	
	null_NULL();
	word_NULL();
	true_NULL();
	
	create_typeNull_with_data();
	
	create_with_data_and_set_typeNull();
	
	create_typeType();
	
	create_list();
	
	create_apple_event();
	
	return 0;
}

