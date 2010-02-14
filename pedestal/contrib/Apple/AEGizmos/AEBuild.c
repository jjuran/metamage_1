/*
 *	AppleEvent Builder Function
 *	by Jens Peter Alfke
 *
 *	Copyright ฉ1991-1993 Apple Computer, Inc. All rights reserved.
 *
 *	APPLE CONFIDENTIAL
 */


/*
	CHANGE HISTORY:
	 ...zillions of changes from 2/91 until...
	 6/30/93	jpa		Fixed AEBuildAppleEvent; wrong arg passed to va_start.
	10/11/93	jpa		Fix '\r' reference for MPW C.
	 4/14/94	jpa		Optimize & fix warnings for Metrowerks.
	 					Disable 'exte' descriptors on PowerPC.
	 1/27/95	jpa		Fixed generation of 'bool' descriptors (1 byte not 2).
	 3/??/95	jpa		Fixed ctype macros for CodeWarrior: cast to unsigned char
	 3/20/94	jpa		**AEGizmos 1.4
*/


#ifndef __POWERPC__
#include <SANE.h>
#endif
#include <MacErrors.h>
#include <stdarg.h>
#include "AEStream.h"
#include "AEBuild.h"

#ifdef __MWERKS__
	// Metrowerks CodeWarrior's <ctype> functions are exactly what we need.
	#include <ctype.h>
	#define ISSPACE(C)		isspace((unsigned char)C)
	#define ISGRAPH(C)		isgraph((unsigned char)C)
	#define ISDIGIT(C)		isdigit((unsigned char)C)
	#define ISXDIGIT(C)		isxdigit((unsigned char)C)
#elif defined(THINK_C)
	// THINK C's <ctype> functions are pretty much dandy, except that all >127 (option-) characters
	// are considered nonspaces and nongraphical. So we make them all graphical except for A0
	// (option-space) which is a space:
	
	#include <ctype.h>
	#define ISSPACE(C)		(isspace(C) || ((uchar)(C))==0xCA)
	#define ISGRAPH(C)		(isgraph(C) || ((uchar)(C))>127 && (uchar)(C)!=0xCA)
	#define ISDIGIT(C)		 isdigit(C)
	#define ISXDIGIT(C)		 isxdigit(C)
#else
	// In MPW there are two problems with using the <ctype> functions:
	//  1. They produce the wrong results for arguments that are <0 or >127.
	//	2. They reference global data which doesn't exist in a code resource.
	// Therefore we define the following functions instead:
	
	static Boolean ISSPACE( register char c ) {
		return c==' ' || c==0x0D || c==0x03 || c=='\t';		// '\r' is _not_ 0x0D in MPW C...
	}
	static Boolean ISGRAPH( char c ) {
		return c<0 || c>32;
	}
	static Boolean ISDIGIT( char c ) {
		return c>='0' && c<='9';
	}
	static Boolean ISXDIGIT( register char c ) {
		return (c>='0' && c<='9') || (c>='A' && c<='F') || (c>='a' && c<='f');
	}
#endif

typedef unsigned char uchar;

typedef enum{							/* Parser tokens, mnemonic for help in debugging */
	tokERROR = '?',
	tokEOF = '\0',
	tokIDENT = 'A',
	tokINTEGER = '0',
	tokSTRING = 'า',
	tokHEXSTRING = 'ว',
	tokCOLON = ':',
	tokATSIGN = '@',
	tokLPAREN = '(',	tokRPAREN = ')',
	tokLBRACKET = '[',	tokRBRACKET = ']',
	tokLBRACE = '{',	tokRBRACE = '}',
	tokOPTIONAL = '~'						// Precedes optional keyword -- JPA 7/14/92
} Token;

#define LITERAL_TOKENS "าว,:@()[]{}~"		/* List of chars that are tokens or begin tokens */

#ifndef NO_SYNTAX_CODES
  #include "AEBuildGlobals.h"
  AEBuild_SyntaxErrType AEBuild_ErrCode;	/* Globals set when syntax error occurs */
  long	AEBuild_ErrPos;
  static OSErr syntaxError( AEBuild_SyntaxErrType errCode );
  #define SYNTAX_ERROR(CODE) syntaxError(CODE)
