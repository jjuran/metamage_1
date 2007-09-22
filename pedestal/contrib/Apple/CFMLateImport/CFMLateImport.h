/*
	File:		CFMLateImport.h

	Contains:	Interface to CFM late import library.

	Written by:	Quinn

	Copyright:	Copyright © 1999 by Apple Computer, Inc., all rights reserved.

				You may incorporate this Apple sample source code into your program(s) without
				restriction. This Apple sample source code has been provided "AS IS" and the
				responsibility for its operation is yours. You are not permitted to redistribute
				this Apple sample source code as "Apple sample source code" after having made
				changes. If you're going to re-distribute the source, we require that you make
				it clear in the source that the code was descended from Apple sample source
				code, but that you've made changes.

	Change History (most recent first):

         <3>    16/11/00    Quinn   Allow symbol finding via a callback and use that to implement
                                    CFBundle support.
         <2>    18/10/99    Quinn   Renamed CFMLateImport to CFMLateImportLibrary to allow for
                                    possible future API expansion.
         <1>     15/6/99    Quinn   First checked in.
*/

#pragma once

/////////////////////////////////////////////////////////////////

// MoreIsBetter Setup

#include "MoreSetup.h"

// Mac OS Interfaces

#ifdef __MRC__
#include <CodeFragments.h>
#include <CFBundle.h>
#endif

/////////////////////////////////////////////////////////////////

//#include <MacTypes.h>
//#include <Devices.h>
//#include <CFBundle.h>