#else
  #define SYNTAX_ERROR(CODE) (aeBuild_SyntaxErr)
#endif


static Token
	peekToken( register const char **src ),
	getToken( const char **src, long *val );
static OSErr parseObj( AEGizmos_AEStreamRef s, const char **src, va_list *args );


#ifdef THINK_C
#if !__option(macsbug_names)
	// Always turn on macsbug names for the non-static functions.
	#define NO_NAMES
	#pragma options(macsbug_names)
#endif
#endif


/* AE_BUILD  Construct an AppleEvent structure based on the format in src and the
			 following parameters (if any). */
OSErr
AEGizmos_AEBuild( AEDesc *dst, const char *src, ... )
{
	va_list args;
	OSErr err;
	
	va_start(args,src);
	err= AEGizmos_vAEBuild(dst,src,args);
	va_end(args);
	return err;
}


/* V_AE_BUILD  Same as AEBuild, but parameters are passed explicitly as a va_list.
			   This is analogous to vprintf. */
OSErr
AEGizmos_vAEBuild( AEDesc *dst, const char *src, const void *args )
{
	OSErr err;
	const char *pos = src;
	AEStream s;
	
#ifndef NO_SYNTAX_CODES
	AEBuild_ErrCode = aeBuildSyntaxNoErr;			/* Reset syntax-err code */
#endif

	err= AEStream_Open(&s);
	if( err==noErr )
		err= parseObj(&s,&pos,(va_list*)&args);
	if( err==noErr )
		if( peekToken(&pos)!=tokEOF )				/* Extra stuff past end! */
			err= SYNTAX_ERROR(aeBuildSyntaxNoEOF);
	if( err ) {
		AEStream_Close(&s,NULL);
		dst->descriptorType = typeNull;
		dst->dataHandle = NULL;
#ifndef NO_SYNTAX_CODES
		AEBuild_ErrPos = StripAddress((Ptr)pos)-StripAddress((Ptr)src);
#endif
	} else
		err= AEStream_Close(&s,dst);
	return err;
}


#ifdef NO_NAMES
	#undef NO_NAMES
	#pragma options(!macsbug_names)
#endif


/*******************************  THE UTILITY BELT  ************************************/


/* WRITE_INT_DESC  Create a descriptor for a long or short integer */
static OSErr
writeIntDesc( AEGizmos_AEStreamRef s, long n )
{
	if( n>32767 || n<-32767 )					/* Pascal doesn't like 16-bit -32768 */
		return AEStream_WriteDesc(s,'long',(Ptr)&n,4);
	else
		return AEStream_WriteDesc(s,'shor',(Ptr)&n+2,2);
}


/*******************************  THE TOKENIZER  ***************************************/


/* IS_LITERAL_TOKEN  Determine whether the character c is one of the literal tokens */
static Boolean
isLiteralToken( register char c )
{
	register char *str = LITERAL_TOKENS;
	register char cc;
	
	while( (cc= *str++) !='\0' )			/* Search through LITERAL_TOKENS string */
		if( c==cc )
			return true;
	return false;
}


/* PEEK_TOKEN  Peek ahead at the next token. Doesn't retrieve value of ints/idents */
static Token
peekToken( register const char **src )
{
	register char c;
	
	do{
		c= *(*src)++;								/* Skip whitespace */
	}while( ISSPACE(c) );
	(*src)--;										/* Don't eat the non-whitespace char */
	
	if( c=='\0' )									/* Classify char */
		c= tokEOF;
	else if( ISDIGIT(c) || c=='-' )
		c= tokINTEGER;
	else if( isLiteralToken(c) )
		;
	else if( ISGRAPH(c) )
		c= tokIDENT;
	else
		c= tokERROR;
	return (Token) c;								/* Return it */
}


/* GET_TOKEN  Scan the next token out of the format string. Here's how val will be set:
 *				tokINTEGER:	Integer value
 *				tokIDENT:	4-char identifier name
 *				tokSTRING:	Pointer to 1st char of string (*src pts to 1st char past 'ำ')
 *				tokHEXSTRING: Pointer to 1st char of hex data
 *				other:		Undefined. */
static Token
getToken( const char **src, long *val )
{
	Token tok;
	register const char *cp;
	register char c;
	
	tok= peekToken(src);							/* Peek at next char */
	cp = *src;
	c = *cp++;
	
	switch( tok ) {
		case tokINTEGER: {							/* Interpret number: */
			Boolean minus = (c=='-');
			
			if( minus ) {
				c= *cp++;
				if( !ISDIGIT(c) ) {						/* "-" not followed by digit! */
					SYNTAX_ERROR(aeBuildSyntaxBadNegative);
					tok= tokERROR;
					cp -= 2;
					goto exit;
				}
			}
			*val = 0;
			do {
				*val= *val*10 + (c-'0');				/* Accumulate digits */
				c= *cp++;
			} while( ISDIGIT(c) );
			cp--;
			if( minus )
				*val= -*val;
			break;
		}

		case tokERROR:								/* Illegal character */
			SYNTAX_ERROR(aeBuildSyntaxBadToken);
			cp--;
			break;
			
		case tokEOF:								/* ษor end of format string */
			SYNTAX_ERROR(aeBuildSyntaxBadEOF);
			cp--;
			break;

		case tokIDENT: {
			char endc;
			
			if( c=='\'' )
				endc = '\'';
			else if( c=='ิ' )
				endc = 'ี';
			else
				endc = '\0';
			
			if( endc ) {							/* Quoted identifier: */
				short count = 0;
				*val = '    ';
				do{
					c= *cp++;
					if( c==endc )
						break;
					if( c=='\\' )						/* Backslash: take next char */
						c= *cp++;						/* literally, unless it's EOF */
					if( c=='\0' )
						break;
					if( count<4 )
						((char*)val)[count++] = c;		/* Copy 1st 4 chars to value */
				}while( true );
				if( c!=endc ) {
					SYNTAX_ERROR(aeBuildSyntaxMissingQuote);
					tok= tokERROR;						/* Unexpected eof */
					goto exit;
				}
			} else {								/* Other identifier: */
				short count = 0;
				*val = '    ';
				do{
					if( count<4 )
						((char*)val)[count++] = c;		/* Copy 1st 4 chars to value */
					c= *cp++;
				}while( ISGRAPH(c) && !isLiteralToken(c) );
				cp--;
			}
			break;
		}
		case tokSTRING:								/* String: */
			*val = (long)cp;							/* Store 1st char posn in *val */
			do{
				c= *cp++;
			}while( c && c!='ำ' );						/* Skip chars 'til end */
			if( c=='\0' ) {
				SYNTAX_ERROR(aeBuildSyntaxNoCloseString);
				tok= tokERROR;
			}
			break;
			
		case tokHEXSTRING:							/* Hex string: */
			*val = (long)cp;							/* Store 1st char posn in *val */
			do{
				c= *cp++;
			}while( ISXDIGIT(c) || ISSPACE(c) );		/* Skip chars 'til end */
			if( c!='ศ' ) {
				SYNTAX_ERROR( c ?aeBuildSyntaxBadHex :aeBuildSyntaxNoCloseHex);
				tok= tokERROR;
			}
			break;
	}
	
exit:
	*src = cp;
	return tok;
}


/* SYNTAX_ERROR  Report a syntax error (somehow) and return the appropriate error code */
#ifndef NO_SYNTAX_CODES
static OSErr
syntaxError( AEBuild_SyntaxErrType errCode )
{
	if( AEBuild_ErrCode==aeBuildSyntaxNoErr )
		AEBuild_ErrCode = errCode;
	return aeBuild_SyntaxErr;
}
#endif


/* WRITE_STRING_DESCRIPTOR  Write a descriptor for a just-parsed string */
static OSErr
writeStringDescriptor( long tokenVal, const char *src, DescType type, AEGizmos_AEStreamRef s )
{
	return AEStream_WriteDesc(s, type,
							  (void*)tokenVal,
							  StripAddress((Ptr)src) -1 -StripAddress((Ptr)tokenVal));
}