#ifdef __cplusplus
extern "C" {
#endif

/*	FAQ
	---
	
	Q:	What does this library do?
	A:	It allows you to resolve a weak linked library at runtime,
	   	by supply a CFM connection to the library that should substitute
	   	for the weak linked one.
	
	Q:	Does the substituted library have to have the same name as the
		weak linked library.
	A:	No.
	
	Q:	What's this useful for?
	A:	The most obvious example of where this is useful is when
		you rely on shared libraries that the user might delete
		or move.  To can find the shared library (possibly even
		using CatSearch), call GetDiskFragment to open a connection
		to it, late import it using this library, and then the
		rest of your code can continue to use the shared library
		as if nothing had happened.  No more defining thousands
		of stub routines which call through routine pointers.
		
		There are, however, numerous less obvious uses.  You can
		use this code to make a 'self repairing' application.  If
		the user removes your shared library from the Extensions
		folder, the startup code for your application can offer
		tor re-install it.  If the user agrees, you can then
		re-install your shared library, late import it, and then
		continue running your application if nothing happened.
		
		You can even use this code to free yourself from the
		Extensions folder entirely.  Say you have a suite of
		applications that currently installs a dozen shared 
		libraries in the Extensions folder.  You can move those
		libraries to another folder entirely and each application's
		startup code can track down the library (using an alias
		in the Preferences file) and late import it.
		
		An even cooler use is to provide easy abstraction layers.
		Say you have a network code for both the MacTCP
		API and the Open Transport API.  Typically, you would be
		force to do this by having an abstraction layer where every
		routine contains a switch between MacTCP and OT.  Your
		OpenSocket routine might look like:

			static int OpenSocket(void)
			{
			    if (gOTAvailable) {
			        return OpenSocketOT();
			    } else {
			        return OpenSocketMacTCP();
			    }
			}
		
		With this code, you can avoid that entirely.  Simply
		weak link to a shared library that you know is never
		going to be implemented ("crea;MySocketsDummy") and then, 
		at runtime, decide whether the system has MacTCP or OT
		and late import the relevant real implementation
		("crea;MySocketsMacTCP" or "crea;MySocketsOT").
		One benefit of this approach is that only the MacTCP or
		the OT code is resident in memory on any given system.
*/

typedef pascal OSStatus (*CFMLateImportLookupProc)(ConstStr255Param symName, CFragSymbolClass symClass,
													void **symAddr, void *refCon);
	// CFMLateImportLookupProc defines a callback for CFMLateImportCore.
	// The routine is expected to look up the address of the symbol named 
	// symName and return it in *symAddr.  The symbol should be of class 
	// symClass, although the callback decides whether a class mismatch is 
	// an error.  refCon is an application defined value that was originally 
	// passed in to CFMLateImportCore.
	//
	// If this routine returns an error, a symbol address of 0 is assumed. 
	// If the symbol is marked as a weak import, the CFMLateImportCore will 
	// continue, otherwise the CFMLateImportCore routine will fail with the 
	// error.
	
extern pascal OSStatus CFMLateImportCore(const CFragSystem7DiskFlatLocator *fragToFixLocator,
										CFragConnectionID fragToFixConnID,
										ConstStr255Param fragToFixExportedRoutineName,
										ConstStr255Param weakLinkedLibraryName,
										CFMLateImportLookupProc lookup,
										void *refCon);
	// This routine will link you, at runtime, to some library 
	// that you were weak linked to and wasn't present when your
	// fragment was prepared.  As well as the obvious functionality
	// of being able to resolve weak links after prepare time,
	// this functionality can be put to a number of less obvious uses,
	// some of which are discussed at the top of this header file.
	//
	// To call this routine, you need a number of pieces of information:
	//
	// 1. fragToFixLocator, fragToFixConnID:  The location of your own
	//    code fragment on disk and the CFM connection ID to your own
	//    code fragment.  Typically you get this information from your 
	//    fragment's CFM init routine.  You must ensure that
	//    fragToFixLocator->fileSpec points to an FSSpec of the
	//    file which holds your code fragment.
	//
	//    IMPORTANT:
	//    The fact that you pass in a CFragSystem7DiskFlatLocator as the
	//    fragToFixLocator implies that the fragment to be fixed up must
	//    be in the data fork of a file.  The code could be modified
	//    to remove this requirement, but on disk code fragments are the most
	//    common case.
	//
	//    IMPORTANT:
	//    The fragment to fix must not have a packed data section.
	//    In CodeWarrior, check the "Expand Uninitialized Data" checkbox
	//    in the "PPC PEF" settings panel.  In MPW, specified the
	//    "-packdata off" option to PPCLink.
	//
	// 2. fragToFixExportedRoutineName:  The name of a routine (any routine,
	//    it doesn't matter what it does, we're not going ta call it)
	//    which you export from your fragment.  Even if your fragment
	//    doesn't need to export any routines, it must do so for this
	//    functionality to work.
	//
	// 3. weakLinkedLibraryName:  The name of the weak linked library which
	//    failed to link.  You must have weak linked to this library.
	//    It is oxymoric for you to pass a strong linked library here,
	//    because your code would not have prepared if a strong linked
	//    library failed to prepare, and so you couldn't supply a valid
	///   fragToFix.
	//
	// 4. lookup, refCon:  A pointer to a callback function that the 
	//	  routine calls to look up the address of a symbol, and a refCon 
	//    for that callback routine.
	//
	// Note:
	// The fragToFixLocator and fragToFixExportedRoutineName parameters
	// are artifacts of the way in which this functionality is implemented.
	// In an ideal world, where CFM export decent introspection APIs
	// to third party developers, these parameters would not be necessary.
	// If you're using this code inside Apple, you probably should investigate
	// using the CFM private APIs for getting at the information these
	// parameters are needed for.  See the comments inside the implementation
	// for more details.
	//
	// Note:
	// The requirement for unpacked data is also an artifact of my workaround
	// for the lack of CFM introspection APIs.  I could have worked around it,
	// but the workaround would probably have required me to allocate a memory
	// block as big as the fragment to fix's data section, which could be
	// very big indeed.  I decided that unpacked data was better.  Packed
	// data only really saves disk space, which isn't at a premium.
	//
	// When the routine returns, any symbols that you imported from the
	// library named weakLinkedLibraryName will be resolved to the address
	// of the symbol provided by the "lookup" callback routine.
	//
	// It is possible for an unresolved import to remain unresolved after
	// this routine returns.  If the symbol import is marked as weak (as
	// opposed to the library, which *must* be marked as weak) and the symbol
	// is not found by the "lookup" callback, the routine will simple skip 
	// that symbol.  If the symbol isn't marked as weak, the routine will fail 
	// in that case.
	//
	// Most of the possible error results are co-opted CFM errors.  These
	// include:
	//
	// cfragFragmentFormatErr  -- The fragment to fix is is an unknown format.
	// cfragNoSectionErr       -- Could not find the loader section in the fragment to fix.
	// cfragNoSymbolErr        -- The fragToFixExportedRoutineName is not exported from the
	//                            fragment to fix.
	// cfragNoLibraryErr       -- The fragment to fix is not weak linked to weakLinkedLibraryName.
	// cfragFragmentUsageErr   -- The fragment to fix doesn't have a data section.
	//                         -- The fragment to fix is strong linked to weakLinkedLibraryName.
	//                         -- The fragment to fix has a packed data section.
	// cfragFragmentCorruptErr -- Encountered an undefined relocation opcode.
	// unimpErr                -- Encountered an unimplement relocation opcode.  The
	//                            relocation engine only implements a subset of the CFM
	//                            relocation opcodes, the subset most commonly used by
	//                            MPW and CodeWarrior PEF containers.  If you encounter
	//                            this error, you'll probably have to add the weird
	//                            relocation opcode to the engine, which shouldn't be
	//                            be too hard.
	// errors returned by FindSymbol
	// errors returned by Memory Manager
	//
	// The routine needs enough memory to hold the loader section of the fragment
	// to fix in memory.  It allocates that memory using NewPtr and dispsoses of 
	// it before it returns.  You may want to change the memory allocator, which
	// is very simple.

extern pascal OSStatus CFMLateImportLibrary(const CFragSystem7DiskFlatLocator *fragToFixLocator,
										CFragConnectionID fragToFixConnID,
										ConstStr255Param fragToFixExportedRoutineName,
										ConstStr255Param weakLinkedLibraryName,
										CFragConnectionID connIDToImport);
	// A wrapper around CFMLateImportCore that looks up symbols by calling 
	// FindSymbol on a connection to a CFM library (connIDToImport).
	// You can get this connection ID through any standard CFM API, for example
	// GetSharedLibrary, GetDiskFragment, or GetMemFragment.
	//
	// IMPORTANT:
	// The fragment name for connIDToImport *does not* have to match
	// weakLinkedLibraryName.  This is part of the power of this library.

extern pascal OSStatus CFMLateImportBundle(const CFragSystem7DiskFlatLocator *fragToFixLocator,
										CFragConnectionID fragToFixConnID,
										ConstStr255Param fragToFixExportedRoutineName,
										ConstStr255Param weakLinkedLibraryName,
										CFBundleRef bundleToImport);
	// A wrapper around CFMLateImportCore that looks up symbols by calling 
	// CFBundleGetFunctionPointerForName on a reference to a Core Foundation 
	// bundle (bundleToImport).  You can get this reference through any 
	// Core Foundation bundle API, for example CFBundleCreate.

#ifdef __cplusplus
}
#endif