/* WRITE_HEX_STRING_DESCRIPTOR  Write a descriptor for a just-parsed hex string */
static OSErr
writeHexStringDescriptor( long tokenVal, const char *src, DescType type, AEGizmos_AEStreamRef s )
{
	long size;
	uchar **h, *dst, *start,*end, *cp, c;
	Boolean hi;
	OSErr err;
	
	// Now that we have streams, we could just write the bytes one by one to the stream,
	// but keeping our own buffer is faster since we're just writing to it via a pointer.
	
	end = (uchar*)StripAddress((Ptr)src) -1;
	start = (uchar*)StripAddress((Ptr)tokenVal);
	size = (end-1-start)/2;							/* Max possible len */
	
	h = (uchar **)NewHandle(size);						/* Create temp buffer for data */
	if( MemError() ) return MemError();
	
	for( cp=(uchar*)tokenVal, hi=true, dst=*h; cp<end; cp++ ) {
		c= *cp;										/* Scan source text: */
		if( ISXDIGIT(c) ) {
			if( ISDIGIT(c) )
				c -= '0';
			else if( c>='a' )
				c -= 'a'-10;
			else
				c -= 'A'-10;
			if( hi )
				*dst = c<<4;							/* Copy to destination, */
			else										/* one nybble at a time */
				*dst++ |= c;
			hi = !hi;
		}
	}
	if( !hi ) {
		DisposeHandle((Handle)h);						/* Odd number of digits -- error */
		return SYNTAX_ERROR(aeBuildSyntaxOddHex);
	}
	size= dst-*h;
	SetHandleSize((Handle)h, size);						/* Resize buffer to make room */
	MoveHHi((Handle)h);
	HLock((Handle)h);
	
	err= AEStream_WriteDesc(s, type, (Ptr)*h, size);	/* Build the descriptor */
	DisposeHandle((Handle)h);
	return err;
}


/*******************************  HEAVY_DUTY PARSING  **********************************/


/* PARSE_DATA  Parse a single data item, the data of an AEDesc. Usually an integer. */
/*			   objType is the type to coerce to, or 0L */
static OSErr
parseData( AEGizmos_AEStreamRef s, const char **src, va_list *args, DescType objType )
{
	Token t;
	long val;
	OSErr err;
	
	t= getToken(src, &val);

	if( t==')' ) {
		if( objType==0L )
			objType= typeNull;
		return AEStream_WriteDesc(s,objType,NULL,0);

	} else if( t==tokINTEGER ) {						/* Direct number: */
		if( objType==0L || objType=='long' )
			err= AEStream_WriteDesc(s,'long',&val,sizeof(long));	/* Longs are e-z */
		else {
			AEDesc desc;
			err= AECoercePtr('long',(Ptr)&val,sizeof(val), objType,&desc);	/* Coerce!! */
			if( !err ) {
				err= AEStream_WriteAEDesc(s,&desc);				/* Write resulting desc */
				AEDisposeDesc(&desc);
			}
		}
	
	} else if( t==tokIDENT )							/* Identifier: */
		err= AEStream_WriteDesc(s, objType?objType:'enum', &val, sizeof(val));
	
	else if( t==tokSTRING ) {							/* String: */
		if( !objType )
			objType = 'TEXT';
		err= writeStringDescriptor(val,*src, objType,s);
	
	} else if( t==tokHEXSTRING )						/* Hex data: */
		if( objType )
			err= writeHexStringDescriptor(val,*src, objType,s);
		else
			err= SYNTAX_ERROR(aeBuildSyntaxUncoercedHex);

	else if( t=='@' )									/* Substitute fn argument: */
		if( peekToken(src) == '@' ) {
			long val;
			AEDesc desc;										// @@ means use a handle
			getToken(src,&val);
			desc.descriptorType = objType;
			desc.dataHandle = va_arg(*args,Handle);
			err= AEStream_WriteAEDesc(s, &desc);
			
		} else {
			short size;
			
			switch( objType ) {
				case 'shor':	size= sizeof(short); break;		/* Standard numeric types */
				case 'type':
				case 'enum':
				case 'long':	size= sizeof(long); break;
				case 'sing':	size= sizeof(float); break;
				case 'doub':	size= sizeof(double); break;	/**** MPW double, not THINK double!! ***/
				case 'exte':
#if powerc
								err = errAECoercionFail; goto done;
								// There is no 'extended' type on PPC.
#else
								size= sizeof(extended); break;
#endif
				case 'bool': {
					// Booleans have to be read as ints but written as 1 byte
					char b = va_arg(*args,int);
					if( b ) b=1;
					err= AEStream_WriteDesc(s,objType,&b,1);
					goto done;
				}
				case 'TEXT': {									/* TEXT: Get C string argument */
					char *str = va_arg(*args,char*);
					char *c;
					long len;
					
					for( c=str,len=0; *c; c++ )
						len++;
					err= AEStream_WriteDesc(s,'TEXT',str,len);
					goto done;
				}
				case 0L:										/* Get AEDesc* from param list */
					err= AEStream_WriteAEDesc(s, va_arg(*args,AEDesc*));
					goto done;
				default: {
					void *data;
					size = va_arg(*args,long);				/* Default: get size, then data ptr */
					data = va_arg(*args,void*);
					err= AEStream_WriteDesc(s,objType,data,size);
					goto done;
				}
			}
		
			err= AEStream_WriteDesc(s,objType,*args,size);	/* Handle standard numeric types: */
			*args = (Ptr)*args + size;						/* Parameter data; assume numeric */
															/* (4 bytes by default) */
done:		;
		}

	else
		return SYNTAX_ERROR(aeBuildSyntaxBadData);

	if( !err && getToken(src,(long*)val) != ')' )		/* Must have closing paren */
		err= SYNTAX_ERROR(aeBuildSyntaxNoCloseParen);
	return err;
}


/* PARSE_LIST  Parse an AEDescList specifier. objType is the type to coerce to, or 0L */
static OSErr
parseList( AEGizmos_AEStreamRef s, const char **src, va_list *args )
{
	Token t;
	long val;
	OSErr err= noErr;
	Boolean first = true;
	
	err= AEStream_OpenList(s);
	if( err )
		return err;
	
	do{
		t= peekToken(src);
		if( t==tokERROR || t==tokEOF ) {
			err= SYNTAX_ERROR(aeBuildSyntaxNoCloseBracket);
			break;
		} else if( t==']' ) {
			getToken(src,&val);
			break;									/* EXIT loop when "]" is found */
		} else if( !first )
			if( t==',' )							/* Entries must be separated by "," */
				getToken(src,&val);
			else {
				err= SYNTAX_ERROR(aeBuildSyntaxNoCloseBracket);
				break;
			}
		else
			first = false;
		
		err= parseObj(s, src,args);
	}while( !err );
	
	if( !err )
		err= AEStream_CloseList(s);
	return err;
}


/* PARSE_KEY_LIST  Parse an AERecord specifier. objType is the type to coerce to, or 0L */
static OSErr
parseKeyList( AEGizmos_AEStreamRef s, const char **src, va_list *args, DescType objType )
{
	Token t;
	DescType val,key;
	OSErr err= noErr;
	Boolean first = true;
	
	err= AEStream_OpenRecord(s,
							 objType ?objType :typeAERecord);
	if( err )
		return err;
	
	do{
		t= getToken(src,(long*)&key);	/* Get token; expecting keyword, "," or "}" */
		if( t == '}' )
			break;						/* It's "}", so we're DONE */
		else if( t==tokERROR || t==tokEOF ) {
			err= SYNTAX_ERROR(aeBuildSyntaxNoCloseBrace);
			break;
		} else if( !first )
			if( t==',' )				/* Entries after 1st are preceded by "," */
				t= getToken(src,(long*)&key);	/* Skip the ","; next token should be ident */
			else {
				err= SYNTAX_ERROR(aeBuildSyntaxNoCloseBrace);
				break;
			}
		else
			first = false;
		
		if( t != tokIDENT ) {
			err= SYNTAX_ERROR(aeBuildSyntaxNoKey);		/* If not keyword, barf mightily */
			break;
		}
		t= getToken(src,(long*)&val);			/* Get token, which must be a ":" */
		if( t != ':' ) {
			err= SYNTAX_ERROR(aeBuildSyntaxNoColon);
			break;
		}
		
		err= AEStream_WriteKey(s,key);					/* Write key for descriptor */
		if( !err )
			err= parseObj(s, src,args);					/* Parse/write object descriptor */
	}while( !err );
	
	if( !err )
		err= AEStream_CloseRecord(s);
	return err;
}


/* PARSE_OBJ  Parse an object descriptor */
static OSErr
parseObj( AEGizmos_AEStreamRef s, const char **src, va_list *args )
{
	Token t;
	DescType val;
	DescType objType = 0L;
	OSErr err;
	
	t= getToken(src, (long*)&val);
	
	if( t==tokINTEGER )
		return writeIntDesc(s,val);					/* Create integer descriptor */

	else if( t==tokSTRING )							/* Or string descriptor */
		return writeStringDescriptor(val,*src, 'TEXT',s);
	
	else if( t==tokHEXSTRING )
		return SYNTAX_ERROR(aeBuildSyntaxUncoercedHex);
	
	else if( t=='@' ) {								/* Get AEDesc* from param list */
		if( peekToken(src)=='@' )
			return SYNTAX_ERROR(aeBuildSyntaxUncoercedDoubleAt);
		else
			return AEStream_WriteAEDesc(s,va_arg(*args,AEDesc*));

	} else if( t==tokIDENT ) {						/* Identifier found: */
		t= peekToken(src);
		if( t=='(' || t=='[' || t=='{' ) {
			objType = val;
			t= getToken(src,(long*)&val);					/* It's a coercion, go on */
		} else
			return AEStream_WriteDesc(s,'enum',&val,4);		/* Plain enum-code, return */
	}
	
	if( t == '(' )
		err= parseData(s, src,args, objType);		/* Get single data item */
	else if( t == '[' )
		if( objType==0L || objType==typeAEList )
			err= parseList(s, src,args);			/* Get list of items */
		else
			err= SYNTAX_ERROR(aeBuildSyntaxCoercedList);
	else if( t == '{' )
		err= parseKeyList(s, src,args, objType);	/* Get keyword list (AERecord) */
	else
		err= SYNTAX_ERROR(aeBuildSyntaxBadDesc);
	return err;
}


/* PARSE_PARAM  Parse desc and add as a parameter/attribute to an Apple event.  JPA 7/14/92 */
static OSErr
parseParam( AppleEvent *event, AEKeyword keyword, Boolean isAttribute, const char **src, va_list args )
{
	AEStream s;
	AEDesc param;
	OSErr err;
	
	err= AEStream_Open(&s);
	if( err==noErr )
		err= parseObj(&s,src,&args);
	if( err )
		AEStream_Close(&s,NULL);
	else
		err= AEStream_Close(&s,&param);
	if( !err ) {
		if( isAttribute )
			err= AEPutAttributeDesc(event,keyword,&param);
		else
			err= AEPutParamDesc(event,keyword,&param);
		AEDisposeDesc(&param);
	}
	return err;
}


/*******************************  BUILDING EVENTS  **********************************/

// JPA: Added 7/14/92


#ifdef THINK_C
#if !__option(macsbug_names)
	#define NO_NAMES
	#pragma options(macsbug_names)
#endif
#endif


/* AEBUILD_APPLE_EVENT  Construct an Apple Event and read params */
OSErr
AEGizmos_AEBuildAppleEvent(	AEEventClass theClass, AEEventID theID,
					DescType addressType, const void *addressData, long addressLength,
					short returnID, long transactionID, AppleEvent *result,
					const char *paramsFmt, ... )
{
	va_list args;
	OSErr err;
	
	va_start(args,paramsFmt);
	err= AEGizmos_vAEBuildAppleEvent(theClass,theID,
							addressType,addressData,addressLength,
							returnID,transactionID,result,
							paramsFmt,
							args);
	va_end(args);
	return err;
}


/* V_AEBUILD_APPLE_EVENT  Construct an Apple Event and read params */
OSErr
AEGizmos_vAEBuildAppleEvent(	AEEventClass theClass, AEEventID theID,
					DescType addressType, const void *addressData, long addressLength,
					short returnID, long transactionID, AppleEvent *resultEvt,
					const char *paramsFmt, const void *args )
{
	AEDesc addressDesc = {typeNull,NULL};
	OSErr err;
	
	if( resultEvt==NULL ) return paramErr;
	resultEvt->descriptorType = typeNull;
	resultEvt->dataHandle = NULL;
	
	err= AECreateDesc(addressType,(Ptr)addressData,addressLength, &addressDesc);
	if( err ) goto exit;
	
	err= AECreateAppleEvent(theClass,theID, &addressDesc,returnID,transactionID, resultEvt);
	if( err ) goto exit;
	
	AEDisposeDesc(&addressDesc);
	err= AEGizmos_vAEBuildParameters(resultEvt,paramsFmt, args);
	
exit:
	AEDisposeDesc(&addressDesc);
	if( err )
		AEDisposeDesc(resultEvt);
	return err;
}


OSErr
AEGizmos_AEBuildParameters( AppleEvent *event, const char *format, ... )
{
	va_list args;
	OSErr err;
	
	va_start(args,format);
	err= AEGizmos_vAEBuildParameters(event,format, args);
	va_end(args);
	return err;
}


OSErr
AEGizmos_vAEBuildParameters( AppleEvent *event, const char *format, const void *args )
{
	const char *pos = format;
	AEStream s;
	Token t;
	DescType val,key;
	OSErr err= noErr;
	Boolean first = true;
	
#ifndef NO_SYNTAX_CODES
	AEBuild_ErrCode = aeBuildSyntaxNoErr;		/* Reset syntax-err code */
#endif

	err= AEStream_OpenEvent(&s, event);
	if( err ) return err;
	
	// Now read all the parameters:

	do{
		t= getToken(&pos,(long*)&key);			/* Get token; expecting keyword, "," or "}" */
		if( t == tokEOF )
			break;								/* End of string, so we're DONE */
		else if( t==tokERROR ) {
			err= SYNTAX_ERROR(aeBuildSyntaxNoEOF);
			break;
		} else if( !first )
			if( t==',' )						/* Entries after 1st are preceded by "," */
				t= getToken(&pos,(long*)&key);	/* Skip the ","; next token should be ident */
			else {
				err= SYNTAX_ERROR(aeBuildSyntaxNoEOF);
				break;
			}
		else
			first = false;
		
		if( t == tokOPTIONAL ) {
			t= getToken(&pos,(long*)&key);
			err= AEStream_OptionalParam(&s,key);
			if( err ) break;
		}
		
		if( t != tokIDENT ) {
			err= SYNTAX_ERROR(aeBuildSyntaxNoKey);		/* If not keyword, barf mightily */
			break;
		}
		t= getToken(&pos,(long*)&val);					/* Get token, which must be a ":" */
		if( t != ':' ) {
			err= SYNTAX_ERROR(aeBuildSyntaxNoColon);
			break;
		}
		
		err= AEStream_WriteKey(&s,key);					/* Write key for parameter */
		if( !err )
			err= parseObj(&s, &pos,(va_list*)&args);	/* Parse/write parameter */
	}while( !err );
	
	// Finally, finish and clean up:
	
	if( err )
		AEStream_Close(&s,NULL);
	else
		err= AEStream_Close(&s, event);
	
#ifndef NO_SYNTAX_CODES
	if( err )
		AEBuild_ErrPos = StripAddress((Ptr)pos)-StripAddress((Ptr)format);
#endif
	
	return err;
}


#ifdef NO_NAMES
	#undef NO_NAMES
	#pragma options(!macsbug_names)
#endif
